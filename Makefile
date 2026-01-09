TARGET        := main
TEST_TARGET   := test_runner  

SRC_DIR       := src
LIB_DIR       := lib
EXTERNAL_DIR  := external
OBJ_DIR       := obj
TEST_DIR      := tests

CXX           := g++
CC            := gcc

CXXFLAGS      := -Wall -Wextra -Wpedantic -Wshadow -Wconversion -std=c++17 \
                -I$(EXTERNAL_DIR)/headers -I$(LIB_DIR)/headers -I$(TEST_DIR)

CFLAGS        := -Wall -Wextra \
                -I$(EXTERNAL_DIR)/headers -I$(LIB_DIR)/headers

LDFLAGS       := -lstdc++

CPP_SRCS      := $(wildcard $(SRC_DIR)/*.cpp) \
                $(wildcard $(LIB_DIR)/*.cpp)

C_SRCS        := $(wildcard $(EXTERNAL_DIR)/sqlite*.c)

TEST_SRCS     := $(wildcard $(TEST_DIR)/*.cpp)

OBJS          := $(CPP_SRCS:%.cpp=$(OBJ_DIR)/%.o) \
                $(C_SRCS:%.c=$(OBJ_DIR)/%.o)

TEST_OBJS     := $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/$(TEST_DIR)/%.o,$(TEST_SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

test: $(OBJS) $(TEST_OBJS)
	$(CXX) $(filter-out $(OBJ_DIR)/$(SRC_DIR)/main.o, $(OBJS)) $(TEST_OBJS) -o $(TEST_TARGET) $(LDFLAGS)
	./$(TEST_TARGET)

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET) 

.PHONY: all clean test