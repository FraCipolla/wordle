#ifndef __USERS_H__
#define __USERS_H__

typedef enum {
    NO_ATTEMPTS,
    ONE_ATTEMPTS,
    TWO_ATTEMPTS,
    THREE_ATTEMPTS,
    FOUR_ATTEMPTS,
    FIVE_ATTEMPTS,
    END_GAME,
    ERROR
}   estatus_t;

typedef struct user_s {
    char name[32];
    int socket;
    estatus_t status;
    struct user_s *next;
}   user_t;

extern user_t *user_list;

void add_user(char *username, int socket);
void del_user(int socket);
user_t *get_user(int socket);

#endif