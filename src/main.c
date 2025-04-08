#include <stdio.h>
#include <string.h>

#include "../include/wordle.h"

int main(int argc, char *argv[])
{
    if (argc == 1) {
        printf(INFO);
        return 0;
    }
    if (!strcmp(argv[1], "serve")) {
        serve();
        return 0;
    } else if (!strcmp(argv[1], "signup")) {
        if (argc != 4) {
            printf("error\nusage:\n\twordle signup <username>:<password> <url>\n");
            return 1;
        }
        signup(&argv[2]);
    } else if (!strcmp(argv[1], "login")) {
        if (argc != 4) {
            printf("error\nusage:\n\twordle login <username>:<password> <url>\n");
            return 1;
        }
        login(&argv[2]);
    } else {
        printf("unrecognized command %s\nusage:\n\tsignup:\twordle signup <username>:<password> <url>\n\tsignup:\twordle login <username>:<password> <url>\n", argv[1]);
        return 1;
    }
    return 0;
}