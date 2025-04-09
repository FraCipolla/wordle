#include "../include/utility.h"
#include "../include/wordle.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int increase_attempt(char *username)
{
    FILE *user;
    char path[1024];

    sprintf(path, "records/%s.txt", username);
    user = fopen(path, "r+");

    if (!user) { return 0; }

    char line[64];
    char *r;
    char *status = NULL;
    while ((r = fgets(line, 64, user))) {
        char *word = strtok(line, " ");
        if (!strcmp(word, choosen_word)) {
            printf("pos: %ld\n", ftell(user));
            fseek(user, -2, SEEK_CUR);
            status = strtok(NULL, " ");
            char c = status[0];
            c++;
            fputc(c, user);
            fclose(user);
            return atoi(&c);
        }
    }
    fclose(user);
    return 0;
}

void prompt()
{
    int prompt = write(1, "wordle> ", 9);
    if (prompt <= 0) exit(0);
}