NAME=wordle
SRC=main.c server.c client.c
SRCDIR=src
SRCS=$(addprefix $(SRCDIR)/, $(SRC))
CC=gcc
CFLAGS=-Wall -Werror -Wextra -O3
OBJ=$(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all