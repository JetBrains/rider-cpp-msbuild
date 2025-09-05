CXX ?= clang++
CXXFLAGS ?= -std=c++17 -O0 -g
OUT_DIR := out
BIN := $(OUT_DIR)/hellognumake

all: $(BIN)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(BIN): main.cpp | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -o $@ main.cpp

clean:
	rm -rf $(OUT_DIR)
