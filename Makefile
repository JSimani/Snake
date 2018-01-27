# Makefile for Snake

CC = g++ # The compiler being used
CFLAGS = -g -Wall -Wextra -Werror -pedantic 
INCLUDES = $(shell echo *.h)

# Executables to built using "make all"
EXECUTABLES = snake

all: $(EXECUTABLES)

%.o: %.cpp $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@

snake: snake.o Game.o termfuncs.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f $(EXECUTABLES) *.o 
