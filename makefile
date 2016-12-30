all: main.c -lX11
	gcc -Wall -o xscreenruler main.c -lX11

clean:
	rm -f xscreenruler