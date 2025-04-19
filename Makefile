## VARIABLES
# Compiler and flags
CC = gcc

CFLAGS = -Wall -Wextra $(shell pkg-config --cflags raylib)

# LDFLAGS = -Llib -lraylib -lm -lpthread -ldl -lrt -lX11
LDFLAGS = -lm -lpthread $(shell pkg-config --libs raylib)

# Output executable name
OUTPUT = jumplab

# Directories
SRC_DIR = src
BUILD_DIR = build
INPUTS_DIR = inputs

# Source and object files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)


## RECIPES
# Default target
all: $(BUILD_DIR)/$(OUTPUT)

# Link the object files to create the executable
$(BUILD_DIR)/$(OUTPUT): $(OBJ)
	@echo "* Link object files into $(OUTPUT):"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "* Compiling $< into $@ object file:"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	@echo "* Cleaning build files:"
	$(RM) -rv $(BUILD_DIR)

# Run target
run: $(BUILD_DIR)/$(OUTPUT)
	@if [ -f $< ]; then \
		./$< < $(INPUTS_DIR)/example.txt; \
	else \
		echo "Error: $< not found."; \
	fi

# Debug target
debug: CFLAGS += -g
debug: all

recompile: clean all

.PHONY: all clean run debug recompile
