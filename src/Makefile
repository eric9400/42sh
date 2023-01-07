CC = gcc
CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -I./include -D_POSIX_C_SOURCE=200809L
CFILES = $(wildcard */*.c)
OBJS = $(CFILES:.c=.o)
MAIN = main.c

all : 42sh

debug : CFLAGS += -fsanitize=address -g
debug : clean 42sh

42sh : $(OBJS) $(MAIN)
	$(CC) $(CFLAGS) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean :
	rm -rf $(OBJS)
	rm -rf 42sh
