/*
 *    GRAVITY FORCE, gfscale.c  (Linux port)
 *
 *    -- borderless fullscreen on the primary monitor without changing its
 *       video mode: the game draws into a 640x480 memory bitmap posing as
 *       `screen`, a timer stretches it (letterboxed) into a monitor-sized
 *       window, and the window manager is asked to fullscreen that window.
 *
 *    Opt-in with GF_SCALE=1 (experimental); default is a plain 640x480 window.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <allegro.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>

#undef set_gfx_mode   /* gfhead.h routes the game's calls here */

static BITMAP *real_screen = NULL;   /* NULL = not scaling */
static int ox, oy, dw, dh;           /* letterbox rectangle inside real_screen */
static int exit_hooked = FALSE;

static void present(void)
{
  if (real_screen)
    stretch_blit(screen, real_screen, 0, 0, screen->w, screen->h, ox, oy, dw, dh);
}
END_OF_STATIC_FUNCTION(present)

/* Put Allegro's own screen back before it (or we) destroy anything. */
static void unscale(void)
{
  BITMAP *buf;

  if (!real_screen) return;
  remove_int(present);
  buf = screen;
  screen = real_screen;
  real_screen = NULL;
  destroy_bitmap(buf);
}

/* Primary monitor geometry via RandR; returns FALSE if unavailable. */
static int primary_monitor(Display *d, int *x, int *y, int *w, int *h)
{
  XRRMonitorInfo *m;
  int n, i, pick = 0;

  m = XRRGetMonitors(d, DefaultRootWindow(d), True, &n);
  if (!m || n < 1) return FALSE;
  for (i = 0; i < n; i++) if (m[i].primary) pick = i;
  *x = m[pick].x; *y = m[pick].y; *w = m[pick].width; *h = m[pick].height;
  XRRFreeMonitors(m);
  return TRUE;
}

/* Our Allegro window: WM_CLASS "allegro" owned by this process. */
static Window find_own_window(Display *d)
{
  Atom list = XInternAtom(d, "_NET_CLIENT_LIST", False);
  Atom pid = XInternAtom(d, "_NET_WM_PID", False);
  Atom type; int fmt; unsigned long n, after, i;
  unsigned char *data = NULL, *pdata;
  Window found = 0, *wins;
  XClassHint ch;

  if (XGetWindowProperty(d, DefaultRootWindow(d), list, 0, 4096, False, XA_WINDOW,
                         &type, &fmt, &n, &after, &data) != Success || !data)
    return 0;
  wins = (Window *)data;
  for (i = 0; i < n && !found; i++)
  {
    if (!XGetClassHint(d, wins[i], &ch)) continue;
    if (ch.res_name && !strcmp(ch.res_name, "allegro"))
    {
      unsigned long pn, pa; Atom pt; int pf;
      pdata = NULL;
      if (XGetWindowProperty(d, wins[i], pid, 0, 1, False, XA_CARDINAL,
                             &pt, &pf, &pn, &pa, &pdata) == Success && pdata)
      {
        if (pn && *(unsigned long *)pdata == (unsigned long)getpid()) found = wins[i];
        XFree(pdata);
      }
    }
    if (ch.res_name) XFree(ch.res_name);
    if (ch.res_class) XFree(ch.res_class);
  }
  XFree(data);
  return found;
}

/* Move the window onto the monitor, then ask the WM to fullscreen it there. */
static void go_fullscreen(Display *d, int mx, int my)
{
  Window w = 0;
  XEvent ev;
  int tries;

  for (tries = 0; tries < 20 && !w; tries++)   /* WM maps it asynchronously */
  {
    w = find_own_window(d);
    if (!w) rest(50);
  }
  if (!w) return;

  XMoveWindow(d, w, mx, my);
  memset(&ev, 0, sizeof(ev));
  ev.xclient.type = ClientMessage;
  ev.xclient.window = w;
  ev.xclient.message_type = XInternAtom(d, "_NET_WM_STATE", False);
  ev.xclient.format = 32;
  ev.xclient.data.l[0] = 1;   /* _NET_WM_STATE_ADD */
  ev.xclient.data.l[1] = XInternAtom(d, "_NET_WM_STATE_FULLSCREEN", False);
  ev.xclient.data.l[3] = 1;   /* source: normal application */
  XSendEvent(d, DefaultRootWindow(d), False,
             SubstructureRedirectMask | SubstructureNotifyMask, &ev);
  XFlush(d);
}

int gf_set_gfx_mode(int card, int w, int h, int v_w, int v_h)
{
  Display *d;
  BITMAP *buf;
  int mx, my, mw, mh, ret;

  if (!exit_hooked) { atexit(unscale); exit_hooked = TRUE; }  /* runs before allegro_exit */
  unscale();

  if (card == GFX_TEXT || !getenv("GF_SCALE"))
    return set_gfx_mode(card == GFX_TEXT ? card : GFX_AUTODETECT_WINDOWED, w, h, v_w, v_h);

  d = XOpenDisplay(NULL);
  if (!d || !primary_monitor(d, &mx, &my, &mw, &mh) || mw < w || mh < h)
  {
    if (d) XCloseDisplay(d);
    return set_gfx_mode(GFX_AUTODETECT_WINDOWED, w, h, v_w, v_h);
  }

  ret = set_gfx_mode(GFX_AUTODETECT_WINDOWED, mw, mh, 0, 0);
  if (ret || !(buf = create_bitmap(w, h)))
  {
    XCloseDisplay(d);
    return set_gfx_mode(GFX_AUTODETECT_WINDOWED, w, h, v_w, v_h);
  }

  /* largest aspect-correct fit, centred */
  if (mw * h > mh * w) { dh = mh; dw = mh * w / h; }
  else                 { dw = mw; dh = mw * h / w; }
  ox = (mw - dw) / 2; oy = (mh - dh) / 2;

  clear_bitmap(screen);
  clear_bitmap(buf);
  real_screen = screen;
  screen = buf;
  LOCK_FUNCTION(present);
  install_int_ex(present, BPS_TO_TIMER(60));

  go_fullscreen(d, mx, my);
  XCloseDisplay(d);
  return 0;
}

/* Real-window mouse position -> 640x480 game coordinates. */
int gf_mouse_x(void) { return real_screen ? (mouse_x - ox) * screen->w / dw : mouse_x; }
int gf_mouse_y(void) { return real_screen ? (mouse_y - oy) * screen->h / dh : mouse_y; }
