PROG_NAME=huffman
BUILD_DIR=build
SRC_DIR=src

CXXFLAGS=-Wall -Wextra -std=c++17 -pedantic

ifdef $(DEBUG)
CXXFLAGS+= -ggdb
endif

$(PROG_NAME): $(SRC_DIR)/main.cc
	$(CXX) $(CXXCLAGS) -o $(BUILD_DIR)/$@ $^

init:
	@if [ ! -d $(BUILD_DIR) ]; then \
		mkdir $(BUILD_DIR); \
	fi