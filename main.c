#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

/***** defines *****/
typedef unsigned int DIRECTION;
typedef unsigned int SIZE;

#define DEFAULT_HEIGHT (SIZE)75
#define SIZE_SMALL (SIZE)(DEFAULT_HEIGHT * 5)
#define SIZE_MEDIUM (SIZE)(DEFAULT_HEIGHT * 7)
#define SIZE_TALL (SIZE)(DEFAULT_HEIGHT * 9)
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
void on_mouse_move(XEvent *);
void change_position(DIRECTION);
void change_size(SIZE);
void change_direction(DIRECTION);
void draw_bg();
void draw_mouse_position();

/***** global variables *****/

Display *display;
Window window;
int screen_num;
Screen *screen_ptr;
GC gc;
Font font;
XFontStruct *xfs;
XSizeHints *size_hints;
XWMHints *wm_hints;
XClassHint *class_hints;

SIZE SIZE_FULLSCREEN;

unsigned int CURRENT_DIRECTION = DEFAULT_DIRECTON;
unsigned int CURRENT_WIDTH, CURRENT_HEIGHT, CURRENT_POSITION_X, CURRENT_POSITION_Y, CURRENT_MOUSE_POSITION;


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

	font = XLoadFont(display, "*misc*fixed*");
	xfs = XQueryFont(display, font);
	if (! font) { exit(-1); }
	XSetFont(display, gc, font);
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
				on_mouse_move(&e);
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
		case XK_d:
			if (ControlMask != (e->xkey.state & ControlMask)) {
				break;
			}
			change_size(DEFAULT_WIDTH);
			break;
		case XK_s:
			if (ControlMask != (e->xkey.state & ControlMask)) {
				break;
			}
			change_size(SIZE_SMALL);
			break;
		/***** medium *****/
		case XK_m:
			if (ControlMask != (e->xkey.state & ControlMask)) {
				break;
			}
			change_size(SIZE_MEDIUM);
			break;
		/***** tall *****/
		case XK_t:
			if (ControlMask != (e->xkey.state & ControlMask)) {
				break;
			}
			change_size(SIZE_TALL);
			break;
		/***** fullscreen *****/
		case XK_f:
			if (ControlMask != (e->xkey.state & ControlMask)) {
				break;
			}
			change_size(SIZE_FULLSCREEN);
			break;



		/***** direction *****/
		case XK_N:
			change_direction(DIRECTION_N);
			break;
		case XK_S:
			change_direction(DIRECTION_S);
			break;
		case XK_W:
			change_direction(DIRECTION_W);
			break;
		case XK_E:
			change_direction(DIRECTION_E);
			break;



		/***** exit *****/
		case XK_q:
			if (ControlMask != (e->xkey.state & ControlMask)) {
				break;
			}
		case XK_Escape:
			XFreeGC(display, gc);
			XFreeFont(display, xfs);
			XCloseDisplay(display);
			exit(1);
			break;
		default:
			break;
	}
}

void on_mouse_move(XEvent *e) {
	switch (CURRENT_DIRECTION) {
		case DIRECTION_N:
		case DIRECTION_S:
			CURRENT_MOUSE_POSITION = e->xmotion.x;
			break;
		case DIRECTION_W:
		case DIRECTION_E:
			CURRENT_MOUSE_POSITION = e->xmotion.y;
			break;
	}
	draw_mouse_position();
}

void init_default_sizes_and_positions() {
	XWindowAttributes xwa;
	Window root = RootWindow(display, screen_num);
	if (XGetWindowAttributes(display, root, &xwa) != True) {
		exit(-1);
	}

	SIZE_FULLSCREEN = xwa.width;
	CURRENT_POSITION_X = DEFAULT_POSITION_X; CURRENT_POSITION_Y = DEFAULT_POSITION_Y;
	CURRENT_MOUSE_POSITION = 0;
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

	draw_bg();
}

void change_direction(DIRECTION direction) {
	CURRENT_DIRECTION = direction;
	get_default_sizes_for_direction(CURRENT_DIRECTION, &CURRENT_WIDTH, &CURRENT_HEIGHT);
	change_size(DEFAULT_WIDTH);
}

void draw_bg() {
	XClearWindow(display, window);
	unsigned int i;
	unsigned int line_height_default = 10;
	unsigned int line_height_medium = 20;
	unsigned int line_height_big = 30;

	SIZE FINISH = 0;
	switch (CURRENT_DIRECTION) {
		case DIRECTION_N:
		case DIRECTION_S:
			FINISH = CURRENT_WIDTH;
			break;
		case DIRECTION_W:
		case DIRECTION_E:
			FINISH = CURRENT_HEIGHT;
			break;
	}

	unsigned int line_height = line_height_default;
	for(i = 0; i < FINISH; i+=2) {
		if ((i % 10) == 0) {
			line_height = line_height_medium;
			if ((i % 20) == 0) {
				line_height = line_height_big;
			}
		} else {
			line_height = line_height_default;
		}
		
		switch(CURRENT_DIRECTION) {
			case DIRECTION_N:
				XDrawLine(display, window, gc, i-1 /***** magic :-) offset *****/, 0, i-1, line_height);
				break;
			case DIRECTION_S:
				XDrawLine(display, window, gc, i-1, DEFAULT_HEIGHT, i-1, DEFAULT_HEIGHT-line_height);
				break;
			case DIRECTION_W:
				XDrawLine(display, window, gc, 0, i-1, line_height, i-1);
				break;
			case DIRECTION_E:
				XDrawLine(display, window, gc, DEFAULT_HEIGHT, i-1, DEFAULT_HEIGHT-line_height, i-1);
				break;
		}
	}

	draw_mouse_position();
}

void draw_mouse_position() {
	unsigned int chars_in_number = 3;
	char *number_formated = (char *)malloc(sizeof(char)*chars_in_number);
	sprintf(number_formated, "%03d", CURRENT_MOUSE_POSITION);
	SIZE total_width = xfs->max_bounds.width * chars_in_number + xfs->max_bounds.descent;
	switch(CURRENT_DIRECTION) {
		case DIRECTION_N:
			XDrawImageString(display, window, gc, 
				xfs->max_bounds.descent * 2, CURRENT_HEIGHT - xfs->max_bounds.descent * 2, 
				number_formated, chars_in_number);
			break;
		case DIRECTION_S:
			XDrawImageString(display, window, gc, 
				xfs->max_bounds.descent * 2, xfs->max_bounds.ascent + xfs->max_bounds.descent, 
				number_formated, chars_in_number);
			break;
		case DIRECTION_W:
			XDrawImageString(display, window, gc, 
				CURRENT_WIDTH - total_width, xfs->max_bounds.ascent + xfs->max_bounds.descent, 
				number_formated, chars_in_number);
			break;
		case DIRECTION_E:
			XDrawImageString(display, window, gc, 
				xfs->max_bounds.descent, xfs->max_bounds.ascent + xfs->max_bounds.descent, 
				number_formated, chars_in_number);
			break;
	}

	free(number_formated);
}