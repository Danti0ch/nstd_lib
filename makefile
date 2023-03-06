BUILD_DIR = build
SRC_DIR   = src
INC_DIR   = inc

GRAPH = graph

all: main
	./main

main: $(BUILD_DIR)/main.o
	g++ $(BUILD_DIR)/main.o -o main

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp $(INC_DIR)/vector.hpp
	g++ -c -std=c++20 -I$(INC_DIR) $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

clear:
	rm $(BUILD_DIR)/*
