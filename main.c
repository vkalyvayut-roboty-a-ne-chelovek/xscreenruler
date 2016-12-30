#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

#define DEFAULT_HEIGHT 50
#define DEFAULT_WIDTH 350

Display *display;
int screen_num;
Screen *screen_ptr;
GC gc;


int main(int argc, char *argv[]) {
	Window window;
	XEvent e;

	XSizeHints *size_hints;
	XWMHints *wm_hints;
	XClassHint *class_hints;

	size_hints = XAllocSizeHints();
	wm_hints = XAllocWMHints();
	class_hints = XAllocClassHint();

	if ((!size_hints) || (!wm_hints) || (!class_hints)) { exit(-1); }

	display = XOpenDisplay(NULL);
	if (display == NULL) { exit(-1); }

	screen_num = DefaultScreen(display);
	screen_ptr = DefaultScreenOfDisplay(display);

	unsigned int width, height, border_width;
	width = DEFAULT_WIDTH;
	height = DEFAULT_HEIGHT;
	border_width = 0;

	window = XCreateSimpleWindow(display, RootWindow(display, screen_num),
		0, 0, width, height, border_width, BlackPixel(display,
		screen_num), WhitePixel(display, screen_num));

	/***** GC *****/
	XGCValues gc_values;
	unsigned long gc_mask = GCBackground | GCForeground;
	gc_values.background = WhitePixel(display, screen_num);
	gc_values.foreground = BlackPixel(display, screen_num);
	gc = XCreateGC(display, window, gc_mask, &gc_values);
	/**************/

	size_hints->flags = PPosition | PSize | PMinSize | PMaxSize;
	size_hints->min_width = width;
	size_hints->min_height = height;

	size_hints->max_width = width;
	size_hints->max_height = height;

	wm_hints->initial_state = NormalState;
	wm_hints->input = True;
	wm_hints->flags = StateHint | InputHint;

	XSetWMProperties(display, window, NULL, NULL, argv, argc, size_hints, wm_hints, class_hints);
	XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | PointerMotionMask);

	XMapWindow(display, window);

	while(1) {
		XNextEvent(display, &e);
		switch(e.type) {
			case Expose:
				if (e.xexpose.count != 0) {
					break;
				}
				break;
			case KeyPress:
				XFreeGC(display, gc);
				XCloseDisplay(display);
				exit(1);
				break;
			case ConfigureNotify:
				break;
			case MotionNotify:
				break;
			default:
				break;
		}
	}

	exit(1);
}
