all: fp_growth
	g++ main.cpp ./FP-growth/fptree.o -std=c++11 -o main

fp_growth:
	g++ -g -c FP-growth/fptree.cpp -std=c++11 -o FP-growth/fptree.o
