#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/wordle.h"

char *choosen_word = "testo";

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
	char msg[256];
	char fmt[32];

	memset(fmt, 0, sizeof(fmt));
	memset(msg, 0, sizeof(msg));
	if (strcmp(guess, choosen_word) == 0) {
		sprintf(msg, "\033[30;42m %c %c %c %c %c \033[0m", guess[0], guess[1], guess[2], guess[3], guess[4]);
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
	send(socket, msg, strlen(msg), 0);
	return 0;
}

void play_game(int socket)
{
	char guess[32];
	char buf[128];
	char msg[64];
	int numbytes = 0;

	while (1) {
		memset(guess, 0, sizeof(guess));
		memset(msg, 0, sizeof(msg));
		memset(buf, 0, sizeof(buf));
		PROMPT
		int x = write(1, "Guess a 5 chars long word: ", 28);
		if (x <= 0) {
			printf("Something went wrong, please try again\n");
		}
		int n = scanf("%s", guess);
		if (!guess[0]) { exit(0); } // EOF case
		if (n <= 0) { continue; }
		if (strlen(guess) != 5) {
			printf("Error: word must be 5 characters long\n");
		} else {
			sprintf(msg, "guess\r\n%s", guess);
			send(socket, msg, strlen(msg), 0);
			if ((numbytes = recv(socket, buf, 128, 0)) == -1) {
				perror("recv");
				exit(1);
			}
			printf("\n%s\n\n", buf);
		}
	}
}