FP_DIR = ./FP-growth
GROWTH_OBJ = $(FP_DIR)/fptree.o
INCLUDES = -I ./dclib-code/dlib/all/source.cpp
FLAGS = -lpthread -std=c++11
IO_OBJ = utils/io.o
OBJ = $(GROWTH_OBJ) $(IO_OBJ)

all: main

main: fp_growth_obj io_obj
	g++ main.cpp $(OBJ) $(INCLUDES) $(FLAGS)  -o main
	./main

fp_growth_obj:
	g++ -g -c $(FP_DIR)/fptree.cpp $(FLAGS) -o $(GROWTH_OBJ)

io_obj: utils/io.cpp utils/io.hpp
	g++ -g -c utils/io.cpp $(FLAGS) -o $(IO_OBJ)