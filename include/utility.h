#ifndef __UTILITY_H__
#define __UTILITY_H__

typedef struct stat_s {
    char password[32];
    int total_game;
    int total_win;
    int win_streak;
    int current_win_streak;
}   stat_t;

extern long long time_ms;

// void *get_in_addr(struct sockaddr *sa);
// int get_listener_socket(void);
int increase_attempt(char *username, char c);
void prompt();
stat_t get_stats(char *username);
void paste_and_copy(char *username, stat_t stats);
long long timeInMilliseconds(void);
void setRandomWord();
char *print_file(char *path);
void enter_to_continue();

#endif