all: main.c -lX11
	gcc -Wall -o xscreenruler main.c -lX11

.PHONY: clean
clean:
	rm -f xscreenruler