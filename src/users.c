#include "../include/users.h"
#include "../include/wordle.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

estatus_t get_status(char *username)
{
    FILE *user;
    char path[1024];

    sprintf(path, "records/%s.txt", username);
    user = fopen(path, "r");

    if (!user) { return ERROR; }

    char line[64];
    char *r;
    char *status = NULL;
    while ((r = fgets(line, 64, user))) {
        char *word = strtok(line, " ");
        if (!strcmp(word, choosen_word)) {
            status = strtok(NULL, " ");
            break;
        }
    }
    fclose(user);
    // if status is NULL we are in the first attempt, so we add the word to the file
    if (!status) {
        user = fopen(path, "a");
        fprintf(user, "\n%s %d\n", choosen_word, 0);
        fclose(user);
        return NO_ATTEMPTS;
    } else if (!strcmp(status, "w") || !strcmp(status, "l")) {
        return END_GAME;
    }
    return (estatus_t)atoi(status);
}

void add_user(char *username, int socket)
{
    printf("adduser %s socket %d\n", username, socket);
    user_t *new = malloc(sizeof(user_t));

    strcpy(new->name, username);
    new->socket = socket;
    new->next = NULL;
    new->status = get_status(username);

    if (!user_list) {
        user_list = new;
    } else {
        user_t *cpy = user_list;
        while (cpy->next) {
            cpy = cpy->next;
        }
        cpy->next = new;
    }
}

void del_user(int socket)
{
    printf("deluser socket %d\n", socket);
    user_t *cpy = user_list;
    if (!cpy) {
        return ;
    }
    while (cpy->next && cpy->next->socket != socket) {
        cpy = cpy->next;
    }
    if (cpy->next && cpy->next->socket == socket) {
        user_t *tmp = cpy->next;
        cpy = cpy->next->next;
        free(tmp);
    }
}

user_t *get_user(int socket)
{
    printf("getuser socket %d\n", socket);
    user_t *cpy = user_list;
    while (cpy->next && cpy->socket != socket) {
        cpy = cpy->next;
    }
    if (cpy->socket == socket) {
        return cpy;
    }
    return NULL;
}