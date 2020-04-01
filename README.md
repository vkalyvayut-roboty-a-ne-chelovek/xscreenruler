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

```
./xscreenruler [-h] [-fg <color>] [-bg <color>]

  -h - this message
  -fg <color> - set foreground (i.e. text) color (e.g. yellow, white).
  -bg <color> - set background color (e.g. black, red).

  Where <color> is either:
    An X11 color name (e.g. 'red').
    A hex colour code (e.g. '#ff0000')

Keybindings:
  Measurement edge:
    shift+n - north
    shift+s - south
    shift+w - west
    shift+e - east

  Measurement direction:
    shift+r - invert(default from left-to-right to right-to-left, from top-to-bottom to bottom-to-top)

  Ruler size:
    ctrl+d - default (150px) size
    ctrl+s - small (375px) size
    ctrl+m - medium (525px) size
    ctrl+t - tall (675px) size
    ctrl+f - fullscreen

  Movement
    arrows (up, down, left, right)
    ctrl+arrows - move faster

  Misc:
    ESC or ctrl+q - exit

```

## SCREENSHOTS

![xscreenruler.png](https://github.com/6d7367/xscreenruler/blob/master/xscreenruler.png)
![xscreenruler.png](https://raw.githubusercontent.com/6d7367/xscreenruler/master/xscreenruler%20-fg%20white%20-bg%20black.png)
![xscreenruler.png](https://raw.githubusercontent.com/6d7367/xscreenruler/master/xscreenruler%20-fg%20red%20-bg%20blue.png)
