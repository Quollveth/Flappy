build:
	gcc -Wall -std=c99 -g src/*.c -lSDL2 -lSDL2_ttf -o bin/main

run:
	./bin/main

clean:
	rm -f bin/main