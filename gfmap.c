/*
 *    GRAVITY FORCE, gfmap.c
 *
 *    -- handling of map movement and scrolling, make_levelmap() has some
 *       serious bugs (not used in game)
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfmap.h"
#include "gfmisc.h"
#include "gfdraw.h"
#include "gf.h"
#include "mappyal.h"

void map_stuff(int c)
{
    int w,h,px,py;

    w = PLAYSCREEN_WIDTH;
    h = PLAYSCREEN_HEIGHT;

    px = PLUS_X;
    py = PLUS_Y;

    // Sonderfall: "Sprung" an Mapgrenzen
    if (playship[c].xpos > map_x[1] + scroll_x[1] + px) { map_x[1] = (((int)(playship[c].xpos / px))-1) * px; scroll_x[1] = (MAP_WIDTH*TILE_W)-map_x[1]-px+TILE_W; }
    if (playship[c].ypos > map_y[1] + scroll_y[1] + py) { map_y[1] = (((int)(playship[c].ypos / py))-1) * py; scroll_y[1] = (MAP_WIDTH*TILE_H)-map_y[1]-py+TILE_H; }
    if (playship[c].ypos < map_y[1]) { map_y[1] = 0; scroll_y[1] = 0; }

    if ((playship[c].xpos >= w/2) && (playship[c].xpos <= MAP_WIDTH*TILE_W - w/2))
      scroll_x[1]=playship[c].xpos - map_x[1] - w/2;
    else if (playship[c].xpos <= w/2)
      scroll_x[1]=0;

    if ((playship[c].ypos >= h/2) && (playship[c].ypos <= MAP_HEIGHT*TILE_H - h/2))
      scroll_y[1]=playship[c].ypos - map_y[1] - h/2;
    else if (playship[c].ypos <= h/2)
      scroll_y[1]=0;

    if (scroll_x[1] >= px)
    {
      map_x[1] += px;
      redraw = TRUE;
      scroll_x[1] = 0;
    }
    else if (scroll_x[1] < 0)
    {
      if (map_x[1] >= px)
      {
        map_x[1] -= px;
        if (map_x[1] < 0) map_x[1] = 0;
        redraw = TRUE;
        scroll_x[1] = px;
      }
    }

    if (scroll_y[1] >= py)
    {
      map_y[1] += py;
      redraw = TRUE;
      scroll_y[1] = 0;
    }
    else if (scroll_y[1] < 0)
    {
      if (map_y[1] >= py)
      {
        map_y[1] -= py;
        if (map_y[1] < 0) map_y[1] = 0;
        redraw = TRUE;
        scroll_y[1] = py;
      }
    }
}

void make_levelmap()
{
  BITMAP *tempb,*tscreen;
  char tmp[50];
  int zoom = 11;
  int maxw = 640*5;
  int maxh = 480*8;
  int tw = TILE_W*15, th = TILE_H*15;
  int x,y,nr;

  while (!key[KEY_ESC])
  {
    clear(screen);

    tempb = create_bitmap(tw+1,th+1);
    tscreen = create_bitmap(maxw/zoom,maxh/zoom);
    clear(tscreen);

    for (x=0; x < maxw/tw; x++)
      for (y=0; y < maxh/th; y++)
      {
        clear(tempb);
        MapDraw8BG(tempb,x*tw,y*th,0,0,tw,th);
        stretch_sprite(tscreen, tempb, (x*tw)/zoom, (y*th)/zoom, tw/zoom+1, th/zoom+1);
      }

    blit(tscreen,screen,0,0,0,0,640,480);
    textout(screen,font,mitoa(tmp,zoom),550,400,col_white);
    textout(screen,font,"+/- zoom",550,412,col_white);
    textout(screen,font,"F6  save",550,424,col_white);
    textout(screen,font,"ESC back",550,436,col_white);

    // Spielerposition
    _putpixel(screen,playship[1].xpos/zoom-1,playship[1].ypos/zoom-1,col_yellow);
    _putpixel(screen,playship[1].xpos/zoom+1,playship[1].ypos/zoom-1,col_yellow);
    _putpixel(screen,playship[1].xpos/zoom,playship[1].ypos/zoom,col_yellow);
    _putpixel(screen,playship[1].xpos/zoom-1,playship[1].ypos/zoom+1,col_yellow);
    _putpixel(screen,playship[1].xpos/zoom+1,playship[1].ypos/zoom+1,col_yellow);

    // Gegner
    for (nr=0; nr < enemy_anz; nr++)
      if (enemy[nr].active)
      {
        putpixel(screen,enemy[nr].xpos/zoom-1,enemy[nr].ypos/zoom-1,col_orange);
        putpixel(screen,enemy[nr].xpos/zoom+1,enemy[nr].ypos/zoom-1,col_orange);
        putpixel(screen,enemy[nr].xpos/zoom,enemy[nr].ypos/zoom,col_orange);
        putpixel(screen,enemy[nr].xpos/zoom-1,enemy[nr].ypos/zoom+1,col_orange);
        putpixel(screen,enemy[nr].xpos/zoom+1,enemy[nr].ypos/zoom+1,col_orange);
      }

    // Objekte
    for (nr=0; nr < object_anz; nr++)
      if (object[nr].active)
      {
        putpixel(screen,object[nr].xpos/zoom-1,object[nr].ypos/zoom-1,col_blue);
        putpixel(screen,object[nr].xpos/zoom+1,object[nr].ypos/zoom-1,col_blue);
        putpixel(screen,object[nr].xpos/zoom,object[nr].ypos/zoom,col_blue);
        putpixel(screen,object[nr].xpos/zoom-1,object[nr].ypos/zoom+1,col_blue);
        putpixel(screen,object[nr].xpos/zoom+1,object[nr].ypos/zoom+1,col_blue);
      }
    for (nr=0; nr < spobject_anz; nr++)
      if (spobject[nr].active)
      {
        putpixel(screen,spobject[nr].xpos/zoom-1,spobject[nr].ypos/zoom-1,col_lightblue);
        putpixel(screen,spobject[nr].xpos/zoom+1,spobject[nr].ypos/zoom-1,col_lightblue);
        putpixel(screen,spobject[nr].xpos/zoom,spobject[nr].ypos/zoom,col_lightblue);
        putpixel(screen,spobject[nr].xpos/zoom-1,spobject[nr].ypos/zoom+1,col_lightblue);
        putpixel(screen,spobject[nr].xpos/zoom+1,spobject[nr].ypos/zoom+1,col_lightblue);
      }

    // Basen
    hline(screen,base[0].x1/zoom,base[0].y/zoom,base[0].x2/zoom,col_yellow);
    for (nr=1; nr < base_anz; nr++)
    {
      hline(screen,base[nr].x1/zoom,base[nr].y/zoom,base[nr].x2/zoom,col_green);
      if (base[nr].cargo)
      {
        putpixel(screen,(base[nr].x1+((base[nr].x2-base[nr].x1)/2))/zoom,(base[nr].y-2)/zoom-1,col_white);
        putpixel(screen,(base[nr].x1+((base[nr].x2-base[nr].x1)/2))/zoom+1,(base[nr].y-2)/zoom-1,col_white);
        putpixel(screen,(base[nr].x1+((base[nr].x2-base[nr].x1)/2))/zoom,(base[nr].y-2)/zoom,col_white);
        putpixel(screen,(base[nr].x1+((base[nr].x2-base[nr].x1)/2))/zoom+1,(base[nr].y-2)/zoom,col_white);
      }

    }

    clear_keybuf(); while (!keypressed());

    if (key[KEY_PLUS_PAD]) { if (zoom < 30) zoom++; clear_keybuf(); }
    if (key[KEY_MINUS_PAD]) { if (zoom > 2) zoom--; clear_keybuf(); }

    if (key[KEY_F6])
    {
      textout(screen,font,"saving",550,448,col_white);
      save_tga("map.tga",tscreen,data[gamepal].dat); clear_keybuf();
    }

    if (mapbit) destroy_bitmap(mapbit);
    mapbit = create_bitmap(maxw/zoom+1,maxh/zoom+1);
    blit(tscreen,mapbit,0,0,0,0,maxw/zoom,maxh/zoom);

    destroy_bitmap(tempb); destroy_bitmap(tscreen);
    
  } // while !esc

  clear_keybuf();

}

void update_screen_map()
{
  int w,h,ph,pw;

  pw = PLAYSCREEN_WIDTH;

  if (show_panel)
  {
    ph = PLAYSCREEN_HEIGHT;
  }
  else
  {
    ph = PLAYSCREEN_HEIGHT+SCORE_HEIGHT+USCORE_HEIGHT;
  }

  w = (map_x[1] + pw+PLUS_X > MAP_WIDTH*TILE_W) ?
      MAP_WIDTH*TILE_W - map_x[1] :
      pw+PLUS_X;

  h = (map_y[1] + ph+PLUS_Y > MAP_HEIGHT*TILE_H) ?
      MAP_HEIGHT*TILE_H - map_y[1] :
      ph+PLUS_Y;

  MapDraw8BG(vscreen,map_x[1],map_y[1],0,0,w,h);
  make_base_room(vscreen,0);
  draw_craters(vscreen,0);
}

int do_scroll()
{
  int cx,cy,tmp;

  if (scroll_to_x && scroll_to_y && scroll_speed)
  {
    cx = scroll_x[1] + map_x[1];
    cy = scroll_y[1] + map_y[1];

    if (cx < scroll_to_x-scroll_speed)
      cx += scroll_speed;
    else if (cx < scroll_to_x)
      cx = scroll_to_x;

    if (cy < scroll_to_y-scroll_speed)
      cy += scroll_speed;
    else if (cy < scroll_to_y)
      cy = scroll_to_y;

    if (cx > scroll_to_x+scroll_speed)
      cx -= scroll_speed;
    else if (cx > scroll_to_x)
      cx = scroll_to_x;

    if (cy > scroll_to_y+scroll_speed)
      cy -= scroll_speed;
    else if (cy > scroll_to_y)
      cy = scroll_to_y;

    // get scroll_x/y and map_x/y out of cx/y
    tmp = map_x[1];
    map_x[1]    = ((int)(cx / PLUS_X)) * PLUS_X;
    if (map_x[1] != tmp) redraw = TRUE;
    scroll_x[1] = cx % PLUS_X;

    tmp = map_y[1];
    map_y[1]    = ((int)(cy / PLUS_Y)) * PLUS_Y;
    if (map_y[1] != tmp) redraw = TRUE;
    scroll_y[1] = cy % PLUS_Y;

    // abort when target reached
    if (cx == scroll_to_x && cy == scroll_to_y)
    {
      scroll_to_x = scroll_to_y = scroll_speed = 0;
      return 1;
    }
  }

  return 0;
}

void scroll_to(int x, int y, int speed)
{
  scroll_to_x = x;
  scroll_to_y = y;
  scroll_speed = speed;
}

