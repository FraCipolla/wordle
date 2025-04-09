#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../include/wordle.h"
#include "../include/utility.h"

#define PORT "9034" // the port client will be connecting to 

enum op {
    SIGNUP,
    LOGIN
};

static void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int client(char *username, char *password, char *address, int op)
{
    int sockfd, numbytes;  
    char buf[1024];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(address, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);

    if (op == SIGNUP) {
        char buff[1024];
        sprintf(buff, "signup\n%s\n%s\n", username, password);
        send(sockfd, buff, strlen(buff), 0);
        if ((numbytes = recv(sockfd, buf, 1024, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        printf("%s\n",buf);
        close(sockfd);
        return 0;
    } else if (op == LOGIN) {
        char buff[1024];
        sprintf(buff, "login\r\n%s\r\n%s\r\n\r\n", username, password);
        send(sockfd, buff, strlen(buff), 0);
        if ((numbytes = recv(sockfd, buf, 1024, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        char *tok = strtok(buf, "\r\n");
        if (!strcmp(tok, "accept")) {
            char input[128];
            int n_read;
            printf("login succesfull!\n%s\n", WORDLE);
            printf("Welcome back %s!\n[p]lay    [s]tats    [q]uit\n\n", username);
            prompt();
            while((n_read = read(0, input, 128))) {
                input[n_read - 1] = 0;
                if (n_read == 0) {
                    continue;
                } else if (strlen(input) > 1 && strcmp(input, "play") != 0 && strcmp(input, "leaderboard") != 0
                        && strcmp(input, "quit") != 0 && strcmp(input, "score") != 0) {
                            input[0] = 'z';
                    }
                switch (input[0]) {
                    case 'p': {
                        send(sockfd, "play", 5, 0);
                        if ((numbytes = recv(sockfd, buf, 1024, 0)) == -1) {
                            perror("recv");
                            exit(1);
                        }
                        buf[numbytes] = '\0';
                        char *tok = strtok(buf, "\r\n");
                        prompt();
                        printf("Guess a 5 chars long word\n\n");
                        printf("%s\n\n", strtok(NULL, "\r\n"));
                        if (!strcmp(tok, "ko")) {
                            printf("[p]lay  [s]tats    [q]uit\n\n");
                            prompt();
                            break;
                        }
                        play_game(sockfd);
                        printf("<<<<<<<<<<<<<<<<<<press enter to continue<<<<<<<<<<<<<<<<<<\n");
                        read(1, input, 128);
                        int s = system("@cls||clear");
                        if (s < 0) { exit(0); }
                        printf("\n%s\n", WORDLE);
                        printf("Welcome back %s!\n[p]lay    [s]tats    [q]uit\n\n", username);
                        prompt();
                        break;
                    }
                    case 'l': /* leaderboard */ break;
                    case 's': {
                        send(sockfd, "stats", 6, 0);
                        if ((numbytes = recv(sockfd, buf, 1024, 0)) == -1) {
                            perror("recv");
                            exit(1);
                        }
                        buf[numbytes] = '\0';
                        printf("\n%s\n\n", buf);
                        printf("[p]lay    [s]tats    [q]uit\n\n");
                        prompt();
                        break;
                    }
                    case 'q': printf("See you soon %s!\n", username); exit(0);
                    default:
                    printf("\nerror: wrong input, please insert one of the following:\n");
                    printf("[p]lay    [s]tats    [q]uit\n\n");
                    prompt();
                    break;
                }
                memset(input, 0, sizeof(input));
            }
        } else {
            printf("%s\n", buf);
            exit(0);
        }
    }
    return 0;
}
