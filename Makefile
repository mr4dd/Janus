TARGET        := main

SRC_DIR       := src
LIB_DIR       := lib
EXTERNAL_DIR  := external
OBJ_DIR       := obj

CXX           := g++
CC            := gcc

CXXFLAGS      := -Wall -Wextra -Wpedantic -Wshadow -Wconversion -std=c++17 \
                -I$(EXTERNAL_DIR)/headers -I$(LIB_DIR)/headers

CFLAGS        := -Wall -Wextra \
                -I$(EXTERNAL_DIR)/headers -I$(LIB_DIR)/headers

LDFLAGS       := -lstdc++

CPP_SRCS      := $(wildcard $(SRC_DIR)/*.cpp) \
                $(wildcard $(LIB_DIR)/*.cpp)

C_SRCS        := $(wildcard $(EXTERNAL_DIR)/sqlite*.c)

OBJS          := $(CPP_SRCS:%.cpp=$(OBJ_DIR)/%.o) \
                $(C_SRCS:%.c=$(OBJ_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
