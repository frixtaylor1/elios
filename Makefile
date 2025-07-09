CXX         = gcc
CXX_DEV_OP  = -O0
CXX_PROD_OP = -O3
OP_FLAG     = $(CXX_DEV_OP)
CXX_FLAGS   = -Wextra -Wall -Wpedantic $(OP_FLAG) -g
CC          = $(CXX) $(CXX_FLAGS)

BIN_DIR     = ./build
BIN         = $(BIN_DIR)/elio

SRC_DIR     = ./src
INCLUDE_DIR = ./include


OBJ_COMMAND = $(CC) -c $< -o $@ -I$(INCLUDE_DIR)
OBJS        = $(BIN_DIR)/main.o
OBJS        += $(BIN_DIR)/entity.o
OBJS        += $(BIN_DIR)/components.o
OBJS        += $(BIN_DIR)/alloc.o
OBJS        += $(BIN_DIR)/heap.o

all: clean $(BIN_DIR) $(BIN) run

$(BIN): $(OBJS)
	$(CC) -o $@ $^

$(BIN_DIR)/main.o: $(SRC_DIR)/main.c
	$(OBJ_COMMAND)

$(BIN_DIR)/entity.o: $(SRC_DIR)/entity.c $(BIN_DIR)/alloc.o 
	$(OBJ_COMMAND)

$(BIN_DIR)/alloc.o: $(SRC_DIR)/alloc.c $(BIN_DIR)/heap.o
	$(OBJ_COMMAND)

$(BIN_DIR)/components.o: $(SRC_DIR)/components.c
	$(OBJ_COMMAND)

$(BIN_DIR)/heap.o: $(SRC_DIR)/heap.asm
	nasm -f elf64 $^ -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

.PHONY: all clean release run


clean:
	rm -rf $(BIN_DIR)

run:
	$(BIN)
