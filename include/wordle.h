#ifndef __WORDLE_H__
#define __WORDLE_H__

#define INFO "\nFind the word in 6 attempts! A new word will be choose each day\n \
             \t - A yellow letter means the letter is inside the word but in a different position!\n \
             \t - A green letter means it's in the right position!\n\n \
             Signup and play!\nUsage:\n \
             \t - signup <username>:<password> <url>:  create a new user to remote server\n \
             \t - login <username>:<password> <url>:   login to remote server\n\n \
             After logging in follow the instructions to play!\n\n"
#define PROMPT int prompt = write(1, "wordle> ", 9); if (prompt <= 0) exit(0);

#define WORDLE \
"                        _ _         \n \
                      | | |        \n \
__      _____  _ __ __| | | ___    \n \
\\ \\ /\\ / / _ \\| '__/ _` | |/ _ \\   \n \
 \\ V  V / (_) | | | (_| | |  __/   \n \
  \\_/\\_/ \\___/|_|  \\__,_|_|\\___|   \n \
"                               

typedef struct words_s {
  char **words;
  int  size;
} words_t;

extern words_t words_arr[27];
extern char choosen_word[6];

int serve(void);
int client(char *username, char *password, char *address, int op);
void play_game(int socket);
int guess_word(char *guess, int socket);

#endif