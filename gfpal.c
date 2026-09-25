/*
 *    GRAVITY FORCE, gfpal.c
 *
 *    -- palette stuff (fading), most functions not used
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfpal.h"

extern int game_time;

void nfade_from_range(PALLETE source, PALLETE dest, float speed, int from, int to)
{
   PALLETE temp;
   int c;

   for (c=0; c<PAL_SIZE; c++)
      temp[c] = source[c];

//   fade_pos += speed;
//   c = fade_pos;

   fade_interpolate(source, dest, temp, /*(int)fade_pos*/0, from, to);
   set_pallete_range(temp, from, to, FALSE);

/*
   for (c=0; c<64; c+=speed) {
      fade_interpolate(source, dest, temp, c, from, to);
      set_pallete_range(temp, from, to, FALSE);
      set_pallete_range(temp, from, to, FALSE);

      clear_screen();
      clear_explosions();

      if (!playship[0].dead) blit(vscreen,playship[0].player_temp,playship[0].xpos-3-map_x[1],playship[0].ypos-3-map_y[1],0,0,24,24);

      game_time = 0;
      map_stuff(0);
      if (redraw)
      {
        MapDraw8BG(vscreen,map_x[1],map_y[1],0,0,PLAYSCREEN_WIDTH+PLUS_X,PLAYSCREEN_HEIGHT+PLUS_Y);
        make_base_room();
        redraw = FALSE;
      } // if redraw
      level_runtime_checks();
      do_enemies();
      do_bullets();
      do_pixels();
      draw_bases();
      do_objects();
      do_spobjects();
      draw_objects();
      do_explosions();
      draw_enemies();
      draw_bullets();
      draw_spobjects();
      draw_pixels();
      draw_player();
      draw_cargo();
      blit(vscreen,screen,scroll_x[1],scroll_y[1],PLAYSCREEN_XSTART,PLAYSCREEN_YSTART,PLAYSCREEN_WIDTH,PLAYSCREEN_HEIGHT);
      blit(score_table,screen,0,0,0,SCREEN_H-SCORE_HEIGHT,SCORE_WIDTH,SCORE_HEIGHT);

      vsync();
   }

   set_pallete_range(dest, from, to, TRUE);
*/
}


/* fade_in_range:
 *  Fades from a solid black pallete to p, at the specified speed (1 is
 *  the slowest, 64 is instantaneous). Only affects colors between from and 
 *  to (inclusive, pass 0 and 255 to fade the entire pallete).
 */
void nfade_in_range(PALLETE p, float speed, int from, int to)
{
   nfade_from_range(black_pallete, p, speed, from, to);
   if (fade_pos >= 63) fade_in_active = FALSE;
}


/* fade_out_range:
 *  Fades from the current pallete to a solid black pallete, at the 
 *  specified speed (1 is the slowest, 64 is instantaneous). Only affects 
 *  colors between from and to (inclusive, pass 0 and 255 to fade the 
 *  entire pallete).
 */
void nfade_out_range(float speed, int from, int to)
{
   PALLETE temp;

   get_pallete(temp);
   nfade_from_range(temp, black_pallete, speed, from, to);

   if (fade_pos >= 63) fade_out_active = FALSE;
}

void nnfade_from_range(PALLETE temp, PALLETE t, float speed, int from, int to)
{
   int n;

   if (fade_pos >= fade_opos+1)
   {
     for (n = from; n <= to; n++)
     {
       if (temp[n].r < t[n].r) temp[n].r++;
       else if (temp[n].r > t[n].r) temp[n].r--;
       if (temp[n].g < t[n].g) temp[n].g++;
       else if (temp[n].g > t[n].g) temp[n].g--;
       if (temp[n].b < t[n].b) temp[n].b++;
       else if (temp[n].b > t[n].b) temp[n].b--;
     } // for n
     fade_opos = fade_pos;
     set_pallete_range(temp, from, to, FALSE);
   }

   fade_pos += speed;
}

void nnfade_up(int from, int to)
{
   int n;
   PALETTE temp;

   get_palette(temp);

     for (n = from; n <= to; n++)
     {
       if (temp[n].r < 63) temp[n].r++;
       if (temp[n].g < 63) temp[n].g++;
       if (temp[n].b < 63) temp[n].b++;
     } // for n
     set_pallete_range(temp, from, to, TRUE);
}

void nnfade_down(int from, int to)
{
   int n;
   PALETTE temp;

   get_palette(temp);

     for (n = from; n <= to; n++)
     {
       if (temp[n].r > 0) temp[n].r--;
       if (temp[n].g > 0) temp[n].g--;
       if (temp[n].b > 0) temp[n].b--;
     } // for n
     set_pallete_range(temp, from, to, TRUE);
}


/* fade_from:
 *  Fades from source to dest, at the specified speed (1 is the slowest, 64
 *  is instantaneous).
 */
void nfade_from(PALLETE source, PALLETE dest, float speed)
{
   nfade_from_range(source, dest, speed, 0, PAL_SIZE-1);
}



/* fade_in:
 *  Fades from a solid black pallete to p, at the specified speed (1 is
 *  the slowest, 64 is instantaneous).
 */
void nfade_in(PALLETE p, float speed)
{
   nfade_in_range(p, speed, 0, PAL_SIZE-1);
}


/* fade_out:
 *  Fades from the current pallete to a solid black pallete, at the 
 *  specified speed (1 is the slowest, 64 is instantaneous).
 */
void nfade_out(float speed)
{
   nfade_out_range(speed, 0, PAL_SIZE-1);
}


void CheckFade()
{
  PALETTE tempp;

  if (fade_count < fade_count_to)
  {
    get_palette(tempp);
    switch (fade_type)
    {
      case 1 :
               nnfade_from_range(tempp,black_palette,fade_speed,fade_from_nr,fade_to_nr);
               break;
      case 2 :
               nnfade_from_range(tempp,mainpal,fade_speed,fade_from_nr,fade_to_nr);
               break;
      case 3 :
               nnfade_from_range(tempp,white_palette,fade_speed,fade_from_nr,fade_to_nr);
               break;
      case 4 :
               nnfade_from_range(tempp,red_palette,fade_speed,fade_from_nr,fade_to_nr);
               break;
    } // switch fade_type
    fade_count++;
  }
  else
  {
    fade_out_active = FALSE;
    fade_count = fade_count_to = fade_speed = 0;
    fade_from_nr = 0; fade_to_nr = 255;
  }

  if (p_fade_count < p_fade_count_to)
  {
    get_palette(tempp);
    switch (p_fade_type)
    {
      case 1 :
               nnfade_from_range(tempp,black_palette,p_fade_speed,0,255);
               break;
      case 2 :
               nnfade_from_range(tempp,mainpal,p_fade_speed,0,255);
               break;
    }
    p_fade_count++;
  }
  else
  {
    p_fade_out_active = FALSE;
    p_fade_count = p_fade_count_to = p_fade_speed = 0;
  }

}

