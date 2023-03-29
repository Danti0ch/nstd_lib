BUILD_DIR = build
SRC_DIR   = src
INC_DIR   = inc

GRAPH = graph

all: main
	make clear
	./main

main: $(BUILD_DIR)/main.o
	g++ $(BUILD_DIR)/main.o -o main

function_test: $(BUILD_DIR)/function_test.o
	g++ $(BUILD_DIR)/function_test.o -o main

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp $(INC_DIR)/vector.hpp
	g++ -c -std=c++20 -I$(INC_DIR) $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/function_test.o: $(SRC_DIR)/function_test.cpp $(INC_DIR)/function.hpp
	g++ -c -std=c++20 -I$(INC_DIR) $(SRC_DIR)/function_test.cpp -o $(BUILD_DIR)/function_test.o

clear:
	rm $(BUILD_DIR)/*
