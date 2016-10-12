install: src/main.c src/constants.h src/timer.h src/util.h
	gcc src/main.c src/util.c -lpthread -Wall -o huffman.out

clean:
	rm *.out
	rm *.csv

test: huffman.out
	make install
	./huffman.out dados/ricardo_reis.txt out.csv