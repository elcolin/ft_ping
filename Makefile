CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -I.
LIB     = libftping.a
EXEC    = ft_ping

SRCMAIN = main.c
SRCDIR  = utils
SRCS    = $(addprefix $(SRCDIR)/, args.c domain.c packet.c print.c rtt.c socket.c utils.c)

OBJDIR  = obj
OBJS    = $(OBJDIR)/$(SRCMAIN:.c=.o) \
          $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

LIBOBJS = $(filter-out $(OBJDIR)/main.o,$(OBJS))

all: $(EXEC)

$(EXEC): $(LIB) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -L. -lftping -lm

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(LIBOBJS)
	ar rcs $@ $^

lib: $(LIB)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(LIB) $(EXEC)

re: fclean all

.PHONY: all clean fclean re lib
