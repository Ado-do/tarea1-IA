CC = gcc

CFLAGS = -Wall -Wextra -Iinclude

# LDFLAGS = -Llib -lraylib -lm -lpthread -ldl -lrt lX11
LDFLAGS = -Llib -lraylib -lm -lpthread

SRC = $(wildcard src/*.c)

OBJ = $(SRC:src/%.c=build/%.o)

OUTPUT = build/jumplab


# Build target
all: $(OUTPUT)

# Link the object files to create the executable
$(OUTPUT): $(OBJ)
	$(CC) $(OBJ) -o $(OUTPUT) $(LDFLAGS)

# Compile source files into object files
build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf build

# Run target
run: $(OUTPUT)
	./$(OUTPUT)

.PHONY: all clean run


