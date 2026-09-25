/*
 *    GRAVITY FORCE, gfsprt.c
 *
 *    -- some sprite specific functions
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfsprt.h"

int enemy_on_screen(SPRITE_TYPE en)
{
  return ( (en.xpos < map_x[1]+scroll_x[1]+PLAYSCREEN_WIDTH) &&
           (en.xpos > map_x[1]+scroll_x[1]) &&
           (en.ypos < map_y[1]+scroll_y[1]+PLAYSCREEN_HEIGHT) &&
           (en.ypos > map_y[1]+scroll_y[1]) );
}

int on_draw_field(int x, int y, int w, int h)
{
  if ( x > map_x[1]+PLAYSCREEN_WIDTH+PLUS_X || x+w < map_x[1] ||
       y > map_y[1]+PLAYSCREEN_HEIGHT+PLUS_Y || y+h < map_y[1] )
    return FALSE;
  else
    return TRUE;
}

int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
  if (x1 > x2+w2 || x2 > x1+w1 || y1 > y2+h2 || y2 > y1+h1)
    return FALSE;
  else
    return TRUE;
}

float strecke(int x1, int y1, int x2, int y2)
{
   return (sqrt(pow(x2-x1,2.0) + pow(y2-y1,2.0)));
}


