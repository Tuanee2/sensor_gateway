.PHONY: all clean

LIB_NAME_STATIC := linkedlist

CUR_DIR := .
BIN_DIR := $(CUR_DIR)/bin
INC_DIR := $(CUR_DIR)/inc
SRC_DIR := $(CUR_DIR)/src
OBJ_DIR := $(CUR_DIR)/objs
LIB_DIR := $(CUR_DIR)/libs
LIB_STATIC := $(LIB_DIR)/static

CC_ := gcc
CFLAGS := -I $(INC_DIR)
LDFLAGS :=-L$(LIB_STATIC) -l$(LIB_NAME_STATIC)

make_dirs:
	mkdir -p $(BIN_DIR) $(OBJ_DIR) $(LIB_STATIC)

mk_objs: make_dirs
	$(CC_) -c $(CUR_DIR)/main.c -o $(OBJ_DIR)/main.o $(CFLAGS)
	$(CC_) -c $(SRC_DIR)/shared_data.c -o $(OBJ_DIR)/shared_data.o $(CFLAGS)

mk_static:
	ar rcs $(LIB_STATIC)/lib$(LIB_NAME_STATIC).a $(OBJ_DIR)/shared_data.o

install:
	sudo cp -f $(LIB_STATIC)/lib$(LIB_NAME_STATIC).a /usr/lib

all: mk_objs mk_static install
	$(CC_) $(OBJ_DIR)/main.o $(LDFLAGS) -o $(BIN_DIR)/sensor-gateway
	$(CC_) -o client client.c
clean:
	rm -rf $(OBJ_DIR)/*
	rm -rf $(LIB_STATIC)/*
	rm -rf $(BIN_DIR)/*
	rm -rf logfile.txt
	rm -rf client