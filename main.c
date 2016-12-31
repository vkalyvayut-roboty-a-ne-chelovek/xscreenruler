#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

/***** defines *****/
typedef unsigned int DIRECTION;

#define DEFAULT_HEIGHT 50
#define DEFAULT_WIDTH 350

#define DIRECTION_N (DIRECTION)1
#define DIRECTION_S (DIRECTION)2
#define DIRECTION_W (DIRECTION)3
#define DIRECTION_E (DIRECTION)4

#define DEFAULT_DIRECTON DIRECTION_N


/***** functions *****/

void init_directions_sizes();
void get_default_sizes_for_direction(DIRECTION, unsigned int *, unsigned int *);
void on_key_press(XEvent *);


/***** global variables *****/

Display *display;
int screen_num;
Screen *screen_ptr;
GC gc;

unsigned int HEIGHT_MAX_N, WIDTH_MAX_N,
	HEIGHT_MAX_S, WIDTH_MAX_S,
	HEIGHT_MAX_W, WIDTH_MAX_W,
	HEIGHT_MAX_E, WIDTH_MAX_E;

unsigned int CURRENT_DIRECTION = DEFAULT_DIRECTON;
unsigned int CURRENT_WIDTH, CURRENT_HEIGHT;


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

	/***** init defaults *****/
	init_directions_sizes();
	get_default_sizes_for_direction(CURRENT_DIRECTION, &CURRENT_WIDTH, &CURRENT_HEIGHT);
	/******************************/

	window = XCreateSimpleWindow(display, RootWindow(display, screen_num),
		0, 0, CURRENT_WIDTH, CURRENT_HEIGHT, 0, BlackPixel(display,
		screen_num), WhitePixel(display, screen_num));

	/***** GC *****/
	XGCValues gc_values;
	unsigned long gc_mask = GCBackground | GCForeground;
	gc_values.background = WhitePixel(display, screen_num);
	gc_values.foreground = BlackPixel(display, screen_num);
	gc = XCreateGC(display, window, gc_mask, &gc_values);
	/**************/

	size_hints->flags = PPosition | PSize | PMinSize | PMaxSize;
	size_hints->min_width = CURRENT_WIDTH;
	size_hints->min_height = CURRENT_HEIGHT;

	size_hints->max_width = CURRENT_WIDTH;
	size_hints->max_height = CURRENT_HEIGHT;

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
				on_key_press(&e);
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

void init_directions_sizes() {
	XWindowAttributes xwa;
	Window root = RootWindow(display, screen_num);
	if (XGetWindowAttributes(display, root, &xwa) != True) {
		exit(-1);
	}

	HEIGHT_MAX_N = DEFAULT_HEIGHT; WIDTH_MAX_N = xwa.width;
	HEIGHT_MAX_S = DEFAULT_HEIGHT; WIDTH_MAX_S = xwa.width;
	HEIGHT_MAX_W = xwa.width; WIDTH_MAX_W = DEFAULT_HEIGHT;
	HEIGHT_MAX_E = xwa.width; WIDTH_MAX_E = DEFAULT_HEIGHT;
}

void get_default_sizes_for_direction(DIRECTION direction, unsigned int *width, unsigned int *height) {
	switch (direction) {
		case DIRECTION_N:
			*width = DEFAULT_WIDTH;
			*height = DEFAULT_HEIGHT;
			break;
		case DIRECTION_S:
			*width = DEFAULT_WIDTH;
			*height = DEFAULT_HEIGHT;
			break;
		case DIRECTION_W:
			*width = DEFAULT_HEIGHT;
			*height = DEFAULT_WIDTH;
			break;
		case DIRECTION_E:
			*width = DEFAULT_HEIGHT;
			*height = DEFAULT_WIDTH;
			break;
		default:
			break;
	}
}

void on_key_press(XEvent *e) {
	KeySym ks;
	XComposeStatus xcs;
	XLookupString(&(e->xkey), NULL, 0, &ks, &xcs);
	switch(ks) {
		/***** move *****/
		case XK_Up:
			break;
		case XK_Down:
			break;
		case XK_Left:
			break;
		case XK_Right:
			break;
		/***** size ****/
		/***** short *****/
		case XK_s:
		case XK_S:
			break;
		/***** medium *****/
		case XK_m:
		case XK_M:
			break;
		/***** tall *****/
		case XK_t:
		case XK_T:
			break;
		/***** fullscreen *****/
		case XK_f:
		case XK_F:
			break;
		/***** exit *****/
		case XK_Escape:
			XFreeGC(display, gc);
			XCloseDisplay(display);
			exit(1);
			break;
		default:
			break;
	}
}