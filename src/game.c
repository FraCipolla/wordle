#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/wordle.h"
#include "../include/users.h"
#include "../include/utility.h"

static int is_inside(char c)
{
	for (int i = 0; i < 5; i++) {
		if (choosen_word[i] == c) {
			return 1;
		}
	}
	return 0;
}

int guess_word(char *guess, int socket)
{
	user_t *user = get_user(socket);
	char msg[2048];
	char fmt[32];
	char path[64];

	memset(fmt, 0, sizeof(fmt));
	memset(msg, 0, sizeof(msg));
	sprintf(path, "records/games/%s.%s.txt", user->name, choosen_word);
	FILE *f = fopen(path, "a");
	char **words_arr = words_load[guess[0] - 97].words;
	int exists = 0;
	for (int i = 0; i < words_load[guess[0] - 97].size; i++) {
	    if (!strcmp(words_arr[i], guess)) {
	        exists = 1;
	        break;
	    }
	}
	if (!exists) {
		char *history = print_file(path);
		sprintf(msg, "error<<You must input a valid word\n\n%s", history);
		free(history);
		send(socket, msg, strlen(msg), 0);
		return 0;
	}
	increase_attempt(user->name, 'z');
	if (strcmp(guess, choosen_word) == 0) {
		stat_t stats = get_stats(user->name);
		stats.total_win++;
		stats.current_win_streak++;
		if (stats.current_win_streak > stats.win_streak) {
			stats.win_streak = stats.current_win_streak;
		}
		increase_attempt(user->name, 'w');
		paste_and_copy(user->name, stats);
		fprintf(f,
			"\033[30;42m %c  %c  %c  %c  %c \033[0m\n",
			guess[0], guess[1], guess[2], guess[3], guess[4]);
		fclose(f);
		char *history = print_file(path);
		sprintf(msg, "win<<%s", history);
		free(history);
		send(socket, msg, strlen(msg), 0);
		return 1;
	} else {
		for (int i = 0; i < 5; i++) {
			if (guess[i] == choosen_word[i]) {
				sprintf(fmt, "\033[30;42m %c \033[0m", guess[i]);
				strcat(msg, fmt);
				memset(fmt, 0, sizeof(fmt));
			} else if (is_inside(guess[i])) {
				sprintf(fmt, "\033[30;43m %c \033[0m", guess[i]);
				strcat(msg, fmt);
				memset(fmt, 0, sizeof(fmt));
			} else {
				sprintf(fmt, "\033[30;30m %c \033[0m", guess[i]);
				strcat(msg, fmt);
				memset(fmt, 0, sizeof(fmt));
			}
		}
	}
	fprintf(f, "%s\n", msg);
	fclose(f);
	char *history = print_file(path);
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "%s", history);
	free(history);
	estatus_t status = get_status(user->name);
	int attempts = 6 - status;
	char append[2048];
	if (attempts == 0) {
		stat_t stats = get_stats(user->name);
		if (stats.current_win_streak > stats.win_streak) {
			stats.win_streak = stats.current_win_streak;
		}
		stats.current_win_streak = 0;
		increase_attempt(user->name, 'l');
		paste_and_copy(user->name, stats);
		sprintf(append, "end<<%s", msg);
	} else {
		sprintf(append, "skip<<Attempts left: %d\n\n%s", attempts, msg);
	}
	send(socket, append, strlen(append), 0);
	return 0;
}

void play_game(int socket)
{
	char guess[32];
	char buf[2048];
	char msg[64];
	int numbytes = 0;
	
	while (1) {
		printf("enter exit to exit\n\n");
		prompt();
		memset(guess, 0, sizeof(guess));
		memset(msg, 0, sizeof(msg));
		memset(buf, 0, sizeof(buf));
		int n = scanf("%s", guess);
		if (!guess[0]) { exit(0); } // EOF case
		if (n <= 0) { continue; }
		if (!strcmp(guess, "exit")) {
			return ;
		}
		else if (strlen(guess) != 5) {
			printf("Error: word must be 5 characters long\n");
		} else {
			sprintf(msg, "guess<<%s", guess);
			send(socket, msg, strlen(msg), 0);
			if ((numbytes = recv(socket, buf, sizeof(buf), 0)) == -1) {
				perror("recv");
				exit(1);
			}
			char *tok = strtok(buf, "<<");
			if (!strcmp("end", tok)) {
				printf("\n%s\n", strtok(NULL, "<<"));
				printf("Oh no! You miss the correct word!\n\n");
				return ;
			} else if (!strcmp("win", tok)) {
				printf("\n%s\n", strtok(NULL, "<<"));
				printf("\nCongratulations! You found the correct word!\n\n");
				return ;
			} else {
				printf("\n%s\n", strtok(NULL, "<<"));
			}
			printf("\n");
		}
	}
}