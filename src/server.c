#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

#define PORT "9034"

#include "../include/wordle.h"
#include "../include/users.h"
#include "../include/utility.h"

words_t words_load[27];
user_t *user_list = {0};
long long time_ms;

static int login(char *username, char *password)
{
    FILE *user;
    char path[1024];
    
    sprintf(path, "records/%s.txt", username);
    user = fopen(path, "r");
    if (!user) {
        return -1;
    } else {
        char pwd[32];
        char *r = fgets(pwd, 32, user);
        pwd[strlen(pwd) - 1] = 0;
        if (!r) {
            fclose(user);
            return -2;
        }
        if (!strcmp(pwd, password)) {
            fclose(user);
            return 1;
        }
    }
    return(0);
}

static int register_user(char *username, char *password)
{
    FILE *user;
    char path[1024];
    
    sprintf(path, "records/%s.txt", username);
    user = fopen(path, "r");
    if (!user) {
        user = fopen(path, "w");
        fprintf(user, "%s\n%s\n%s\n%s\n%s\n", password, "0", "0", "0", "0");
        fclose(user);
    } else {
        fclose(user);
        return(0);
    }
    return (1);
}

void load_words()
{
    FILE *words;

    char buffer[8];

    words = fopen("words.txt", "r");
    if (!words) {
        printf("error: cannot open words.txt\n");
        exit(1);
    }
    int i = 0;
    char c = 'a';
    char *word;
    int size = 0;
    while ((word = fgets(buffer, 8, words))) {
        if (word[0] != c) {
            words_load[i].words = malloc(sizeof(char*) * size + 1);
            words_load[i].words[size] = NULL;
            words_load[i].size = size;
            i++;
            c = word[0];
            size = 1;
        } else {
            size++;
        }
    }
    words_load[i].words = malloc(sizeof(char*) * size + 1);
    words_load[i].words[size] = NULL;
    words_load[i].size = size;
    fclose(words);

    words = fopen("words.txt", "r");
    if (!words) {
        printf("error: cannot open words.txt\n");
        exit(1);
    }

    i = 0;
    c = 'a';
    int j = 0;
    while ((word = fgets(buffer, 7, words))) {
        word[5] = 0;
        if (word[0] != c) {
            i++;
            c = word[0];
            j = 0;
        }
        words_load[i].words[j] = malloc(sizeof(char) * 6);
        words_load[i].words[j][5] = 0;
        strncpy(words_load[i].words[j], word, 5);
        j++;
    }
    fclose(words);
}

static void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

static int get_listener_socket(void)
{
    int listener;
    int yes = 1;
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "wordle: %s\n", gai_strerror(rv));
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        break;
    }
    if (p == NULL) {
        return -1;
    }

    freeaddrinfo(ai);

    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

static void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    if (*fd_count == *fd_size) {
        *fd_size *= 2;
        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN;
    (*pfds)[*fd_count].revents = 0;

    (*fd_count)++;
}

static void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    pfds[i] = pfds[*fd_count-1];
    (*fd_count)--;
}

int serve(void)
{
    int listener;
    int newfd;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;

    char buf[512];

    char remoteIP[INET6_ADDRSTRLEN];

    int fd_count = 0;
    int fd_size = 5;
    struct pollfd *pfds = malloc(sizeof *pfds * fd_size);

    listener = get_listener_socket();

    if (listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    pfds[0].fd = listener;
    pfds[0].events = POLLIN;

    fd_count = 1;

    load_words();
    setRandomWord();
    time_ms = timeInMilliseconds();

    for(;;) {
        int poll_count = poll(pfds, fd_count, -1);
        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        for(int i = 0; i < fd_count; i++) {
            if (pfds[i].revents & (POLLIN | POLLHUP)) {
                if (pfds[i].fd == listener) {
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

                        printf("wordle: new connection from %s on "
                            "socket %d\n",
                            inet_ntop(
                                remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP,
                                INET6_ADDRSTRLEN),
                                newfd
                            );
                    }
                } else {
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);
                    int sender_fd = pfds[i].fd;
                    buf[nbytes] = 0;
                    if (nbytes <= 0) {
                        if (nbytes == 0) {
                            printf("wordle: socket %d hung up\n", sender_fd);
                        } else {
                            perror("recv");
                        }
                        del_user(pfds[i].fd);
                        close(pfds[i].fd);
                        del_from_pfds(pfds, i, &fd_count);
                        i--;
                    } else {
                        char msg[1024];
                        char *op = strtok(buf, "<<");
                        if (!strcmp(op, "signup")) {
                            char *username = strtok(NULL, "<<");
                            char *password = strtok(NULL, "<<");
                            if (register_user(username, password))
                                send(pfds[i].fd, "\nwell done!\nuser registered\nYou can now login using:\n\n\twordle login <username>:<password> <host>\n", 97, 0);
                            else
                                send(pfds[i].fd, "\ntry again: user already exists\n", 35, 0);
                        } else if (!strcmp(op, "login")) {
                            char *username = strtok(NULL, "<<");
                            char *password = strtok(NULL, "<<");
                            int status = login(username, password);
                            switch (status) {
                                case 1: {
                                    send(pfds[i].fd, "accept", 7, 0);
                                    add_user(username, pfds[i].fd);
                                    break;
                                }
                                case 0: send(pfds[i].fd, "password does not match", 24, 0); break;
                                case -1: send(pfds[i].fd, "user does not exists", 21, 0); break;
                                case -2: send(pfds[i].fd, "Internal Server Error", 22, 0); break;
                                default: break;
                            }
                        } else if (!strcmp(op, "play")) {
                            if (timeInMilliseconds() - time_ms >= 864e+7) {
                                time_ms = timeInMilliseconds();
                                setRandomWord();
                            }
                            user_t *user = get_user(pfds[i].fd);
                            int status = get_status(user->name);
                            if (status == END_GAME) {
                                send(pfds[i].fd, "ko<<You have no more attempts for today, wait tomorrow for the next word!<<", 76, 0);
                            } else {
                                if (status == NEW_WORD) {
                                    stat_t stats = get_stats(user->name);
                                    stats.total_game++;
                                    paste_and_copy(user->name, stats);
                                }
                                int attempts = 6 - get_status(user->name);
                                char path[64];
                                sprintf(path, "records/games/%s.%s.txt", user->name, choosen_word);
                                FILE *f = fopen(path, "r");
                                if (f) {
                                    char *history = print_file(path);
                                    sprintf(msg, "ok<<Attempts left: %d\n\n%s", attempts, history);
                                    free(history);
                                } else {
                                    sprintf(msg, "ok<<Attempts left: %d<<", attempts);
                                }
                                send(pfds[i].fd, msg, strlen(msg), 0);
                            }
                        } else if (!strcmp(op, "guess")) {
                            char *tok = strtok(NULL, "<<");
                            guess_word(tok, pfds[i].fd);
                        } else if (!strcmp(op, "stats")) {
                            user_t *user = get_user(pfds[i].fd);
                            stat_t stats = get_stats(user->name);
                            sprintf(
                                msg,
                                "total game: %d\ntotal win: %d\nmax win streak: %d\ncurrent win streak: %d\nwin ratio: %f%%",
                                stats.total_game, stats.total_win, stats.win_streak, stats.current_win_streak, stats.total_game == 0 ? 0 : (float)(((float)stats.total_win / (float)stats.total_game) * 100)
                            );
                            send(pfds[i].fd, msg, strlen(msg), 0);
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}