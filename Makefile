NAME=wordle
SRC=main.c server.c client.c game.c users.c
SRCDIR=src
SRCS=$(addprefix $(SRCDIR)/, $(SRC))
CC=gcc
CFLAGS=-Wall -Werror -Wextra -O3
OBJDIR=obj
OBJ = $(SRCS:%.c=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	@echo "Compiling $(NAME)"
	@$(CC) $(OBJ) -o $(NAME)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up object files"
	@rm -f $(OBJ)
	@rm -rf $(OBJDIR)

fclean: clean
	@echo "Cleaning up $(NAME)"
	@rm -f $(NAME)

re: fclean all