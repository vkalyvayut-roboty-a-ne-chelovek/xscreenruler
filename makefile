xscreenruler: main.c
	${CC} ${CPPFLAGS} ${CFLAGS} ${LDFLAGS} -Wall -o $@ main.c -lX11

.PHONY: clean
clean:
	rm -f xscreenruler
