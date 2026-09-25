/*
 *    GRAVITY FORCE, gfedmain.c
 *
 *    -- main functions for "editor"
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfedhd.h"

int cur_max_levels;

extern DIALOG main_dialog[];
extern DIALOG ppoints_dialog[];
extern DIALOG bases_dialog[];
extern DIALOG objects_dialog[];
extern DIALOG spobjects_dialog[];
extern DIALOG enemies_dialog[];

void add_2_list(RLIST *l, int x, int y, int w, int h)
{

  if (l->count+1 < MAX_RECTANGLES)
  {
    l->rect[l->count].x = x;
    l->rect[l->count].y = y;
    l->rect[l->count].w = w;
    l->rect[l->count].h = h;
    l->count++;
  }
  else
  {
    error_exit(33,"Error in add_2_list","",33,0);
  }
}

void clear_screen()
{
  int cnt;

  for (cnt=0; cnt < rl.count; cnt++)
  {
    if ((rl.rect[cnt].w == 1) && (rl.rect[cnt].h == 1))
    {
      putpixel(vscreen, rl.rect[cnt].x, rl.rect[cnt].y, 0);
    }
    else
    {
//      rectfill(vscreen, rl.rect[cnt].x, rl.rect[cnt].y,
//               rl.rect[cnt].x + rl.rect[cnt].w,
//               rl.rect[cnt].y + rl.rect[cnt].h, 0);
    }
  }

//  blit(playship[0].player_temp,vscreen,0,0,playship[0].xpos-3-map_x[1],playship[0].ypos-3-map_y[1],24,24);

  orl = rl;
  rl.count = 0;
}

char *mitoa(int n)
{
  char tmp[200] = "";

  return itoa(n,tmp,10);
}

char *ftoa(float n)
{
  char tmp[200] = "";

  sprintf(tmp,"%.4f",n);
  return tmp;
}

void draw_text()
{
  text_mode(0);
  textout(score_table,font,ftoa(mox),100,10,col_white);
  textout(score_table,font,ftoa(moy),100,20,col_white);

  if (current_action == PPOINTS)
  {
    textout(score_table,font,ftoa(node_anz),100,40,col_yellow);
  }
  if (current_action == BASES)
  {
    textout(score_table,font,ftoa(base_anz),100,40,col_yellow);
  }
  if (current_action == OBJECTS)
  {
    textout(score_table,font,ftoa(object_anz),100,40,col_yellow);
  }
  if (current_action == SPOBJECTS)
  {
    textout(score_table,font,ftoa(spobject_anz),100,40,col_yellow);
  }
  if (current_action == ENEMIES)
  {
    textout(score_table,font,ftoa(enemy_anz),100,40,col_yellow);
  }

}


void map_stuff()
{
    int w,h,s;

    w = PLAYSCREEN_WIDTH;
    h = PLAYSCREEN_HEIGHT;

    if (key_shifts & KB_SHIFT_FLAG) s = 6; else s = 3;

    if ( (scroll_x[1] + map_x[1] + s < 100*48-PLAYSCREEN_WIDTH+s) && key[KEY_RIGHT])  scroll_x[1]+=s;
    if ( ((scroll_x[1] >= 2) || (map_x[1] >= 2)) && (key[KEY_LEFT]))  scroll_x[1]-=s;
    if ( ((scroll_y[1] >= 2) || (map_y[1] >= 2)) && (key[KEY_UP]))    scroll_y[1]-=s;
    if ( (scroll_y[1] + map_y[1] + s < 100*48-PLAYSCREEN_HEIGHT+s) && key[KEY_DOWN])  scroll_y[1]+=s;


    if (scroll_x[1] >= PLUS_X)
    {
      map_x[1] += PLUS_X;
      redraw = TRUE;
      scroll_x[1] = 0;
    }
    else if (scroll_x[1] < 0)
    {
      if (map_x[1] >= PLUS_X)
      {
        map_x[1] -= PLUS_X;
        if (map_x[1] < 0) map_x[1] = 0;
        redraw = TRUE;
        scroll_x[1] = PLUS_X;
      }
    }

    if (scroll_y[1] >= PLUS_Y)
    {
      map_y[1] += PLUS_Y;
      redraw = TRUE;
      scroll_y[1] = 0;
    }
    else if (scroll_y[1] < 0)
    {
      if (map_y[1] >= PLUS_Y)
      {
        map_y[1] -= PLUS_Y;
        if (map_y[1] < 0) map_y[1] = 0;
        redraw = TRUE;
        scroll_y[1] = PLUS_Y;
      }
    }

}

void update_screen_map()
{
  int w,h;

  w = (map_x[1] + PLAYSCREEN_WIDTH+PLUS_X > MAP_WIDTH*TILE_W) ?
      MAP_WIDTH*TILE_W - map_x[1] :
      PLAYSCREEN_WIDTH+PLUS_X;

  h = (map_y[1] + PLAYSCREEN_HEIGHT+PLUS_Y > MAP_HEIGHT*TILE_H) ?
      MAP_HEIGHT*TILE_H - map_y[1] :
      PLAYSCREEN_HEIGHT+PLUS_Y;

  MapDraw8BG(vscreen,map_x[1],map_y[1],0,0,w,h);
  make_base_room(vscreen,1);
}

int main(void)
{
  int ret;
  void *player;
  void *ppoints_pl;
  void *bases_pl;
  void *enemies_pl;
  void *objects_pl;
  void *spobjects_pl;

  init();

  map_x[1] = map_y[1] = scroll_x[1] = scroll_y[1] = 0;

  redraw = TRUE;
  show_mouse(screen);

  MapDraw8BG(vscreen,map_x[1],map_y[1],0,0,PLAYSCREEN_WIDTH+PLUS_X,PLAYSCREEN_HEIGHT+PLUS_Y);

  player = init_dialog(main_dialog, 0);
  ppoints_pl = init_dialog(ppoints_dialog, 0);
  bases_pl = init_dialog(bases_dialog, 0);
  objects_pl = init_dialog(objects_dialog, 0);
  spobjects_pl = init_dialog(spobjects_dialog, 0);
  enemies_pl = init_dialog(enemies_dialog, 0);

  current_action = -1;

  while (!key[KEY_ESC])
  {
    do_dlg();

    mox = mouse_x + scroll_x[1] + map_x[1];
    moy = mouse_y + scroll_y[1] + map_y[1];

    draw_text();

    switch (current_action)
    {
      case PPOINTS : update_dialog(ppoints_pl);
                     scare_mouse(); draw_nodes(); unscare_mouse();
                     break;
      case BASES   : update_dialog(bases_pl);
                     bases_drawbase();
                     break;
      case OBJECTS : update_dialog(objects_pl);
                     objects_drawobjects();
                     break;
      case SPOBJECTS: update_dialog(spobjects_pl);
                     spobjects_drawobjects();
                     break;
      case ENEMIES : update_dialog(enemies_pl);
                     enemies_drawenemy();
                     break;
      default      : update_dialog(player);
    }

    map_stuff(0);

    if (redraw)
    {
      scare_mouse();
      update_screen_map();
      redraw = FALSE;
    }

    scare_mouse();
    draw_bases();
    draw_objects();
    draw_spobjects();
    draw_enemies();
    if (key[KEY_P]) draw_nodes();
 
    blit(vscreen,screen,scroll_x[1],scroll_y[1],PLAYSCREEN_XSTART,PLAYSCREEN_YSTART,PLAYSCREEN_WIDTH,PLAYSCREEN_HEIGHT);
    blit(score_table,screen,0,0,420,SCREEN_H-SCORE_HEIGHT,220,SCORE_HEIGHT);
    set_clip(screen,0,0,640,480-SCORE_HEIGHT-1);
    if (mouse_on_screen())
    {
      masked_blit(temp_sprite,screen,0,0,mouse_x,mouse_y,temp_sprite->w,temp_sprite->h);
    }
    set_clip(screen,0,0,640,480);

    if ((mouse_on_screen()) && !(current_action == PPOINTS))
      show_mouse(NULL);
    else
      show_mouse(screen);

    clear(temp_sprite);

    clear_screen();
    if (redraw)
    {
       vsync();
    }

  }

  exit(0);
}
