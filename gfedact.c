/*
 *    GRAVITY FORCE, gfedact.c
 *
 *    -- main handling functions for "editor"
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfedhd.h"

extern DIALOG main_dialog[];
extern DIALOG ppoints_dialog[];
extern DIALOG bases_dialog[];
extern DIALOG objects_dialog[];
extern DIALOG enemies_dialog[];

static int pnt_na[200];


int get_nearest_node(int x, int y)
{
  int nr;
  int rl=50,rr=50,ru=50,rd=50;

  for (nr=1; nr < node_anz; nr++)
  {
    if (x < path_node[nr].x+rr && x > path_node[nr].x-rl &&
        y < path_node[nr].y+rd && y > path_node[nr].y-ru && !pnt_na[nr])
    {
      pnt_na[nr] = 1;
      return nr;
    }
  }

  return 0;
}

int mouse_on_screen()
{
  if ( (mouse_x > 0) && (mouse_x < PLAYSCREEN_WIDTH) &&
       (mouse_y > 0) && (mouse_y < PLAYSCREEN_HEIGHT) )
  {
    return TRUE;
  }

  return FALSE;
}


// PPOINTS............................................................

void ppoints_addpnts()
{
  if ((mouse_b & 1) && (mouse_on_screen()))
  {
    node_anz++;
    path_node[node_anz].x = mox;
    path_node[node_anz].y = moy;
  }

  while (mouse_b & 1);
}

void ppoints_write()
{
  FILE *tmp_file;
  int nr;

  tmp_file = fopen(".\\maps\\gfdat.pnt","at");

  for (nr=1;nr<=node_anz;nr++)
    fprintf(tmp_file,
            "  path_node[%i].x = %i; path_node[%i].y = %i;\n  path_node[%i].connect[0] = %i;\n  path_node[%i].connect[1] = %i;\n  path_node[%i].connect[2] = %i;\n",
            nr, path_node[nr].x, nr, path_node[nr].y, nr, nr-1, nr, nr+1, nr, nr+2);

  fclose(tmp_file);
}


// BASES...............................................................

void bases_drawbase()
{
  BITMAP *dat = data[base_home].dat;

  if (mouse_on_screen())
  {
    draw_rle_sprite(temp_sprite,data[cur_base].dat,0,0);
  }

}

void bases_addbase()
{
  BITMAP *tmpb;

  if ((mouse_b & 1) && (mouse_on_screen()))
  {
    switch (cur_base)
    {
      case base_home  : tmpb = data[base_home].dat;
                        base[base_anz].x1 = mox + PLBASE_TOWER_W;
                        base[base_anz].x2 = base[base_anz].x1 + BASE_LANDING_W - 11;
                        base[base_anz].y  = moy + tmpb->h - BASE_LANDING_H;
                        base[base_anz].type = PLAYER_BASE;
                        break;
      case base_cargo : tmpb = data[base_cargo].dat;
                        base[base_anz].x1 = mox+4;
                        base[base_anz].x2 = base[base_anz].x1 + BASE_LANDING_W - 9;
                        base[base_anz].y  = moy + tmpb->h - BASE_LANDING_H;
                        base[base_anz].cargo = 0;
                        base[base_anz].type = CARGO_BASE;
                        break;
      case base_senemy: tmpb = data[base_senemy].dat;
                        base[base_anz].x1 = mox+2;
                        base[base_anz].x2 = base[base_anz].x1 + 33;
                        base[base_anz].y  = moy + tmpb->h - 1;
                        base[base_anz].cargo = 0;
                        base[base_anz].type = ENEMY_S_BASE;
                        break;
      case base_lenemy: tmpb = data[base_lenemy].dat;
                        base[base_anz].x1 = mox+2;
                        base[base_anz].x2 = base[base_anz].x1 + 67;
                        base[base_anz].y  = moy + tmpb->h - 1;
                        base[base_anz].cargo = 0;
                        base[base_anz].type = ENEMY_L_BASE;
                        break;
    }
    base_anz++;

  }

  while (mouse_b & 1);

  if ((mouse_b & 2) && (mouse_on_screen()) && (!(key_shifts & KB_SHIFT_FLAG)))
  {
    switch (cur_base)
    {
      case base_cargo   : cur_base = base_home;
                          break;
      case base_home    : cur_base = base_senemy;
                          break;
      case base_senemy  : cur_base = base_lenemy;
                          break;
      case base_lenemy  : cur_base = base_cargo;
                          break;
    }

  }
  else if ((mouse_b & 2) && (mouse_on_screen()) && (key_shifts & KB_SHIFT_FLAG))
  {
    switch (cur_base)
    {
      case base_cargo   : cur_base = base_lenemy;
                          break;
      case base_home    : cur_base = base_cargo;
                          break;
      case base_senemy  : cur_base = base_home;
                          break;
      case base_lenemy  : cur_base = base_senemy;
                          break;
    }

  }

  while (mouse_b & 2);

}

void bases_write()
{
  FILE *tmp_file;
  int nr;

  tmp_file = fopen(".\\maps\\gfdat.bas","at");

  for (nr=0;nr<=base_anz;nr++)
  {
    fprintf(tmp_file,
            "  base[%i].x1 = %i; base[%i].x2 = %i; base[%i].y = %i;\n  base[%i].type = %i; base[%i].cargo = %i;\n\n",
            nr, base[nr].x1, nr, base[nr].x2, nr, base[nr].y, nr, base[nr].type, nr, base[nr].cargo);
  }

  fclose(tmp_file);
}


// objects...............................................................

void objects_drawobjects()
{
  if (mouse_on_screen())
  {
    draw_rle_sprite(temp_sprite,data[cur_object].dat,0,0);
  }

}

void objects_addobject()
{
  BITMAP *tmpb;

  if ((mouse_b & 1) && (mouse_on_screen()))
  {
    switch (cur_object)
    {
      case pile1      : tmpb = data[pile1].dat;
                        object[object_anz].type = PILE;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].maxframe = 10;
                        object[object_anz].maxframet = 6;
                        object[object_anz].image.data = pile1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_building1_1: tmpb = data[obj_building1_1].dat;
                        object[object_anz].type = BUILDING1;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].maxframe = 1;
                        object[object_anz].maxframet = 6;
                        object[object_anz].image.data = obj_building1_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_building2_1: tmpb = data[obj_building2_1].dat;
                        object[object_anz].type = BUILDING2;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].maxframe = 1;
                        object[object_anz].maxframet = 6;
                        object[object_anz].image.data = obj_building2_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_building3_1: tmpb = data[obj_building3_1].dat;
                        object[object_anz].type = BUILDING3;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].maxframe = 1;
                        object[object_anz].maxframet = 6;
                        object[object_anz].image.data = obj_building3_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_building4_1: tmpb = data[obj_building4_1].dat;
                        object[object_anz].type = BUILDING4;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].maxframe = 1;
                        object[object_anz].maxframet = 6;
                        object[object_anz].image.data = obj_building4_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_building4_2: tmpb = data[obj_building4_2].dat;
                        object[object_anz].type = BUILDING4_2;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].maxframe = 1;
                        object[object_anz].maxframet = 6;
                        object[object_anz].image.data = obj_building4_2;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_b_grav_1 : tmpb = data[obj_b_grav_1].dat;
                        object[object_anz].type = BUILDING_GRAV;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].maxframe = 2;
                        object[object_anz].frame = 0;
                        object[object_anz].maxframet = 5;
                        object[object_anz].image.data = obj_b_grav_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_tunnel1_1: tmpb = data[obj_tunnel1_1].dat;
                        object[object_anz].type = TUNNEL1;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].maxframe = 5;
                        object[object_anz].maxframet = 6;
                        object[object_anz].image.data = obj_tunnel1_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_man1_1     : tmpb = data[obj_man1_1].dat;
                        object[object_anz].type = MAN1;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].maxframe = 3;
                        object[object_anz].maxframet = 7;
                        object[object_anz].image.data = obj_man1_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_doorh_1 : tmpb = data[obj_doorh_1].dat;
                        object[object_anz].type = DOORH;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].startframe = 1;
                        object[object_anz].maxframe = 3;
                        object[object_anz].maxframet = 8;
                        object[object_anz].image.data = obj_doorh_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_doorv_1 : tmpb = data[obj_doorv_1].dat;
                        object[object_anz].type = DOORV;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].startframe = 1;
                        object[object_anz].maxframe = 3;
                        object[object_anz].maxframet = 8;
                        object[object_anz].image.data = obj_doorv_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_doorh2_1 : tmpb = data[obj_doorh2_1].dat;
                        object[object_anz].type = DOORH2;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].startframe = 1;
                        object[object_anz].maxframe = 3;
                        object[object_anz].maxframet = 8;
                        object[object_anz].image.data = obj_doorh2_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_doorv2_1 : tmpb = data[obj_doorv2_1].dat;
                        object[object_anz].type = DOORV2;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].startframe = 1;
                        object[object_anz].maxframe = 3;
                        object[object_anz].maxframet = 8;
                        object[object_anz].image.data = obj_doorv2_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_switchl_1: tmpb = data[obj_switchl_1].dat;
                        object[object_anz].type = SWITCHL;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].frame = 0;
                        object[object_anz].image.data = obj_switchl_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_switchr_1: tmpb = data[obj_switchr_1].dat;
                        object[object_anz].type = SWITCHR;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].frame = 0;
                        object[object_anz].image.data = obj_switchr_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_switchu_1: tmpb = data[obj_switchu_1].dat;
                        object[object_anz].type = SWITCHU;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].frame = 0;
                        object[object_anz].image.data = obj_switchu_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_switchd_1: tmpb = data[obj_switchd_1].dat;
                        object[object_anz].type = SWITCHD;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].frame = 0;
                        object[object_anz].image.data = obj_switchd_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_pool1_1    : tmpb = data[obj_pool1_1].dat;
                        object[object_anz].type = POOL1;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].maxframe = 3;
                        object[object_anz].maxframet = 15;
                        object[object_anz].image.data = obj_pool1_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_pool2_1    : tmpb = data[obj_pool2_1].dat;
                        object[object_anz].type = POOL2;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].maxframe = 3;
                        object[object_anz].maxframet = 15;
                        object[object_anz].image.data = obj_pool2_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_ppile1     : tmpb = data[obj_ppile1].dat;
                        object[object_anz].type = PPILE1;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].maxframe = 1;
                        object[object_anz].maxframet = 6;
                        object[object_anz].image.data = obj_ppile1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_ppile2     : tmpb = data[obj_ppile2].dat;
                        object[object_anz].type = PPILE2;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].maxframe = 1;
                        object[object_anz].maxframet = 6;
                        object[object_anz].image.data = obj_ppile2;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_rpolel_1 : tmpb = data[obj_rpolel_1].dat;
                        object[object_anz].type = RPOLEL;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].startframe = 1;
                        object[object_anz].maxframe = 5;
                        object[object_anz].maxframet = 8;
                        object[object_anz].image.data = obj_rpolel_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_rpoler_1 : tmpb = data[obj_rpoler_1].dat;
                        object[object_anz].type = RPOLER;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].startframe = 1;
                        object[object_anz].maxframe = 5;
                        object[object_anz].maxframet = 8;
                        object[object_anz].image.data = obj_rpoler_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_rpoleu_1 : tmpb = data[obj_rpoleu_1].dat;
                        object[object_anz].type = RPOLEU;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].startframe = 1;
                        object[object_anz].maxframe = 5;
                        object[object_anz].maxframet = 8;
                        object[object_anz].image.data = obj_rpoleu_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_rpoled_1 : tmpb = data[obj_rpoled_1].dat;
                        object[object_anz].type = RPOLED;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].startframe = 1;
                        object[object_anz].maxframe = 5;
                        object[object_anz].maxframet = 8;
                        object[object_anz].image.data = obj_rpoled_1;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;
      case obj_tree_2 : tmpb = data[obj_tree_2].dat;
                        object[object_anz].type = TREE;
                        object[object_anz].xpos = mox;
                        object[object_anz].ypos = moy;
                        object[object_anz].image.data = obj_tree_2;
                        object[object_anz].image.width = tmpb->w;
                        object[object_anz].image.height = tmpb->h;
                        object[object_anz].active = TRUE;
                        break;

    }

    object_anz++;
  }

  while (mouse_b & 1);

  if ((mouse_b & 2) && (mouse_on_screen()) && (!(key_shifts & KB_SHIFT_FLAG)))
  {
    switch (cur_object)
    {
      case pile1        : cur_object = obj_building1_1;
                          break;
      case obj_building1_1  : cur_object = obj_building2_1;
                          break;
      case obj_building2_1  : cur_object = obj_building3_1;
                          break;
      case obj_building3_1  : cur_object = obj_building4_1;
                          break;
      case obj_building4_1  : cur_object = obj_building4_2;
                          break;
      case obj_building4_2  : cur_object = obj_b_grav_1;
                          break;
      case obj_b_grav_1    : cur_object = obj_tunnel1_1;
                          break;
      case obj_tunnel1_1 : cur_object = obj_pool1_1;
                          break;
      case obj_pool1_1   : cur_object = obj_pool2_1;
                          break;
      case obj_pool2_1   : cur_object = obj_ppile1;
                          break;
      case obj_ppile1    : cur_object = obj_ppile2;
                          break;
      case obj_ppile2    : cur_object = obj_man1_1;
                          break;
      case obj_man1_1    : cur_object = obj_doorh_1;
                          break;
      case obj_doorh_1   : cur_object = obj_doorv_1;
                          break;
      case obj_doorv_1   : cur_object = obj_doorh2_1;
                          break;
      case obj_doorh2_1  : cur_object = obj_doorv2_1;
                          break;
      case obj_doorv2_1  : cur_object = obj_switchl_1;
                          break;
      case obj_switchl_1 : cur_object = obj_switchr_1;
                          break;
      case obj_switchr_1 : cur_object = obj_switchu_1;
                          break;
      case obj_switchu_1 : cur_object = obj_switchd_1;
                          break;
      case obj_switchd_1 : cur_object = obj_rpolel_1;
                          break;
      case obj_rpolel_1  : cur_object = obj_rpoler_1;
                          break;
      case obj_rpoler_1  : cur_object = obj_rpoleu_1;
                          break;
      case obj_rpoleu_1  : cur_object = obj_rpoled_1;
                          break;
      case obj_rpoled_1  : cur_object = obj_tree_2;
                          break;
      case obj_tree_2    : cur_object = pile1;
                          break;

    }

  }
  else if ((mouse_b & 2) && (mouse_on_screen()) && (key_shifts & KB_SHIFT_FLAG))
  {
    switch (cur_object)
    {
      case pile1        : cur_object = obj_tree_2;
                          break;
      case obj_building1_1  : cur_object = pile1;
                          break;
      case obj_building2_1  : cur_object = obj_building1_1;
                          break;
      case obj_building3_1  : cur_object = obj_building2_1 ;
                          break;
      case obj_building4_1  : cur_object = obj_building3_1;
                          break;
      case obj_building4_2 : cur_object = obj_building4_1;
                          break;
      case obj_b_grav_1 : cur_object = obj_building4_2;
                          break;
      case obj_tunnel1_1 : cur_object = obj_b_grav_1;
                          break;
      case obj_pool1_1   : cur_object = obj_tunnel1_1;
                          break;
      case obj_pool2_1   : cur_object = obj_pool1_1;
                          break;
      case obj_ppile1    : cur_object = obj_pool2_1;
                          break;
      case obj_ppile2    : cur_object = obj_ppile1;
                          break;
      case obj_man1_1    : cur_object = obj_ppile2;
                          break;
      case obj_doorh_1   : cur_object = obj_man1_1;
                          break;
      case obj_doorv_1   : cur_object = obj_doorh_1;
                          break;
      case obj_doorh2_1   : cur_object = obj_doorv_1;
                          break;
      case obj_doorv2_1   : cur_object = obj_doorh2_1;
                          break;
      case obj_switchl_1 : cur_object = obj_doorv2_1 ;
                          break;
      case obj_switchr_1 : cur_object = obj_switchl_1;
                          break;
      case obj_switchu_1 : cur_object = obj_switchr_1;
                          break;
      case obj_switchd_1 : cur_object = obj_switchu_1;
                          break;
      case obj_rpolel_1  : cur_object = obj_switchd_1;
                          break;
      case obj_rpoler_1  : cur_object = obj_rpolel_1;
                          break;
      case obj_rpoleu_1  : cur_object = obj_rpoler_1;
                          break;
      case obj_rpoled_1  : cur_object = obj_rpoleu_1;
                          break;
      case obj_tree_2    : cur_object = obj_rpoled_1;
                          break;

    }

  }

  while (mouse_b & 2);

}

void objects_write()
{
  FILE *tmp_file;
  int nr;

  tmp_file = fopen(".\\maps\\gfdat.obj","at");

  for (nr=0;nr<=object_anz;nr++)
  {
    switch (object[nr].type)
    {
      case PILE   :
                     fprintf(tmp_file,
                             "  object[%i].type = PILE;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].maxframe = 10; object[%i].maxframet = 6; object[%i].frame = %i; object[%i].maxhit = 10;\n",
                             nr, nr, nr, rand() % 10, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = pile1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case BUILDING1   :
                     fprintf(tmp_file,
                             "  object[%i].type = BUILDING1;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].maxframe = 1; object[%i].maxframet = 1; object[%i].maxhit = 1000;\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_building1_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case BUILDING2   :
                     fprintf(tmp_file,
                             "  object[%i].type = BUILDING2;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].maxframe = 1; object[%i].maxframet = 1; object[%i].maxhit = 1000;\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_building2_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case BUILDING3   :
                     fprintf(tmp_file,
                             "  object[%i].type = BUILDING3;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].maxframe = 1; object[%i].maxframet = 1; object[%i].maxhit = 1000;\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_building3_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case BUILDING4   :
                     fprintf(tmp_file,
                             "  object[%i].type = BUILDING4;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].maxframe = 1; object[%i].maxframet = 1; object[%i].maxhit = 1000;\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_building4_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case BUILDING4_2   :
                     fprintf(tmp_file,
                             "  object[%i].type = BUILDING4_2;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].maxframe = 1; object[%i].maxframet = 1; object[%i].maxhit = 1000;\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_building4_2;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case BUILDING_GRAV  :
                     fprintf(tmp_file,
                             "  object[%i].type = BUILDING_GRAV;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].maxframe = 2; object[%i].frame = 0; object[%i].maxframet = 5; object[%i].maxhit = 2000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_b_grav_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case TUNNEL1   :
                     fprintf(tmp_file,
                             "  object[%i].type = TUNNEL1;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].maxframe = 4; object[%i].maxframet = 12; object[%i].maxhit = 200;\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_tunnel1_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case MAN1        :
                     fprintf(tmp_file,
                             "  object[%i].type = MAN1;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].maxframe = 3; object[%i].maxframet = 7; object[%i].maxhit = 1;\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_man1_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case DOORH     :
                     fprintf(tmp_file,
                             "  object[%i].type = DOORH;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].startframe = 1; object[%i].maxframe = 3; object[%i].maxframet = 8; object[%i].maxhit = 5000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_doorh_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case DOORV     :
                     fprintf(tmp_file,
                             "  object[%i].type = DOORV;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].startframe = 1; object[%i].maxframe = 3; object[%i].maxframet = 8; object[%i].maxhit = 5000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_doorv_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case DOORH2    :
                     fprintf(tmp_file,
                             "  object[%i].type = DOORH2;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].startframe = 1; object[%i].maxframe = 3; object[%i].maxframet = 8; object[%i].maxhit = 5000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_doorh2_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case DOORV2    :
                     fprintf(tmp_file,
                             "  object[%i].type = DOORV2;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].startframe = 1; object[%i].maxframe = 3; object[%i].maxframet = 8; object[%i].maxhit = 5000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_doorv2_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case SWITCHL     :
                     fprintf(tmp_file,
                             "  object[%i].type = SWITCHL;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].frame = 0; object[%i].maxhit = 5000;\n",
                             nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_switchl_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case SWITCHR     :
                     fprintf(tmp_file,
                             "  object[%i].type = SWITCHR;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].frame = 0; object[%i].maxhit = 5000;\n",
                             nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_switchr_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case SWITCHU     :
                     fprintf(tmp_file,
                             "  object[%i].type = SWITCHU;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].frame = 0; object[%i].maxhit = 5000;\n",
                             nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_switchu_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case SWITCHD     :
                     fprintf(tmp_file,
                             "  object[%i].type = SWITCHD;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].frame = 0; object[%i].maxhit = 5000;\n",
                             nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_switchd_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case POOL1       :
                     fprintf(tmp_file,
                             "  object[%i].type = POOL1;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].maxframe = 3; object[%i].maxframet = 15; object[%i].maxhit = 3000;\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_pool1_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case POOL2       :
                     fprintf(tmp_file,
                             "  object[%i].type = POOL2;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].maxframe = 3; object[%i].maxframet = 15; object[%i].maxhit = 3000;\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_pool2_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case PPILE1      :
                     fprintf(tmp_file,
                             "  object[%i].type = PPILE1;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].maxframe = 1; object[%i].maxframet = 1; object[%i].maxhit = 500;\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_ppile1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case PPILE2      :
                     fprintf(tmp_file,
                             "  object[%i].type = PPILE2;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].maxframe = 1; object[%i].maxframet = 1; object[%i].maxhit = 500;\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_ppile2;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case RPOLEL    :
                     fprintf(tmp_file,
                             "  object[%i].type = RPOLEL;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].startframe = 0; object[%i].maxframe = 4; object[%i].maxframet = 8; object[%i].maxhit = 5000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_rpolel_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case RPOLER    :
                     fprintf(tmp_file,
                             "  object[%i].type = RPOLER;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].startframe = 0; object[%i].maxframe = 4; object[%i].maxframet = 8; object[%i].maxhit = 5000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_rpoler_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case RPOLEU    :
                     fprintf(tmp_file,
                             "  object[%i].type = RPOLEU;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].startframe = 0; object[%i].maxframe = 4; object[%i].maxframet = 8; object[%i].maxhit = 5000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_rpoleu_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case RPOLED    :
                     fprintf(tmp_file,
                             "  object[%i].type = RPOLED;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].startframe = 0; object[%i].maxframe = 4; object[%i].maxframet = 8; object[%i].maxhit = 5000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_rpoled_1;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;
      case TREE      :
                     fprintf(tmp_file,
                             "  object[%i].type = TREE;\n",
                             nr);
                     fprintf(tmp_file,
                             "  object[%i].xpos = %.0f; object[%i].ypos = %.0f;\n",
                             nr,object[nr].xpos,nr,object[nr].ypos);
                     fprintf(tmp_file,
                             "  object[%i].image.data = obj_tree_2;\n  object[%i].image.width = %i; object[%i].image.height = %i;\n  object[%i].active = TRUE;\n\n",
                             nr, nr, object[nr].image.width, nr, object[nr].image.height, nr);
                     break;

    }

  }

  fclose(tmp_file);
}

// spobjects ...........................................................

void spobjects_drawobjects()
{
  if (mouse_on_screen())
  {
    draw_rle_sprite(temp_sprite,data[cur_spobject].dat,0,0);
  }

}

void spobjects_addobject()
{
  BITMAP *tmpb;

  if ((mouse_b & 1) && (mouse_on_screen()))
  {
    switch (cur_spobject)
    {
      case effect_tele_01 : tmpb = data[effect_tele_01].dat;
                        spobject[spobject_anz].type = TELEPORT;
                        spobject[spobject_anz].xpos = mox;
                        spobject[spobject_anz].ypos = moy;
                        spobject[spobject_anz].maxframe = 10;
                        spobject[spobject_anz].maxframet = 6;
                        spobject[spobject_anz].image.data = effect_tele_01;
                        spobject[spobject_anz].image.width = tmpb->w;
                        spobject[spobject_anz].image.height = tmpb->h;
                        spobject[spobject_anz].active = TRUE;
                        break;
      case effect_tele2_01 : tmpb = data[effect_tele2_01].dat;
                        spobject[spobject_anz].type = TELEPORT2;
                        spobject[spobject_anz].xpos = mox;
                        spobject[spobject_anz].ypos = moy;
                        spobject[spobject_anz].maxframe = 9;
                        spobject[spobject_anz].maxframet = 6;
                        spobject[spobject_anz].image.data = effect_tele2_01;
                        spobject[spobject_anz].image.width = tmpb->w;
                        spobject[spobject_anz].image.height = tmpb->h;
                        spobject[spobject_anz].active = TRUE;
                        break;
      case obj_gwalll_1 : tmpb = data[obj_gwalll_1].dat;
                        spobject[spobject_anz].type = GWALL_L;
                        spobject[spobject_anz].xpos = mox;
                        spobject[spobject_anz].ypos = moy;
                        spobject[spobject_anz].startframe = 1;
                        spobject[spobject_anz].maxframe = 3;
                        spobject[spobject_anz].maxframet = 6;
                        spobject[spobject_anz].image.data = obj_gwalll_1;
                        spobject[spobject_anz].image.width = tmpb->w;
                        spobject[spobject_anz].image.height = tmpb->h;
                        spobject[spobject_anz].active = TRUE;
                        break;
      case obj_gwallr_1 : tmpb = data[obj_gwallr_1].dat;
                        spobject[spobject_anz].type = GWALL_R;
                        spobject[spobject_anz].xpos = mox;
                        spobject[spobject_anz].ypos = moy;
                        spobject[spobject_anz].startframe = 1;
                        spobject[spobject_anz].maxframe = 3;
                        spobject[spobject_anz].maxframet = 6;
                        spobject[spobject_anz].image.data = obj_gwallr_1;
                        spobject[spobject_anz].image.width = tmpb->w;
                        spobject[spobject_anz].image.height = tmpb->h;
                        spobject[spobject_anz].active = TRUE;
                        break;
      case obj_gwallu_1 : tmpb = data[obj_gwallu_1].dat;
                        spobject[spobject_anz].type = GWALL_U;
                        spobject[spobject_anz].xpos = mox;
                        spobject[spobject_anz].ypos = moy;
                        spobject[spobject_anz].startframe = 1;
                        spobject[spobject_anz].maxframe = 3;
                        spobject[spobject_anz].maxframet = 6;
                        spobject[spobject_anz].image.data = obj_gwallu_1;
                        spobject[spobject_anz].image.width = tmpb->w;
                        spobject[spobject_anz].image.height = tmpb->h;
                        spobject[spobject_anz].active = TRUE;
                        break;
      case obj_gwalld_1 : tmpb = data[obj_gwalld_1].dat;
                        spobject[spobject_anz].type = GWALL_D;
                        spobject[spobject_anz].xpos = mox;
                        spobject[spobject_anz].ypos = moy;
                        spobject[spobject_anz].startframe = 1;
                        spobject[spobject_anz].maxframe = 3;
                        spobject[spobject_anz].maxframet = 6;
                        spobject[spobject_anz].image.data = obj_gwalld_1;
                        spobject[spobject_anz].image.width = tmpb->w;
                        spobject[spobject_anz].image.height = tmpb->h;
                        spobject[spobject_anz].active = TRUE;
                        break;
      case obj_bumpu_1 : tmpb = data[obj_bumpu_1].dat;
                        spobject[spobject_anz].type = BUMPU;
                        spobject[spobject_anz].xpos = mox;
                        spobject[spobject_anz].ypos = moy;
                        spobject[spobject_anz].startframe = 0;
                        spobject[spobject_anz].maxframe = 0;
                        spobject[spobject_anz].maxframet = 0;
                        spobject[spobject_anz].image.data = obj_bumpu_1;
                        spobject[spobject_anz].image.width = tmpb->w;
                        spobject[spobject_anz].image.height = tmpb->h;
                        spobject[spobject_anz].active = TRUE;
                        break;
      case obj_bumpd_1 : tmpb = data[obj_bumpd_1].dat;
                        spobject[spobject_anz].type = BUMPD;
                        spobject[spobject_anz].xpos = mox;
                        spobject[spobject_anz].ypos = moy;
                        spobject[spobject_anz].startframe = 0;
                        spobject[spobject_anz].maxframe = 0;
                        spobject[spobject_anz].maxframet = 0;
                        spobject[spobject_anz].image.data = obj_bumpd_1;
                        spobject[spobject_anz].image.width = tmpb->w;
                        spobject[spobject_anz].image.height = tmpb->h;
                        spobject[spobject_anz].active = TRUE;
                        break;
      case obj_bumpl_1 : tmpb = data[obj_bumpl_1].dat;
                        spobject[spobject_anz].type = BUMPL;
                        spobject[spobject_anz].xpos = mox;
                        spobject[spobject_anz].ypos = moy;
                        spobject[spobject_anz].startframe = 0;
                        spobject[spobject_anz].maxframe = 0;
                        spobject[spobject_anz].maxframet = 0;
                        spobject[spobject_anz].image.data = obj_bumpl_1;
                        spobject[spobject_anz].image.width = tmpb->w;
                        spobject[spobject_anz].image.height = tmpb->h;
                        spobject[spobject_anz].active = TRUE;
                        break;
      case obj_bumpr_1 : tmpb = data[obj_bumpr_1].dat;
                        spobject[spobject_anz].type = BUMPR;
                        spobject[spobject_anz].xpos = mox;
                        spobject[spobject_anz].ypos = moy;
                        spobject[spobject_anz].startframe = 0;
                        spobject[spobject_anz].maxframe = 0;
                        spobject[spobject_anz].maxframet = 0;
                        spobject[spobject_anz].image.data = obj_bumpr_1;
                        spobject[spobject_anz].image.width = tmpb->w;
                        spobject[spobject_anz].image.height = tmpb->h;
                        spobject[spobject_anz].active = TRUE;
                        break;

    }
    spobject_anz++;
  }

  while (mouse_b & 1);

  if ((mouse_b & 2) && (mouse_on_screen()) && (!(key_shifts & KB_SHIFT_FLAG)))
  {
    switch (cur_spobject)
    {
      case effect_tele_01   : cur_spobject = effect_tele2_01;
                          break;
      case effect_tele2_01   : cur_spobject = obj_gwalll_1;
                          break;
      case obj_gwalll_1  : cur_spobject = obj_gwallr_1;
                          break;
      case obj_gwallr_1  : cur_spobject = obj_gwallu_1;
                          break;
      case obj_gwallu_1  : cur_spobject = obj_gwalld_1;
                          break;
      case obj_gwalld_1  : cur_spobject = obj_bumpu_1;
                          break;
      case obj_bumpu_1   : cur_spobject = obj_bumpd_1;
                          break;
      case obj_bumpd_1   : cur_spobject = obj_bumpl_1;
                          break;
      case obj_bumpl_1   : cur_spobject = obj_bumpr_1;
                          break;
      case obj_bumpr_1   : cur_spobject = effect_tele_01;
                          break;
    }
  }
  else if ((mouse_b & 2) && (mouse_on_screen()) && (key_shifts & KB_SHIFT_FLAG))
  {
    switch (cur_spobject)
    {
      case effect_tele_01   : cur_spobject = obj_bumpr_1;
                          break;
      case effect_tele2_01   : cur_spobject = effect_tele_01;
                          break;
      case obj_gwalll_1  : cur_spobject = effect_tele2_01;
                          break;
      case obj_gwallr_1  : cur_spobject = obj_gwalll_1;
                          break;
      case obj_gwallu_1  : cur_spobject = obj_gwallr_1;
                          break;
      case obj_gwalld_1  : cur_spobject = obj_gwallu_1;
                          break;
      case obj_bumpu_1   : cur_spobject = obj_gwalld_1;
                          break;
      case obj_bumpd_1   : cur_spobject = obj_bumpu_1;
                          break;
      case obj_bumpl_1   : cur_spobject = obj_bumpd_1;
                          break;
      case obj_bumpr_1   : cur_spobject = obj_bumpl_1;
                          break;

    }

  }

  while (mouse_b & 2);

}

void spobjects_write()
{
  FILE *tmp_file;
  int nr;

  tmp_file = fopen(".\\maps\\gfdat.obs","at");

  for (nr=0;nr<=spobject_anz;nr++)
  {
    switch (spobject[nr].type)
    {
      case TELEPORT :
                     fprintf(tmp_file,
                             "  spobject[%i].type = TELEPORT;\n",
                             nr);
                     fprintf(tmp_file,
                             "  spobject[%i].xpos = %.0f; spobject[%i].ypos = %.0f;\n",
                             nr,spobject[nr].xpos,nr,spobject[nr].ypos);
                     fprintf(tmp_file,
                             "  spobject[%i].maxframe = 9; spobject[%i].maxframet = 6;\n",
                             nr, nr);
                     fprintf(tmp_file,
                             "  spobject[%i].image.data = effect_tele_01;\n  spobject[%i].image.width = %i; spobject[%i].image.height = %i;\n  spobject[%i].active = TRUE;\n",
                             nr, nr, spobject[nr].image.width, nr, spobject[nr].image.height, nr);
                     fprintf(tmp_file,
                             "  // spobject[%i].min_x = 0; spobject[%i].min_y = 0;\n  spobject[%i].connect = 0;\n\n",nr,nr,nr);
                     break;
      case TELEPORT2 :
                     fprintf(tmp_file,
                             "  spobject[%i].type = TELEPORT2;\n",
                             nr);
                     fprintf(tmp_file,
                             "  spobject[%i].xpos = %.0f; spobject[%i].ypos = %.0f;\n",
                             nr,spobject[nr].xpos,nr,spobject[nr].ypos);
                     fprintf(tmp_file,
                             "  spobject[%i].maxframe = 9; spobject[%i].maxframet = 6;\n",
                             nr, nr);
                     fprintf(tmp_file,
                             "  spobject[%i].image.data = effect_tele2_01;\n  spobject[%i].image.width = %i; spobject[%i].image.height = %i;\n  spobject[%i].active = TRUE;\n",
                             nr, nr, spobject[nr].image.width, nr, spobject[nr].image.height, nr);
                     fprintf(tmp_file,
                             "  // spobject[%i].min_x = 0; spobject[%i].min_y = 0;\n  spobject[%i].connect = 0;\n\n",nr,nr,nr);
                     break;
      case GWALL_L   :
                     fprintf(tmp_file,
                             "  spobject[%i].type = GWALL_L;\n",
                             nr);
                     fprintf(tmp_file,
                             "  spobject[%i].xpos = %.0f; spobject[%i].ypos = %.0f;\n",
                             nr,spobject[nr].xpos,nr,spobject[nr].ypos);
                     fprintf(tmp_file,
                             "  spobject[%i].startframe = 1; spobject[%i].maxframe = 3; spobject[%i].maxhit = 5000;\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  spobject[%i].powr = 20; spobject[%i].maxframet = 25-(spobject[%i].powr/2);\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  spobject[%i].image.data = obj_gwalll_1;\n  spobject[%i].image.width = %i; spobject[%i].image.height = %i;\n  spobject[%i].active = TRUE;\n\n",
                             nr, nr, spobject[nr].image.width, nr, spobject[nr].image.height, nr);
                     break;
      case GWALL_R   :
                     fprintf(tmp_file,
                             "  spobject[%i].type = GWALL_R;\n",
                             nr);
                     fprintf(tmp_file,
                             "  spobject[%i].xpos = %.0f; spobject[%i].ypos = %.0f;\n",
                             nr,spobject[nr].xpos,nr,spobject[nr].ypos);
                     fprintf(tmp_file,
                             "  spobject[%i].startframe = 1; spobject[%i].maxframe = 3; spobject[%i].maxframet = 6; spobject[%i].maxhit = 5000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  spobject[%i].powr = 20; spobject[%i].maxframet = 25-(spobject[%i].powr/2);\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  spobject[%i].image.data = obj_gwallr_1;\n  spobject[%i].image.width = %i; spobject[%i].image.height = %i;\n  spobject[%i].active = TRUE;\n\n",
                             nr, nr, spobject[nr].image.width, nr, spobject[nr].image.height, nr);
                     break;
      case GWALL_U   :
                     fprintf(tmp_file,
                             "  spobject[%i].type = GWALL_U;\n",
                             nr);
                     fprintf(tmp_file,
                             "  spobject[%i].xpos = %.0f; spobject[%i].ypos = %.0f;\n",
                             nr,spobject[nr].xpos,nr,spobject[nr].ypos);
                     fprintf(tmp_file,
                             "  spobject[%i].startframe = 1; spobject[%i].maxframe = 3; spobject[%i].maxframet = 6; spobject[%i].maxhit = 5000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  spobject[%i].powr = 20; spobject[%i].maxframet = 25-(spobject[%i].powr/2);\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  spobject[%i].image.data = obj_gwallu_1;\n  spobject[%i].image.width = %i; spobject[%i].image.height = %i;\n  spobject[%i].active = TRUE;\n\n",
                             nr, nr, spobject[nr].image.width, nr, spobject[nr].image.height, nr);
                     break;
      case GWALL_D   :
                     fprintf(tmp_file,
                             "  spobject[%i].type = GWALL_D;\n",
                             nr);
                     fprintf(tmp_file,
                             "  spobject[%i].xpos = %.0f; spobject[%i].ypos = %.0f;\n",
                             nr,spobject[nr].xpos,nr,spobject[nr].ypos);
                     fprintf(tmp_file,
                             "  spobject[%i].startframe = 1; spobject[%i].maxframe = 3; spobject[%i].maxframet = 6; spobject[%i].maxhit = 5000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  spobject[%i].powr = 20; spobject[%i].maxframet = 25-(spobject[%i].powr/2);\n",
                             nr, nr, nr);
                     fprintf(tmp_file,
                             "  spobject[%i].image.data = obj_gwalld_1;\n  spobject[%i].image.width = %i; spobject[%i].image.height = %i;\n  spobject[%i].active = TRUE;\n\n",
                             nr, nr, spobject[nr].image.width, nr, spobject[nr].image.height, nr);
                     break;

      case BUMPU   :
                     fprintf(tmp_file,
                             "  spobject[%i].type = BUMPU;\n",
                             nr);
                     fprintf(tmp_file,
                             "  spobject[%i].xpos = %.0f; spobject[%i].ypos = %.0f;\n",
                             nr,spobject[nr].xpos,nr,spobject[nr].ypos);
                     fprintf(tmp_file,
                             "  spobject[%i].startframe = 0; spobject[%i].maxframe = 0; spobject[%i].maxframet = 0; spobject[%i].maxhit = 30000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  spobject[%i].image.data = obj_bumpu_1;\n  spobject[%i].image.width = %i; spobject[%i].image.height = %i;\n  spobject[%i].active = TRUE;\n\n",
                             nr, nr, spobject[nr].image.width, nr, spobject[nr].image.height, nr);
                     break;

      case BUMPD   :
                     fprintf(tmp_file,
                             "  spobject[%i].type = BUMPD;\n",
                             nr);
                     fprintf(tmp_file,
                             "  spobject[%i].xpos = %.0f; spobject[%i].ypos = %.0f;\n",
                             nr,spobject[nr].xpos,nr,spobject[nr].ypos);
                     fprintf(tmp_file,
                             "  spobject[%i].startframe = 0; spobject[%i].maxframe = 0; spobject[%i].maxframet = 0; spobject[%i].maxhit = 30000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  spobject[%i].image.data = obj_bumpd_1;\n  spobject[%i].image.width = %i; spobject[%i].image.height = %i;\n  spobject[%i].active = TRUE;\n\n",
                             nr, nr, spobject[nr].image.width, nr, spobject[nr].image.height, nr);
                     break;

      case BUMPL   :
                     fprintf(tmp_file,
                             "  spobject[%i].type = BUMPL;\n",
                             nr);
                     fprintf(tmp_file,
                             "  spobject[%i].xpos = %.0f; spobject[%i].ypos = %.0f;\n",
                             nr,spobject[nr].xpos,nr,spobject[nr].ypos);
                     fprintf(tmp_file,
                             "  spobject[%i].startframe = 0; spobject[%i].maxframe = 0; spobject[%i].maxframet = 0; spobject[%i].maxhit = 30000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  spobject[%i].image.data = obj_bumpl_1;\n  spobject[%i].image.width = %i; spobject[%i].image.height = %i;\n  spobject[%i].active = TRUE;\n\n",
                             nr, nr, spobject[nr].image.width, nr, spobject[nr].image.height, nr);
                     break;

      case BUMPR   :
                     fprintf(tmp_file,
                             "  spobject[%i].type = BUMPR;\n",
                             nr);
                     fprintf(tmp_file,
                             "  spobject[%i].xpos = %.0f; spobject[%i].ypos = %.0f;\n",
                             nr,spobject[nr].xpos,nr,spobject[nr].ypos);
                     fprintf(tmp_file,
                             "  spobject[%i].startframe = 0; spobject[%i].maxframe = 0; spobject[%i].maxframet = 0; spobject[%i].maxhit = 30000;\n",
                             nr, nr, nr, nr);
                     fprintf(tmp_file,
                             "  spobject[%i].image.data = obj_bumpr_1;\n  spobject[%i].image.width = %i; spobject[%i].image.height = %i;\n  spobject[%i].active = TRUE;\n\n",
                             nr, nr, spobject[nr].image.width, nr, spobject[nr].image.height, nr);
                     break;

    }

  }

  fclose(tmp_file);
}


// enemies..............................................................

void enemies_drawenemy()
{
  if (mouse_on_screen())
  {
    draw_rle_sprite(temp_sprite,data[cur_enemy].dat,0,0);
  }

}

void enemies_addenemy()
{
  BITMAP *tmpb;

  if ((mouse_b & 1) && (mouse_on_screen()))
  {
    enemy[enemy_anz].xpos = mox;
    enemy[enemy_anz].ypos = moy;
    enemy[enemy_anz].active = TRUE;

    switch (cur_enemy)
    {
      case shooter_up1     : tmpb = data[shooter_up1].dat;
                             enemy[enemy_anz].type = SHOOTER_UP;
                             enemy[enemy_anz].shootf = 40;
                             enemy[enemy_anz].frame = 0; enemy[enemy_anz].maxframe = 19;
                             enemy[enemy_anz].maxshot = 1; enemy[enemy_anz].maxdead = 10;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].shot[0].xoff = 2; enemy[enemy_anz].shot[0].yoff = 0;
                             enemy[enemy_anz].shot[0].xspd = 0; enemy[enemy_anz].shot[0].yspd = -5;
                             enemy[enemy_anz].shot[0].min_x = mox-10; enemy[enemy_anz].shot[0].max_x = mox+10;
                             enemy[enemy_anz].shot[0].min_y = moy-800; enemy[enemy_anz].shot[0].max_y = moy+10;
                             enemy[enemy_anz].maxhit = 4;
                             enemy[enemy_anz].image.data = shooter_up1;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h;
                             enemy[enemy_anz].react.xw1 = -200; enemy[enemy_anz].react.xw2 = 200;
                             enemy[enemy_anz].react.yw1 = -200; enemy[enemy_anz].react.yw2 = 200;
                             break;
      case shooter_right1  : tmpb = data[shooter_right1].dat;
                             enemy[enemy_anz].type = SHOOTER_RIGHT;
                             enemy[enemy_anz].shootf = 40;
                             enemy[enemy_anz].frame = 0; enemy[enemy_anz].maxframe = 19;
                             enemy[enemy_anz].maxshot = 1; enemy[enemy_anz].maxdead = 10;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].shot[0].xoff = 9; enemy[enemy_anz].shot[0].yoff = 2;
                             enemy[enemy_anz].shot[0].xspd = 3; enemy[enemy_anz].shot[0].yspd = 0;
                             enemy[enemy_anz].shot[0].min_x = mox-10; enemy[enemy_anz].shot[0].max_x = mox+800;
                             enemy[enemy_anz].shot[0].min_y = moy-10; enemy[enemy_anz].shot[0].max_y = moy+10;
                             enemy[enemy_anz].maxhit = 4;
                             enemy[enemy_anz].image.data = shooter_right1;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h;
                             enemy[enemy_anz].react.xw1 = -200; enemy[enemy_anz].react.xw2 = 200;
                             enemy[enemy_anz].react.yw1 = -200; enemy[enemy_anz].react.yw2 = 200;
                             break;
      case shooter_left1   : tmpb = data[shooter_left1].dat;
                             enemy[enemy_anz].type = SHOOTER_LEFT;
                             enemy[enemy_anz].shootf = 40;
                             enemy[enemy_anz].frame = 0; enemy[enemy_anz].maxframe = 19;
                             enemy[enemy_anz].maxshot = 1; enemy[enemy_anz].maxdead = 10;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].shot[0].xoff = 1; enemy[enemy_anz].shot[0].yoff = 2;
                             enemy[enemy_anz].shot[0].xspd = -3; enemy[enemy_anz].shot[0].yspd = 0;
                             enemy[enemy_anz].shot[0].min_x = mox-800; enemy[enemy_anz].shot[0].max_x = mox+10;
                             enemy[enemy_anz].shot[0].min_y = moy-10; enemy[enemy_anz].shot[0].max_y = moy+10;
                             enemy[enemy_anz].maxhit = 4;
                             enemy[enemy_anz].image.data = shooter_left1;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h;
                             enemy[enemy_anz].react.xw1 = -200; enemy[enemy_anz].react.xw2 = 200;
                             enemy[enemy_anz].react.yw1 = -200; enemy[enemy_anz].react.yw2 = 200;
                             break;
      case shooter_down1   : tmpb = data[shooter_down1].dat;
                             enemy[enemy_anz].type = SHOOTER_DOWN;
                             enemy[enemy_anz].shootf = 40;
                             enemy[enemy_anz].frame = 0; enemy[enemy_anz].maxframe = 19;
                             enemy[enemy_anz].maxshot = 1; enemy[enemy_anz].maxdead = 10;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].shot[0].xoff = 2; enemy[enemy_anz].shot[0].yoff = 9;
                             enemy[enemy_anz].shot[0].xspd = 0; enemy[enemy_anz].shot[0].yspd = 3;
                             enemy[enemy_anz].shot[0].min_x = mox-10; enemy[enemy_anz].shot[0].max_x = mox+10;
                             enemy[enemy_anz].shot[0].min_y = moy-20; enemy[enemy_anz].shot[0].max_y = moy+800;
                             enemy[enemy_anz].maxhit = 4;
                             enemy[enemy_anz].image.data = shooter_down1;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h;
                             enemy[enemy_anz].react.xw1 = -200; enemy[enemy_anz].react.xw2 = 200;
                             enemy[enemy_anz].react.yw1 = -200; enemy[enemy_anz].react.yw2 = 200;
                             break;
      case shooter1_1      : tmpb = data[shooter1_1].dat;
                             enemy[enemy_anz].type = THREE_SHOOTER;
                             enemy[enemy_anz].shootf = 220;
                             enemy[enemy_anz].frame = enemy[enemy_anz].maxframe = 0;
                             enemy[enemy_anz].maxshot = 3; enemy[enemy_anz].maxdead = 10;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].shot[0].xoff = 3; enemy[enemy_anz].shot[0].yoff = 0;
                             enemy[enemy_anz].shot[0].xspd = 0; enemy[enemy_anz].shot[0].yspd = -1.5;
                             enemy[enemy_anz].shot[1].xoff = 3; enemy[enemy_anz].shot[1].yoff = 0;
                             enemy[enemy_anz].shot[1].xspd = 1; enemy[enemy_anz].shot[1].yspd = -0.7;
                             enemy[enemy_anz].shot[2].xoff = 3; enemy[enemy_anz].shot[2].yoff = 0;
                             enemy[enemy_anz].shot[2].xspd = -1; enemy[enemy_anz].shot[2].yspd = -0.7;
                             enemy[enemy_anz].shot[0].min_x = mox-10; enemy[enemy_anz].shot[0].max_x = mox+10;
                             enemy[enemy_anz].shot[0].min_y = moy-400; enemy[enemy_anz].shot[0].max_y = moy+10;
                             enemy[enemy_anz].shot[1].min_x = mox-10; enemy[enemy_anz].shot[1].max_x = mox+200;
                             enemy[enemy_anz].shot[1].min_y = moy-400; enemy[enemy_anz].shot[1].max_y = moy+10;
                             enemy[enemy_anz].shot[2].min_x = mox-100; enemy[enemy_anz].shot[2].max_x = mox+10;
                             enemy[enemy_anz].shot[2].min_y = moy-400; enemy[enemy_anz].shot[2].max_y = moy+10;
                             enemy[enemy_anz].maxhit = 5;
                             enemy[enemy_anz].image.data = shooter1_1;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h-1;
                             enemy[enemy_anz].react.xw1 = -300; enemy[enemy_anz].react.xw2 = 300;
                             enemy[enemy_anz].react.yw1 = -300; enemy[enemy_anz].react.yw2 = 100;
                             break;
      case shooter_pulse_1 : tmpb = data[shooter_pulse_1].dat;
                             enemy[enemy_anz].type = PULSE_SHOOTER;
                             enemy[enemy_anz].shootf = 20;
                             enemy[enemy_anz].frame = 0; enemy[enemy_anz].maxframe = 19;
                             enemy[enemy_anz].maxshot = 3; enemy[enemy_anz].maxdead = 10;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].shot[0].xoff = 2; enemy[enemy_anz].shot[0].yoff = 2;
                             enemy[enemy_anz].shot[0].xspd = 2; enemy[enemy_anz].shot[0].yspd = -4;
                             enemy[enemy_anz].shot[0].min_x = mox-400; enemy[enemy_anz].shot[0].max_x = mox+400;
                             enemy[enemy_anz].shot[0].min_y = moy-500; enemy[enemy_anz].shot[0].max_y = moy+20;
                             enemy[enemy_anz].shot[1].xoff = 6; enemy[enemy_anz].shot[1].yoff = 0;
                             enemy[enemy_anz].shot[1].xspd = 8; enemy[enemy_anz].shot[1].yspd = -4;
                             enemy[enemy_anz].shot[1].min_x = mox-400; enemy[enemy_anz].shot[1].max_x = mox+400;
                             enemy[enemy_anz].shot[1].min_y = moy-500; enemy[enemy_anz].shot[1].max_y = moy+20;
                             enemy[enemy_anz].shot[2].xoff = 10; enemy[enemy_anz].shot[2].yoff = 2;
                             enemy[enemy_anz].shot[2].xspd = 4; enemy[enemy_anz].shot[2].yspd = -4;
                             enemy[enemy_anz].shot[2].min_x = mox-400; enemy[enemy_anz].shot[2].max_x = mox+400;
                             enemy[enemy_anz].shot[2].min_y = moy-500; enemy[enemy_anz].shot[2].max_y = moy+20;
                             enemy[enemy_anz].maxhit = 20;
                             enemy[enemy_anz].image.data = shooter_pulse_1;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h;
                             enemy[enemy_anz].react.xw1 = -350; enemy[enemy_anz].react.xw2 = 350;
                             enemy[enemy_anz].react.yw1 = -200; enemy[enemy_anz].react.yw2 = 200;
                             break;
      case enemy_s1_01     : tmpb = data[enemy_s1_01].dat;
                             enemy[enemy_anz].type = ENEMY_SHIP_1;
                             enemy[enemy_anz].shootf = 100;
                             enemy[enemy_anz].frame = 0; enemy[enemy_anz].maxframe = 10;
                             enemy[enemy_anz].framet = 0; enemy[enemy_anz].maxframet = 20;
                             enemy[enemy_anz].maxshot = 1; enemy[enemy_anz].maxdead = 10;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].shot[0].min_x = mox-600; enemy[enemy_anz].shot[0].max_x = mox+600;
                             enemy[enemy_anz].shot[0].min_y = moy-400; enemy[enemy_anz].shot[0].max_y = moy+400;
                             enemy[enemy_anz].maxhit = 10;
                             enemy[enemy_anz].image.data = enemy_s1_01;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h;
                             enemy[enemy_anz].image.boundxa = 5;
                             enemy[enemy_anz].image.boundya = 3;
                             enemy[enemy_anz].image.boundxe = tmpb->w-10;
                             enemy[enemy_anz].image.boundye = tmpb->h-6;
                             enemy[enemy_anz].waypnt[1].x = path_node[1].x; enemy[enemy_anz].waypnt[1].y = path_node[1].y; enemy[enemy_anz].waypnt[1].s = 150;
                             enemy[enemy_anz].cur_waypnt = 1; enemy[enemy_anz].max_waypnt = 1;
                             enemy[enemy_anz].react.xw1 = -500; enemy[enemy_anz].react.xw2 = 500;
                             enemy[enemy_anz].react.yw1 = -500; enemy[enemy_anz].react.yw2 = 500;
                             break;
      case enemy_s2_01     : tmpb = data[enemy_s2_01].dat;
                             enemy[enemy_anz].type = ENEMY_SHIP_2;
                             enemy[enemy_anz].shootf = 100;
                             enemy[enemy_anz].frame = 0; enemy[enemy_anz].maxframe = 10;
                             enemy[enemy_anz].framet = 0; enemy[enemy_anz].maxframet = 20;
                             enemy[enemy_anz].maxshot = 0; enemy[enemy_anz].maxdead = 10;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].maxhit = 200;
                             enemy[enemy_anz].image.data = enemy_s2_01;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h;
                             enemy[enemy_anz].image.boundxa = 5;
                             enemy[enemy_anz].image.boundya = 3;
                             enemy[enemy_anz].image.boundxe = tmpb->w-10;
                             enemy[enemy_anz].image.boundye = tmpb->h-6;
                             enemy[enemy_anz].waypnt[1].x = path_node[1].x; enemy[enemy_anz].waypnt[1].y = path_node[1].y; enemy[enemy_anz].waypnt[1].s = 150;
                             enemy[enemy_anz].cur_waypnt = 1; enemy[enemy_anz].max_waypnt = 1;
                             enemy[enemy_anz].react.xw1 = -500; enemy[enemy_anz].react.xw2 = 500;
                             enemy[enemy_anz].react.yw1 = -500; enemy[enemy_anz].react.yw2 = 500;
                             break;
      case enemy_s3_01     : tmpb = data[enemy_s3_01].dat;
                             enemy[enemy_anz].type = ENEMY_SHIP_3;
                             enemy[enemy_anz].shootf = 6000;
                             enemy[enemy_anz].frame = 5; enemy[enemy_anz].maxframe = 8;
                             enemy[enemy_anz].framet = 0; enemy[enemy_anz].maxframet = 20;
                             enemy[enemy_anz].startframe = 1; enemy[enemy_anz].shootr = TRUE;
                             enemy[enemy_anz].maxshot = 1; enemy[enemy_anz].maxdead = 10;
                             enemy[enemy_anz].shot[0].min_x = mox-700; enemy[enemy_anz].shot[0].max_x = mox+700;
                             enemy[enemy_anz].shot[0].min_y = moy-700; enemy[enemy_anz].shot[0].max_y = moy+700;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].maxhit = 30;
                             enemy[enemy_anz].image.data = enemy_s3_06;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h;
                             enemy[enemy_anz].image.boundxa = 5;
                             enemy[enemy_anz].image.boundya = 3;
                             enemy[enemy_anz].image.boundxe = tmpb->w-10;
                             enemy[enemy_anz].image.boundye = tmpb->h-6;
                             enemy[enemy_anz].react.xw1 = -500; enemy[enemy_anz].react.xw2 = 500;
                             enemy[enemy_anz].react.yw1 = -500; enemy[enemy_anz].react.yw2 = 500;
                             enemy[enemy_anz].max_waypnt = 3; enemy[enemy_anz].cur_waypnt = 1;
                             enemy[enemy_anz].nwaypnt[1] = 1;
                             enemy[enemy_anz].nwaypnt[2] = 2;
                             enemy[enemy_anz].nwaypnt[3] = 3;
                             enemy[enemy_anz].cur_point = 250;
                             enemy[enemy_anz].dest_point = 0;
                             enemy[enemy_anz].real_waypnt = 0;
                             enemy[enemy_anz].curviness = ftofix(0.15);
                             enemy[enemy_anz].bezier = TRUE;
                             enemy[enemy_anz].chase = TRUE;
                             enemy[enemy_anz].xspd = 2;
                             enemy[enemy_anz].active = TRUE;
                             break;
      case enemy_s4_01     : tmpb = data[enemy_s4_01].dat;
                             enemy[enemy_anz].type = ENEMY_SHIP_4;
                             enemy[enemy_anz].shootf = 2000;
                             enemy[enemy_anz].frame = 0; enemy[enemy_anz].maxframe = 17;
                             enemy[enemy_anz].framet = 0; enemy[enemy_anz].maxframet = 8;
                             enemy[enemy_anz].startframe = 1; enemy[enemy_anz].shootr = TRUE;
                             enemy[enemy_anz].maxshot = 1; enemy[enemy_anz].maxdead = 10;
                             enemy[enemy_anz].shot[0].min_x = mox-700; enemy[enemy_anz].shot[0].max_x = mox+700;
                             enemy[enemy_anz].shot[0].min_y = moy-700; enemy[enemy_anz].shot[0].max_y = moy+700;
                             enemy[enemy_anz].shot[1].min_x = mox-700; enemy[enemy_anz].shot[1].max_x = mox+700;
                             enemy[enemy_anz].shot[1].min_y = moy-700; enemy[enemy_anz].shot[1].max_y = moy+700;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].maxhit = 30;
                             enemy[enemy_anz].image.data = enemy_s4_01;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h;
                             enemy[enemy_anz].image.boundxa = 3;
                             enemy[enemy_anz].image.boundya = 2;
                             enemy[enemy_anz].image.boundxe = tmpb->w-5;
                             enemy[enemy_anz].image.boundye = tmpb->h-3;
                             enemy[enemy_anz].react.xw1 = -500; enemy[enemy_anz].react.xw2 = 500;
                             enemy[enemy_anz].react.yw1 = -500; enemy[enemy_anz].react.yw2 = 500;
                             enemy[enemy_anz].max_waypnt = 3; enemy[enemy_anz].cur_waypnt = 1;
                             enemy[enemy_anz].nwaypnt[1] = 1;
                             enemy[enemy_anz].nwaypnt[2] = 2;
                             enemy[enemy_anz].nwaypnt[3] = 3;
                             enemy[enemy_anz].cur_point = 250;
                             enemy[enemy_anz].dest_point = 0;
                             enemy[enemy_anz].real_waypnt = 0;
                             enemy[enemy_anz].curviness = ftofix(0.15);
                             enemy[enemy_anz].bezier = TRUE;
                             enemy[enemy_anz].chase = TRUE;
                             enemy[enemy_anz].xspd = 2.5;
                             enemy[enemy_anz].active = TRUE;
                             break;
      case enemy_s5_01     : tmpb = data[enemy_s5_01].dat;
                             enemy[enemy_anz].type = ENEMY_SHIP_5;
                             enemy[enemy_anz].frame = 0; enemy[enemy_anz].maxframe = 11;
                             enemy[enemy_anz].shoott = 48; enemy[enemy_anz].shootf = 96;
                             enemy[enemy_anz].framet = 0; enemy[enemy_anz].maxframet = 8;
                             enemy[enemy_anz].maxshot = 10; enemy[enemy_anz].maxdead = 10;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].shot[0].min_x = mox-600; enemy[enemy_anz].shot[0].max_x = mox+600;
                             enemy[enemy_anz].shot[0].min_y = moy-400; enemy[enemy_anz].shot[0].max_y = moy+400;
                             enemy[enemy_anz].maxhit = 50;
                             enemy[enemy_anz].image.data = enemy_s5_01;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h;
                             enemy[enemy_anz].image.boundxa = 8;
                             enemy[enemy_anz].image.boundya = 5;
                             enemy[enemy_anz].image.boundxe = tmpb->w-16;
                             enemy[enemy_anz].image.boundye = tmpb->h-10;
                             enemy[enemy_anz].waypnt[1].x = path_node[1].x; enemy[enemy_anz].waypnt[1].y = path_node[1].y; enemy[enemy_anz].waypnt[1].s = 150;
                             enemy[enemy_anz].cur_waypnt = 1; enemy[enemy_anz].max_waypnt = 1;
                             enemy[enemy_anz].react.xw1 = -700; enemy[enemy_anz].react.xw2 = 700;
                             enemy[enemy_anz].react.yw1 = -700; enemy[enemy_anz].react.yw2 = 700;
                             break;
      case enemy_n1_1      : tmpb = data[enemy_n1_1].dat;
                             enemy[enemy_anz].type = ENEMY_NSHIP_1;
                             enemy[enemy_anz].shootf = 100;
                             enemy[enemy_anz].frame = 0; enemy[enemy_anz].maxframe = 8;
                             enemy[enemy_anz].framet = 0; enemy[enemy_anz].maxframet = 20;
                             enemy[enemy_anz].maxshot = 0; enemy[enemy_anz].maxdead = 10;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].shot[0].min_x = mox-600; enemy[enemy_anz].shot[0].max_x = mox+600;
                             enemy[enemy_anz].shot[0].min_y = moy-400; enemy[enemy_anz].shot[0].max_y = moy+400;
                             enemy[enemy_anz].maxhit = 50;
                             enemy[enemy_anz].image.data = enemy_n1_1;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h;
                             enemy[enemy_anz].image.boundxa = 5;
                             enemy[enemy_anz].image.boundya = 3;
                             enemy[enemy_anz].image.boundxe = tmpb->w-10;
                             enemy[enemy_anz].image.boundye = tmpb->h-6;
                             enemy[enemy_anz].waypnt[1].x = path_node[1].x; enemy[enemy_anz].waypnt[1].y = path_node[1].y; enemy[enemy_anz].waypnt[1].s = 150;
                             enemy[enemy_anz].cur_waypnt = 1; enemy[enemy_anz].max_waypnt = 1;
                             enemy[enemy_anz].react.xw1 = -500; enemy[enemy_anz].react.xw2 = 500;
                             enemy[enemy_anz].react.yw1 = -500; enemy[enemy_anz].react.yw2 = 500;
                             break;
      case obj_gstone_b1  : tmpb = data[obj_gstone_b1].dat;
                             enemy[enemy_anz].type = GSTONE_B1;
                             enemy[enemy_anz].frame = 0; enemy[enemy_anz].maxframe = 0;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].maxhit = 9999;
                             enemy[enemy_anz].image.data = obj_gstone_b1;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h;
                             enemy[enemy_anz].image.boundxa = 3;
                             enemy[enemy_anz].image.boundya = 3;
                             enemy[enemy_anz].image.boundxe = tmpb->w-6;
                             enemy[enemy_anz].image.boundye = tmpb->h-6;
                             enemy[enemy_anz].waypnt[1].x = path_node[1].x; enemy[enemy_anz].waypnt[1].y = path_node[1].y; enemy[enemy_anz].waypnt[1].s = 150;
                             enemy[enemy_anz].cur_waypnt = 1; enemy[enemy_anz].max_waypnt = 1;
                             enemy[enemy_anz].react.xw1 = -500; enemy[enemy_anz].react.xw2 = 500;
                             enemy[enemy_anz].react.yw1 = -500; enemy[enemy_anz].react.yw2 = 500;
                             break;

      case obj_gstone_s1: tmpb = data[obj_gstone_s1].dat;
                             enemy[enemy_anz].type = GSTONE_S1;
                             enemy[enemy_anz].frame = 0; enemy[enemy_anz].maxframe = 0;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].maxhit = 9999;
                             enemy[enemy_anz].image.data = obj_gstone_s1;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h;
                             enemy[enemy_anz].image.boundxa = 3;
                             enemy[enemy_anz].image.boundya = 3;
                             enemy[enemy_anz].image.boundxe = tmpb->w-6;
                             enemy[enemy_anz].image.boundye = tmpb->h-6;
                             enemy[enemy_anz].waypnt[1].x = path_node[1].x; enemy[enemy_anz].waypnt[1].y = path_node[1].y; enemy[enemy_anz].waypnt[1].s = 150;
                             enemy[enemy_anz].cur_waypnt = 1; enemy[enemy_anz].max_waypnt = 1;
                             enemy[enemy_anz].react.xw1 = -500; enemy[enemy_anz].react.xw2 = 500;
                             enemy[enemy_anz].react.yw1 = -500; enemy[enemy_anz].react.yw2 = 500;
                             break;


      case obj_gstone_s2: tmpb = data[obj_gstone_s2].dat;
                             enemy[enemy_anz].type = GSTONE_S2;
                             enemy[enemy_anz].frame = 0; enemy[enemy_anz].maxframe = 0;
                             enemy[enemy_anz].dead = 0;
                             enemy[enemy_anz].maxhit = 9999;
                             enemy[enemy_anz].image.data = obj_gstone_s2;
                             enemy[enemy_anz].image.width = tmpb->w;
                             enemy[enemy_anz].image.height = tmpb->h;
                             enemy[enemy_anz].image.boundxa = 3;
                             enemy[enemy_anz].image.boundya = 3;
                             enemy[enemy_anz].image.boundxe = tmpb->w-6;
                             enemy[enemy_anz].image.boundye = tmpb->h-6;
                             enemy[enemy_anz].waypnt[1].x = path_node[1].x; enemy[enemy_anz].waypnt[1].y = path_node[1].y; enemy[enemy_anz].waypnt[1].s = 150;
                             enemy[enemy_anz].cur_waypnt = 1; enemy[enemy_anz].max_waypnt = 1;
                             enemy[enemy_anz].react.xw1 = -500; enemy[enemy_anz].react.xw2 = 500;
                             enemy[enemy_anz].react.yw1 = -500; enemy[enemy_anz].react.yw2 = 500;
                             break;

    } // switch cur_enemy;

    enemy_anz++;
  }

  while (mouse_b & 1);

  if ((mouse_b & 2) && (mouse_on_screen()) && (!(key_shifts & KB_SHIFT_FLAG)))
  {
    switch (cur_enemy)
    {
      case shooter_up1    : cur_enemy = shooter_right1;
                            break;
      case shooter_right1 : cur_enemy = shooter_down1;
                            break;
      case shooter_down1  : cur_enemy = shooter_left1;
                            break;
      case shooter_left1  : cur_enemy = shooter1_1;
                            break;
      case shooter1_1     : cur_enemy = shooter_pulse_1;
                            break;
      case shooter_pulse_1: cur_enemy = enemy_s1_01;
                            break;
      case enemy_s1_01    : cur_enemy = enemy_s2_01;
                            break;
      case enemy_s2_01    : cur_enemy = enemy_s3_01;
                            break;
      case enemy_s3_01    : cur_enemy = enemy_s4_01;
                            break;
      case enemy_s4_01    : cur_enemy = enemy_s5_01;
                            break;
      case enemy_s5_01    : cur_enemy = enemy_n1_1;
                            break;
      case enemy_n1_1     : cur_enemy = obj_gstone_b1;
                            break;
      case obj_gstone_b1: cur_enemy = obj_gstone_s1;
                            break;
      case obj_gstone_s1: cur_enemy = obj_gstone_s2;
                            break;
      case obj_gstone_s2: cur_enemy = shooter_up1;
                            break;
    }

  }
  else if ((mouse_b & 2) && (mouse_on_screen()) && (key_shifts & KB_SHIFT_FLAG))
  {
    switch (cur_enemy)
    {
      case shooter_up1    : cur_enemy = obj_gstone_s2;
                            break;
      case shooter_right1 : cur_enemy = shooter_up1;
                            break;
      case shooter_down1  : cur_enemy = shooter_right1;
                            break;
      case shooter_left1  : cur_enemy = shooter_down1;
                            break;
      case shooter1_1     : cur_enemy = shooter_left1;
                            break;
      case shooter_pulse_1: cur_enemy = shooter1_1;
                            break;
      case enemy_s1_01    : cur_enemy = shooter_pulse_1;
                            break;
      case enemy_s2_01    : cur_enemy = enemy_s1_01;
                            break;
      case enemy_s3_01    : cur_enemy = enemy_s2_01 ;
                            break;
      case enemy_s4_01    : cur_enemy = enemy_s3_01;
                            break;
      case enemy_s5_01    : cur_enemy = enemy_s4_01;
                            break;
      case enemy_n1_1     : cur_enemy = enemy_s5_01;
                            break;
      case obj_gstone_b1: cur_enemy = enemy_n1_1;
                            break;
      case obj_gstone_s1: cur_enemy = obj_gstone_b1;
                            break;
      case obj_gstone_s2: cur_enemy = obj_gstone_s1;
                            break;

    }

  }

  while (mouse_b & 2);

}

void enemies_write()
{
  FILE *tmp_file;
  int nr,pnr;

  tmp_file = fopen(".\\maps\\gfdat.enm","at");

  memset(&pnt_na,0,sizeof(pnt_na));

  for (nr=0;nr<=enemy_anz;nr++)
  {
    switch (enemy[nr].type)
    {
      case SHOOTER_UP    :
                            fprintf(tmp_file, "  enemy[%i].xpos = %.0f; enemy[%i].ypos = %.0f;\n",nr,enemy[nr].xpos,nr,enemy[nr].ypos);

                            fprintf(tmp_file,"  enemy[%i].type = SHOOTER_UP;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = shooter_up1;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xoff = %i; enemy[%i].shot[0].yoff = %i;\n", nr, enemy[nr].shot[0].xoff, nr, enemy[nr].shot[0].yoff);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xspd = %.1f; enemy[%i].shot[0].yspd = %.1f;\n", nr, enemy[nr].shot[0].xspd, nr, enemy[nr].shot[0].yspd);
                            fprintf(tmp_file,"  enemy[%i].shot[0].min_x = %i; enemy[%i].shot[0].min_y = %i;\n", nr, enemy[nr].shot[0].min_x, nr, enemy[nr].shot[0].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[0].max_x = %i; enemy[%i].shot[0].max_y= %i;\n", nr, enemy[nr].shot[0].max_x, nr, enemy[nr].shot[0].max_y);
                            fprintf(tmp_file,"  enemy[%i].framet = 0; enemy[%i].maxframet = %i;\n",nr,nr, enemy[nr].shootf/enemy[nr].maxframe);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, 0, nr, 0);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.width, nr, enemy[nr].image.height);
                            break;
      case SHOOTER_DOWN  :
                            fprintf(tmp_file, "  enemy[%i].xpos = %.0f; enemy[%i].ypos = %.0f;\n",nr,enemy[nr].xpos,nr,enemy[nr].ypos);

                            fprintf(tmp_file,"  enemy[%i].type = SHOOTER_DOWN;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = shooter_down1;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xoff = %i; enemy[%i].shot[0].yoff = %i;\n", nr, enemy[nr].shot[0].xoff, nr, enemy[nr].shot[0].yoff);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xspd = %.1f; enemy[%i].shot[0].yspd = %.1f;\n", nr, enemy[nr].shot[0].xspd, nr, enemy[nr].shot[0].yspd);
                            fprintf(tmp_file,"  enemy[%i].shot[0].min_x = %i; enemy[%i].shot[0].min_y = %i;\n", nr, enemy[nr].shot[0].min_x, nr, enemy[nr].shot[0].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[0].max_x = %i; enemy[%i].shot[0].max_y= %i;\n", nr, enemy[nr].shot[0].max_x, nr, enemy[nr].shot[0].max_y);
                            fprintf(tmp_file,"  enemy[%i].framet = 0; enemy[%i].maxframet = %i;\n",nr,nr, enemy[nr].shootf/enemy[nr].maxframe);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, 0, nr, 0);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.width, nr, enemy[nr].image.height);
                            break;
      case SHOOTER_LEFT  :
                            fprintf(tmp_file, "  enemy[%i].xpos = %.0f; enemy[%i].ypos = %.0f;\n",nr,enemy[nr].xpos,nr,enemy[nr].ypos);

                            fprintf(tmp_file,"  enemy[%i].type = SHOOTER_LEFT;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = shooter_left1;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xoff = %i; enemy[%i].shot[0].yoff = %i;\n", nr, enemy[nr].shot[0].xoff, nr, enemy[nr].shot[0].yoff);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xspd = %.1f; enemy[%i].shot[0].yspd = %.1f;\n", nr, enemy[nr].shot[0].xspd, nr, enemy[nr].shot[0].yspd);
                            fprintf(tmp_file,"  enemy[%i].shot[0].min_x = %i; enemy[%i].shot[0].min_y = %i;\n", nr, enemy[nr].shot[0].min_x, nr, enemy[nr].shot[0].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[0].max_x = %i; enemy[%i].shot[0].max_y= %i;\n", nr, enemy[nr].shot[0].max_x, nr, enemy[nr].shot[0].max_y);
                            fprintf(tmp_file,"  enemy[%i].framet = 0; enemy[%i].maxframet = %i;\n",nr,nr, enemy[nr].shootf/enemy[nr].maxframe);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, 0, nr, 0);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.width, nr, enemy[nr].image.height);
                            break;
      case SHOOTER_RIGHT :
                            fprintf(tmp_file, "  enemy[%i].xpos = %.0f; enemy[%i].ypos = %.0f;\n",nr,enemy[nr].xpos,nr,enemy[nr].ypos);

                            fprintf(tmp_file,"  enemy[%i].type = SHOOTER_RIGHT;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = shooter_right1;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xoff = %i; enemy[%i].shot[0].yoff = %i;\n", nr, enemy[nr].shot[0].xoff, nr, enemy[nr].shot[0].yoff);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xspd = %.1f; enemy[%i].shot[0].yspd = %.1f;\n", nr, enemy[nr].shot[0].xspd, nr, enemy[nr].shot[0].yspd);
                            fprintf(tmp_file,"  enemy[%i].shot[0].min_x = %i; enemy[%i].shot[0].min_y = %i;\n", nr, enemy[nr].shot[0].min_x, nr, enemy[nr].shot[0].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[0].max_x = %i; enemy[%i].shot[0].max_y= %i;\n", nr, enemy[nr].shot[0].max_x, nr, enemy[nr].shot[0].max_y);
                            fprintf(tmp_file,"  enemy[%i].framet = 0; enemy[%i].maxframet = %i;\n",nr,nr, enemy[nr].shootf/enemy[nr].maxframe);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, 0, nr, 0);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.width, nr, enemy[nr].image.height);
                            break;
      case PULSE_SHOOTER :
                            fprintf(tmp_file, "  enemy[%i].xpos = %.0f; enemy[%i].ypos = %.0f;\n",nr,enemy[nr].xpos,nr,enemy[nr].ypos);

                            fprintf(tmp_file,"  enemy[%i].type = PULSE_SHOOTER;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = shooter_pulse_1;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xoff = %i; enemy[%i].shot[0].yoff = %i;\n", nr, enemy[nr].shot[0].xoff, nr, enemy[nr].shot[0].yoff);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xspd = %.1f; enemy[%i].shot[0].yspd = %.1f;\n", nr, enemy[nr].shot[0].xspd, nr, enemy[nr].shot[0].yspd);
                            fprintf(tmp_file,"  enemy[%i].shot[0].min_x = %i; enemy[%i].shot[0].min_y = %i;\n", nr, enemy[nr].shot[0].min_x, nr, enemy[nr].shot[0].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[0].max_x = %i; enemy[%i].shot[0].max_y= %i;\n", nr, enemy[nr].shot[0].max_x, nr, enemy[nr].shot[0].max_y);
                            fprintf(tmp_file,"  enemy[%i].shot[1].xoff = %i; enemy[%i].shot[1].yoff = %i;\n", nr, enemy[nr].shot[1].xoff, nr, enemy[nr].shot[1].yoff);
                            fprintf(tmp_file,"  enemy[%i].shot[1].xspd = %.1f; enemy[%i].shot[1].yspd = %.1f;\n", nr, enemy[nr].shot[1].xspd, nr, enemy[nr].shot[1].yspd);
                            fprintf(tmp_file,"  enemy[%i].shot[1].min_x = %i; enemy[%i].shot[1].min_y = %i;\n", nr, enemy[nr].shot[1].min_x, nr, enemy[nr].shot[1].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[1].max_x = %i; enemy[%i].shot[1].max_y= %i;\n", nr, enemy[nr].shot[1].max_x, nr, enemy[nr].shot[1].max_y);
                            fprintf(tmp_file,"  enemy[%i].shot[2].xoff = %i; enemy[%i].shot[2].yoff = %i;\n", nr, enemy[nr].shot[2].xoff, nr, enemy[nr].shot[2].yoff);
                            fprintf(tmp_file,"  enemy[%i].shot[2].xspd = %.1f; enemy[%i].shot[2].yspd = %.1f;\n", nr, enemy[nr].shot[2].xspd, nr, enemy[nr].shot[2].yspd);
                            fprintf(tmp_file,"  enemy[%i].shot[2].min_x = %i; enemy[%i].shot[2].min_y = %i;\n", nr, enemy[nr].shot[2].min_x, nr, enemy[nr].shot[2].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[2].max_x = %i; enemy[%i].shot[2].max_y= %i;\n", nr, enemy[nr].shot[2].max_x, nr, enemy[nr].shot[2].max_y);
                            fprintf(tmp_file,"  enemy[%i].framet = 0; enemy[%i].maxframet = %i;\n",nr,nr, enemy[nr].shootf/enemy[nr].maxframe);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, 0, nr, 0);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.width, nr, enemy[nr].image.height);
                            break;
      case THREE_SHOOTER :
                            fprintf(tmp_file, "  enemy[%i].xpos = %.0f; enemy[%i].ypos = %.0f;\n",nr,enemy[nr].xpos,nr,enemy[nr].ypos);
                            fprintf(tmp_file,"  enemy[%i].type = THREE_SHOOTER;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = shooter1_1;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xoff = %i; enemy[%i].shot[0].yoff = %i;\n", nr, enemy[nr].shot[0].xoff, nr, enemy[nr].shot[0].yoff);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xspd = %.1f; enemy[%i].shot[0].yspd = %.1f;\n", nr, enemy[nr].shot[0].xspd, nr, enemy[nr].shot[0].yspd);
                            fprintf(tmp_file,"  enemy[%i].shot[1].xoff = %i; enemy[%i].shot[1].yoff = %i;\n", nr, enemy[nr].shot[1].xoff, nr, enemy[nr].shot[1].yoff);
                            fprintf(tmp_file,"  enemy[%i].shot[1].xspd = %.1f; enemy[%i].shot[1].yspd = %.1f;\n", nr, enemy[nr].shot[1].xspd, nr, enemy[nr].shot[1].yspd);
                            fprintf(tmp_file,"  enemy[%i].shot[2].xoff = %i; enemy[%i].shot[2].yoff = %i;\n", nr, enemy[nr].shot[2].xoff, nr, enemy[nr].shot[2].yoff);
                            fprintf(tmp_file,"  enemy[%i].shot[2].xspd = %.1f; enemy[%i].shot[2].yspd = %.1f;\n", nr, enemy[nr].shot[2].xspd, nr, enemy[nr].shot[2].yspd);
                            fprintf(tmp_file,"  enemy[%i].shot[0].min_x = %i; enemy[%i].shot[0].min_y = %i;\n", nr, enemy[nr].shot[0].min_x, nr, enemy[nr].shot[0].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[0].max_x = %i; enemy[%i].shot[0].max_y= %i;\n", nr, enemy[nr].shot[0].max_x, nr, enemy[nr].shot[0].max_y);
                            fprintf(tmp_file,"  enemy[%i].shot[1].min_x = %i; enemy[%i].shot[1].min_y = %i;\n", nr, enemy[nr].shot[1].min_x, nr, enemy[nr].shot[1].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[1].max_x = %i; enemy[%i].shot[1].max_y= %i;\n", nr, enemy[nr].shot[1].max_x, nr, enemy[nr].shot[1].max_y);
                            fprintf(tmp_file,"  enemy[%i].shot[2].min_x = %i; enemy[%i].shot[2].min_y = %i;\n", nr, enemy[nr].shot[2].min_x, nr, enemy[nr].shot[2].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[2].max_x = %i; enemy[%i].shot[2].max_y= %i;\n", nr, enemy[nr].shot[2].max_x, nr, enemy[nr].shot[2].max_y);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, 0, nr, 0);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.width, nr, enemy[nr].image.height);
                            break;
      case ENEMY_SHIP_1  :
                            // Path-Node zu Punkt finden
                            pnr = get_nearest_node(enemy[nr].xpos,enemy[nr].ypos);
                            fprintf(tmp_file, "  enemy[%i].xpos = path_node[%i].x; enemy[%i].ypos = path_node[%i].y; // %.0f, %.0f\n",nr,pnr,nr,pnr,enemy[nr].xpos,enemy[nr].ypos);

                            fprintf(tmp_file,"  enemy[%i].type = ENEMY_SHIP_1;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = enemy_s1_01;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xoff = %i; enemy[%i].shot[0].yoff = %i;\n", nr, enemy[nr].shot[0].xoff, nr, enemy[nr].shot[0].yoff);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xspd = %.1f; enemy[%i].shot[0].yspd = %.1f;\n", nr, enemy[nr].shot[0].xspd, nr, enemy[nr].shot[0].yspd);
                            fprintf(tmp_file,"  enemy[%i].shot[0].min_x = %i; enemy[%i].shot[0].min_y = %i;\n", nr, enemy[nr].shot[0].min_x, nr, enemy[nr].shot[0].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[0].max_x = %i; enemy[%i].shot[0].max_y= %i;\n", nr, enemy[nr].shot[0].max_x, nr, enemy[nr].shot[0].max_y);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, enemy[nr].image.boundxa, nr, enemy[nr].image.boundya);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.boundxe, nr, enemy[nr].image.boundye);
                            fprintf(tmp_file,"  enemy[%i].framet = 0; enemy[%i].maxframet = %i;\n",nr,nr, 10);
                            fprintf(tmp_file,"  enemy[%i].waypnt[1].x = path_node[%i].x; enemy[%i].waypnt[1].y = path_node[%i].y; enemy[%i].waypnt[1].s = 150;\n",nr,pnr,nr,pnr,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[2].x = path_node[%i].x; enemy[%i].waypnt[2].y = path_node[%i].y; enemy[%i].waypnt[2].s = 150;\n",nr,pnr+1,nr,pnr+1,nr);
                            fprintf(tmp_file,"  enemy[%i].cur_waypnt = %i; enemy[%i].max_waypnt = %i;\n",nr,enemy[nr].cur_waypnt,nr,enemy[nr].max_waypnt);
                            break;
      case ENEMY_SHIP_2  :
                            // Path-Node zu Punkt finden
                            pnr = get_nearest_node(enemy[nr].xpos,enemy[nr].ypos);
                            fprintf(tmp_file, "  enemy[%i].xpos = path_node[%i].x; enemy[%i].ypos = path_node[%i].y; // %.0f, %.0f\n",nr,pnr,nr,pnr,enemy[nr].xpos,enemy[nr].ypos);

                            fprintf(tmp_file,"  enemy[%i].type = ENEMY_SHIP_2;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = enemy_s2_01;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, enemy[nr].image.boundxa, nr, enemy[nr].image.boundya);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.boundxe, nr, enemy[nr].image.boundye);
                            fprintf(tmp_file,"  enemy[%i].framet = 0; enemy[%i].maxframet = %i;\n",nr,nr, 10);
                            fprintf(tmp_file,"  enemy[%i].waypnt[1].x = path_node[%i].x; enemy[%i].waypnt[1].y = path_node[%i].y; enemy[%i].waypnt[1].s = 150;\n",nr,pnr,nr,pnr,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[2].x = path_node[%i].x; enemy[%i].waypnt[2].y = path_node[%i].y; enemy[%i].waypnt[2].s = 150;\n",nr,pnr+1,nr,pnr+1,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[3].x = path_node[%i].x; enemy[%i].waypnt[3].y = path_node[%i].y; enemy[%i].waypnt[2].s = 150;\n",nr,pnr+2,nr,pnr+2,nr);
                            fprintf(tmp_file,"  enemy[%i].cur_waypnt = %i; enemy[%i].max_waypnt = %i;\n",nr,enemy[nr].cur_waypnt,nr,enemy[nr].max_waypnt);
                            break;
      case ENEMY_SHIP_3  :
                            // Path-Node zu Punkt finden
                            pnr = get_nearest_node(enemy[nr].xpos,enemy[nr].ypos);
                            fprintf(tmp_file, "  enemy[%i].xpos = path_node[%i].x; enemy[%i].ypos = path_node[%i].y; // %.0f, %.0f\n",nr,pnr,nr,pnr,enemy[nr].xpos,enemy[nr].ypos);

                            fprintf(tmp_file,"  enemy[%i].type = ENEMY_SHIP_3;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = enemy_s3_01;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[0].min_x = %i; enemy[%i].shot[0].min_y = %i;\n", nr, enemy[nr].shot[0].min_x, nr, enemy[nr].shot[0].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[0].max_x = %i; enemy[%i].shot[0].max_y= %i;\n", nr, enemy[nr].shot[0].max_x, nr, enemy[nr].shot[0].max_y);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, enemy[nr].image.boundxa, nr, enemy[nr].image.boundya);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.boundxe, nr, enemy[nr].image.boundye);
                            fprintf(tmp_file,"  enemy[%i].startframe = %i; enemy[%i].shootr = TRUE;\n",nr, enemy[nr].startframe, nr);
                            fprintf(tmp_file,"  enemy[%i].framet = 0; enemy[%i].maxframet = %i;\n",nr,nr, 10);
                            fprintf(tmp_file,"  enemy[%i].cur_waypnt = %i; enemy[%i].max_waypnt = %i;\n",nr,enemy[nr].cur_waypnt,nr,enemy[nr].max_waypnt);
                            fprintf(tmp_file,"  enemy[%i].nwaypnt[1] = 1;\n  enemy[%i].nwaypnt[2] = 2;\n  enemy[%i].nwaypnt[3] = 3;\n",nr,nr,nr);
                            fprintf(tmp_file,"  enemy[%i].cur_point = 250; enemy[%i].dest_point = 0; enemy[%i].real_waypnt = 0;\n",nr,nr,nr);
                            fprintf(tmp_file,"  enemy[%i].curviness = ftofix(0.15);  enemy[%i].bezier = TRUE; enemy[%i].chase = TRUE;\n",nr,nr,nr);
                            fprintf(tmp_file,"  enemy[%i].xspd = %.1f;\n",nr,enemy[nr].xspd);
                            break;
      case ENEMY_SHIP_4  :
                            // Path-Node zu Punkt finden
                            pnr = get_nearest_node(enemy[nr].xpos,enemy[nr].ypos);
                            fprintf(tmp_file, "  enemy[%i].xpos = path_node[%i].x; enemy[%i].ypos = path_node[%i].y; // %.0f, %.0f\n",nr,pnr,nr,pnr,enemy[nr].xpos,enemy[nr].ypos);

                            fprintf(tmp_file,"  enemy[%i].type = ENEMY_SHIP_4;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = enemy_s4_01;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[0].min_x = %i; enemy[%i].shot[0].min_y = %i;\n", nr, enemy[nr].shot[0].min_x, nr, enemy[nr].shot[0].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[0].max_x = %i; enemy[%i].shot[0].max_y= %i;\n", nr, enemy[nr].shot[0].max_x, nr, enemy[nr].shot[0].max_y);
                            fprintf(tmp_file,"  enemy[%i].shot[1].min_x = %i; enemy[%i].shot[1].min_y = %i;\n", nr, enemy[nr].shot[1].min_x, nr, enemy[nr].shot[1].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[1].max_x = %i; enemy[%i].shot[1].max_y= %i;\n", nr, enemy[nr].shot[1].max_x, nr, enemy[nr].shot[1].max_y);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, enemy[nr].image.boundxa, nr, enemy[nr].image.boundya);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.boundxe, nr, enemy[nr].image.boundye);
                            fprintf(tmp_file,"  enemy[%i].startframe = %i; enemy[%i].shootr = TRUE;\n",nr, enemy[nr].startframe, nr);
                            fprintf(tmp_file,"  enemy[%i].framet = 0; enemy[%i].maxframet = %i;\n",nr,nr, enemy[nr].maxframet);
                            fprintf(tmp_file,"  enemy[%i].cur_waypnt = %i; enemy[%i].max_waypnt = %i;\n",nr,enemy[nr].cur_waypnt,nr,enemy[nr].max_waypnt);
                            fprintf(tmp_file,"  enemy[%i].nwaypnt[1] = 1;\n  enemy[%i].nwaypnt[2] = 2;\n  enemy[%i].nwaypnt[3] = 3;\n",nr,nr,nr);
                            fprintf(tmp_file,"  enemy[%i].cur_point = 250; enemy[%i].dest_point = 0; enemy[%i].real_waypnt = 0;\n",nr,nr,nr);
                            fprintf(tmp_file,"  enemy[%i].curviness = ftofix(0.15);  enemy[%i].bezier = TRUE; enemy[%i].chase = TRUE;\n",nr,nr,nr);
                            fprintf(tmp_file,"  enemy[%i].xspd = %.1f;\n",nr,enemy[nr].xspd);
                            break;
      case ENEMY_SHIP_5  :
                            // Path-Node zu Punkt finden
                            pnr = get_nearest_node(enemy[nr].xpos,enemy[nr].ypos);
                            fprintf(tmp_file, "  enemy[%i].xpos = path_node[%i].x; enemy[%i].ypos = path_node[%i].y; // %.0f, %.0f\n",nr,pnr,nr,pnr,enemy[nr].xpos,enemy[nr].ypos);

                            fprintf(tmp_file,"  enemy[%i].type = ENEMY_SHIP_5;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = enemy_s5_01;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, enemy[nr].image.boundxa, nr, enemy[nr].image.boundya);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.boundxe, nr, enemy[nr].image.boundye);
                            fprintf(tmp_file,"  enemy[%i].framet = 0; enemy[%i].maxframet = 8;\n",nr,nr);
//                            fprintf(tmp_file,"  enemy[%i].shoott = 48; enemy[%i].shootf = 96; enemy[%i].maxhit = 50;\n",nr,nr,nr);
//                            fprintf(tmp_file,"  enemy[%i].maxshot = 10; enemy[%i].dead = 0; enemy[%i].maxdead = 10;\n",nr,nr,nr);

                            fprintf(tmp_file,"  enemy[%i].shot[0].xoff = 40; enemy[%i].shot[0].yoff = 5;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xspd = 0; enemy[%i].shot[0].yspd = -1.5;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[1].xoff = 40; enemy[%i].shot[1].yoff = 5;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[1].xspd = 1; enemy[%i].shot[1].yspd = -0.7;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[2].xoff = 40; enemy[%i].shot[2].yoff = 5;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[2].xspd = -1; enemy[%i].shot[2].yspd = -0.7;\n",nr,nr);

                            fprintf(tmp_file,"  enemy[%i].shot[3].xoff = 40; enemy[%i].shot[3].yoff = 41;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[3].xspd = 0; enemy[%i].shot[3].yspd = 1.5;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[4].xoff = 40; enemy[%i].shot[4].yoff = 41;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[4].xspd = 1; enemy[%i].shot[4].yspd = 0.7;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[5].xoff = 40; enemy[%i].shot[5].yoff = 41;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[5].xspd = -1; enemy[%i].shot[5].yspd = 0.7;\n",nr,nr);

                            fprintf(tmp_file,"  enemy[%i].shot[6].xoff = 40; enemy[%i].shot[6].yoff = 5;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[6].xspd = -2; enemy[%i].shot[6].yspd = 0;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[7].xoff = 40; enemy[%i].shot[7].yoff = 5;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[7].xspd = 2;  enemy[%i].shot[7].yspd = 0;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[8].xoff = 40; enemy[%i].shot[8].yoff = 41;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[8].xspd = -2; enemy[%i].shot[8].yspd = 0;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[9].xoff = 40; enemy[%i].shot[9].yoff = 41;\n",nr,nr);
                            fprintf(tmp_file,"  enemy[%i].shot[9].xspd =  2; enemy[%i].shot[9].yspd = 0;\n",nr,nr);

                            fprintf(tmp_file,"  enemy[%i].shot[0].min_x = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[0].max_x = 640*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[0].min_y = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[0].max_y = 480*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[1].min_x = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[1].max_x = 640*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[1].min_y = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[1].max_y = 480*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[2].min_x = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[2].max_x = 640*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[2].min_y = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[2].max_y = 480*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[3].min_x = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[3].max_x = 640*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[3].min_y = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[3].max_y = 480*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[4].min_x = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[4].max_x = 640*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[4].min_y = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[4].max_y = 480*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[5].min_x = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[5].max_x = 640*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[5].min_y = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[5].max_y = 480*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[6].min_x = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[6].max_x = 640*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[6].min_y = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[6].max_y = 480*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[7].min_x = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[7].max_x = 640*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[7].min_y = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[7].max_y = 480*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[8].min_x = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[8].max_x = 640*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[8].min_y = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[8].max_y = 480*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[9].min_x = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[9].max_x = 640*6;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[9].min_y = 0;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[9].max_y = 480*6;\n\n",nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[1].x = path_node[1].x; enemy[%i].waypnt[1].y = path_node[1].y; enemy[%i].waypnt[1].s = 250;\n",nr,nr,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[2].x = path_node[2].x; enemy[%i].waypnt[2].y = path_node[2].y; enemy[%i].waypnt[2].s = 250;\n",nr,nr,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[3].x = path_node[3].x; enemy[%i].waypnt[3].y = path_node[3].y; enemy[%i].waypnt[3].s = 250;\n",nr,nr,nr);
                            fprintf(tmp_file,"  enemy[%i].cur_waypnt = %i; enemy[%i].max_waypnt = %i;\n",nr,enemy[nr].cur_waypnt,nr,enemy[nr].max_waypnt);
                            break;
      case ENEMY_NSHIP_1 :
                            // Path-Node zu Punkt finden
                            pnr = get_nearest_node(enemy[nr].xpos,enemy[nr].ypos);
                            fprintf(tmp_file, "  enemy[%i].xpos = path_node[%i].x; enemy[%i].ypos = path_node[%i].y; // %.0f, %.0f\n",nr,pnr,nr,pnr,enemy[nr].xpos,enemy[nr].ypos);

                            fprintf(tmp_file,"  enemy[%i].type = ENEMY_NSHIP_1;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = enemy_n1_1;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xoff = %i; enemy[%i].shot[0].yoff = %i;\n", nr, enemy[nr].shot[0].xoff, nr, enemy[nr].shot[0].yoff);
                            fprintf(tmp_file,"  enemy[%i].shot[0].xspd = %.1f; enemy[%i].shot[0].yspd = %.1f;\n", nr, enemy[nr].shot[0].xspd, nr, enemy[nr].shot[0].yspd);
                            fprintf(tmp_file,"  enemy[%i].shot[0].min_x = %i; enemy[%i].shot[0].min_y = %i;\n", nr, enemy[nr].shot[0].min_x, nr, enemy[nr].shot[0].min_y);
                            fprintf(tmp_file,"  enemy[%i].shot[0].max_x = %i; enemy[%i].shot[0].max_y= %i;\n", nr, enemy[nr].shot[0].max_x, nr, enemy[nr].shot[0].max_y);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, enemy[nr].image.boundxa, nr, enemy[nr].image.boundya);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.boundxe, nr, enemy[nr].image.boundye);
                            fprintf(tmp_file,"  enemy[%i].framet = 0; enemy[%i].maxframet = %i;\n",nr,nr, 10);
                            fprintf(tmp_file,"  enemy[%i].waypnt[1].x = path_node[%i].x; enemy[%i].waypnt[1].y = path_node[%i].y; enemy[%i].waypnt[1].s = 150;\n",nr,pnr,nr,pnr,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[2].x = path_node[%i].x; enemy[%i].waypnt[2].y = path_node[%i].y; enemy[%i].waypnt[2].s = 150;\n",nr,pnr+1,nr,pnr+1,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[3].x = path_node[%i].x; enemy[%i].waypnt[3].y = path_node[%i].y; enemy[%i].waypnt[2].s = 150;\n",nr,pnr+2,nr,pnr+2,nr);
                            fprintf(tmp_file,"  enemy[%i].cur_waypnt = %i; enemy[%i].max_waypnt = %i;\n",nr,enemy[nr].cur_waypnt,nr,enemy[nr].max_waypnt);
                            break;

      case GSTONE_B1     :
                            // Path-Node zu Punkt finden
                            pnr = get_nearest_node(enemy[nr].xpos,enemy[nr].ypos);
                            fprintf(tmp_file, "  enemy[%i].xpos = path_node[%i].x; enemy[%i].ypos = path_node[%i].y; // %.0f, %.0f\n",nr,pnr,nr,pnr,enemy[nr].xpos,enemy[nr].ypos);
                            fprintf(tmp_file,"  enemy[%i].type = GSTONE_B1;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = obj_gstone_b1;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, enemy[nr].image.boundxa, nr, enemy[nr].image.boundya);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.boundxe, nr, enemy[nr].image.boundye);
                            fprintf(tmp_file,"  enemy[%i].waypnt[1].x = path_node[%i].x; enemy[%i].waypnt[1].y = path_node[%i].y; enemy[%i].waypnt[1].s = 150;\n",nr,pnr,nr,pnr,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[2].x = path_node[%i].x; enemy[%i].waypnt[2].y = path_node[%i].y; enemy[%i].waypnt[2].s = 150;\n",nr,pnr+1,nr,pnr+1,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[3].x = path_node[%i].x; enemy[%i].waypnt[3].y = path_node[%i].y; enemy[%i].waypnt[3].s = 150;\n",nr,pnr+2,nr,pnr+2,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[4].x = path_node[%i].x; enemy[%i].waypnt[4].y = path_node[%i].y; enemy[%i].waypnt[4].s = 150;\n",nr,pnr+3,nr,pnr+3,nr);
                            fprintf(tmp_file,"  enemy[%i].cur_waypnt = %i; enemy[%i].max_waypnt = 4;\n",nr,1,nr);
                            break;

      case GSTONE_S1     :
                            // Path-Node zu Punkt finden
                            pnr = get_nearest_node(enemy[nr].xpos,enemy[nr].ypos);
                            fprintf(tmp_file, "  enemy[%i].xpos = path_node[%i].x; enemy[%i].ypos = path_node[%i].y; // %.0f, %.0f\n",nr,pnr,nr,pnr,enemy[nr].xpos,enemy[nr].ypos);
                            fprintf(tmp_file,"  enemy[%i].type = GSTONE_S1;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = obj_gstone_s1;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, enemy[nr].image.boundxa, nr, enemy[nr].image.boundya);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.boundxe, nr, enemy[nr].image.boundye);
                            fprintf(tmp_file,"  enemy[%i].waypnt[1].x = path_node[%i].x; enemy[%i].waypnt[1].y = path_node[%i].y; enemy[%i].waypnt[1].s = 150;\n",nr,pnr,nr,pnr,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[2].x = path_node[%i].x; enemy[%i].waypnt[2].y = path_node[%i].y; enemy[%i].waypnt[2].s = 150;\n",nr,pnr+1,nr,pnr+1,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[3].x = path_node[%i].x; enemy[%i].waypnt[3].y = path_node[%i].y; enemy[%i].waypnt[3].s = 150;\n",nr,pnr+2,nr,pnr+2,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[4].x = path_node[%i].x; enemy[%i].waypnt[4].y = path_node[%i].y; enemy[%i].waypnt[4].s = 150;\n",nr,pnr+3,nr,pnr+3,nr);
                            fprintf(tmp_file,"  enemy[%i].cur_waypnt = %i; enemy[%i].max_waypnt = 4;\n",nr,1,nr);
                            break;

      case GSTONE_S2     :
                            // Path-Node zu Punkt finden
                            pnr = get_nearest_node(enemy[nr].xpos,enemy[nr].ypos);
                            fprintf(tmp_file, "  enemy[%i].xpos = path_node[%i].x; enemy[%i].ypos = path_node[%i].y; // %.0f, %.0f\n",nr,pnr,nr,pnr,enemy[nr].xpos,enemy[nr].ypos);
                            fprintf(tmp_file,"  enemy[%i].type = GSTONE_S2;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.data = obj_gstone_s2;\n",nr);
                            fprintf(tmp_file,"  enemy[%i].image.boundxa = %i; enemy[%i].image.boundya = %i;\n", nr, enemy[nr].image.boundxa, nr, enemy[nr].image.boundya);
                            fprintf(tmp_file,"  enemy[%i].image.boundxe = %i; enemy[%i].image.boundye = %i;\n", nr, enemy[nr].image.boundxe, nr, enemy[nr].image.boundye);
                            fprintf(tmp_file,"  enemy[%i].waypnt[1].x = path_node[%i].x; enemy[%i].waypnt[1].y = path_node[%i].y; enemy[%i].waypnt[1].s = 150;\n",nr,pnr,nr,pnr,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[2].x = path_node[%i].x; enemy[%i].waypnt[2].y = path_node[%i].y; enemy[%i].waypnt[2].s = 150;\n",nr,pnr+1,nr,pnr+1,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[3].x = path_node[%i].x; enemy[%i].waypnt[3].y = path_node[%i].y; enemy[%i].waypnt[3].s = 150;\n",nr,pnr+2,nr,pnr+2,nr);
                            fprintf(tmp_file,"  enemy[%i].waypnt[4].x = path_node[%i].x; enemy[%i].waypnt[4].y = path_node[%i].y; enemy[%i].waypnt[4].s = 150;\n",nr,pnr+3,nr,pnr+3,nr);
                            fprintf(tmp_file,"  enemy[%i].cur_waypnt = %i; enemy[%i].max_waypnt = 4;\n",nr,1,nr);
                            break;

    } // switch type

    fprintf(tmp_file,"  enemy[%i].image.width = %i; enemy[%i].image.height = %i;\n", nr, enemy[nr].image.width, nr, enemy[nr].image.height);
    fprintf(tmp_file,"  enemy[%i].shootf = %i; enemy[%i].dead = 0;\n", nr, enemy[nr].shootf, nr);
    fprintf(tmp_file,"  enemy[%i].frame = %i; enemy[%i].maxframe = %i;\n", nr, enemy[nr].frame, nr, enemy[nr].maxframe);
    fprintf(tmp_file,"  enemy[%i].maxshot = %i; enemy[%i].shoott = %i; enemy[%i].maxdead = %i;\n", nr, enemy[nr].maxshot, nr, enemy[nr].shoott, nr, enemy[nr].maxdead);
    fprintf(tmp_file,"  enemy[%i].maxhit = %i;\n", nr, enemy[nr].maxhit);
    fprintf(tmp_file,"  enemy[%i].react.xw1 = %i; enemy[%i].react.xw2 = %i;\n", nr, enemy[nr].react.xw1, nr, enemy[nr].react.xw2);
    fprintf(tmp_file,"  enemy[%i].react.yw1 = %i; enemy[%i].react.yw2 = %i;\n", nr, enemy[nr].react.xw1, nr, enemy[nr].react.xw2);
    fprintf(tmp_file,"  enemy[%i].active = TRUE;\n\n", nr);
  } // for nr

  fclose(tmp_file);
}

