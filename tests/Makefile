# Define directories
SRC_DIR := ../src
OBJ_DIR := ./obj
UNITY_DIR := ./Unity/src
INCLUDES := -I$(UNITY_DIR) -I$(SRC_DIR)

# Compiler flags
CFLAGS := -g -ffreestanding

C_FILES := $(SRC_DIR)/memory/heap/heap.c
UNITY_SRC_FILES := $(UNITY_DIR)/unity.c
TEST_SRC_FILES := $(TEST_DIR)/test_heap.c  # Add your test source file here

C_OBJS := $(C_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
UNITY_OBJS := $(UNITY_SRC_FILES:$(UNITY_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_OBJS := $(OBJ_DIR)/test_heap.o

OBJS := $(C_OBJS) $(UNITY_OBJS) $(TEST_OBJS)

TEST_EXEC := test_heap

.PHONY: all clean test

all: test

test: setup $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TEST_EXEC) $(SRC_DIR)/memory/heap/heap.c $(UNITY_DIR)/unity.c ./test_heap.c

setup:
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(UNITY_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: ./%.c  # Rule for compiling the test source file
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TEST_EXEC)

