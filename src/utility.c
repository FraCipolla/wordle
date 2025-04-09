#include "../include/utility.h"
#include "../include/wordle.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void paste_and_copy(char *username, stat_t stats)
{
    FILE *user;
    char path[1024];

    sprintf(path, "records/%s.txt", username);
    user = fopen(path, "r+");

    if (!user) { return ; }

    char line[64];
    char *r;
    int skips = 0;
    char tmp_path[1024];
    stats.total_game = 0;
    sprintf(tmp_path, "records/%s_tmp.txt", username);
    FILE *tmp = fopen(tmp_path, "w");
    fprintf(tmp, "%s\n%d\n%d\n%d\n%d\n\n", stats.password, stats.total_game, stats.total_win, stats.win_streak, stats.current_win_streak);
    while (skips < 6) {
        r = fgets(line, 64, user);
        ++skips;
    }
    while ((r = fgets(line, 64, user))) {
        fprintf(tmp, "%s", line);
    }
    fclose(user);
    fclose(tmp);
    rename(tmp_path, path);
}

int increase_attempt(char *username, char c)
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
            fseek(user, -2, SEEK_CUR);
            status = strtok(NULL, " ");
            if (c == 'w') {
                fputc('w', user);
                fclose(user);
                return 0;
            }
            else if (c == 'l') {
                fputc('l', user);
                fclose(user);
                return 0;
            }
            c = status[0];
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

stat_t get_stats(char *username)
{
    FILE *user;
    char path[1024];

    sprintf(path, "records/%s.txt", username);
    user = fopen(path, "r");

    stat_t stats;
    fscanf(user, "%s\n%d\n%d\n%d\n%d", stats.password, &stats.total_game, &stats.total_win, &stats.win_streak, &stats.current_win_streak);

    fclose(user);
    return stats;
}