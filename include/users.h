#ifndef __USERS_H__
#define __USERS_H__

typedef enum {
    NEW_WORD = -1,
    NO_ATTEMPTS,
    ONE_ATTEMPTS,
    TWO_ATTEMPTS,
    THREE_ATTEMPTS,
    FOUR_ATTEMPTS,
    FIVE_ATTEMPTS,
    END_GAME,
    WIN,
    LOSE,
    ERROR
}   estatus_t;

typedef struct user_s {
    char name[32];
    int socket;
    struct user_s *next;
}   user_t;

extern user_t *user_list;

void add_user(char *username, int socket);
void del_user(int socket);
user_t *get_user(int socket);
estatus_t get_status(char *username);

#endif