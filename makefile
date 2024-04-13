build:
	gcc -Wall -std=c99 src/*.c -lSDL2 -o bin/main

run:
	./bin/main

clean:
	rm -f main