/*
 *    GRAVITY FORCE, gfinit.c
 *
 *    -- most of the initialization stuff
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gf.h"
#include "gfinit.h"
#include "gfmes.h"
#include "gfstat.h"
#include "gflink.h"
#include "gfmenu.h"
#include "gfosd.h"
#include "gfpanel.h"
#include "gfcnfg.h"
#include "gflinit.h"
#include "gflevel.h"
#include "mappyal.h"
#include "gfcontrl.h"
#include "gfg2pdog.h"

extern int seed;

BITMAP *cursor_tempbit;
BITMAP *cursor_background;


void error_exit(int el, char *t1, char *t2, int i1, int i2)
{
  set_gfx_mode(GFX_TEXT,0,0,0,0);
  allegro_message("%s: %s (%i, %i)",t1,t2,i1,i2);
  free_all();
  exit(el);
}

char *encrypt_password(char *pwd)
{
  int nr;

  for (nr=0; nr < (signed int)strlen(pwd); nr++)
    pwd[nr]++;

  return pwd;
}

char *decrypt_password(char *pwd)
{
  int nr;

  for (nr=0; nr < (signed int)strlen(pwd); nr++)
    pwd[nr]--;

  return pwd;
}

void init_passwords()
{
  strcpy(level_passwords[0], "UFTUJU");          // testit
  strcpy(level_passwords[1], "GJSTUUJNF");       // firsttime
  strcpy(level_passwords[2], "XFMDPNF");         // welcome
  strcpy(level_passwords[3], "CJHCBOH");         // bigbang
  strcpy(level_passwords[4], "NFHBMPBE");        // megaload
  strcpy(level_passwords[5], "CSPUIFS");         // brother
  strcpy(level_passwords[6], "TJTUFS");          // sister
  strcpy(level_passwords[7], "HJSMGSJFOE");      // girlfriend
  strcpy(level_passwords[8], "EPPN3E");          // doom2d
  strcpy(level_passwords[9], "QPXFSQMBZ");       // powerplay
  strcpy(level_passwords[10],"[JWJ");       // zivi
}

void setup_global_stuff_once()
{
  LANGUAGE = 0;
  PIXEL_DIVISOR   =100; // Prozent-Angabe
  SOUND_VOLUME    =100;
  ZOOM_ACTIVE       = FALSE;
  GRAV_4_PIXELS     = TRUE;
  SHOW_DEBRIS       = TRUE;

  frag_limit = 20;
  time_limit = 10;

  strcpy(map_dir,"./maps/");
  strcpy(dat_dir,"./dat/");
  strcpy(lng_dir,"./lng/");
  strcpy(snd_dir,"./sound/");
  strcpy(dem_dir,"./demo/");

  init_controls();
}

void setup_global_stuff()
{
  strcpy(VERSION,"1.0");

  DEFDOUBLE_POWER = 4;

  MAP_WIDTH = 100;
  MAP_HEIGHT = 100;
  SCORE_WIDTH  = 640;
  SCORE_HEIGHT = 61;
  USCORE_HEIGHT = 18;

  XRES = 640;
  YRES = 480;

  PLAYSCREEN_XSTART =  0;
  PLAYSCREEN_YSTART =  0;
  PLAYSCREEN_WIDTH  =640;
  PLAYSCREEN_HEIGHT =480-SCORE_HEIGHT-USCORE_HEIGHT;

  timing_method = TIMER;

  PLUS_X = 700;
  PLUS_Y = 700;

  KAMIKAZE_HITS = 25;

  BOMB_SHOOT_TIME =50;
  NEW_SHIP_TIME   =60;

  PARTICLE_GRAVITY=0.015;
  LEVEL_GRAVITY   =0.006;
  BOMB_GRAVITY    =0.040;
  PLAYER_ACC      =0.037;

  MAX_SHIP_WEIGHT =PLAYER_ACC-LEVEL_GRAVITY-0.001;
  CARGO_WEIGHT    =0.005;
  SHOOT_SPEED     =5;
  SHOOT_SPEED_BOMB=3;
  TURN_SPEED      =3.5;
  MAX_FUEL        =99;
  MAX_SHIELD      =5;
  FUEL_DEC        =0.02;
  REFUEL_TIME     =100;

  ROCKET_TURN     =0.07;
  ALIEN_ROCKET_TURN = 0.04;

  MAX_CRATERS     =40;
  MAX_ROCKET_LOCK =500;

  MAX_LANDING_ANGLE = 15;
  MAX_LANDING_SPEED = -3;

  playship[0].START_BASE = 0;

  // NO_CLIP         =FALSE; // Wand-Durchflieg-Cheat
}

void init_colors()
{
  col_red       = makecol8(255,0,0);
  col_bred      = makecol8(255,255,0);
  col_green     = makecol8(0,255,0);
  col_blue      = makecol8(0,0,255);
  col_black     = 0;  // background index; collision treats >0 as solid (makecol8 never returns 0 in Allegro 4)
  col_white     = makecol8(255,255,255);
  col_yellow    = makecol8(255,255,0);
  col_orange    = makecol8(255,128,0);
  col_lightblue = makecol8(20,20,200);
  col_grey      = makecol8(147,147,147);
  col_darkgrey  = makecol8(47,47,47);
}

void init_bullets()
{
  bullet_sprite[W_DEFAULT].data = create_bitmap(1,1);
  putpixel(bullet_sprite[W_DEFAULT].data,0,0,col_white);

  bullet_sprite[W_DEFAULT].width = 1; bullet_sprite[W_DEFAULT].height = 1;
  bullet_sprite[W_DEFAULT].boundxe = 1; bullet_sprite[W_DEFAULT].boundye = 1;

  bullet_sprite[W_DEFAULT2].data = create_bitmap(1,1);
  putpixel(bullet_sprite[W_DEFAULT2].data,0,0,col_yellow);

  bullet_sprite[W_DEFAULT2].width = 1; bullet_sprite[W_DEFAULT2].height = 1;
  bullet_sprite[W_DEFAULT2].boundxe = 1; bullet_sprite[W_DEFAULT2].boundye = 1;

  bullet_sprite[W_DEFAULT3].data = create_bitmap(1,1);
  putpixel(bullet_sprite[W_DEFAULT3].data,0,0,col_orange);

  bullet_sprite[W_DEFAULT3].width = 1; bullet_sprite[W_DEFAULT3].height = 1;
  bullet_sprite[W_DEFAULT3].boundxe = 1; bullet_sprite[W_DEFAULT3].boundye = 1;

  bullet_sprite[W_BOMB].data = create_bitmap(2,2);
  putpixel(bullet_sprite[W_BOMB].data,0,0,col_white);
  putpixel(bullet_sprite[W_BOMB].data,0,1,col_white);
  putpixel(bullet_sprite[W_BOMB].data,1,0,col_white);
  putpixel(bullet_sprite[W_BOMB].data,1,1,col_white);
  bullet_sprite[W_BOMB].width = 2; bullet_sprite[W_BOMB].height = 2;
  bullet_sprite[W_BOMB].boundxe = 2; bullet_sprite[W_BOMB].boundye = 2;

  bullet_sprite[W_BOMB2].data = create_bitmap(2,2);
  putpixel(bullet_sprite[W_BOMB2].data,0,0,col_yellow);
  putpixel(bullet_sprite[W_BOMB2].data,0,1,col_yellow);
  putpixel(bullet_sprite[W_BOMB2].data,1,0,col_yellow);
  putpixel(bullet_sprite[W_BOMB2].data,1,1,col_yellow);
  bullet_sprite[W_BOMB2].width = 2; bullet_sprite[W_BOMB2].height = 2;
  bullet_sprite[W_BOMB2].boundxe = 2; bullet_sprite[W_BOMB2].boundye = 2;

  bullet_sprite[W_BOMB3].data = create_bitmap(2,2);
  putpixel(bullet_sprite[W_BOMB3].data,0,0,col_red);
  putpixel(bullet_sprite[W_BOMB3].data,0,1,col_red);
  putpixel(bullet_sprite[W_BOMB3].data,1,0,col_red);
  putpixel(bullet_sprite[W_BOMB3].data,1,1,col_red);
  bullet_sprite[W_BOMB3].width = 2; bullet_sprite[W_BOMB3].height = 2;
  bullet_sprite[W_BOMB3].boundxe = 2; bullet_sprite[W_BOMB3].boundye = 2;

  bullet_sprite[W_BIGBOMB].data = create_bitmap(3,3);
  clear(bullet_sprite[W_BIGBOMB].data);
  putpixel(bullet_sprite[W_BIGBOMB].data,1,0,col_grey);
  putpixel(bullet_sprite[W_BIGBOMB].data,0,1,col_grey);
  putpixel(bullet_sprite[W_BIGBOMB].data,1,1,col_white);
  putpixel(bullet_sprite[W_BIGBOMB].data,2,1,col_grey);
  putpixel(bullet_sprite[W_BIGBOMB].data,1,2,col_grey);
  bullet_sprite[W_BIGBOMB].width = 3; bullet_sprite[W_BIGBOMB].height = 3;
  bullet_sprite[W_BIGBOMB].boundxe = 3; bullet_sprite[W_BIGBOMB].boundye = 3;

  bullet_sprite[W_BIGBOMB2].data = create_bitmap(3,3);
  clear(bullet_sprite[W_BIGBOMB2].data);
  putpixel(bullet_sprite[W_BIGBOMB2].data,1,0,col_yellow);
  putpixel(bullet_sprite[W_BIGBOMB2].data,0,1,col_yellow);
  putpixel(bullet_sprite[W_BIGBOMB2].data,1,1,col_white);
  putpixel(bullet_sprite[W_BIGBOMB2].data,2,1,col_yellow);
  putpixel(bullet_sprite[W_BIGBOMB2].data,1,2,col_yellow);
  bullet_sprite[W_BIGBOMB2].width = 3; bullet_sprite[W_BIGBOMB2].height = 3;
  bullet_sprite[W_BIGBOMB2].boundxe = 3; bullet_sprite[W_BIGBOMB2].boundye = 3;

  bullet_sprite[W_BIGBOMB3].data = create_bitmap(3,3);
  clear(bullet_sprite[W_BIGBOMB3].data);
  putpixel(bullet_sprite[W_BIGBOMB3].data,1,0,col_red);
  putpixel(bullet_sprite[W_BIGBOMB3].data,0,1,col_red);
  putpixel(bullet_sprite[W_BIGBOMB3].data,1,1,col_white);
  putpixel(bullet_sprite[W_BIGBOMB3].data,2,1,col_red);
  putpixel(bullet_sprite[W_BIGBOMB3].data,1,2,col_red);
  bullet_sprite[W_BIGBOMB3].width = 3; bullet_sprite[W_BIGBOMB3].height = 3;
  bullet_sprite[W_BIGBOMB3].boundxe = 3; bullet_sprite[W_BIGBOMB3].boundye = 3;

  bullet_sprite[W_ROCKET].data = create_bitmap(4,4);
  clear(bullet_sprite[W_ROCKET].data);
  putpixel(bullet_sprite[W_ROCKET].data,0,0,col_grey);
  putpixel(bullet_sprite[W_ROCKET].data,3,0,col_grey);
  putpixel(bullet_sprite[W_ROCKET].data,1,1,col_white);
  putpixel(bullet_sprite[W_ROCKET].data,2,1,col_white);
  putpixel(bullet_sprite[W_ROCKET].data,1,2,col_white);
  putpixel(bullet_sprite[W_ROCKET].data,2,2,col_white);
  putpixel(bullet_sprite[W_ROCKET].data,0,3,col_grey);
  putpixel(bullet_sprite[W_ROCKET].data,3,3,col_grey);
  bullet_sprite[W_ROCKET].width = 4; bullet_sprite[W_ROCKET].height = 4;
  bullet_sprite[W_ROCKET].boundxe = 4; bullet_sprite[W_ROCKET].boundye = 4;

  bullet_sprite[W_ROCKET2].data = create_bitmap(4,4);
  clear(bullet_sprite[W_ROCKET2].data);
  putpixel(bullet_sprite[W_ROCKET2].data,0,0,col_grey);
  putpixel(bullet_sprite[W_ROCKET2].data,3,0,col_grey);
  putpixel(bullet_sprite[W_ROCKET2].data,1,1,col_yellow);
  putpixel(bullet_sprite[W_ROCKET2].data,2,1,col_yellow);
  putpixel(bullet_sprite[W_ROCKET2].data,1,2,col_yellow);
  putpixel(bullet_sprite[W_ROCKET2].data,2,2,col_yellow);
  putpixel(bullet_sprite[W_ROCKET2].data,0,3,col_grey);
  putpixel(bullet_sprite[W_ROCKET2].data,3,3,col_grey);
  bullet_sprite[W_ROCKET2].width = 4; bullet_sprite[W_ROCKET2].height = 4;
  bullet_sprite[W_ROCKET2].boundxe = 4; bullet_sprite[W_ROCKET2].boundye = 4;

  bullet_sprite[W_ROCKET3].data = create_bitmap(4,4);
  clear(bullet_sprite[W_ROCKET3].data);
  putpixel(bullet_sprite[W_ROCKET3].data,0,0,col_grey);
  putpixel(bullet_sprite[W_ROCKET3].data,3,0,col_grey);
  putpixel(bullet_sprite[W_ROCKET3].data,1,1,col_red);
  putpixel(bullet_sprite[W_ROCKET3].data,2,1,col_red);
  putpixel(bullet_sprite[W_ROCKET3].data,1,2,col_red);
  putpixel(bullet_sprite[W_ROCKET3].data,2,2,col_red);
  putpixel(bullet_sprite[W_ROCKET3].data,0,3,col_grey);
  putpixel(bullet_sprite[W_ROCKET3].data,3,3,col_grey);
  bullet_sprite[W_ROCKET3].width = 4; bullet_sprite[W_ROCKET3].height = 4;
  bullet_sprite[W_ROCKET3].boundxe = 4; bullet_sprite[W_ROCKET3].boundye = 4;
}

void init_ship(int c)
{
  if (playship[c].START_BASE >= 0)
  {
    playship[c].xpos = ((base[playship[c].START_BASE].x2-base[playship[c].START_BASE].x1) / 2) + base[playship[c].START_BASE].x1;
    playship[c].ypos = base[playship[c].START_BASE].y-PLAYER_HEIGHT;
  }
  else
  {
    playship[c].xpos = playship[c].oxpos;
    playship[c].ypos = playship[c].oypos;
  }

  playship[c].xspd = 0;
  playship[c].yspd = 0;
  playship[c].xacc = 0;
  playship[c].yacc = 0;
  playship[c].xacc2 = 0;
  playship[c].yacc2 = 0;
  playship[c].wght = 0;
  playship[c].head = 0;
  playship[c].fuel = MAX_FUEL;
  playship[c].ldur = 0;
  playship[c].base = 0;
  playship[c].powr = 1;
  playship[c].weapon.type = W_DEFAULT;
  playship[c].dead = FALSE;
  playship[c].thrust = FALSE;
  if (playship[c].shield_active) playship[c].shield = MAX_SHIELD;
  if (playship[c].pshield_active) playship[c].pshield = 3;

  if (game_mode == MP_2PDOGFIGHT)
  {
    playship[c].AUTOFIRE_RATE = DEF_AUTOFIRE_RATE;
    playship[c].SHIELD_RECHARGE = DEF_SHIELD_RECHARGE;
  }

  draw_panel();
//  update_panel_fuel();
//  update_panel_lifes();
//  update_panel_cargo();
//  update_panel_weapons();
//  update_panel_shield();
//  update_panel_infobox();

  NO_CLIP = FALSE;
}

void init_sounds()
{
  reserve_voices(15,-1);
  if (install_sound(DIGI_AUTODETECT, MIDI_NONE, ""))
  {
    fprintf(stderr,"%s %s\n",error_message[MSG_ERROR_SOUND_INIT].text1,allegro_error);
    fprintf(stderr,"%s \n",error_message[MSG_ERROR_SOUND_INIT2].text1);
    play_sound = FALSE;
  }
  else
  {
    strcpy(tmp_dir,dat_dir);
    packfile_password("tanja");
    sounddata = load_datafile((char*)strcat(tmp_dir,"gfsnd.dat"));
    packfile_password(NULL);
    if (!sounddata) { allegro_message("%s gfsnd.dat",error_message[MSG_ERROR_FILE_NOT_FOUND].text1); }
  }
}

void clear_arrays()
{
  struct ENODE *etemp, *next_explosion;
  struct BNODE *btemp, *next_bullet;
  struct PNODE *ptemp, *next_pixel;
  struct CNODE *ctemp, *next_crater;

  memset(&enemy,0,sizeof(enemy));
  memset(&object,0,sizeof(object));
  memset(&base,0,sizeof(base));

  etemp = first_explosion;
  btemp = first_bullet;
  ptemp = first_pixel;
  ctemp = first_crater;

  while (ctemp!=NULL)
  {
    next_crater = ctemp->next;
    remove_crater(ctemp);
    ctemp = next_crater;
  }
  while (etemp!=NULL)
  {
    next_explosion = etemp->next;
    remove_explosion(etemp);
    etemp = next_explosion;
  }
  while (btemp!=NULL)
  {
    next_bullet = btemp->next;
    erase_bullet(btemp);
    btemp = next_bullet;
  }
  while (ptemp!=NULL)
  {
    next_pixel = ptemp->next;
    remove_pixel(ptemp);
    ptemp = next_pixel;
  }

  first_bullet = NULL; last_bullet = NULL;
  first_pixel = NULL; last_pixel = NULL;
  first_crater = NULL; last_crater = NULL;
  first_explosion = NULL; last_explosion = NULL;
  first_rect = NULL; last_rect = NULL;
  rl.count = orl.count = 0;

  playship[0].xpos = playship[0].ypos = map_x[1] = map_y[1] = scroll_x[1] = scroll_y[1] = 0;
}

void init_tables()
{
  int nr;

  for (nr=0; nr <= 10; nr++)
  {
    tbl_blue[nr] = makecol8(nr*13,nr*16,nr*25);
    tbl_white[nr] = makecol8(nr*23,nr*23,nr*23);
    tbl_green[nr] = makecol8(0,nr*25,0);
    tbl_red[nr] = makecol8(nr*25,nr*18,nr*15);
    tbl_yellow[nr] = makecol8(nr*25,nr*25,0);
  }

  for (nr=0; nr < 256; nr++)
    tbl_all[nr] = makecol8(rand() % 255,rand() % 255,rand() % 255);

  for (nr=0; nr < 256; nr++)
  {
    white_palette[nr].r = 63;
    white_palette[nr].g = 63;
    white_palette[nr].b = 63;

    red_palette[nr].r = 63;
    red_palette[nr].g = 0;
    red_palette[nr].b = 0;
  }

}


void init_spec(int tst)
{
//  int nr;
/*  first_bullet = NULL; last_bullet = NULL;
  first_pixel = NULL; last_pixel = NULL;
  first_explosion = NULL; last_explosion = NULL;
  first_rect = NULL; last_rect = NULL;
  rl.count = orl.count = 0;
*/
//  srand(time(0));
//  srandom(time(0));
  srand(seed);
  srandom(seed);

  clear(vscreen);  clear(screen);
  clear(score_table); clear(uscore_table);

  init_upanel();

  packfile_password(NULL);

  set_palette(data[gamepal].dat);
  get_palette(mainpal);

  setup_global_stuff();
  level_inits();

  global_weight = global_saved = 0;

  strcpy(tmp_dir,dat_dir);
  if (MapLoad((char*)strcat(tmp_dir,gfx_name))) { error_exit(33,error_message[MSG_ERROR_GFXFILE].text1,gfx_name,33,maperror); }

  if (mapmappt[0]) free(mapmappt[0]); mapmappt[0] = NULL; mappt = NULL;
//  strcpy(tmp_dir,map_dir);
//  mapfilept = pack_fopen((char*)strcat(tmp_dir,map_name),"r");
  sprintf(tmp_dir,"%s%s#%s",dat_dir,"gfmaps.dat",map_name);
  packfile_password("tanja");
  mapfilept = pack_fopen(tmp_dir,"r");
  if (!mapfilept) { error_exit(35,error_message[MSG_ERROR_MAPFILE].text1,map_name,35,0); }
  MapDecodeBODY();
  pack_fclose(mapfilept);

  create_rgb_table(&rgb_table, mainpal, NULL);
  rgb_map = &rgb_table;
//  create_trans_table(&trans_table, mainpal, 128, 128, 128, NULL);
  create_light_table(&light_table, mainpal, 0, 0, 200, NULL);
  color_map = &light_table;

  init_tables();

  MapInitAnims();

  init_colors();
  init_bullets();

  clear_arrays();

  if (write_files)
  {
    init_paths();
    init_bases();
    init_objects();
    init_spobjects();
    init_enemies();
    write_level_data();
  }
  else
  {
    if (!tst) read_level_data();
    else
    {
      base[0].x1 = 100; base[0].x2 = 164; base[0].y = 100;
      base[0].type = 1; base[0].cargo = 0;
      base_anz = 1;
    }
  }

  clear_level_info();
  read_level_text(LANGUAGE);
} // init spec

void init_first()
{
//  _control87(MCW_EM|PC_24,MCW_EM|MCW_PC);

  set_uformat(U_ASCII);

  allegro_init();
  install_timer();
  install_keyboard();
  { extern int gf_mouse; gf_mouse = (install_mouse() >= 0); }

  read_message_file(LANGUAGE);
  init_passwords();


#ifdef ALLEGRO_DOS
  if (install_joystick(JOY_TYPE_4BUTTON)==0) use_joystick = TRUE; else { use_joystick = FALSE; install_joystick(JOY_TYPE_NONE); }
#else
  if (install_joystick(JOY_TYPE_AUTODETECT)==0) use_joystick = TRUE; else { use_joystick = FALSE; install_joystick(JOY_TYPE_NONE); }
#endif
  

  if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,XRES,YRES,0,0))
  {
    error_exit(36,error_message[MSG_ERROR_GRAPHIC_INIT].text1,allegro_error,36,0);
  };
  { extern int gf_mouse; if (gf_mouse) show_os_cursor(MOUSE_CURSOR_ARROW); }

}

void init(int tst)
{
  BITMAP *test;

  //////////// MENU ////////////////
  if (!tst) show_menu();
  //////////// MENU ////////////////

  vscreen = create_bitmap(PLAYSCREEN_WIDTH+PLUS_X+100,PLAYSCREEN_HEIGHT+PLUS_Y+100);
  if (!vscreen) { error_exit(50,error_message[MSG_ERROR_GRAPHIC_INIT].text1,"not enough ram",50,0); }

//  vscreen2 = create_bitmap(PLAYSCREEN_WIDTH+PLUS_X+100,PLAYSCREEN_HEIGHT+PLUS_Y+100);
//  clear(vscreen2);

  if (play_sound) init_sounds();

  strcpy(tmp_dir,dat_dir);
  packfile_password("tanja");
  data = load_datafile((char*)strcat(tmp_dir,"gf.dat"));
  packfile_password(NULL);
  if (!data) { error_exit(37,error_message[MSG_ERROR_FILE_NOT_FOUND].text1,"gf.dat",37,0); }

  playship[0].player_temp = create_bitmap(25,25);
  playship[1].player_temp = create_bitmap(25,25);
  playship[2].player_temp = create_bitmap(25,25);
  playship[3].player_temp = create_bitmap(25,25);
  playship[4].player_temp = create_bitmap(25,25);

  impact10h = data[impact10].dat;
  impact14h = data[impact14].dat;
  arial14h = data[arial14].dat;

  impact10hm = data[impact10m].dat;
  impact14hm = data[impact14m].dat;
  arial14hm = data[arial14m].dat;

  paneldat = data[panel].dat;

  score_table = create_bitmap(SCORE_WIDTH,SCORE_HEIGHT);
  uscore_table = create_bitmap(SCORE_WIDTH,USCORE_HEIGHT);

#ifdef ALLEGRO_DOS
  antialias_init(NULL);
#endif

  test = data[plship1].dat;
  PLAYER_WIDTH = test->w;
  PLAYER_HEIGHT = test->h;
  killable_enemies = 0;

  playship[0].maxdead = 11;
  playship[1].maxdead = 11;
  playship[3].maxdead = 11;
  playship[4].maxdead = 11;

  init_osd();

  init_spec(tst);

//  play_midi(mid_music,0);
}

void free_all()
{
  int nr;

  destroy_bitmap(vscreen);
  unload_datafile(data);
  unload_datafile(sounddata);
  destroy_bitmap(playship[0].player_temp);
  destroy_bitmap(playship[1].player_temp);
  destroy_bitmap(playship[2].player_temp);
  destroy_bitmap(playship[3].player_temp);
  destroy_bitmap(playship[4].player_temp);
  destroy_bitmap(cursor_tempbit);
  destroy_bitmap(cursor_background);
  destroy_bitmap(score_table);
  destroy_bitmap(uscore_table);
  destroy_bitmap(osd);
  destroy_bitmap(osdb);

  MapFreeMem();
//  free(mapmappt[0]); mapmappt[0] = NULL; mappt = NULL;
  for (nr=0; nr < 15; nr++) if (bullet_sprite[nr].data) destroy_bitmap(bullet_sprite[nr].data);

  osd_buf.active = FALSE;
  remove_int(update_timer);
  osd_counter = 0;

  clear_arrays();
}

void init_controls()
{
  pcontrol[0].shoot   = KEY_UP;
  pcontrol[0].thrust  = KEY_SPACE;
  pcontrol[0].left    = KEY_LEFT;
  pcontrol[0].right   = KEY_RIGHT;

  pcontrol[0].w1      = KEY_1;
  pcontrol[0].w2      = KEY_2;
  pcontrol[0].w3      = KEY_3;
  pcontrol[0].w4      = KEY_4;
  pcontrol[0].w5      = KEY_5;
  pcontrol[0].w6      = KEY_6;
  pcontrol[0].pw      = KEY_7;
  pcontrol[0].nw      = KEY_8;
  pcontrol[0].special1= KEY_ENTER;

/* TEST
  pcontrol[0].pw      = JOY0_B2;
  pcontrol[0].nw      = JOY0_B4;
  pcontrol[0].shoot   = JOY0_UP;
  pcontrol[0].thrust  = JOY0_DOWN;
* TEST */

  pcontrol[1].shoot   = KEY_W;
  pcontrol[1].thrust  = KEY_LCONTROL;
  pcontrol[1].left    = KEY_A;
  pcontrol[1].right   = KEY_D;
  pcontrol[1].w1      = KEY_1;
  pcontrol[1].w2      = KEY_2;
  pcontrol[1].w3      = KEY_3;
  pcontrol[1].w4      = KEY_4;
  pcontrol[1].w5      = KEY_5;
  pcontrol[1].w6      = KEY_6;
  pcontrol[1].pw      = KEY_Q;
  pcontrol[1].nw      = KEY_E;
  pcontrol[1].special1= KEY_TAB;

  pcontrol[2].shoot   = KEY_UP;
  pcontrol[2].thrust  = KEY_RCONTROL;
  pcontrol[2].left    = KEY_LEFT;
  pcontrol[2].right   = KEY_RIGHT;
  pcontrol[2].w1      = KEY_1_PAD;
  pcontrol[2].w2      = KEY_2_PAD;
  pcontrol[2].w3      = KEY_3_PAD;
  pcontrol[2].w4      = KEY_4_PAD;
  pcontrol[2].w5      = KEY_5_PAD;
  pcontrol[2].w6      = KEY_6_PAD;
  pcontrol[2].pw      = KEY_7_PAD;
  pcontrol[2].nw      = KEY_9_PAD;
  pcontrol[2].special1= KEY_ENTER;
}


