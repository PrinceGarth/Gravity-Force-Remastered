# Gravity Force Remastered

Linux (x86_64) port of **Gravity Force v1.02**. Jens Hassler made the original in 2000 as a freeware DOS remake of the 1989 Amiga classic.

The first commit is the original source and data, unchanged. Everything after it is port work. See [LEGAL.md](LEGAL.md).

## Build and run

```
sudo apt install liballegro4-dev libxmp4 libxrandr-dev libx11-dev
make -f Makefile.linux
./gf
```

## Changes from the original

- Builds for 64-bit Linux with Allegro 4.4.
- Menu music plays through libxmp, which replaces jgmod.
- Runs in a 640x480 window, and never changes the monitor's resolution.
- Mouse control:
  - the ship points at the cursor;
  - right button thrusts;
  - left button fires;
  - you can click menu entries.
  - Turn it off with `GF_MOUSE=0 ./gf`.
- Holding an arrow key in the menus moves one step, then repeats.
- Bug fixes:
  - enemy paths broken by a name clash with glibc;
  - bomb craters that stayed solid;
  - language switching.
- Experimental: set `GF_SCALE=1` for a scaled borderless fullscreen window. The screen can go blank after a keypress.

## Documentation

The original manuals and notes are in `doc/`:

- `gfdoceng.txt`: the English manual;
- `source.txt`: the author's notes on the source code.
