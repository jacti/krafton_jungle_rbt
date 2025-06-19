.PHONY: all test clean

CC = gcc
CFLAGS = -I ../src -Wall -g

OBJ_DIR ?= ../out/obj
BIN_DIR ?= ../out/bin

TARGET = $(BIN_DIR)/test-rbtree
OBJS = $(OBJ_DIR)/test-rbtree.o $(OBJ_DIR)/rbtree.o

all: $(TARGET)

test: $(TARGET)
	$(TARGET)
	valgrind ./$(TARGET)

# 실행 파일 생성
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# 테스트용 목적 파일
$(OBJ_DIR)/test-rbtree.o: test-rbtree.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# src 쪽 rbtree.o가 없다면 src/Makefile을 통해 빌드
$(OBJ_DIR)/rbtree.o:
	$(MAKE) -C ../src OBJ_DIR=$(OBJ_DIR) BIN_DIR=$(BIN_DIR) $(OBJ_DIR)/rbtree.o

clean:
	rm -f $(OBJ_DIR)/test-rbtree.o $(TARGET)
