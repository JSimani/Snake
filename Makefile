# Makefile for Snake

############## Variables ###############

CC = g++ # The compiler being used

# Compile flags
# Set debugging information and max out warnings
CFLAGS = -g -Wall -Wextra -Werror -pedantic 

# Collect all .h files in your directory.
# This way, you can never forget to add
# a local .h file in your dependencies.
INCLUDES = $(shell echo *.h)

# Executables to built using "make all"

EXECUTABLES = snake

############### Rules ###############

all: $(EXECUTABLES)


## Compile step (.cpp files -> .o files)

# To get *any* .o file, compile its .cpp file with the following rule.
%.o: %.cpp $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@


## Linking step (.o -> executable program)

snake: snake.o Game.o termfuncs.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f $(EXECUTABLES) *.o 
