CC = gcc
CFLAGS = -Wall -Wextra -g
INC = -Iinclude
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, obj/%.o, $(SRCS))

all: obj bin tmp number  myshell

myshell: $(OBJS)
	$(CC) $(CFLAGS) $(INC) -o $@ $^

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $(INC) -c -o $@ $^

obj:
	mkdir obj

bin:
	mkdir bin

tmp:
	mkdir tmp

clean:
	rm obj/*.o myshell bin/*

number:
	g++ -o ./bin/number bsrc/number.cpp
