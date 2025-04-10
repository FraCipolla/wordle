#include "../include/utility.h"
#include "../include/wordle.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

char *print_file(char *path)
{
    char line[512];
    char *ret = malloc(1024);
    FILE *fp;

    fp = fopen(path,"r");
    if(fp == NULL) {
        exit(0);
    }
    int j = 0;
    while (fgets(line, 512, fp)){
        for (int i = 0; line[i] != '\n'; i++) {
            ret[j++] = line[i];
        }
        ret[j++] = '\n';
    };
    ret[j] = 0;
    fclose(fp);
    printf("%s\n", ret);
    return ret;
}

void setRandomWord()
{
    srand(time(NULL));
    int alphabet = rand() % 27;
    int word_n = rand() % words_load[alphabet].size;
    strcpy(choosen_word, words_load[alphabet].words[word_n]);
    printf("choosen word: %s\n", choosen_word);
}

long long timeInMilliseconds(void)
{
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

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
    int n = fscanf(user, "%s\n%d\n%d\n%d\n%d", stats.password, &stats.total_game, &stats.total_win, &stats.win_streak, &stats.current_win_streak);
    if (n <= 0) { exit(0); }

    fclose(user);
    return stats;
}