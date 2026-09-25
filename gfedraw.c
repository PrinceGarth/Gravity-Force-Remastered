/*
 *    GRAVITY FORCE, gfedraw.c
 *
 *    -- drawing functions for "editor"
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"


void make_base_room()
{
  int nr;
  BITMAP *tempbit;
  int nx,ny;

  for (nr=0; nr < base_anz; nr++)
  {
    if ( (base[nr].x1 > map_x[1]-PLUS_X) && (base[nr].x2 < map_x[1]+PLAYSCREEN_WIDTH+PLUS_X) &&
         (base[nr].y  > map_y[1]-PLUS_Y) && (base[nr].y  < map_y[1]+PLAYSCREEN_HEIGHT+PLUS_Y) )
    {
      switch (base[nr].type)
      {
        case PLAYER_BASE : tempbit = data[base_home].dat;
                           nx = base[nr].x1-PLBASE_TOWER_W;
                           ny = (base[nr].y+BASE_LANDING_H) - tempbit->h;
                           rectfill(vscreen,nx-map_x[1]+5,ny-map_y[1],nx-map_x[1]+tempbit->w-5,ny-map_y[1]+tempbit->h-5,0);
                           break;
        case CARGO_BASE  : tempbit = data[base_cargo].dat;
                           nx = base[nr].x1-4;
                           ny = (base[nr].y+BASE_LANDING_H) - tempbit->h-5;
                           rectfill(vscreen,nx-map_x[1]+5,ny-map_y[1],nx-map_x[1]+tempbit->w-5,ny-map_y[1]+tempbit->h,0);
                           break;
        case ENEMY_S_BASE: tempbit = data[base_senemy].dat;
                           nx = base[nr].x1-2;
                           ny = (base[nr].y+1) - tempbit->h-2;
                           rectfill(vscreen,nx-map_x[1]+2,ny-map_y[1],nx-map_x[1]+tempbit->w-2,ny-map_y[1]+tempbit->h,0);
                           break;
        case ENEMY_L_BASE: tempbit = data[base_lenemy].dat;
                           nx = base[nr].x1-2;
                           ny = (base[nr].y+1) - tempbit->h-5;
                           rectfill(vscreen,nx-map_x[1]+2,ny-map_y[1],nx-map_x[1]+tempbit->w-2,ny-map_y[1]+tempbit->h+3,0);
                           break;

      }
    } // if...

  } // for

}

void draw_bases()
{
  int nr;
  int nx,ny;
  int scale;
  BITMAP *tempbit;

  for (nr=0; nr < base_anz; nr++)
  {
    if ( (base[nr].x1 > map_x[1]-PLUS_X) && (base[nr].x2 < map_x[1]+PLAYSCREEN_WIDTH+PLAYSCREEN_WIDTH) &&
         (base[nr].y  > map_y[1]-PLUS_Y) && (base[nr].y  < map_y[1]+PLAYSCREEN_HEIGHT+PLUS_Y) )
    {
      switch (base[nr].type)
      {
        case PLAYER_BASE : tempbit = data[base_home].dat;
                           nx = base[nr].x1-PLBASE_TOWER_W;
                           ny = (base[nr].y+BASE_LANDING_H) - tempbit->h;
                           draw_rle_sprite(vscreen,data[base_home].dat,nx-map_x[1],ny-map_y[1]);
                           break;
        case CARGO_BASE  : tempbit = data[base_cargo].dat;
                           nx = base[nr].x1-4;
                           ny = (base[nr].y+BASE_LANDING_H) - tempbit->h;
                           draw_rle_sprite(vscreen,data[base_cargo].dat,nx-map_x[1],ny-map_y[1]);
                           break;
        case ENEMY_S_BASE: tempbit = data[base_senemy].dat;
                           nx = base[nr].x1-2;
                           ny = (base[nr].y+2) - tempbit->h;
                           draw_rle_sprite(vscreen,data[base_senemy].dat,nx-map_x[1],ny-map_y[1]);
                           break;
        case ENEMY_L_BASE: tempbit = data[base_lenemy].dat;
                           nx = base[nr].x1-2;
                           ny = (base[nr].y+2) - tempbit->h;
                           draw_rle_sprite(vscreen,data[base_lenemy].dat,nx-map_x[1],ny-map_y[1]);
                           break;

      }
     textout(vscreen,font,mitoa(nr),base[nr].x1+10-map_x[1],base[nr].y-10-map_y[1],col_yellow);
//      hline(vscreen,base[nr].x1-map_x[1],base[nr].y-map_y[1],base[nr].x2-map_x[1],col_yellow);
    } // if...

  } // for

}

void draw_cargo()
{
  int nr;
  int nx,ny;
  int scale,scaley;
  BITMAP *tempbit;

  for (nr=0; nr < base_anz; nr++)
  {
    if ( (base[nr].x1 > map_x[1]-PLUS_X) && (base[nr].x2 < map_x[1]+PLAYSCREEN_WIDTH+PLAYSCREEN_WIDTH) &&
         (base[nr].y  > map_y[1]-PLUS_Y) && (base[nr].y  < map_y[1]+PLAYSCREEN_HEIGHT+PLUS_Y) )
    {
      if ((base[nr].cargo > 0) && (base[nr].cargo < 10))
      {
        scale = 5;
        scale += base[nr].cargo*1.5;
        stretch_sprite(vscreen,data[cargo1].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scale - map_y[1],scale,scale);
        add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scale,scale,scale);
      }
      else
      {
        switch (base[nr].cargo)
        {
          case FUEL   :  scale = 8; scaley = 9;
                         draw_sprite(vscreen,data[cargo_fuel].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case LIFE   :  scale = 8; scaley = 9;
                         draw_sprite(vscreen,data[cargo_life].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CSHIELD:  scale = 8; scaley = 9;
                         draw_sprite(vscreen,data[cargo_shield].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CBOMBS10 :  scale = 8; scaley = 9;
                         draw_sprite(vscreen,data[cargo_bombs10].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CBOMBS20 :  scale = 8; scaley = 9;
                         draw_sprite(vscreen,data[cargo_bombs20].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CBIGBOMB1:  scale = 8; scaley = 9;
                         draw_sprite(vscreen,data[cargo_bigbombs1].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CBIGBOMB3:  scale = 8; scaley = 9;
                         draw_sprite(vscreen,data[cargo_bigbombs3].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CMGBULLTS:  scale = 8; scaley = 9;
                         draw_sprite(vscreen,data[cargo_mgbullets].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CMAN1  :  scale = 5; scaley = 5;
                         draw_sprite(vscreen,data[object[base[nr].cargo_obj].image.data].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
        } // switch
      } // else
    }
  }

}

void draw_objects()
{
  int nr;
  int x,y,w,h;

  for (nr=0; nr < object_anz; nr++)
  {
    if (object[nr].active)
    {
      if ( (object[nr].xpos > map_x[1]-PLUS_X) && (object[nr].xpos < map_x[1]+PLAYSCREEN_WIDTH+PLUS_X) &&
           (object[nr].ypos > map_y[1]-PLUS_Y) && (object[nr].ypos < map_y[1]+PLAYSCREEN_HEIGHT+PLUS_Y) )
      {
        if ((object[nr].xpos > 0) && (object[nr].ypos > 0))
        {
          draw_rle_sprite(vscreen,data[object[nr].image.data].dat,object[nr].xpos-map_x[1],object[nr].ypos-map_y[1]);
          textout(vscreen,font,mitoa(nr),object[nr].xpos-map_x[1],object[nr].ypos-5-map_y[1],col_yellow);
        }

        switch (object[nr].type)
        {
          case PILE    :  x = object[nr].xpos+1;//-map_x[1]+1;
                          y = object[nr].ypos;//-map_y[1];
                          w = object[nr].image.width-2;
                          h = object[nr].image.height-6;
                          add_2_list(&rl,x,y,w,h);
                          break;
          case DOORH   :  x = object[nr].xpos+3;
                          y = object[nr].ypos+1;
                          w = object[nr].image.width-6;
                          h = object[nr].image.height-2;
                          add_2_list(&rl,x,y,w,h);
                          break;
          case DOORV   :  x = object[nr].xpos+1;//-map_x[1]+1;
                          y = object[nr].ypos+3;//-map_y[1]+3;
                          w = object[nr].image.width-2;
                          h = object[nr].image.height-6;
                          add_2_list(&rl,x,y,w,h);
                          break;
          default      :  x = object[nr].xpos+1;//-map_x[1]+1;
                          y = object[nr].ypos+1;//-map_y[1]+1;
                          w = object[nr].image.width-2;
                          h = object[nr].image.height-2;
        }
      } // if on screen
    } // if active
  } // for nr

}

void draw_spobjects()
{
  int nr;
  int x,y,w,h;

  for (nr=0; nr < spobject_anz; nr++)
  {
    if (spobject[nr].active)
    {
      if ( (spobject[nr].xpos > map_x[1]-PLUS_X) && (spobject[nr].xpos < map_x[1]+PLAYSCREEN_WIDTH+PLUS_X) &&
           (spobject[nr].ypos > map_y[1]-PLUS_Y) && (spobject[nr].ypos < map_y[1]+PLAYSCREEN_HEIGHT+PLUS_Y) )
      {
        if ((spobject[nr].xpos > 0) && (spobject[nr].ypos > 0))
        {
          draw_rle_sprite(vscreen,data[spobject[nr].image.data].dat,spobject[nr].xpos-map_x[1],spobject[nr].ypos-map_y[1]);
          textout(vscreen,font,mitoa(nr),spobject[nr].xpos-map_x[1],spobject[nr].ypos-5-map_y[1],col_yellow);
        }
        switch (spobject[nr].type)
        {
          default      :  x = spobject[nr].xpos;//-map_x[1];
                          y = spobject[nr].ypos;//-map_y[1];
                          w = spobject[nr].image.width;
                          h = spobject[nr].image.height;
                          add_2_list(&rl,x,y,w,h);

        }
      } // if on screen
    } // if active
  } // for nr

}


void draw_enemies()
{
  int nr;
  int x,y,w,h;

  for (nr=0; nr < enemy_anz; nr++)
  {
    if (enemy[nr].active)
    {
      if ( (enemy[nr].xpos > map_x[1]-PLUS_X) && (enemy[nr].xpos < map_x[1]+PLAYSCREEN_WIDTH+PLUS_X) &&
           (enemy[nr].ypos > map_y[1]-PLUS_Y) && (enemy[nr].ypos < map_y[1]+PLAYSCREEN_HEIGHT+PLUS_Y) )
      {
        if (!enemy[nr].dead)
        {
          if (!enemy[nr].onehit)
          {
            draw_rle_sprite(vscreen,data[enemy[nr].image.data].dat,enemy[nr].xpos-map_x[1],enemy[nr].ypos-map_y[1]);
          }
          else
          {
            draw_lit_rle_sprite(vscreen,data[enemy[nr].image.data].dat,enemy[nr].xpos-map_x[1],enemy[nr].ypos-map_y[1],col_green);
            enemy[nr].onehit = FALSE;
          }
          textout(vscreen,font,mitoa(nr),enemy[nr].xpos-map_x[1],enemy[nr].ypos-5-map_y[1],col_yellow);
        } // if !dead

        switch (enemy[nr].type)
        {
          case SHOOTER_UP  :  x = enemy[nr].xpos;
                              y = enemy[nr].ypos;
                              w = enemy[nr].image.width;
                              h = enemy[nr].image.height-2;
                              break;
          case SHOOTER_DOWN : x = enemy[nr].xpos;
                              y = enemy[nr].ypos+2;
                              w = enemy[nr].image.width;
                              h = enemy[nr].image.height;
                              break;
          case SHOOTER_RIGHT: x = enemy[nr].xpos+2;
                              y = enemy[nr].ypos;
                              w = enemy[nr].image.width;
                              h = enemy[nr].image.height;
                              break;
          case SHOOTER_LEFT : x = enemy[nr].xpos;
                              y = enemy[nr].ypos;
                              w = enemy[nr].image.width-2;
                              h = enemy[nr].image.height;
                              break;
          case PULSE_SHOOTER: x = enemy[nr].xpos;
                              y = enemy[nr].ypos;
                              w = enemy[nr].image.width;
                              h = enemy[nr].image.height-1;
                              break;
          default          :  x = enemy[nr].xpos;
                              y = enemy[nr].ypos;
                              w = enemy[nr].image.width;
                              h = enemy[nr].image.height;
        }
        if (!enemy[nr].dead) add_2_list(&rl,x,y,w,h);
      } // if on screen
    } // if active
  } // for nr

}

void draw_player()
{
  int nr;

  for (nr=1; nr <= 2; nr++)
  {
    if (!playship[nr].dead)
    {
      switch (playship[nr].powr)
      {
        case 1 : rotate_sprite(vscreen,data[plship1].dat,playship[nr].xpos-map_x[1],playship[nr].ypos-map_y[1],ftofix((playship[nr].head)/1.4));
                 break;
        case 2 : rotate_sprite(vscreen,data[plship2].dat,playship[nr].xpos-map_x[1],playship[nr].ypos-map_y[1],ftofix((playship[nr].head)/1.4));
                 break;
      }
    } // if !dead
  } // for nr


}

void draw_bullets()
{
  int nr;
  BNODE *temp, *next_bull;

  temp = first_bullet;

  while (temp!=NULL)
  {
     next_bull = temp->next;

     switch (temp->dat.type2)
     {
       default :
         if (getpixel(vscreen,temp->dat.xpos-map_x[1],temp->dat.ypos-map_y[1]) == 0)
         {
           draw_sprite(vscreen,temp->dat.image.data,temp->dat.xpos-map_x[1],temp->dat.ypos-map_y[1]);
           add_2_list(&rl,temp->dat.xpos,temp->dat.ypos,temp->dat.image.width,temp->dat.image.height);
         }
      } // switch type

     temp = next_bull;
  } // while

}

void draw_pixels()
{
  int nr;
  struct PNODE *temp, *next_pixel;

  temp = first_pixel;

  while (temp!=NULL)
  {
    next_pixel = temp->next;

    if (getpixel(vscreen,temp->dat.x-map_x[1],temp->dat.y-map_y[1]) == 0)
    {
      putpixel(vscreen,temp->dat.x-map_x[1],temp->dat.y-map_y[1],temp->dat.c);
      add_2_list(&rl,temp->dat.x,temp->dat.y,1,1);
    }

    temp = next_pixel;
  } // while

}

void play_sound_sample(SAMPLE *spl, int vol, int pan, int freq, int loop)
{

  if (play_sound)
    play_sample(spl,vol,pan,freq,loop);

}

