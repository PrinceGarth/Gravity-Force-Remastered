# Gravity Force Remastered

Linux (x86_64) port of **Gravity Force v1.02**. Jens Hassler made the original in 2000 as a freeware DOS remake of the 1989 Amiga classic.

- The first commit is the original source and data, unchanged. Everything after it is port work.
- [CHANGELOG.md](CHANGELOG.md) lists what changed.
- [LEGAL.md](LEGAL.md) covers permissions and credits.

## Build and run

```
sudo apt install liballegro4-dev libxmp4 libxrandr-dev libx11-dev
make -f Makefile.linux
./gf
```

The game runs in a 640x480 window.

## Windows

Cross-built from Linux into one `gf.exe` (static Allegro 4.4.3.1 and libxmp, [llvm-mingw](https://github.com/mstorsjo/llvm-mingw)):

```
make -f Makefile.mingw CC=x86_64-w64-mingw32-clang WIN=<prefix holding the static libs>
```

Put `gf.exe` next to `dat/`. The game always runs in a window and never changes the monitor's resolution. `gf.log` next to the exe collects error messages.

## Emergency exit

Ctrl+Shift+Q, or the window's close button, ends the game at once. Settings changed since the game started are not saved.

## Controls

The keyboard controls are the original ones. Change them under Game Options > Controls.

Mouse control, in single-player games:
- the ship turns toward the cursor;
- right button thrusts;
- left button fires.

In the menus, click buttons and level numbers. To turn off mouse control in play, go to Game Options > Controls > Mouse.

## Documentation

The original manuals and notes are in `doc/`:

- `gfdoceng.txt`: the English manual;
- `source.txt`: the author's notes on the source code.
