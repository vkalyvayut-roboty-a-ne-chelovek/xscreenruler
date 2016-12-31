#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

/***** defines *****/
typedef unsigned int DIRECTION;
typedef unsigned int SIZE;

#define SIZE_SMALL (SIZE)250
#define SIZE_MEDIUM (SIZE)350
#define SIZE_TALL (SIZE)450
#define DEFAULT_HEIGHT (SIZE)50
#define DEFAULT_WIDTH SIZE_SMALL



#define DIRECTION_N (DIRECTION)1
#define DIRECTION_S (DIRECTION)2
#define DIRECTION_W (DIRECTION)3
#define DIRECTION_E (DIRECTION)4

#define DEFAULT_DIRECTON DIRECTION_N

#define DEFAULT_POSITION_X (SIZE)50
#define DEFAULT_POSITION_Y (SIZE)50

/***** functions *****/

void init_default_sizes_and_positions();
void get_default_sizes_for_direction(DIRECTION, unsigned int *, unsigned int *);
void on_key_press(XEvent *);
void change_position(DIRECTION);
void change_size(SIZE);
void change_direction(DIRECTION);
void draw_bg();

/***** global variables *****/

Display *display;
Window window;
int screen_num;
Screen *screen_ptr;
GC gc;
XSizeHints *size_hints;
XWMHints *wm_hints;
XClassHint *class_hints;

SIZE SIZE_FULLSCREEN;

unsigned int CURRENT_DIRECTION = DEFAULT_DIRECTON;
unsigned int CURRENT_WIDTH, CURRENT_HEIGHT, CURRENT_POSITION_X, CURRENT_POSITION_Y;


int main(int argc, char *argv[]) {
	XEvent e;

	size_hints = XAllocSizeHints();
	wm_hints = XAllocWMHints();
	class_hints = XAllocClassHint();

	if ((!size_hints) || (!wm_hints) || (!class_hints)) { exit(-1); }

	display = XOpenDisplay(NULL);
	if (display == NULL) { exit(-1); }

	screen_num = DefaultScreen(display);
	screen_ptr = DefaultScreenOfDisplay(display);

	/***** init defaults *****/
	init_default_sizes_and_positions();
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
				draw_bg();
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

void on_key_press(XEvent *e) {
	KeySym ks;
	XComposeStatus xcs;
	XLookupString(&(e->xkey), NULL, 0, &ks, &xcs);
	switch(ks) {
		/***** move *****/
		case XK_Up:
			change_position(DIRECTION_N);
			break;
		case XK_Down:
			change_position(DIRECTION_S);
			break;
		case XK_Left:
			change_position(DIRECTION_W);
			break;
		case XK_Right:
			change_position(DIRECTION_E);
			break;



		/***** size ****/
		/***** default *****/
		case XK_0:
			change_size(DEFAULT_WIDTH);
			break;
		case XK_1:
			change_size(SIZE_SMALL);
			break;
		/***** medium *****/
		case XK_2:
			change_size(SIZE_MEDIUM);
			break;
		/***** tall *****/
		case XK_3:
			change_size(SIZE_TALL);
			break;
		/***** fullscreen *****/
		case XK_4:
			change_size(SIZE_FULLSCREEN);
			break;



		/***** direction *****/
		case XK_n:
			change_direction(DIRECTION_N);
			break;
		case XK_s:
			change_direction(DIRECTION_S);
			break;
		case XK_w:
			change_direction(DIRECTION_W);
			break;
		case XK_e:
			change_direction(DIRECTION_E);
			break;



		/***** exit *****/
		case XK_q:
			if (ControlMask != (e->xkey.state & ControlMask)) {
				break;
			}
		case XK_Escape:
			XFreeGC(display, gc);
			XCloseDisplay(display);
			exit(1);
			break;
		default:
			break;
	}
}

void init_default_sizes_and_positions() {
	XWindowAttributes xwa;
	Window root = RootWindow(display, screen_num);
	if (XGetWindowAttributes(display, root, &xwa) != True) {
		exit(-1);
	}

	SIZE_FULLSCREEN = xwa.width;
	CURRENT_POSITION_X = DEFAULT_POSITION_X; CURRENT_POSITION_Y = DEFAULT_POSITION_Y;
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

void change_position(DIRECTION direction) {
	unsigned int change = 1;
	switch(direction) {
		case DIRECTION_N:
			CURRENT_POSITION_Y -= change;
			break;
		case DIRECTION_S:
			CURRENT_POSITION_Y += change;
			break;
		case DIRECTION_W:
			CURRENT_POSITION_X -= change;
			break;
		case DIRECTION_E:
			CURRENT_POSITION_X += change;
			break;
		default:
			break;
	}
	XMoveResizeWindow(
		display, window, 
		CURRENT_POSITION_X, CURRENT_POSITION_Y, 
		CURRENT_WIDTH, CURRENT_HEIGHT
	);
}

void change_size(SIZE size) {
	switch(CURRENT_DIRECTION) {
		case DIRECTION_N:
			CURRENT_WIDTH = size;
			break;
		case DIRECTION_S:
			CURRENT_WIDTH = size;
			break;
		case DIRECTION_W:
			CURRENT_HEIGHT = size;
			break;
		case DIRECTION_E:
			CURRENT_HEIGHT = size;
			break;
		default:
			break;
	}

	size_hints->min_width = CURRENT_WIDTH;
	size_hints->min_height = CURRENT_HEIGHT;

	size_hints->max_width = CURRENT_WIDTH;
	size_hints->max_height = CURRENT_HEIGHT;
	XSetWMProperties(display, window, NULL, NULL, NULL, 0, size_hints, wm_hints, class_hints);

	XMoveResizeWindow(
		display, window, 
		CURRENT_POSITION_X, CURRENT_POSITION_Y, 
		CURRENT_WIDTH, CURRENT_HEIGHT
	);
}

void change_direction(DIRECTION direction) {
	CURRENT_DIRECTION = direction;
	get_default_sizes_for_direction(CURRENT_DIRECTION, &CURRENT_WIDTH, &CURRENT_HEIGHT);
	change_size(DEFAULT_WIDTH);
}

void draw_bg() {
	unsigned int i;
	unsigned int line_height_default = 10;
	unsigned int line_height_medium = 20;
	unsigned int line_height_big = 30;


	unsigned int line_height = line_height_default;
	for(i = 0; i < CURRENT_WIDTH; i+=2) {
		if ((i % 10) == 0) {
			line_height = line_height_medium;
			if ((i % 20) == 0) {
				line_height = line_height_big;
			}
		} else {
			line_height = line_height_default;
		}
		XDrawLine(display, window, gc, i, 50, i, 50-line_height);
	}
}