# makefile for project

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -pedantic -std=c99 -lcapstone -lncurses -lelf -lm -g


# Source files
SRC = main.c \
      fileio.c \
      elf_menu.c \
      my_elf.c \
      elf_controller.c \


# Object files
OBJ = $(SRC:.c=.o)

# Executable
EXEC = main 

EXEC_OTHER = elf_menu \
	     my_elf \
	     elf_controller \
	     fileio

CLEAN = main \
	elf_menu \
	my_elf \
	elf_controller \
	fileio

# create the rest of the makefile
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC) $(EXEC_OTHER)

.PHONY: all clean

# end of makefile

