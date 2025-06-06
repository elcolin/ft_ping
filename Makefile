CC = gcc
CFLAGS = -Wall -Wextra -Werror -I.
LIB = libftping.a
EXEC = ft_ping
SRC = main.c

SRCDIR = utils
SRCS = $(wildcard $(SRCDIR)/*.c)
LIBOBJS = $(patsubst $(SRCDIR)/%.c, obj/%.o, $(SRCS))

OBJDIR = obj

all: $(EXEC)

$(EXEC): $(LIB) $(OBJDIR)/main.o
	$(CC) $(CFLAGS) -o $@ $^ -L. -lftping

$(OBJDIR)/main.o: $(SRC)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

lib: $(LIB)

clean:
	rm -rf $(OBJDIR) $(EXEC)

fclean: clean
	rm -f $(LIB)

re: fclean all

.PHONY: all clean fclean re lib
