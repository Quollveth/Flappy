build:
	gcc -Wall -std=c99 -g src/*.c -lSDL2 -lSDL2_ttf -o bin/main

assets:
	cp -r assets bin/

run:
	./bin/main

clean:
	rm -f bin/main