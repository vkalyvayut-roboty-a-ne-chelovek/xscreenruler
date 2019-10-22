all: main.c
	gcc -Wall -o xscreenruler main.c -lX11

.PHONY: clean
clean:
	rm -f xscreenruler
