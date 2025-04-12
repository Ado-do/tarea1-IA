## VARIABLES
# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
# LDFLAGS = -Llib -lraylib -lm -lpthread -ldl -lrt -lX11
LDFLAGS = -Llib -lraylib -lm -lpthread

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
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	$(RM) -rf $(BUILD_DIR)

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

# Release target
release: CFLAGS += -O2
release: all

# Target to check the wildcard result
check_wildcards:
	@echo "Source files found:"
	@echo $(SRC)
	@echo "Object files generated:"
	@echo $(OBJ)

.PHONY: all clean run debug release check_wildcards


