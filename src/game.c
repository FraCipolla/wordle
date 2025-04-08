#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/wordle.h"

char *choosen_word = "test";

char *guess_word(char *guess)
{
	if (strcmp(guess, choosen_word) == 0) {
		for (int i = 0; i < 5; i++) {
			printf("\033[30;42m %c \033[0m", guess[i]);
		}
		printf("\nCongratulations! You guessed the right word!");
	} else {
		for (int i = 0; i < 5; i++) {
			if (guess[i] == choosen_word[i]) {
				printf("\033[30;42m %c \033[0m", guess[i]);
			}
			else {
				for (int j = 0; j < 5; j++) {
					if (guess[i] == choosen_word[j]) {
						count  = printf("\033[30;43m %c \033[0m", guess[i]);
						break;
					}
				}
				if (!count) {
					printf("\033[30;40m %c \033[0m", guess[i]);
				}
			}
		}
	}
	printf("\n");
}

int	play_game(int socket)
{
	int	count = 0;
	char [6];
	int nunmbytes = 0;

	while (1) {
		printf("Guess a 5 chars long word: ");
		int n = scanf("%s", guess);
		if (n <= 0) { continue; }
		if (strlen(guess) != 5) {
			printf("\nError: word must be 5 characters long");
		} else {
			char msg[16];
			sprintf(msg, "guess\r\n%s", guess)
			send(socket, guess, strlen(guess), 0);
			if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
				perror("recv");
				exit(1);
			}
		}
	}
}