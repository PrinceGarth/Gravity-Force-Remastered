/*
 *    GRAVITY FORCE, gfrace.c
 *
 *    -- race specific functions
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfsprt.h"
#include "gfsound.h"
#include "gfpanel.h"
#include "gfrace.h"

void tausche(int *x1, int *x2)
{
  int tmp;
  tmp = *x1;
  *x1 = *x2;
  *x2 = tmp;
}

// Object-Array durchgehen und Waypoint-Paare finden
void get_waypoints()
{
  int nr,wnr=0,x1,y1,x2,y2;
  int ww = 5;

  for (nr=0; nr < object_anz; nr++)
  {
    if (object[nr].type == RPOLEL || object[nr].type == RPOLER ||
        object[nr].type == RPOLEU || object[nr].type == RPOLED)
    {
      x1 = object[nr].xpos;
      y1 = object[nr].ypos;
      x2 = object[nr+1].xpos;
      y2 = object[nr+1].ypos;

      if (x1 > x2) tausche(&x1,&x2);
      if (y1 > y2) tausche(&y1,&y2);

      waypoint[wnr].x = x1-ww;
      waypoint[wnr].w = x2-waypoint[wnr].x+ww*2;
      waypoint[wnr].y = y1-ww;
      waypoint[wnr].h = y2-waypoint[wnr].y+ww*2;

      waypoint[wnr].o1 = nr;
      waypoint[wnr].o2 = nr+1;

      // 'deaktivieren'
      if (wnr > 0)
      {
        object[nr].frame = 0; object[nr].maxframe = 0; object[nr].startframe = 0;
        object[nr+1].frame = 0; object[nr+1].maxframe = 0; object[nr+1].startframe = 0;
      }

      nr++;
      wnr++;
    }
  }
  max_waypoint = wnr-1;
  active_waypoint = 0;
  race_done = FALSE;
}


void check_waypoints()
{
  int nr;
  long int hsecs1,hsecs2;
  char mes[30];

  if (!race_done)
  if ( collision(playship[0].xpos,playship[0].ypos,PLAYER_WIDTH,PLAYER_HEIGHT,
                 waypoint[active_waypoint].x,waypoint[active_waypoint].y,
                 waypoint[active_waypoint].w,waypoint[active_waypoint].h) )
  {
    object[waypoint[active_waypoint].o1].maxframe = 0;
    object[waypoint[active_waypoint].o2].maxframe = 0;

    if (active_waypoint == 0)
    {
      if (current_lap == 0) current_lap = 1;
      else if (current_lap > 0)
      {
        // Lap done
        if (current_lap == 1)
        {
          lap_stat[current_lap].m = time_m;
          lap_stat[current_lap].s = time_s;
          lap_stat[current_lap].hs = time_hs;
        }
        else
        {
          hsecs1 = time_m*60*100 + time_s*100 + time_hs;
          hsecs2 = 0;
  
          for (nr=1; nr <= current_lap; nr++)
            hsecs2 += lap_stat[current_lap-nr].m*60*100 +
                      lap_stat[current_lap-nr].s*100 +
                      lap_stat[current_lap-nr].hs;
  
          lap_stat[current_lap].m = (int)((hsecs1-hsecs2) / (60*100));
          lap_stat[current_lap].s = (int)(((hsecs1-hsecs2) - (lap_stat[current_lap].m*6000)) / 100);
          lap_stat[current_lap].hs = (int)((hsecs1-hsecs2) - (lap_stat[current_lap].m*6000) - (lap_stat[current_lap].s*100));
        }
  
        // Rundenzeit festhalten
        sprintf(mes," [%02d:%02d:%02d]", lap_stat[current_lap].m, lap_stat[current_lap].s, lap_stat[current_lap].hs);
        strcat(upanel_text4,mes);
        update_upanel_infobox();
  
  //      active_waypoint = 0;
        if (current_lap < max_lap) current_lap++;
        else { time_start = FALSE; race_done = TRUE; }
      }
    }

    if (active_waypoint < max_waypoint) active_waypoint++;
    else if (active_waypoint == max_waypoint) active_waypoint = 0;

    if (!race_done)
    {
      object[waypoint[active_waypoint].o1].maxframe = 4;
      object[waypoint[active_waypoint].o2].maxframe = 4;
    }

    // bing
    if (!race_done)
      play_sound_sample(snd_cargoout,1000,0,playship[0].xpos,playship[0].ypos,100);
    else
    {
      play_sound_sample(snd_cargoout,1600,0,playship[0].xpos,playship[0].ypos,100);
      play_sound_sample(snd_cargoout,1300,0,playship[0].xpos,playship[0].ypos,100);
    }

    // Timer starten
    if (!time_start && !race_done) time_start = TRUE;
  }

}

