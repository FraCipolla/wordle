#include <stdio.h>
#include <string.h>

#include "../include/wordle.h"

char choosen_word[6];

int main(int argc, char *argv[])
{
    if (argc == 1) {
        printf(INFO);
        return 0;
    }
    if (!strcmp(argv[1], "serve")) {
        serve();
        return 0;
    } else if (!strcmp(argv[1], "signup") || !strcmp(argv[1], "login")) {
        if (argc != 4) {
            printf("error\nusage:\n\twordle %s <username>:<password> <url>\n", argv[1]);
            return 1;
        }
        int op = 0;
        if (!strcmp(argv[1], "login")) {
            op = 1;
        }
        char *address = argv[3];
        char *username = strtok(argv[2], ":");
        char *password = strtok(NULL, "");

        if (!password) {
            printf("error\nusage:\n\twordle signup <username>:<password> <url>\n");
            return 1;
        }
        
        client(username, password, address, op);
    } else {
        printf("unrecognized command %s\nusage:\n\tsignup:\twordle signup <username>:<password> <url>\n\tsignup:\twordle login <username>:<password> <url>\n", argv[1]);
        return 1;
    }
    return 0;
}