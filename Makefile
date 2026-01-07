TARGET   := main
SRC_DIR  := src
EXTERNAL_DIR := external
OBJ_DIR  := obj

CXX      := g++
CC       := gcc
CXXFLAGS := -Wall -Wextra -Wpedantic -Wshadow -Wconversion -std=c++17 -I${EXTERNAL_DIR}/headers
CFLAGS   := -Wall -Wextra -I${EXTERNAL_DIR}/headers 
LDFLAGS  := -lstdc++

CPP_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
C_SRCS   := $(wildcard $(EXTERNAL_DIR)/sqlite*.c)

OBJS     := $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o) \
            $(C_SRCS:$(EXTERNAL_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(EXTERNAL_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
