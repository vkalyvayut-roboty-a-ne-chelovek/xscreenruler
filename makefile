all: main.c
	${CC} ${CPPFLAGS} ${CFLAGS} ${LDFLAGS} -Wall \
		-o xscreenruler main.c -lX11

.PHONY: clean
clean:
	rm -f xscreenruler
