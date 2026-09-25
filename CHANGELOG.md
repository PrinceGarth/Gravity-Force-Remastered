# Changelog

This file has two parts:

1. **Gravity Force Remastered**: changes made in this repository (the Linux port).
2. **Original Gravity Force**: Jens Hassler's version history from 1999–2000, copied word for word from `doc/gfdoceng.txt`.

---

# Part 1: Gravity Force Remastered

## 0.1.0 (2026-09-25)

### Added
- Mouse control in single-player games:
  - the ship turns toward the cursor, at the same speed as keyboard turning;
  - right button thrusts;
  - left button fires;
  - after launching from a pad, the ship stays upright for 500 ms before it starts aiming.
- A "Mouse" on/off setting under Game Options > Controls, saved in `gf.cfg`.
- Mouse support in the menus:
  - hovering selects a button;
  - clicking presses it;
  - clicking a level number selects that level on the Missions and Training screens.
  - clicking an option's left or right half steps it down or up, like the arrow keys;
  - the Pixel amount and Sound Volume bars can be clicked or dragged.
- Menu music, played through libxmp.
- Windows build (`make -f Makefile.mingw`): a single 32-bit `gf.exe`, tested in Wine 11.18, always windowed. Messages go to `gf.log`.
- Emergency exit: Ctrl+Shift+Q or the window's close button ends the game at once.
- Experimental scaled borderless fullscreen with `GF_SCALE=1`. Known bug: the screen can go blank after a keypress.

### Changed
- Builds as a 64-bit Linux program using Allegro 4.4 (`make -f Makefile.linux`).
- Runs in a 640x480 window by default. The monitor's resolution is never changed.
- Menu arrow keys move one step per press. Holding a key repeats after 500 ms, then every 100 ms.
- The game finds its data files when started from any directory.
- Sound start-up errors print to the terminal instead of blocking the game with a dialog box.
- Data file names are lowercase.

### Fixed
- Enemies following a path, such as the orange orb in the test level, jumped back and forth. The cause was a name clash with glibc's `fmul`/`fsqrt`.
- Craters from the big bomb acted as solid ground.
- The in-game toggle keys (P panel, F debug readout, R radar zoom, and the cheat keys) flipped every frame while held, so a press landed on a random state. They now act once per press.
- Keys held from one screen triggered the next:
  - Enter on the high score name skipped the level statistics;
  - releasing Pause re-paused the game;
  - holding Esc in a menu could back out through several menus and quit;
  - holding Enter repeated menu button presses.
- High score Page Up/Page Down scrolled the whole list in one press; F12 saved a screenshot every frame while held.
- Typewriter messages at the start of a level showed as one long line, with the beginning hidden and `^` in place of line breaks. The language files mark line breaks with byte 0xF8, which the code didn't recognise.
- Language switching:
  - text from the previous language stayed on screen;
  - an unrecognised entry in a language file overwrote the text of the entry before it.

---

# Part 2: Original Gravity Force (Jens Hassler)

Unchanged from the v1.02 manual. Key: `+` = added, `*` = fixed, `!` = note.

```
v1.02 (24/09/00)
 + Full translation to Slovak language (by Juraj Michalek)

v1.01 (02/09/00)
 * Bugfix: "Hit percentage" error mainly in Level 10
 + Shift+ESC leaves menu immediately

v1.0 (26/08/00)
 ! First official version after 0.9b
 + some bugfixes, double keys no more possible
 + All points below beginning with v0.9c

v0.9j (18/08/00)
 + All files from the /map and /lng directories now in two data files
 + Text messages now disppaear when you start from a base
 + Menu: ESC now equivalent to "Back" button
 * Bugfixes

v0.9i (14/08/00)
 + Dogfight: Selections for weapon and extras amount
 + Race: Start & End now identical
 + Third race map
 * Some bugfixes

v0.9h (09/08/00)
 ! first pre1.0 version for betatesters
 + slightly more intelligent rockets
 + slightly more intelligent probes
 + compiled a Windows version

v0.9g (08/08/00)
 + New single player level: "Pacifism"
 + Fifth dogfight map ("Black Hole")

v0.9f (03/08/00)
 + complete keyboard configuration
 + Joystick support
 + Arena-Dogfight mode with four maps

v0.9e (07/07/00)
 + Two race maps
 + All training levels implemented
 + New panel on the top of the screen
 + Hiding the panels with "p"
 + some minor graphical changes

v0.9d (...)
 + First version of race mode with one map
 + One new enemy
 + You can (theoretically) now land everywhere
 + Menu enhanced
 + four training levels

v0.9c (...)
 * fixed a very serious bug (which caused crashing of the machine)
 + fixed some bugs in Level 9
 + You can review your scorecodes now in the highscore table
 + Autofire (very slow in basic configuration)

v0.9b (21.02.00)  (sent to the AGCII)
 + Text: "Technical Overview"
 + slight changes and bugfixes

v0.9 (20.02.00)
 + one new level ("Gravity Games")
 + new graphic effect "debris pixels"
 + new weapon: mines
 + menu music
 + some changes to general graphics
 + some little changes in gameplay and level design

v0.8 (02.02.00)
 + four new big levels ("Big Brother I-III" & "Deathmatch")
 + password system (you must complete a level or know the password to
   play the next one)
 + gravitation 'walls'
 + some little changes to gameplay and graphics

v0.7 (15.01.00)
 + complete sound update (but still no music yet)
 + explosion craters
 + scoring system changed (once again); you can now download the
   hiscore.dat on the Internet to your system.
 + one new level
 * fixed some very ugly bugs... (that crashed the system)
 * fixed some not so ugly bugs (fading and other)

v0.6b (23.12.99)
 + scoring system changed (with external hiscore.dat)
 + hiscore tables on the GF website
 + "Kamikaze" now possible (some enemies are killed when you fly against
   them)
 + German documentation
 * some bugfixes (cargo, menu, ...)

v0.6 (15.12.99)
 + completely new timing system (nearly doubles performance!)
 + menu system!
 + multi language support (English & German)
 + new nice tileset (used in level 3)
 + one new level
 + level statistics
 + hiscores!
 + slight changes in graphics and gameplay
 * bugfixes

v0.5 (10/99)
 + Two complete levels (1 & 2)
 + new weapon (guided rockets)
 + radar
 + map
 + fading effects, "bouncing pixels"
 + teleporters and gravitation distortion units (both in test phase,
   only active in test level)
 * minor bugfixes

v0.4
 + Status panel
 + shield!
 + some minor additions to enemies
 * bugfixes
 + "laser doors" and a switch to unlock them
 + on screen display
 + some new objects

v0.3 - v0.0: ??
```
