# Variables
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
SRC = main.c ft_ping.c
TEST_SRC = ft_ping.c tests/test_main.c tests/unity.c
OBJ = $(SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)
EXEC = ft_ping
TEST_EXEC = ft_ping_tests

# Add include paths for tests
INCLUDES = -I./ -I./tests

# Rules
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

tests: $(TEST_EXEC)

$(TEST_EXEC): $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDES)

$(TEST_OBJ): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ) $(TEST_OBJ)

fclean: clean
	rm -f $(EXEC) $(TEST_EXEC)

re: fclean all

.PHONY: all tests clean fclean re