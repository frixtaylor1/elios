CXX              = gcc
CXX_DEV_OP       = -O0
CXX_PROD_OP      = -O3
OP_FLAG          = $(CXX_PROD_OP)
CXX_FLAGS        = -Wextra -Wall -Wpedantic $(OP_FLAG) -g -MMD -MP
CC               = $(CXX) $(CXX_FLAGS)

BIN_DIR          = ./build
BIN              = $(BIN_DIR)/elio

SRC_DIR          = ./src
INCLUDE_DIR      = ./include
DEPENDENCIES_DIR = ./dependencies


OBJ_COMMAND = $(CC) -c $< -o $@ -I$(INCLUDE_DIR) -I$(DEPENDENCIES_DIR)
OBJS        = $(BIN_DIR)/main.o
OBJS        += $(BIN_DIR)/entity.o
OBJS        += $(BIN_DIR)/components.o
OBJS        += $(BIN_DIR)/alloc.o
OBJS        += $(BIN_DIR)/heap.o
OBJS        += $(BIN_DIR)/synch.o
OBJS        += $(BIN_DIR)/threads.o
OBJS        += $(BIN_DIR)/systems.o
OBJS        += $(BIN_DIR)/ui.o
OBJS        += $(BIN_DIR)/pool_alloc.o
OBJS        += $(BIN_DIR)/engine.o
OBJS        += $(BIN_DIR)/camera.o
OBJS        += $(BIN_DIR)/file.o

LIBS = -L$(DEPENDENCIES_DIR)/raylib -lraylib -lopengl32 -lgdi32 -lwinmm

all: $(BIN_DIR) $(BIN) run

$(BIN): $(OBJS) 
	$(CC) -o $@ $^ $(LIBS)

$(BIN_DIR)/main.o: $(SRC_DIR)/main.c
	$(OBJ_COMMAND) 

$(BIN_DIR)/entity.o: $(SRC_DIR)/entity.c $(BIN_DIR)/alloc.o 
	$(OBJ_COMMAND)

$(BIN_DIR)/alloc.o: $(SRC_DIR)/alloc.c $(BIN_DIR)/heap.o
	$(OBJ_COMMAND)

$(BIN_DIR)/pool_alloc.o: $(SRC_DIR)/pool_alloc.c
	$(OBJ_COMMAND)

$(BIN_DIR)/components.o: $(SRC_DIR)/components.c
	$(OBJ_COMMAND)

$(BIN_DIR)/heap.o: $(SRC_DIR)/heap.asm
	nasm -f elf64 $^ -o $@

$(BIN_DIR)/synch.o: $(SRC_DIR)/synch.c
	$(OBJ_COMMAND)

$(BIN_DIR)/threads.o: $(SRC_DIR)/threads.c
	$(OBJ_COMMAND)

$(BIN_DIR)/systems.o: $(SRC_DIR)/systems.c
	$(OBJ_COMMAND)

$(BIN_DIR)/ui.o: $(SRC_DIR)/ui.c
	$(OBJ_COMMAND)

$(BIN_DIR)/engine.o: $(SRC_DIR)/engine.c
	$(OBJ_COMMAND)

$(BIN_DIR)/camera.o: $(SRC_DIR)/camera.c
	$(OBJ_COMMAND)

$(BIN_DIR)/file.o: $(SRC_DIR)/file.c
	$(OBJ_COMMAND)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

.PHONY: all clean release run

clean:
	rm -rf $(BIN_DIR)

run:
	$(BIN)
