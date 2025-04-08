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

#define PORT "9034" // the port client will be connecting to 

#define MAXDATASIZE 512 // max number of bytes we can get at once 

enum op {
    SIGNUP,
    LOGIN
};

// get sockaddr, IPv4 or IPv6:
static void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

static int client(char *username, char *password, char *address, int op)
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
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

    // loop through all the results and connect to the first we can
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

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);

    if (op == SIGNUP) {
        char buff[1024];
        sprintf(buff, "signup\n%s\n%s\n", username, password);
        send(sockfd, buff, strlen(buff), 0);
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        printf("%s\n",buf);
        close(sockfd);
        return 0;
    } else if (op == LOGIN) {
        printf("login\n");
        char buff[1024];
        sprintf(buff, "login\r\nusername %s\r\npassword %s\r\n\r\n", username, password);
        send(sockfd, buff, strlen(buff), 0);
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        printf("%s\n",buf);
        if (!strcmp(buf, "accept")) {
            for (;;) {
                
            }
        }
    }
    return 0;
}

int signup(char *argv[])
{
    char *username = strtok(argv[0], ":");
    char *password = strtok(NULL, "");

    // printf("username %s password %s address %s\n", username, password, argv[1]);
    client(username, password, argv[1], SIGNUP);
    return 0;
}

int login(char *argv[])
{
    char *username = strtok(argv[0], ":");
    char *password = strtok(NULL, "");
    client(username, password, argv[1], LOGIN);
    return 0;
}