# xscreenruler
Simple screen ruler for x11. Only xlib in dependencies!

## BUILDING

To build, use `make`. GNU make and BSD make have been tested.

The build honours the standard variables. On some platforms you will have to
pass extra include and library directories. For example on OpenBSD, you would
do:

```
CPPFLAGS=-I/usr/X11R6/include LDFLAGS=-L/usr/X11R6/lib make
```

## USAGE

Invoke `xscreenruler` like so:
```
xscreenruler [-h|-fg <color-name-or-hex-code>|-bg <color-name-or-hex-code>]
-h - this message
-fg <color-name-or-hex-code> - set foreground (i.e. text) color. Examples: red, green, blue, yellow, etc.
-bg <color-name-or-hex-code> - set background color. Same format as for foreground
```

## KEY BINDINGS

To change the measurement edge:
```
shift+n - north
shift+s - south
shift+w - west
shift+e - east
```

To change the size:
```
ctrl+d - default size
ctrl+s - small size
ctrl+m - medium size
ctrl+t - tall size
ctrl+f - fullscreen
```

To move the ruler, either use your window manager, or:
```
arrows (up, down, left, right)
ctrl+arrows - move faster
```

To quit, press `ESC` or `ctrl+q`.

## SCREENSHOTS

![xscreenruler.png](https://github.com/6d7367/xscreenruler/blob/master/xscreenruler.png)
![xscreenruler.png](https://raw.githubusercontent.com/6d7367/xscreenruler/master/xscreenruler%20-fg%20white%20-bg%20black.png)
![xscreenruler.png](https://raw.githubusercontent.com/6d7367/xscreenruler/master/xscreenruler%20-fg%20red%20-bg%20blue.png)
