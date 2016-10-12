install: main.c timer.h util.h
	gcc util.c main.c -lpthread -ggdb3 -Wall -o huffman.out