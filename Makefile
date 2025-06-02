CC = gcc
CFLAGS = -Wall -Wextra -ggdb -std=c17
LDLIBS = -lraylib

ifeq ($(OS),Windows_NT)
	LD_LIBS += -lopengl32 -lgdi32 -lwinmm
else
	LDLIBS += -lm
endif

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=build/%.o)

GAME := build/JumpingMaze


all: $(GAME)

$(GAME): $(OBJ)
	$(CC) $^ -o $@ $(LDLIBS)

build/%.o: src/%.c
	@mkdir -p build/
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf build/

# Run target
run: $(GAME)
	./$< inputs/example.txt

.PHONY: all clean run
