CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -Iinc
EXEC    = ft_ping

SRCDIR  = src
OBJDIR  = obj

LDFLAGS = -lm

SRCS    = $(addprefix $(SRCDIR)/, main.c args.c addr.c packet.c print.c rtt.c socket.c utils.c)
OBJS    = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(EXEC)

re: fclean all

.PHONY: all clean fclean re
