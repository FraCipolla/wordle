#ifndef __UTILITY_H__
#define __UTILITY_H__

typedef struct stat_s {
    char password[32];
    int total_game;
    int total_win;
    int win_streak;
    int current_win_streak;
}   stat_t;

int increase_attempt(char *username, char c);
void prompt();
stat_t get_stats(char *username);
void paste_and_copy(char *username, stat_t stats);

#endif