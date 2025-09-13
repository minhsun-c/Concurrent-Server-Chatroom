CC = gcc
SQLFLAGS = -I/opt/homebrew/opt/mysql/include -L/opt/homebrew/opt/mysql/lib -lmysqlclient
CFLAGS = -Wall -Wextra -g
INC = -Iinclude

ALLFLAGS = $(CFLAGS) $(INC) $(SQLFLAGS)

SRCS1 = $(wildcard src/shell/*.c)
SRCS2 = $(wildcard src/network/*.c)
SRCS3 = $(wildcard src/db/*.c)
OBJS = $(patsubst src/shell/%.c, obj/%.o, $(SRCS1))
OBJS += $(patsubst src/network/%.c, obj/%.o, $(SRCS2))
OBJS += $(patsubst src/db/%.c, obj/%.o, $(SRCS3))

BSRCS1 = $(wildcard bsrc/*.c)
BSRCS2 = $(wildcard bsrc/*.cpp)
BINS1 = $(patsubst bsrc/%.c, bin/%, $(BSRCS1))
BINS2 = $(patsubst bsrc/%.cpp, bin/%, $(BSRCS2))

DIRS = tmp bin obj

all: $(DIRS) $(BINS1) $(BINS2) myserver

myserver: $(OBJS) 
	$(CC) $(ALLFLAGS) -o $@ $^

obj/%.o: src/shell/%.c
	$(CC) $(ALLFLAGS) -c -o $@ $^

obj/%.o: src/network/%.c
	$(CC) $(ALLFLAGS) -c -o $@ $^

obj/%.o: src/db/%.c
	$(CC) $(ALLFLAGS) -c -o $@ $^

bin/%: bsrc/%.c
	$(CC) $(ALLFLAGS) -o $@ $^

bin/%: bsrc/%.cpp
	g++ $(INC) -o $@ $^

$(DIRS):
	mkdir -p $@

clean:
	rm -rf $(DIRS)
	rm myserver