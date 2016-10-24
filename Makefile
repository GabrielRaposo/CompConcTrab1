install: src/main.c src/constants.h src/timer.h src/util.h
	gcc src/main.c src/util.c -lpthread -Wall -o huffman.out

clean:
	rm *.out
	rm *.csv

test: src/main.c src/constants.h src/timer.h src/util.h
	make install
	./huffman.out dados/ricardo_reis.txt out.csv
	cat out.csv

teste-concorrente: src/main.c src/constants.h src/timer.h src/util.h
	make test
	cp out.csv out2.csv
	./huffman.out dados/ricardo_reis.txt out.csv $(t)
	diff out.csv out2.csv
