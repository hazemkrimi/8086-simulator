CC = gcc
TARGET_NAME = simulator
CFLAGS = -g -Wall -Wextra -DDEBUG
OUT_DIR = out
TARGET = $(OUT_DIR)/$(TARGET_NAME)

SRCS = src/utils.c src/decoder.c src/simulator.c src/main.c
OBJS = $(SRCS:src/%.c=$(OUT_DIR)/%.o)

$(TARGET): $(OBJS)
	@mkdir -p $(OUT_DIR)
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Built $(TARGET_NAME) in $(MODE) mode: $@"

$(OUT_DIR)/%.o: src/%.c
	@mkdir -p $(OUT_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm $(TARGET)
	@echo "Cleaned build."

.PHONY: all run clean
