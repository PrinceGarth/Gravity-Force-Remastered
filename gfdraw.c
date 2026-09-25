/*
 *    GRAVITY FORCE, gfdraw.c
 *
 *    -- most of the drawing stuff
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfdraw.h"
#include "gflink.h"
#include "gfmisc.h"
#include "gfnetw.h"
#include "gfsprt.h"
#include "gf.h"


void make_base_room(BITMAP *scr, int c)
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
                           rectfill(scr,nx-map_x[1]+5,ny-map_y[1],nx-map_x[1]+tempbit->w-5,ny-map_y[1]+tempbit->h-5,0);
                           break;
        case CARGO_BASE  : tempbit = data[base_cargo].dat;
                           nx = base[nr].x1-4;
                           ny = (base[nr].y+BASE_LANDING_H) - tempbit->h-5;
                           rectfill(scr,nx-map_x[1]+5,ny-map_y[1],nx-map_x[1]+tempbit->w-5,ny-map_y[1]+tempbit->h,0);
                           break;
        case ENEMY_S_BASE: tempbit = data[base_senemy].dat;
                           nx = base[nr].x1-2;
                           ny = (base[nr].y+1) - tempbit->h-3;
                           rectfill(scr,nx-map_x[1]+2,ny-map_y[1],nx-map_x[1]+tempbit->w-2,ny-map_y[1]+tempbit->h+1,0);
                           break;
        case ENEMY_L_BASE: tempbit = data[base_lenemy].dat;
                           nx = base[nr].x1-2;
                           ny = (base[nr].y+1) - tempbit->h-5;
                           rectfill(scr,nx-map_x[1]+2,ny-map_y[1],nx-map_x[1]+tempbit->w-2,ny-map_y[1]+tempbit->h+3,0);
                           break;

      }
    } // if...

  } // for

}

void draw_bases(BITMAP *scr, int c)
{
  int nr;
  int nx,ny;
  BITMAP *tempbit;

  for (nr=0; nr < base_anz; nr++)
  {
    if (on_draw_field(base[nr].x1-20,base[nr].y-20,base[nr].x2-base[nr].x1+40,40))
    {
      switch (base[nr].type)
      {
        case PLAYER_BASE : tempbit = data[base_home].dat;
                           nx = base[nr].x1-PLBASE_TOWER_W;
                           ny = (base[nr].y+BASE_LANDING_H) - tempbit->h;
                           draw_rle_sprite(scr,data[base_home].dat,nx-map_x[1],ny-map_y[1]);
                           break;
        case CARGO_BASE  : tempbit = data[base_cargo].dat;
                           nx = base[nr].x1-4;
                           ny = (base[nr].y+BASE_LANDING_H) - tempbit->h;
                           draw_rle_sprite(scr,data[base_cargo].dat,nx-map_x[1],ny-map_y[1]);
                           break;
        case ENEMY_S_BASE: tempbit = data[base_senemy].dat;
                           nx = base[nr].x1-2;
                           ny = (base[nr].y+2) - tempbit->h;
                           draw_rle_sprite(scr,data[base_senemy].dat,nx-map_x[1],ny-map_y[1]);
                           break;
        case ENEMY_L_BASE: tempbit = data[base_lenemy].dat;
                           nx = base[nr].x1-2;
                           ny = (base[nr].y+2) - tempbit->h;
                           draw_rle_sprite(scr,data[base_lenemy].dat,nx-map_x[1],ny-map_y[1]);
                           break;

      }
//      hline(scr,base[nr].x1-map_x[1],base[nr].y-map_y[1],base[nr].x2-map_x[1],col_yellow);
    } // if...

  } // for

}

void draw_cargo(BITMAP *scr, int c)
{
  int nr;
  int scale,scaley;

  for (nr=0; nr < base_anz; nr++)
  {
    if (on_draw_field(base[nr].x1-20,base[nr].y-20,base[nr].x2-base[nr].x1+40,40))
    {
      if ((base[nr].cargo > 0) && (base[nr].cargo < 10))
      {
        scale = 5;
        scale += base[nr].cargo*1.5;
        stretch_sprite(scr,data[cargo1].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scale - map_y[1],scale,scale);
        add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scale,scale,scale);
      }
      else
      {
        switch (base[nr].cargo)
        {
          case FUEL   :  scale = 8; scaley = 9;
                         draw_sprite(scr,data[cargo_fuel].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case LIFE   :  scale = 8; scaley = 9;
                         draw_sprite(scr,data[cargo_life].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CSHIELD:  scale = 8; scaley = 9;
                         draw_sprite(scr,data[cargo_shield].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CBOMBS10 :  scale = 8; scaley = 9;
                         draw_sprite(scr,data[cargo_bombs10].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CBOMBS20 :  scale = 8; scaley = 9;
                         draw_sprite(scr,data[cargo_bombs20].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CBIGBOMB1:  scale = 8; scaley = 9;
                         draw_sprite(scr,data[cargo_bigbombs1].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CBIGBOMB3:  scale = 8; scaley = 9;
                         draw_sprite(scr,data[cargo_bigbombs3].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CMGBULLTS:  scale = 8; scaley = 9;
                         draw_sprite(scr,data[cargo_mgbullets].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CROCKETS :  scale = 8; scaley = 9;
                         draw_sprite(scr,data[cargo_rockets].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CMAN1  :  scale = 5; scaley = 5;
                         draw_rle_sprite(scr,data[object[base[nr].cargo_obj].image.data].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CMINES1 :  scale = 8; scaley = 9;
                         draw_sprite(scr,data[cargo_mine1].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CMINES2 :  scale = 8; scaley = 9;
                         draw_sprite(scr,data[cargo_mine2].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
          case CMINES3 :  scale = 8; scaley = 9;
                         draw_sprite(scr,data[cargo_mine3].dat,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2 - map_x[1],base[nr].y-scaley - map_y[1]);
                         add_2_list(&rl,(base[nr].x2-base[nr].x1)/2 + base[nr].x1 - scale/2,base[nr].y-scaley,scale,scaley);
                         break;
        } // switch
      } // else
    }
  }

}

void draw_objects(BITMAP *scr, int c)
{
  int nr;
  int x,y,w,h;

  for (nr=0; nr < object_anz; nr++)
  {
    if (object[nr].active)
    {
//      if ( (object[nr].xpos > map_x[1]-PLUS_X-scroll_x[1]) && (object[nr].xpos < map_x[1]+PLAYSCREEN_WIDTH+PLUS_X+scroll_x[1]) &&
//           (object[nr].ypos > map_y[1]-PLUS_Y) && (object[nr].ypos < map_y[1]+PLAYSCREEN_HEIGHT+PLUS_Y) )
      if (on_draw_field(object[nr].xpos,object[nr].ypos,object[nr].image.width,object[nr].image.height))
      {
        if ((object[nr].xpos > 0) && (object[nr].ypos > 0))
          draw_rle_sprite(scr,data[object[nr].image.data].dat,object[nr].xpos-map_x[1],object[nr].ypos-map_y[1]);

        switch (object[nr].type)
        {
          case PILE    :  x = object[nr].xpos+1;//-map_x[1]+1;
                          y = object[nr].ypos;//-map_y[1];
                          w = object[nr].image.width-2;
                          h = object[nr].image.height-6;
                          add_2_list(&rl,x,y,w,h);
                          break;
          case MAN1    :  x = object[nr].xpos;
                          y = object[nr].ypos;
                          w = object[nr].image.width;
                          h = object[nr].image.height-1;
                          add_2_list(&rl,x,y,w,h);
                          break;
          case DOORH : case DOORH2 :
                          x = object[nr].xpos+3;
                          y = object[nr].ypos+1;
                          w = object[nr].image.width-6;
                          h = object[nr].image.height-2;
                          add_2_list(&rl,x,y,w,h);
                          break;
          case DOORV : case DOORV2 :
                          x = object[nr].xpos+1;//-map_x[1]+1;
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

void draw_spobjects(BITMAP *scr, int c)
{
  int nr;
  int x,y,w,h;

  for (nr=0; nr < spobject_anz; nr++)
  {
    if (spobject[nr].active)
    {
//      if ( (spobject[nr].xpos > map_x[1]-PLUS_X-scroll_x[1]) && (spobject[nr].xpos < map_x[1]+PLAYSCREEN_WIDTH+PLUS_X+scroll_x[1]) &&
//           (spobject[nr].ypos > map_y[1]-PLUS_Y) && (spobject[nr].ypos < map_y[1]+PLAYSCREEN_HEIGHT+PLUS_Y) )
      if (on_draw_field(spobject[nr].xpos,spobject[nr].ypos,spobject[nr].image.width,spobject[nr].image.height))
      {
        if ((spobject[nr].xpos > 0) && (spobject[nr].ypos > 0) && spobject[nr].image.data >= 0)
          draw_rle_sprite(scr,data[spobject[nr].image.data].dat,spobject[nr].xpos-map_x[1],spobject[nr].ypos-map_y[1]);

        switch (spobject[nr].type)
        {
          case GWALL_L : case GWALL_R :
          case GWALL_U : case GWALL_D :
                          break;
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

void draw_enemies(BITMAP *scr, int c)
{
  int nr;
  int x,y,w,h;

  for (nr=0; nr < enemy_anz; nr++)
  {
    if (enemy[nr].active)
    {
//      if ( (enemy[nr].xpos > map_x[1]-PLUS_X) && (enemy[nr].xpos < map_x[1]+PLAYSCREEN_WIDTH+PLUS_X) &&
//           (enemy[nr].ypos > map_y[1]-PLUS_Y) && (enemy[nr].ypos < map_y[1]+PLAYSCREEN_HEIGHT+PLUS_Y) )
      if (on_draw_field(enemy[nr].xpos,enemy[nr].ypos,enemy[nr].image.width,enemy[nr].image.height))
      {
        if (!enemy[nr].dead)
        {
          if (!enemy[nr].onehit)
          {
            draw_rle_sprite(scr,data[enemy[nr].image.data].dat,enemy[nr].xpos-map_x[1],enemy[nr].ypos-map_y[1]);
          }
          else
          {
            draw_lit_rle_sprite(scr,data[enemy[nr].image.data].dat,enemy[nr].xpos-map_x[1],enemy[nr].ypos-map_y[1],col_green);
            enemy[nr].onehit = FALSE;
          }
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
          case GSTONE_B1    : x = enemy[nr].xpos;
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

void draw_player(BITMAP *scr, int c)
{
  BITMAP *p1, *p2;

  switch (c)
  {
    case 0 :
      p1 = data[plship1].dat;
      p2 = data[plship2].dat;
      break;
    case 1 :
      p1 = data[plship2_1].dat;
      p2 = data[plship2_2].dat;
      break;
    case 2 :
      p1 = data[plship3_1].dat;
      p2 = data[plship3_2].dat;
      break;
    case 3 :
      p1 = data[plship4_1].dat;
      p2 = data[plship4_2].dat;
      break;
    case 4 :
      p1 = data[plship5_1].dat;
      p2 = data[plship5_2].dat;
      break;
  }

  if (!playship[c].dead)
  {
    switch (playship[c].powr)
    {
      case 1 : rotate_sprite(scr,p1,playship[c].xpos-map_x[1],playship[c].ypos-map_y[1],ftofix((playship[c].head)/1.4));
               break;
      case 2 : rotate_sprite(scr,p2,playship[c].xpos-map_x[1],playship[c].ypos-map_y[1],ftofix((playship[c].head)/1.4));
               break;
    }
  } // if !dead

}

void draw_bullets(BITMAP *scr, int c)
{
  BNODE *temp, *next_bull;

  temp = first_bullet;

  while (temp!=NULL)
  {
     next_bull = temp->next;
//     if (on_draw_field(temp->dat.xpos,temp->dat.ypos,temp->dat.image.width,temp->dat.image.height))
       switch (temp->dat.type2)
       {
         default :
           if (getpixel(scr,temp->dat.xpos-map_x[1],temp->dat.ypos-map_y[1]) == 0)
           {
             if (temp->dat.nr > 0)
               draw_lit_sprite(scr,temp->dat.image.data,temp->dat.xpos-map_x[1],temp->dat.ypos-map_y[1],col_yellow);
             else
               draw_sprite(scr,temp->dat.image.data,temp->dat.xpos-map_x[1],temp->dat.ypos-map_y[1]);

             add_2_list(&rl,temp->dat.xpos,temp->dat.ypos,temp->dat.image.width,temp->dat.image.height);
           }
        } // switch type

     temp = next_bull;
  } // while

}

void draw_pixels(BITMAP *scr, int c)
{
  struct PNODE *temp, *next_pixel;

  temp = first_pixel;

  while (temp!=NULL)
  {
    next_pixel = temp->next;

//    if (on_draw_field(temp->dat.x,temp->dat.y,5,5))
    if (getpixel(scr,temp->dat.x-map_x[1],temp->dat.y-map_y[1]) == 0)
    {
      if (temp->dat.image)
      {
        draw_sprite(scr,temp->dat.image,temp->dat.x-map_x[1],temp->dat.y-map_y[1]);
         add_2_list(&rl,temp->dat.x,temp->dat.y,
                   ((BITMAP *)temp->dat.image)->w,
                   ((BITMAP *)temp->dat.image)->h);
      }
      else
      {
        putpixel(scr,temp->dat.x-map_x[1],temp->dat.y-map_y[1],temp->dat.c);
        add_2_list(&rl,temp->dat.x,temp->dat.y,1,1);
      }
    }

    temp = next_pixel;
  } // while

}

void masked_kraxel(BITMAP *bmp, int x, int y, int d)
{
  if (getpixel(bmp,x,y) > 0)
  {
    putpixel(bmp,x,y+random()%5-2,col_black);
    putpixel(bmp,x,y+random()%5-2,col_black);
    putpixel(bmp,x,y+random()%5-2,tbl_red[random() % 6]);
  }
}

void draw_crater(BITMAP *bmp, int x, int y, int size)
{
  circlefill(vscreen,x-map_x[1],y-map_y[1],size,col_black);
  do_circle(vscreen,x-map_x[1],y-map_y[1],size+1,0,masked_kraxel);
}

void draw_craters(BITMAP *scr, int c)
{
  struct CNODE *temp, *next_crater;

  temp = first_crater;

  while (temp!=NULL)
  {
    next_crater = temp->next;

    if (on_draw_field(temp->dat.x,temp->dat.y,temp->dat.size*2,temp->dat.size*2))
      draw_crater(scr,temp->dat.x,temp->dat.y,temp->dat.size);

    temp = next_crater;
  } // while

}

void clear_screen(int c)
{
  int cnt;

  blit(playship[0].player_temp,vscreen,0,0,playship[0].xpos-5-map_x[1],playship[0].ypos-2-map_y[1],19,18);

  if (c > 0)
    for (cnt=1; cnt <= c; cnt++)
      blit(playship[cnt].player_temp,vscreen,0,0,playship[cnt].xpos-5-map_x[1],playship[cnt].ypos-2-map_y[1],19,18);

  for (cnt=0; cnt < rl.count; cnt++)
  {
    if ((rl.rect[cnt].w == 1) && (rl.rect[cnt].h == 1))
    {
      putpixel(vscreen, rl.rect[cnt].x-map_x[1], rl.rect[cnt].y-map_y[1], 0);
    }
    else
    {
      rectfill(vscreen, rl.rect[cnt].x-map_x[1], rl.rect[cnt].y-map_y[1],
               rl.rect[cnt].x-map_x[1] + rl.rect[cnt].w,
               rl.rect[cnt].y-map_y[1] + rl.rect[cnt].h, 0);
    }
  }


  orl = rl;
  rl.count = 0;
}

void clear_explosions()
{
  struct ENODE *temp, *next_explosion;
  BITMAP *bit;

  temp = first_explosion;

  while (temp!=NULL)
  {
    next_explosion = temp->next;

    bit = temp->dat.tempbit;
    if (on_draw_field(temp->dat.x,temp->dat.y,bit->w,bit->h))
      blit(bit,vscreen,0,0,temp->dat.x-map_x[1],temp->dat.y-map_y[1],bit->w,bit->h);

    temp = next_explosion;
  } // while
}


