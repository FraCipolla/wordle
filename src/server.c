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

static int login(char *username, char *password)
{
    FILE *user;
    char path[1024];
    
    sprintf(path, "records/%s.txt", username);
    user = fopen(path, "r");
    if (!user) {
        return -1;
        // user does not exists
    } else {
        char pwd[32];
        char *r = fgets(pwd, 32, user);
        pwd[strlen(pwd) - 1] = 0;
        if (!r) {
            fclose(user);
            return -2;
            // cannot read
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
        fprintf(user, "%s\n%s\n%s\n%s\n", password, "0", "0", "0");
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

// Get sockaddr, IPv4 or IPv6:
static void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Return a listening socket
static int get_listener_socket(void)
{
    int listener;     // Listening socket descriptor
    int yes = 1;      // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
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
        
        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    freeaddrinfo(ai); // All done with this

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

// Add a new file descriptor to the set
static void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it

        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read
    (*pfds)[*fd_count].revents = 0;

    (*fd_count)++;
}

// Remove an index from the set
static void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}

int serve(void)
{
    int listener;     // Listening socket descriptor

    int newfd;        // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;

    char buf[512];    // Buffer for client data

    char remoteIP[INET6_ADDRSTRLEN];

    // Start off with room for 5 connections
    // (We'll realloc as necessary)
    int fd_count = 0;
    int fd_size = 5;
    struct pollfd *pfds = malloc(sizeof *pfds * fd_size);

    // Set up and get a listening socket
    listener = get_listener_socket();

    if (listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    // Add the listener to set
    pfds[0].fd = listener;
    pfds[0].events = POLLIN; // Report ready to read on incoming connection

    fd_count = 1; // For the listener

    load_words();
    strcpy(choosen_word, "testo");
    for(;;) {
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for(int i = 0; i < fd_count; i++) {
            // Check if someone's ready to read
            if (pfds[i].revents & (POLLIN | POLLHUP)) { // We got one!!
                if (pfds[i].fd == listener) {
                    // If listener is ready to read, handle new connection
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

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
                    // If not the listener, we're just a regular client
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);
                    int sender_fd = pfds[i].fd;
                    if (nbytes <= 0) {
                        // Got error or connection closed by client
                        if (nbytes == 0) {
                            // Connection closed
                            printf("wordle: socket %d hung up\n", sender_fd);
                        } else {
                            perror("recv");
                        }
                        del_user(pfds[i].fd);
                        close(pfds[i].fd);
                        del_from_pfds(pfds, i, &fd_count);
                        i--;

                    } else {
                        buf[nbytes] = 0;
                        char *op = strtok(buf, "\r\n");
                        if (!strcmp(op, "signup")) {
                            char *username = strtok(NULL, "\r\n");
                            char *password = strtok(NULL, "\r\n");
                            if (register_user(username, password))
                                send(pfds[i].fd, "well done!\nuser registered", 27, 0);
                            else
                                send(pfds[i].fd, "try again:\nuser already exists", 30, 0);
                        } else if (!strcmp(op, "login")) {
                            char *username = strtok(NULL, "\r\n");
                            char *password = strtok(NULL, "\r\n");
                            int status = login(username, password);
                            switch (status)
                            {
                            case 1: {
                                send(pfds[i].fd,"accept", 7, 0);
                                add_user(username, pfds[i].fd);
                                break;
                            }
                            case 0: send(pfds[i].fd, "password does not match", 24, 0); break;
                            case -1: send(pfds[i].fd, "user does not exists", 21, 0); break;
                            case -2: send(pfds[i].fd, "Internal Server Error", 22, 0); break;
                            default: break;
                            }
                        } else if (!strcmp(op, "play")) {
                            user_t *user = get_user(pfds[i].fd);
                            if (get_status(user->name) == END_GAME) {
                                send(pfds[i].fd, "ko\r\nYou have no more attempts for today, wait tomorrow for the next word!\r\n", 76, 0);    
                            }
                            int attempts = 6 - get_status(user->name);
                            char msg[256];
                            sprintf(msg, "ok\r\nAttempts left: %d\r\n", attempts);
                            send(pfds[i].fd, msg, strlen(msg), 0);
                        } else if (!strcmp(op, "guess")) {
                            char *tok = strtok(NULL, "\r\n");
                            user_t *user = get_user(pfds[i].fd);
                            increase_attempt(user->name);
                            guess_word(tok, pfds[i].fd);
                        }
                    }
                } // END handle data from client
            } // END got ready-to-read from poll()
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    
    return 0;
}