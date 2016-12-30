all: main.c -lX11
	gcc -o xscreenruler main.c -lX11

clean:
	rm -f xscreenruler