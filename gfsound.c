/*
 *    GRAVITY FORCE, gfsound.c
 *
 *    -- sound output
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfsound.h"
#include "gfsprt.h"
#include "gf.h"


void play_sound_sample(int smp, int freq, int loop, int x, int y, int v)
{
  int maxdist = 800;
  int dist,curvol,vol,pan,xab;
  extern int SOUND_VOLUME;

  if (play_sound)
  {
    if (x != 0 && y != 0)
    {
      dist = strecke(playship[0].xpos,playship[0].ypos,x,y);
      xab  = playship[0].xpos - x;
    
      // volume
      curvol = (255.0 * (float)(1-((float)dist/(float)maxdist))) * (v/100.0);
      vol = (float)curvol * (float)(SOUND_VOLUME/100.0);
    
      // pan
      if (xab > 0)
      {
        pan = 128.0 * (float)(1-((float)abs(xab)/(float)maxdist));
      }
      else if (xab < 0)
      {
        pan = (128.0 * (float)(1-((float)abs(xab)/(float)maxdist))) + 128;
      }
      else
      {
        pan = 128;
      }
    
      if (vol > 255) curvol = 255;
    }
    else
    {
      dist = 0;
      curvol = (float)255.0 * (float)(SOUND_VOLUME/100.0);
      pan = 128;
    }

    if (dist < maxdist)
      play_sample(sounddata[smp].dat,curvol,pan,freq,loop);

  }
}

void stop_sound_sample(int smp)
{
  if (play_sound)
    stop_sample(sounddata[smp].dat);
}

