# Variables
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
SRC = main.c ft_ping.c print.c rtt.c packet.c
OBJ = $(SRC:.c=.o)
EXEC = ft_ping
# Rules
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(EXEC)

re: fclean all

.PHONY: all  clean fclean re