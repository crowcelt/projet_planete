CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iheaders
LDFLAGS = -lm

SRC_DIR = sources
OBJ_DIR = objects
HDR_DIR = headers
OUT_DIR = output

SRCS = $(wildcard $(SRC_DIR)/*.c) main.c
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c)) \
       $(OBJ_DIR)/main.o

TARGET = kepler

.PHONY: all clean

all: $(OUT_DIR) $(TARGET)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/main.o: main.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
