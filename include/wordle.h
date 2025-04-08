#ifndef __WORDLE_H__
#define __WORDLE_H__

#define INFO "\nFind the word in 6 attempts! A new word will be choose each day\n \
             \t - A yellow letter means the letter is inside the word but in a different position!\n \
             \t - A green letter means it's in the right position!\n\n \
             Signup and play!\nUsage:\n \
             \t - signup <username>:<password> <url>:  create a new user to remote server\n \
             \t - login <username>:<password> <url>:   login to remote server\n\n \
             After logging in follow the instructions to play!\n\n"

#define WORDLE \
"                        _ _         \n \
                      | | |        \n \
__      _____  _ __ __| | | ___    \n \
\\ \\ /\\ / / _ \\| '__/ _` | |/ _ \\   \n \
 \\ V  V / (_) | | | (_| | |  __/   \n \
  \\_/\\_/ \\___/|_|  \\__,_|_|\\___|   \n \
"                               

extern char **words_arr[27];

int serve(void);
int signup(char *argv[]);
int login(char *argv[]);

#endif