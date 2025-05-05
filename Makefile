GAME_NAME := JumpingMaze

# Detect OS and set appropriate variables
ifeq ($(OS),Windows_NT)
    # Windows settings
    CC := gcc
    EXE_EXT := .exe
    RM := del /Q
    MKDIR := mkdir
    RMDIR := rmdir /S /Q
    PKG_CONFIG := pkg-config
    LDLIBS := $(shell $(PKG_CONFIG) --libs raylib) -lm -lpthread
    RUN_CMD :=
else
    # Linux settings
    CC := gcc
    EXE_EXT :=
    RM := rm -f
    MKDIR := mkdir -p
    RMDIR := rm -rf
    PKG_CONFIG := pkg-config
    LDLIBS := $(shell $(PKG_CONFIG) --libs raylib) -lm -lpthread
    RUN_CMD := ./
endif

CFLAGS := -Wall -Wextra -std=c17

# Debug mode control (set DEBUG=1 to enable debug mode)
ifeq ($(DEBUG),1)
    CFLAGS += -ggdb -DDEBUG
endif

# Directories
SRC_DIR := src
BUILD_DIR := build

# Source and object files
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEP_FILES := $(OBJ_FILES:.o=.d)

GAME := $(BUILD_DIR)/$(GAME_NAME)$(EXE_EXT)

## TARGETS RECIPES
all: $(GAME)

# Link the object files to create the output executable
$(GAME): $(OBJ_FILES)
	$(CC) $^ -o $@ $(LDLIBS)

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@$(MKDIR) $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Include dependency files
-include $(DEP_FILES)

# Clean target
clean:
ifeq ($(OS),Windows_NT)
	@if exist $(BUILD_DIR) $(RMDIR) $(BUILD_DIR)
else
	@$(RMDIR) $(BUILD_DIR)
endif

# Run target
run: $(GAME)
	$(RUN_CMD)$< inputs/example.txt

.PHONY: all clean run
