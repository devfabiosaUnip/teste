CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -finput-charset=UTF-8 -fexec-charset=UTF-8 -Isrc -Isrc/admin -Isrc/storage -Isrc/utils -Isrc/server_logic -Isrc/professor -Isrc/student

SRCS_SERVER = src/main.c \
             src/storage/storage.c \
             src/common_utils.c \
             src/admin/admin.c \
             src/server.c \
             src/server_logic/server_logic.c \
             src/professor/professor.c \
             src/student/student.c

SRCS_CLIENT = src/client_main.c \
              src/client.c \
              src/common_utils.c \
              src/storage/storage.c

BUILD_DIR = build
OBJS_SERVER = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS_SERVER))
OBJS_CLIENT = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS_CLIENT))

TARGET_SERVER = sistema_academico_server
TARGET_CLIENT = sistema_academico_client

# Gerar uma lista de diretórios de saída para os objetos
OBJ_DIRS = $(sort $(dir $(OBJS_SERVER) $(OBJS_CLIENT)))

all: $(BUILD_DIR) $(TARGET_SERVER) $(TARGET_CLIENT)

$(BUILD_DIR): $(OBJ_DIRS)
	mkdir -p $(BUILD_DIR)

$(OBJ_DIRS):
	mkdir -p $@

$(TARGET_SERVER): $(OBJS_SERVER)
	$(CC) $(CFLAGS) $(OBJS_SERVER) -o $@ -lws2_32

$(TARGET_CLIENT): $(OBJS_CLIENT)
	$(CC) $(CFLAGS) $(OBJS_CLIENT) -o $@ -lws2_32

$(BUILD_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: src/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/server.o: src/server.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/server_logic/server_logic.o: src/server_logic/server_logic.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/professor/professor.o: src/professor/professor.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/student/student.o: src/student/student.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/storage/storage.o: src/storage/storage.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET_SERVER) $(TARGET_CLIENT)

.PHONY: all clean
