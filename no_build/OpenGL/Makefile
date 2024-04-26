# Define the compiler
CC=gcc

# Define any compile-time flags
CFLAGS=-Wall -Wextra

# Define any directories containing header files other than /usr/include
INCLUDES=-I./include

# Define library paths in addition to /usr/lib
LFLAGS=

# Define any libraries to link into executable
LIBS= -lglfw -ldl -lpthread

# Detect the operating system
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	LIBS += -lGL -lX11 -lXrandr -lXi 
endif
ifeq ($(UNAME_S),Darwin)
	LIBS += -framework OpenGL -framework Cocoa 
endif

# Automatically find all .c files in the current directory
SRCS=$(wildcard *.c)

# Define the object files
OBJS=$(SRCS:.c=.o)

# Define the executable file
MAIN=program

.PHONY: clean

all:	$(MAIN)
	@echo Program has been compiled

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) *.o *~ $(MAIN)
