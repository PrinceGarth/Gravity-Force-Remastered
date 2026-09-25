/*
 *    GRAVITY FORCE, gfedinit.c
 *
 *    -- initialization functions for "editor"
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfedhd.h"
#include "gfmes.h"
#include "mappyal.h"

int killable_enemies = 0;

void error_exit(int el, char *t1, char *t2, int i1, int i2)
{
  set_gfx_mode(GFX_TEXT,0,0,0,0);
  allegro_message("%s: %s (%i, %i)",t1,t2,i1,i2);
//  free_all();
  exit(el);
}

void setup_global_stuff()
{
         SCORE_WIDTH  = 640;
         SCORE_HEIGHT = 61;

         MAP_WIDTH = 100;
         MAP_HEIGHT = 100;
//         TILE_W = 48;
//         TILE_H = 48;

         PLAYSCREEN_XSTART =  0;
         PLAYSCREEN_YSTART =  0;
         PLAYSCREEN_WIDTH  =640;
         PLAYSCREEN_HEIGHT =480-SCORE_HEIGHT;
         PLAYSCREEN1_XSTART=  0;
         PLAYSCREEN1_YSTART=  0;
         PLAYSCREEN1_WIDTH =320;
         PLAYSCREEN1_HEIGHT=420;
         PLAYSCREEN2_XSTART=320;
         PLAYSCREEN2_YSTART=  0;
         PLAYSCREEN2_WIDTH =320;
         PLAYSCREEN2_HEIGHT=420;

         LANGUAGE = 0;

         PLUS_X = 700;
         PLUS_Y = 700;

         BOMB_SHOOT_TIME =50;
         NEW_SHIP_TIME   =60;

//         REACT_W         =147;
//         REACT_H         =147;

         PARTICLE_GRAVITY=0.015;
         LEVEL_GRAVITY   =0.006;
         BOMB_GRAVITY    =0.040;
         PLAYER_ACC      =0.036;

         MAX_SHIP_WEIGHT =PLAYER_ACC-LEVEL_GRAVITY;//0.029;
         CARGO_WEIGHT    =0.005;
         SHOOT_SPEED     =5;
         SHOOT_SPEED_BOMB=3;
         TURN_SPEED      =3.5;
         MAX_FUEL        =99;
         MAX_SHIELD      =5;
         FUEL_DEC        =0.02;

//         SHIELD_RECHARGE =60;
         ROCKET_TURN     =0.07;

         PIXEL_DIVISOR   =100; // Prozent-Angabe

//         NO_CLIP         =FALSE; // Wand-Durchflieg-Cheat

         strcpy(map_dir,".//MAPS//");
         strcpy(dat_dir,".//DAT//");
         strcpy(lng_dir,".//LNG//");
         strcpy(snd_dir,".//SOUND//");
         strcpy(dem_dir,".//DEMO//");
}

void init_colors()
{
  col_red       = makecol8(255,0,0);
  col_bred      = makecol8(255,255,0);
  col_green     = makecol8(0,255,0);
  col_blue      = makecol8(0,0,255);
  col_black     = makecol8(0,0,0);
  col_white     = makecol8(255,255,255);
  col_yellow    = makecol8(255,255,0);
  col_orange    = makecol8(255,128,0);
  col_lightblue = makecol8(20,20,200);
  col_grey      = makecol8(147,147,147);
  col_darkgrey  = makecol8(47,47,47);
}

void init_bases()
{
  int nr;
  int tx,ty;
  BITMAP *tempbit;
}

void init_objects()
{
  int nr;
  int tx,ty;
  BITMAP *tempbit;
}

void init_spobjects()
{
  int nr;
  int tx,ty;
  BITMAP *tempbit;
}

void init_enemies()
{
  int nr,w;
  int tx,ty;
  BITMAP *tempbit;
}

void init_bullets()
{
  int nr;

  bullet_sprite[W_DEFAULT].data = create_bitmap(1,1);
  putpixel(bullet_sprite[W_DEFAULT].data,0,0,col_white);

  bullet_sprite[W_DEFAULT].width = 1; bullet_sprite[0].height = 1;
  bullet_sprite[W_DEFAULT].boundxe = 1; bullet_sprite[0].boundye = 1;

  bullet_sprite[W_DEFAULT2].data = create_bitmap(1,1);
  putpixel(bullet_sprite[W_DEFAULT2].data,0,0,col_yellow);

  bullet_sprite[W_DEFAULT2].width = 1; bullet_sprite[0].height = 1;
  bullet_sprite[W_DEFAULT2].boundxe = 1; bullet_sprite[0].boundye = 1;

  bullet_sprite[W_BOMB].data = create_bitmap(2,2);
  putpixel(bullet_sprite[W_BOMB].data,0,0,col_white);
  putpixel(bullet_sprite[W_BOMB].data,0,1,col_white);
  putpixel(bullet_sprite[W_BOMB].data,1,0,col_white);
  putpixel(bullet_sprite[W_BOMB].data,1,1,col_white);

  bullet_sprite[W_BOMB].width = 2; bullet_sprite[W_BOMB].height = 2;
  bullet_sprite[W_BOMB].boundxe = 2; bullet_sprite[W_BOMB].boundye = 2;

  bullet_sprite[W_BIGBOMB].data = create_bitmap(3,3);
  clear(bullet_sprite[W_BIGBOMB].data);
  putpixel(bullet_sprite[W_BIGBOMB].data,1,0,col_grey);
  putpixel(bullet_sprite[W_BIGBOMB].data,0,1,col_grey);
  putpixel(bullet_sprite[W_BIGBOMB].data,1,1,col_white);
  putpixel(bullet_sprite[W_BIGBOMB].data,2,1,col_grey);
  putpixel(bullet_sprite[W_BIGBOMB].data,1,2,col_grey);

  bullet_sprite[W_BIGBOMB].width = 3; bullet_sprite[W_BIGBOMB].height = 3;
  bullet_sprite[W_BIGBOMB].boundxe = 3; bullet_sprite[W_BIGBOMB].boundye = 3;
}

void init_ship()
{
  playship[0].xpos = ((base[0].x2-base[0].x1) / 2) + base[0].x1;
  playship[0].ypos = base[0].y-PLAYER_HEIGHT;
  playship[0].xspd = 0;
  playship[0].yspd = 0;
  playship[0].xacc = 0;
  playship[0].yacc = 0;
  playship[0].wght = 0;
  playship[0].head = 0;
  playship[0].fuel = MAX_FUEL;
  playship[0].ldur = 0;
  playship[0].base = 0;
  playship[0].powr = 1;
  playship[0].weapon.type = W_DEFAULT;
  playship[0].dead = FALSE;
  playship[0].thrust = FALSE;
}

void callback_func()
{
}

void init_paths()
{
}

void init()
{
  FILE *datf;
  BITMAP *test;

  setup_global_stuff();

  node_anz = 0;
  base_anz = object_anz = enemy_anz = 0;

  first_bullet = NULL; last_bullet = NULL;
  first_pixel = NULL; last_pixel = NULL;
  first_explosion = NULL; last_explosion = NULL;
  first_rect = NULL; last_rect = NULL;

  PLUS_X = 500; PLUS_Y = 500;

  strcpy(gfx_name,".\\dat\\gfw2.fmp");

  ///////////////////////////////////////////
  strcpy(cfg_name,"gfr03.cfg");
  strcpy(map_name,"./maps/gfr03.mar");
  ///////////////////////////////////////////

  strcpy(tmp_name,"gf.pnt");

  srand(time(0));

  allegro_init();
  install_timer();
  install_keyboard();
  install_mouse();
  if (set_gfx_mode(GFX_AUTODETECT,640,480,0,0))
  {
    allegro_message("Error initialising graphics: %s\n",allegro_error);
  };
  vscreen = create_bitmap(SCREEN_W+PLUS_X+100,SCREEN_H+PLUS_Y+100);
  clear(vscreen);  clear(screen);
  temp_sprite = create_bitmap(150,150);
  clear(temp_sprite);

  packfile_password("tanja");
  data = load_datafile("./DAT/gf.dat");
  packfile_password(NULL);
  if (!data) error_exit(51,"Error loading data file","gf.dat",51,0);

  playship[0].player_temp = black_player = create_bitmap(25,25);

  clear(black_player);

  score_table = create_bitmap(SCORE_WIDTH,SCORE_HEIGHT);
  clear(score_table);

  test = data[plship1].dat;
  PLAYER_WIDTH = test->w;
  PLAYER_HEIGHT = test->h;

  playship[0].maxdead = 11;

  if (MapLoad(gfx_name)) { error_exit(32,"Karte konnte nicht geladen werden!",gfx_name,32,0); }
  if (mapmappt[0]) free(mapmappt[0]); mapmappt[0] = NULL; mappt = NULL;
  strcpy(tmp_dir,map_name);
  mapfilept = pack_fopen((char*)tmp_dir,"r");
  if (!mapfilept) { error_exit(35,error_message[MSG_ERROR_MAPFILE].text1,map_name,35,0); }
  MapDecodeBODY();
  pack_fclose(mapfilept);

  set_palette(data[gamepal].dat);
  get_palette(mainpal);

  create_rgb_table(&rgb_table, mainpal, callback_func);
  rgb_map = &rgb_table;
  create_trans_table(&trans_table, mainpal, 128, 128, 128, callback_func);
  create_light_table(&light_table, mainpal, 0, 0, 200, callback_func);
  color_map = &light_table;

  MapInitAnims();

  init_colors();
  init_bullets();

  strcpy(tmp_dir,map_dir);
  if (exists((char*)strcat(tmp_dir,cfg_name))) read_level_data();

  cur_base = base_home;
  cur_object = pile1;
  cur_spobject = effect_tele_01;
  cur_enemy = enemy_s1_01;


//  play_midi(mid_music,0);
}

