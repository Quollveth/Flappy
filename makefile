build:
	gcc -Wall -std=c99 -g src/*.c -lSDL2 -o bin/main

run:
	./bin/main

clean:
	rm -f bin/main