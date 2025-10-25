# ================================
# # Compiler and Flags
# # ================================
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude -I../common

# ================================
# Directories
# ================================
SRC_DIR := src
TEST_DIR := test
OBJ_DIR := obj
BIN_DIR := bin

# ================================
# Source Files
# ================================
SRC_FILES := $(filter-out $(SRC_DIR)/main.cpp, $(wildcard $(SRC_DIR)/*.cpp))
TEST_FILES := $(wildcard $(TEST_DIR)/*.cpp)

SRC_OBJS := $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/src/%.o)
MAIN_OBJ := $(OBJ_DIR)/src/main.o
TEST_OBJS := $(TEST_FILES:$(TEST_DIR)/%.cpp=$(OBJ_DIR)/test/%.o)

# ================================
# Targets
# ================================
.PHONY: all clean client test

all: client test

client: $(BIN_DIR)/client.exe
test: $(BIN_DIR)/test.exe

# --- client.exe (includes main.cpp) ---
$(BIN_DIR)/client.exe: $(SRC_OBJS) $(MAIN_OBJ) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# --- test.exe (excludes main.cpp) ---
$(BIN_DIR)/test.exe: $(SRC_OBJS) $(TEST_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# ================================
# Compilation Rules
# ================================
$(OBJ_DIR)/src/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)/src
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/test/%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)/test
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ================================
# Directory Creation
# ================================
$(OBJ_DIR)/src $(OBJ_DIR)/test $(BIN_DIR):
	mkdir -p $@

# ================================
# Clean
# ================================
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

