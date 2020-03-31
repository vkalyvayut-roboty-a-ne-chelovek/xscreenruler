#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

/***** defines *****/
typedef unsigned int DIRECTION;
typedef unsigned int SIZE;

#define DEFAULT_HEIGHT (SIZE)75
#define DEFAULT_WIDTH (SIZE)(DEFAULT_HEIGHT * 2)
#define SIZE_SMALL (SIZE)(DEFAULT_HEIGHT * 5)
#define SIZE_MEDIUM (SIZE)(DEFAULT_HEIGHT * 7)
#define SIZE_TALL (SIZE)(DEFAULT_HEIGHT * 9)



#define DIRECTION_N (DIRECTION)1
#define DIRECTION_S (DIRECTION)2
#define DIRECTION_W (DIRECTION)3
#define DIRECTION_E (DIRECTION)4

#define DEFAULT_DIRECTON DIRECTION_N

#define DEFAULT_POSITION_X (SIZE)50
#define DEFAULT_POSITION_Y (SIZE)50

#define MEASURE_FLOW_NORMAL 1
#define MEASURE_FLOW_INVERTED 2


/***** functions *****/

void init_default_sizes_and_positions();
void get_default_sizes_for_direction(DIRECTION, unsigned int *, unsigned int *);
void on_key_press(XEvent *);
void on_mouse_move(XEvent *);
void change_position(DIRECTION, XEvent *);
void change_size(SIZE);
void update_position_and_size();
void change_direction(DIRECTION);
void flip_flow();

void draw_bg();
void _draw_bg_measurement_number(unsigned int, unsigned int, SIZE, unsigned int, char *, unsigned int *, unsigned int *, unsigned int *, unsigned int *);

void draw_mouse_position();
void draw_mouse_position_marker();
void draw_bg_and_mouse_position();

void usage(int, char **);
void users_colors(int, char **);

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
SIZE CURRENT_WIDTH, CURRENT_HEIGHT, CURRENT_POSITION_X, CURRENT_POSITION_Y, CURRENT_MOUSE_POSITION, MOUSE_POSITION_PREV_X, MOUSE_POSITION_PREV_Y;

DIRECTION CURRENT_MEASURE_FLOW;

typedef struct {
    unsigned long   flags;
    unsigned long   functions;
    unsigned long   decorations;
} Hints; /*****  достаточно и 3 свойств *****/

unsigned long pixel_color_1, pixel_color_2;

int main(int argc, char *argv[]) {
	usage(argc, argv);
	
	XEvent e;

	size_hints = XAllocSizeHints();
	wm_hints = XAllocWMHints();
	class_hints = XAllocClassHint();

	if ((!size_hints) || (!wm_hints) || (!class_hints)) { exit(-1); }

	display = XOpenDisplay(NULL);
	if (display == NULL) {
		errx(1, "can't open display");
		exit(-1);
	}

	screen_num = DefaultScreen(display);
	screen_ptr = DefaultScreenOfDisplay(display);

	/***** init defaults *****/
	init_default_sizes_and_positions();
	get_default_sizes_for_direction(CURRENT_DIRECTION, &CURRENT_WIDTH, &CURRENT_HEIGHT);
	/******************************/

	pixel_color_1 = WhitePixel(display, screen_num);
	pixel_color_2 = BlackPixel(display, screen_num);

	users_colors(argc, argv);

	window = XCreateSimpleWindow(display, RootWindow(display, screen_num),
		0, 0, CURRENT_WIDTH, CURRENT_HEIGHT, 0, pixel_color_2, pixel_color_1);

	/***** window decorations *****/
	/***** based on http://tonyobryan.com/index.php?article=9 *****/
	Hints hints;
	Atom property;
	hints.flags = 2;
	hints.decorations = 0;
	property = XInternAtom(display, "_MOTIF_WM_HINTS", True);
	XChangeProperty(display, window, 
		property, property, 
		/* format */32, /* mode */PropModeReplace, 
		(unsigned char *)&hints, 3);

	/***** GC *****/
	XGCValues gc_values;
	unsigned long gc_mask = GCBackground | GCForeground;
	gc_values.background = pixel_color_1;
	gc_values.foreground = pixel_color_2;
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
	XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask | PointerMotionMask | Button1MotionMask);

	XMapWindow(display, window);

	while(1) {
		XNextEvent(display, &e);
		switch(e.type) {
			case Expose:
				if (e.xexpose.count != 0) {
					break;
				}
				draw_bg_and_mouse_position();
				break;
			case KeyPress:
				on_key_press(&e);
				break;
			case ConfigureNotify:
				break;
			case MotionNotify:
				if ((e.xmotion.state & Button1MotionMask) == Button1MotionMask ) {
					CURRENT_POSITION_X += (e.xmotion.x - MOUSE_POSITION_PREV_X);
					CURRENT_POSITION_Y += (e.xmotion.y - MOUSE_POSITION_PREV_Y);
					update_position_and_size();
					break;
				}
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
			change_position(DIRECTION_N, e);
			break;
		case XK_Down:
			change_position(DIRECTION_S, e);
			break;
		case XK_Left:
			change_position(DIRECTION_W, e);
			break;
		case XK_Right:
			change_position(DIRECTION_E, e);
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

		case XK_R:
			flip_flow();
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
	MOUSE_POSITION_PREV_X = e->xmotion.x;
	MOUSE_POSITION_PREV_Y = e->xmotion.y;
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
	draw_bg_and_mouse_position();
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
	CURRENT_MEASURE_FLOW = MEASURE_FLOW_INVERTED;
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

void update_position_and_size() {
	XMoveResizeWindow(
		display, window, 
		CURRENT_POSITION_X, CURRENT_POSITION_Y, 
		CURRENT_WIDTH, CURRENT_HEIGHT
	);
}

void change_position(DIRECTION direction, XEvent *e) {
	unsigned int change = 1;
	if (ControlMask == (e->xkey.state & ControlMask)) {
		change = 10;
	}

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

	update_position_and_size();
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

	update_position_and_size();

	draw_bg_and_mouse_position();
}

void change_direction(DIRECTION direction) {
	CURRENT_DIRECTION = direction;
	get_default_sizes_for_direction(CURRENT_DIRECTION, &CURRENT_WIDTH, &CURRENT_HEIGHT);
	change_size(DEFAULT_WIDTH);
}

void flip_flow() {
	switch (CURRENT_MEASURE_FLOW) {
		case MEASURE_FLOW_NORMAL:
			CURRENT_MEASURE_FLOW = MEASURE_FLOW_INVERTED;
			break;
		case MEASURE_FLOW_INVERTED:
			CURRENT_MEASURE_FLOW = MEASURE_FLOW_NORMAL;
			break;
	}

	draw_bg_and_mouse_position();
}

void draw_bg() {
	XClearWindow(display, window);
	unsigned int i, step_i, start_i, finish_i;
	unsigned int line_height_default = 10;
	unsigned int line_height_medium = 20;
	unsigned int line_height_big = 30;

	start_i = 0;
	step_i = 1;
	switch (CURRENT_DIRECTION) {
		case DIRECTION_N:
		case DIRECTION_S:
			finish_i = CURRENT_WIDTH + step_i;
			break;
		case DIRECTION_W:
		case DIRECTION_E:
			finish_i = CURRENT_HEIGHT + step_i;
			break;
	}

	unsigned int chars_in_number = 4;
	char *number_formated = (char *)malloc(sizeof(char)*chars_in_number);
	SIZE total_width = xfs->max_bounds.width * chars_in_number;

	unsigned int x1, x2, y1, y2;
	unsigned int line_height = line_height_default;
	for(i = start_i; i < finish_i; i+=step_i) {
		if ((i % 10) == 0) {
			line_height = line_height_medium;
			if ((i % 50) == 0) {
				line_height = line_height_big;
			}
		} else {
			line_height = line_height_default;
		}

		_draw_bg_measurement_number(i, chars_in_number, total_width, line_height, number_formated, &x1, &y1, &x2, &y2);
		
		if (i % 2 == 0) {
			XDrawLine(display, window, gc, x1, y1, x2, y2);
		}
	}

	free(number_formated);
}

void _draw_bg_measurement_number(unsigned int i, unsigned int chars_in_number, SIZE total_width, unsigned int line_height, char *number_formated, unsigned int *x1, unsigned int *y1, unsigned int *x2, unsigned int *y2) {
	sprintf(number_formated, "%04d", i);
	unsigned int number_formated_position_x, number_formated_position_y;
	switch(CURRENT_DIRECTION) {
		case DIRECTION_N:
			*x1 = i-1; *y1 = 0; *x2 = i-1; *y2 = line_height;
			number_formated_position_x = i - total_width / 2;
			number_formated_position_y = line_height + xfs->max_bounds.ascent * 2;
			break;
		case DIRECTION_S:
			*x1 = i-1; *y1 = DEFAULT_HEIGHT; *x2 = i-1; *y2 = DEFAULT_HEIGHT-line_height;
			number_formated_position_x = i - total_width / 2;
			number_formated_position_y = line_height + xfs->max_bounds.ascent;
			break;
		case DIRECTION_W:
			*x1 = 0; *y1 = i-2; *x2 = line_height; *y2 = i-2;
			number_formated_position_x = DEFAULT_HEIGHT - total_width;
			number_formated_position_y = i + xfs->max_bounds.descent + 1;
			break;
		case DIRECTION_E:
			*x1 = DEFAULT_HEIGHT; *y1 = i-2; *x2 = DEFAULT_HEIGHT-line_height; *y2 = i-2;
			number_formated_position_x = xfs->max_bounds.width;
			number_formated_position_y = i + xfs->max_bounds.descent + 1;
			break;
	}

	if ((i != 0) && ((i % 50) == 0)) {
		XDrawImageString(display, window, gc, 
			number_formated_position_x, number_formated_position_y, 
			number_formated, chars_in_number);
	}
}

void draw_mouse_position() {
	unsigned int chars_in_number = 4;

	SIZE mouse_position_for_print = CURRENT_MOUSE_POSITION;
	char *number_formated = (char *)malloc(sizeof(char)*chars_in_number);

	SIZE total_width = xfs->max_bounds.width * chars_in_number + xfs->max_bounds.descent;

	unsigned int mouse_position_for_print_x, mouse_position_for_print_y;
	switch(CURRENT_DIRECTION) {
		case DIRECTION_N:
			mouse_position_for_print_x = xfs->max_bounds.descent * 2;
			mouse_position_for_print_y = CURRENT_HEIGHT - xfs->max_bounds.descent * 2;
			break;
		case DIRECTION_S:
			mouse_position_for_print_x = xfs->max_bounds.descent * 2;
			mouse_position_for_print_y = xfs->max_bounds.ascent + xfs->max_bounds.descent;
			break;
		case DIRECTION_W:
			mouse_position_for_print_x = CURRENT_WIDTH - total_width;
			mouse_position_for_print_y = xfs->max_bounds.ascent + xfs->max_bounds.descent;
			break;
		case DIRECTION_E:
			mouse_position_for_print_x = xfs->max_bounds.descent;
			mouse_position_for_print_y = xfs->max_bounds.ascent + xfs->max_bounds.descent;
			break;
	}

	switch (CURRENT_MEASURE_FLOW) {
		case MEASURE_FLOW_NORMAL:
			break;
		case MEASURE_FLOW_INVERTED:
			switch (CURRENT_DIRECTION) {
				case DIRECTION_N:
				case DIRECTION_S:
					mouse_position_for_print = CURRENT_WIDTH - mouse_position_for_print;

					mouse_position_for_print_x = CURRENT_WIDTH - total_width;
					break;
				case DIRECTION_W:
				case DIRECTION_E:
					mouse_position_for_print = CURRENT_HEIGHT - mouse_position_for_print;

					mouse_position_for_print_y = CURRENT_HEIGHT;
					break;
			}
			break;
		default:
			break;
	}

	sprintf(number_formated, "%04d", mouse_position_for_print);

	XDrawImageString(display, window, gc, 
		mouse_position_for_print_x, mouse_position_for_print_y, 
		number_formated, chars_in_number);

	free(number_formated);
}

void draw_mouse_position_marker() {
	SIZE mouse_marker_position_x1,
		mouse_marker_position_y1,
		mouse_marker_position_x2,
		mouse_marker_position_y2,
		marker_size,
		mouse_position_for_size_marker;

	marker_size = DEFAULT_POSITION_X * 2;

	mouse_position_for_size_marker = CURRENT_MOUSE_POSITION;

	switch(CURRENT_DIRECTION) {
		case DIRECTION_N:
		case DIRECTION_S:
			if (mouse_position_for_size_marker > 0) {
				mouse_position_for_size_marker -= 1;
			}

			mouse_marker_position_x1 = mouse_position_for_size_marker;
			mouse_marker_position_y1 = 0;
			mouse_marker_position_x2 = mouse_position_for_size_marker;
			mouse_marker_position_y2 = marker_size;

			break;
		case DIRECTION_W:
		case DIRECTION_E:
			if (mouse_position_for_size_marker > 0) {
				mouse_position_for_size_marker -= 2;
			}

			mouse_marker_position_x1 = 0;
			mouse_marker_position_y1 = mouse_position_for_size_marker;
			mouse_marker_position_x2 = marker_size;
			mouse_marker_position_y2 = mouse_position_for_size_marker;

			break;
	}

	XDrawLine(display, window, gc, 
				mouse_marker_position_x1, mouse_marker_position_y1, 
				mouse_marker_position_x2, mouse_marker_position_y2);
}

void draw_bg_and_mouse_position() {
	draw_bg();
	draw_mouse_position();
	draw_mouse_position_marker();
}

void usage(int argc, char **argv) {
	unsigned int argcounter;
	unsigned int show_usage = False;
	for(argcounter = 0; argcounter < argc; argcounter++) {
		if (strcmp(argv[argcounter], "-h") == 0) {
			show_usage = True;
		}
	}

	if (show_usage != True) { return; }
	char *usage = \
	    "%s [-h] [-fg <color>] [-bg <color>]\n"
	    "\n"
	    "  -h - this message\n"
	    "  -fg <color> - set foreground (i.e. text) color.\n"
	    "  -bg <color> - set background color.\n"
	    "\n"
            "  Where <color> is either:\n"
	    "    An X11 color name (e.g. 'red').\n"
	    "    A hex colour code (e.g. '#ff0000')\n"
	    "\n"
	    "Keybindings:\n"
	    "  Measurement edge:\n"
	    "    shift+n - north\n"
	    "    shift+s - south\n"
	    "    shift+w - west\n"
	    "    shift+e - east\n"
	    "\n"
	    "  Ruler size:\n"
	    "    ctrl+d - default (%dpx) size\n"
	    "    ctrl+s - small (%dpx) size\n"
	    "    ctrl+m - medium (%dpx) size\n"
	    "    ctrl+t - tall (%dpx) size\n"
	    "    ctrl+f - fullscreen\n"
	    "\n"
	    "  Movement\n"
	    "    arrows (up, down, left, right)\n"
	    "    ctrl+arrows - move faster\n"
	    "\n"
	    "  Misc:\n"
	    "    ESC or ctrl+q - exit\n\n";

	fprintf(stdout, usage, argv[0], DEFAULT_WIDTH, SIZE_SMALL, SIZE_MEDIUM, SIZE_TALL);
	exit(1);
}

void users_colors(int argc, char **argv) {
	unsigned int argcounter;
	for(argcounter = 0; argcounter < argc; argcounter++) {
		if (strcmp(argv[argcounter], "-fg") == 0) {
			if (argc >= (argcounter+2)) {
				XColor fg;
				if (!XParseColor(display, DefaultColormap(display, screen_num), argv[argcounter+1], &fg)) {
					fprintf(stderr, "Error: XParseColor(%s) Back on default\n", argv[argcounter+1]);
					return;
				}
				if (!XAllocColor(display, DefaultColormap(display, screen_num), &fg)) {
					fprintf(stderr, "Error: XQueryColor(%s). Back on default\n", argv[argcounter+1]);
					return;
				}
				pixel_color_2 = fg.pixel;
			}
		}

		if (strcmp(argv[argcounter], "-bg") == 0) {
			if (argc >= (argcounter+2)) {
				XColor bg;
				if (XParseColor(display, DefaultColormap(display, screen_num), argv[argcounter+1], &bg) != True) {
					fprintf(stderr, "Error: XParseColor(%s) Back on default\n", argv[argcounter+1]);
					return;
				}
				if (XAllocColor(display, DefaultColormap(display, screen_num), &bg) != True) {
					fprintf(stderr, "Error: XQueryColor(%s) Back on default\n", argv[argcounter+1]);
					return;
				}
				pixel_color_1 = bg.pixel;
			}
		}
	}
}
