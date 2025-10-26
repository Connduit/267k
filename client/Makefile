# === Compiler and flags ===
CXX := x86_64-w64-mingw32-g++
# Debug CXXFLAGS
CXXFLAGS := -std=c++17 -Wall -Wextra -O0 -g -Iinclude -I../common -I/home/connor/docs/lib/libcurl/include -I/usr/x86_64-w64-mingw32/include 
LIBS := -L/home/connor/docs/lib/libcurl/lib -lssl -lcrypto -lws2_32

# === Directory structure ===
SRC_DIR := src
TEST_DIR := test
OBJ_DIR := obj
BIN_DIR := bin

# === Source and object files ===
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
TEST_FILES := $(wildcard $(TEST_DIR)/*.cpp)

SRC_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/$(SRC_DIR)/%.o,$(SRC_FILES))
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/$(TEST_DIR)/%.o,$(TEST_FILES))

# === Executable targets ===
CLIENT_EXE := $(BIN_DIR)/client.exe
TEST_EXE := $(BIN_DIR)/test.exe

# === Default target ===
all: client test

# === Build client ===
client: $(CLIENT_EXE)

$(CLIENT_EXE): $(SRC_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_OBJS) -o $@ $(LIBS)

# === Build test ===
test: $(TEST_EXE)

# Exclude src/main.o so we don’t get multiple definitions of main()
$(TEST_EXE): $(TEST_OBJS) $(filter-out $(OBJ_DIR)/$(SRC_DIR)/main.o,$(SRC_OBJS)) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

# === Object file rules ===
$(OBJ_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)/$(SRC_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)/$(TEST_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Directory creation ===
$(OBJ_DIR)/$(SRC_DIR) $(OBJ_DIR)/$(TEST_DIR) $(BIN_DIR):
	mkdir -p $@

# === Cleanup ===
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean client test

