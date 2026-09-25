/* Linux port: jgmod API on top of libxmp (runtime lib only, so declare what we use) */
#include <stdio.h>
#include <allegro.h>
#include "jgmod.h"

typedef char *xmp_context;
xmp_context xmp_create_context(void);
int  xmp_load_module(xmp_context, const char *);
int  xmp_start_player(xmp_context, int rate, int format);
int  xmp_play_buffer(xmp_context, void *buf, int size, int loop);
void xmp_end_player(xmp_context);
void xmp_release_module(xmp_context);
void xmp_free_context(xmp_context);
#define XMP_FORMAT_UNSIGNED (1 << 1)   /* Allegro streams are unsigned */

#define RATE 44100
#define LEN  2048                      /* stereo frames per stream buffer */

static xmp_context ctx;
static AUDIOSTREAM *stream;
static int volume = 255;

static void pump(void)                 /* timer: keep the stream fed */
{
  void *buf;
  if (stream && (buf = get_audio_stream_buffer(stream)))
  {
    xmp_play_buffer(ctx, buf, LEN * 4, 0);
    free_audio_stream_buffer(stream);
  }
}
END_OF_STATIC_FUNCTION(pump)

int install_mod(int voices) { return 0; }

JGMOD *load_mod(char *file)
{
  if (!ctx) ctx = xmp_create_context();
  if (ctx && xmp_load_module(ctx, file) == 0) return (JGMOD *)ctx;
  fprintf(stderr, "music: cannot load %s\n", file);
  return NULL;
}

void stop_mod(void)
{
  if (!stream) return;
  remove_int(pump);
  stop_audio_stream(stream);
  stream = NULL;
  xmp_end_player(ctx);
}

void play_mod(JGMOD *m, int loop)      /* xmp loops by default */
{
  if (!m) return;
  stop_mod();
  if (xmp_start_player(ctx, RATE, XMP_FORMAT_UNSIGNED)) { fprintf(stderr, "music: xmp_start_player failed\n"); return; }
  if (!(stream = play_audio_stream(LEN, 16, TRUE, RATE, volume, 128))) { fprintf(stderr, "music: play_audio_stream failed\n"); return; }
  LOCK_FUNCTION(pump);
  install_int(pump, 10);
}

void set_mod_volume(int vol)
{
  volume = MID(0, vol, 255);
  if (stream) voice_set_volume(stream->voice, volume);
}

void remove_mod(void) { stop_mod(); }

void destroy_mod(JGMOD *m)
{
  if (!ctx) return;
  xmp_release_module(ctx);
  xmp_free_context(ctx);
  ctx = NULL;
}
