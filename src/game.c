#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/wordle.h"

int	play_game(char *word)
{
	static int	tries = 0;
	int	count = 0;
	char	guess[6];

	printf("Try a world: ");
	scanf("%s", guess);
	if (strlen(guess) != 5) {
		printf("\nError: word must be 5 characters long");
		return (0);
	} // controllare che sia una parola valida 
	else if (strcmp(guess, word) == 0) {
		printf("\033[30;42m %s \033[0m", guess);
		printf("Congratulations! You guessed the word!\n");
		// devo salvare tries + 1
		tries = 0;
		return (1);
	} else {
		tries++;
		for (int i = 0; i < 5; i++) {
			if (guess[i] == word[i]) {
				printf("\033[30;42m %c \033[0m", guess[i]);
			}
			else {
				for (int j = 0; j < 5; j++) {
					if (guess[i] == word[j]) {
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
	if (tries >= 6){
		printf("You lose\nToo may tries");
		tries = 0;
		return (1);
	}
	return (0);
}

int	main ()
{
	char word[6] = "testo";
	while (1) {
		if (play_game(word) == 1) {
			break;
		}
	}
}