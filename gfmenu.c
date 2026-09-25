/*
 *    GRAVITY FORCE, gfmenu.c
 *
 *    -- all the menu stuff
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gf.h"
#include "gfdraw.h"
#include "gfinit.h"
#include "gfmisc.h"
#include "gfpal.h"
#include "gfosd.h"
#include "gfspibul.h"
#include "gflink.h"
#include "gfcnfg.h"
#include "gfmenu.h"
#include "gfmes.h"
#include "gfstat.h"
#include "gfdmnu.h"
#include "gfsound.h"
#include "gflog.h"
#include "gfcontrl.h"
#include "ctype.h"
#include "gfg2pdog.h"

#define  maxstars  200
#define  maxbuttons 20
volatile int mgame_time=0;
int actual_menu=0,new_menu=0;
int button_anz,actual_button=0;
int menu_change = FALSE;
int retval;
int level_end=FALSE;
int set_option = FALSE;
DATAFILE *odata;
int quit_menu = FALSE, begin_play = FALSE;
int show_level = -1;
int lock_keyboard = FALSE;
char tmpstr[100];
static int input_active = FALSE;
char input_string[60];
char score_str[30]="";
int score_pos=0;

STARTYPE star[maxstars+1];

BUTTONTYPE button[maxbuttons+1];

BITMAP *title;
int title_in=FALSE, title_x=0, title_y=0, title_ex=0, title_ey=0, title_ax=0, title_ay=0,
    title_sx=0, title_sy=0, title_dx=0, title_dy=0;

static void mgame_timer(void)
{
  mgame_time++;
}
END_OF_FUNCTION(mgame_timer);

void get_lng_name(char *p, int level)
{
  char f[10],tmp[20];

  strcpy(f,p);
  if ( (level <= 9) && (level > 0) )
    strcat(f,"0");

  strcat(f,mitoa(tmp,level));

  strcat(f,".lng");
  strcpy(lng_name,f);
}

int joy_activity()
{
  if (joy[0].stick[0].axis[0].d1 || joy[0].stick[0].axis[0].d2 ||
      joy[0].stick[0].axis[1].d1 || joy[0].stick[0].axis[1].d2 ||
      joy[0].button[0].b || joy[0].button[1].b ||
      joy[0].button[2].b || joy[0].button[3].b ||
      joy[1].stick[0].axis[0].d1 || joy[1].stick[0].axis[0].d2 ||
      joy[1].stick[0].axis[1].d1 || joy[1].stick[0].axis[1].d2 ||
      joy[1].button[0].b || joy[1].button[1].b ||
      joy[1].button[2].b || joy[1].button[3].b)
  {
    return 1;
  }

  return 0;
}

int get_pressed_key()
{
  if (key[KEY_LSHIFT]) return KEY_LSHIFT;
  if (key[KEY_RSHIFT]) return KEY_RSHIFT;
  if (key[KEY_ALT]) return KEY_ALT;
  if (key[KEY_ALTGR]) return KEY_ALTGR;
  if (key[KEY_LCONTROL]) return KEY_LCONTROL;
  if (key[KEY_RCONTROL]) return KEY_RCONTROL;
  if (joy[0].stick[0].axis[0].d1) return JOY0_LEFT;
  if (joy[0].stick[0].axis[0].d2) return JOY0_RIGHT;
  if (joy[0].stick[0].axis[1].d1) return JOY0_UP;
  if (joy[0].stick[0].axis[1].d2) return JOY0_DOWN;
  if (joy[0].button[0].b) return JOY0_B1;
  if (joy[0].button[1].b) return JOY0_B2;
  if (joy[0].button[2].b) return JOY0_B3;
  if (joy[0].button[3].b) return JOY0_B4;
  if (joy[1].stick[0].axis[0].d1) return JOY1_LEFT;
  if (joy[1].stick[0].axis[0].d2) return JOY1_RIGHT;
  if (joy[1].stick[0].axis[1].d1) return JOY1_UP;
  if (joy[1].stick[0].axis[1].d2) return JOY1_DOWN;
  if (joy[1].button[0].b) return JOY1_B1;
  if (joy[1].button[1].b) return JOY1_B2;
  if (joy[1].button[2].b) return JOY1_B3;
  if (joy[1].button[3].b) return JOY1_B4;

  return 0;
}

// return 0 on duplicate or predefined keys
int get_key_okay()
{
  int nr,keynum;

  if (set_option)
  {
    keynum = get_pressed_key();

    // duplicate?
    if (actual_menu == MNU_KEYMAP_1P)
    {
      if (pcontrol[0].thrust < 1000) if (key[pcontrol[0].thrust]) return 0;
      if (pcontrol[0].shoot < 1000) if (key[pcontrol[0].shoot]) return 0;
      if (pcontrol[0].left < 1000) if (key[pcontrol[0].left]) return 0;
      if (pcontrol[0].right < 1000) if (key[pcontrol[0].right]) return 0;
      if (pcontrol[0].special1 < 1000) if (key[pcontrol[0].special1]) return 0;
      if (pcontrol[0].nw < 1000) if (key[pcontrol[0].nw]) return 0;
      if (pcontrol[0].pw < 1000) if (key[pcontrol[0].pw]) return 0;
      if (pcontrol[0].w1 < 1000) if (key[pcontrol[0].w1]) return 0;
      if (pcontrol[0].w2 < 1000) if (key[pcontrol[0].w2]) return 0;
      if (pcontrol[0].w3 < 1000) if (key[pcontrol[0].w3]) return 0;
      if (pcontrol[0].w4 < 1000) if (key[pcontrol[0].w4]) return 0;
      if (pcontrol[0].w5 < 1000) if (key[pcontrol[0].w5]) return 0;
      if (pcontrol[0].w6 < 1000) if (key[pcontrol[0].w6]) return 0;

      if (keynum == pcontrol[0].thrust) return 0;
      if (keynum == pcontrol[0].shoot) return 0;
      if (keynum == pcontrol[0].left) return 0;
      if (keynum == pcontrol[0].right) return 0;
      if (keynum == pcontrol[0].special1) return 0;
      if (keynum == pcontrol[0].nw) return 0;
      if (keynum == pcontrol[0].pw) return 0;
      if (keynum == pcontrol[0].w1) return 0;
      if (keynum == pcontrol[0].w2) return 0;
      if (keynum == pcontrol[0].w3) return 0;
      if (keynum == pcontrol[0].w4) return 0;
      if (keynum == pcontrol[0].w5) return 0;
      if (keynum == pcontrol[0].w6) return 0;
    }
    else if (actual_menu == MNU_KEYMAP_2P)
    {
      for (nr=1; nr <= 2; nr++)
      {
        if (pcontrol[nr].thrust < 1000) if (key[pcontrol[nr].thrust]) return 0;
        if (pcontrol[nr].shoot < 1000) if (key[pcontrol[nr].shoot]) return 0;
        if (pcontrol[nr].left < 1000) if (key[pcontrol[nr].left]) return 0;
        if (pcontrol[nr].right < 1000) if (key[pcontrol[nr].right]) return 0;
        if (pcontrol[nr].special1 < 1000) if (key[pcontrol[nr].special1]) return 0;
        if (pcontrol[nr].nw < 1000) if (key[pcontrol[nr].nw]) return 0;
        if (pcontrol[nr].pw < 1000) if (key[pcontrol[nr].pw]) return 0;
        if (keynum == pcontrol[nr].thrust) return 0;
        if (keynum == pcontrol[nr].shoot) return 0;
        if (keynum == pcontrol[nr].left) return 0;
        if (keynum == pcontrol[nr].right) return 0;
        if (keynum == pcontrol[nr].special1) return 0;
        if (keynum == pcontrol[nr].nw) return 0;
        if (keynum == pcontrol[nr].pw) return 0;
      }
    }
  
    // predefined? (r [radar], pause [pause], p [panel], f12 [screenshot])
    if (key[KEY_R]) return 0;
    if (key[KEY_PAUSE]) return 0;
    if (key[KEY_P]) return 0;
    if (key[KEY_F12]) return 0;
  }

  return 1;
}

void init_title()
{
  title = create_bitmap(250,12);
  clear(title);

  title_x = 650;
  title_y = 66;
  title_ax = 650;
  title_ay = 66;
  title_ex = 320;
  title_ey = 66;
  title_sx = 0;
  title_sy = 0;
  title_dx = 0;
  title_dy = 0;
  title_in = FALSE;
}

void kill_title()
{
  destroy_bitmap(title);
}

void change_title(int num)
{
  clear(title);
  if (num >= 0)
    draw_rle_sprite(title,data[num].dat,0,0);

  title_sx = title_sy = 0;
}

void mnu_clear_screen()
{
  int cnt;

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

void do_input_box()
{
  int c,tc;
  int maxtext = 22;

  if (keypressed())
  {
    c = readkey();
    tc = strlen(input_string);

    if (c>>8 == KEY_BACKSPACE)
    {
      input_string[tc-1] = '\0';
    }
    else if (tc <= maxtext)
    {
      input_string[tc] = c;
      input_string[tc+1] = '\0';
    } // else

    if ((getctrl(KEY_ENTER,1) || getctrl(JOY0_B1,1)))
    {
      clear(vscreen);
      strtrim(input_string);
      switch (input_active)
      {
        case 1 : if (strstr(encrypt_password(strupr(input_string)),strupr(level_passwords[current_level])))
                 {
                   config_file.config.level_available[current_level] = TRUE;
                   strcpy(button[0].text1,menu_message[MSG_MENU_PLAYLEVEL].text1);
                 }
                 else
                 {
                   play_sound_sample(snd_scream,1000,0,0,0,100);
                   strcpy(button[0].text1,menu_message[MSG_MENU_UNLOCKLEVEL].text1);
                 }
                 break;
      }
      input_active = FALSE;
    }

    if (key[KEY_ESC])
    {
      clear(vscreen);
      input_active = FALSE;
    }

    clear_keybuf();
  } // if keypressed
}

void draw_input_box()
{
  int l,w,h;

  l = text_length(impact14h,menu_message[MSG_MENU_ENTER_PASSWORD].text1);

  // back box
  w = ((RLE_SPRITE *)data[mnu_window_1].dat)->w;
  h = ((RLE_SPRITE *)data[mnu_window_1].dat)->h;
  draw_rle_sprite(vscreen,data[mnu_window_1].dat,middle(SCREEN_W,w),middle(SCREEN_H,h));

  // title text
  jhtextout(vscreen,impact14h,menu_message[MSG_MENU_ENTER_PASSWORD].text1,
            SCREEN_W/2-l/2,
            192,
            col_yellow);

  // input text
  jhtextout(vscreen,impact14h,input_string,
            middle(l,text_length(impact14h,input_string))+SCREEN_W/2 - l/2,
            242,
            col_white);

  vline(vscreen,
        middle(l,text_length(impact14h,input_string))+SCREEN_W/2 - l/2 + text_length(impact14h,input_string) + 2,
        245,
        245+text_height(impact14h)-7,
        col_orange);
}

void menu_draw_panel_text()
{
  text_mode(0);
  textout(vscreen,font,ftoa(tmpstr,pixel_anz),400,420,col_white);
  textout(vscreen,font,ftoa(tmpstr,counter),400,430,col_white);
  //textout(vscreen,font,ftoa(tmpstr,_go32_dpmi_remaining_virtual_memory()),400,440,col_white);
  //textout(vscreen,font,ftoa(tmpstr,_go32_dpmi_remaining_physical_memory()),400,450,col_white);
}


static void init_stars()
{
  int nr;

  for (nr=0; nr < maxstars; nr++)
  {
    star[nr].x = random() % 640;
    star[nr].y = random() % 480;
    star[nr].xs = (random() % 5)+1;
    star[nr].ys = 0;
    star[nr].col = makecol(star[nr].xs*40,star[nr].xs*40,star[nr].xs*50);
  } // for nr
}

static void update_stars()
{
  int nr;

  for (nr=0; nr < maxstars; nr++)
  {
    star[nr].x -= star[nr].xs;
    if (star[nr].x <= 0)
    {
      star[nr].x = 640;
      star[nr].y = random() % 480;
      star[nr].xs = (random() % 5)+1;
      star[nr].ys = 0;
      star[nr].col = makecol(star[nr].xs*40,star[nr].xs*40,star[nr].xs*50);
    }
//    if ((int)star[nr].x % 4 == 0)
//    add_pixels(1,star[nr].x-1+random()%2,star[nr].y-1+random()%2,0,-PARTICLE_GRAVITY,0,tbl_white,star[nr].xs*2,2,NULL);
  } // for nr
}

static void draw_stars()
{
  int nr;

  for (nr=0; nr < maxstars; nr++)
  {
    _putpixel(vscreen,star[nr].x,star[nr].y,star[nr].col);
    add_2_list(&rl,star[nr].x,star[nr].y,1,1);
  } // for nr
}

void init_menu_sounds()
{
  reserve_voices(8,-1);
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
  
    if (install_mod(5) >= 0)
    {
      gfmod = load_mod("./dat/gf.xm");
      set_mod_volume(150);
    }
  }
}


static void menu_mouse(void)
{
  extern int gf_mouse;
  static int lx = -1, ly = -1, lb = 0, injected = FALSE;
  int nr, mx = gf_mouse_x(), my = gf_mouse_y(), b = mouse_b & 1;

  if (injected) { key[KEY_ENTER] = 0; injected = FALSE; }
  if (lx < 0) { lx = gf_mouse_x(); ly = gf_mouse_y(); }   // resting cursor must not steal the selection
  if (!gf_mouse) return;
  for (nr = 0; nr < button_anz; nr++)
    if (button[nr].state != ST_DEACTIVATED &&
        mx >= button[nr].x && mx < button[nr].x + button[nr].w &&
        my >= button[nr].y && my < button[nr].y + button[nr].h)
    {
      if (mx != lx || my != ly) actual_button = nr;
      if (b && !lb && actual_button == nr) { key[KEY_ENTER] = 1; injected = TRUE; }
    }

  // click a "[nn]" level number in the mission / training high score box
  if (b && !lb && (actual_menu == MNU_SPLAYER || actual_menu == MNU_SP_TRAINING))
  {
    int t = (actual_menu == MNU_SP_TRAINING);
    int bx = t ? button[2].x + 79 : button[3].x + 15;   // same layout as the drawing code
    int by = (t ? button[2].y : button[3].y) + 10;
    int max = t ? cur_max_training_levels : cur_max_levels;
    nr = (mx - bx) / 28 + 1;
    if (mx >= bx && my >= by && my < by + text_height(impact10h) &&
        (mx - bx) % 28 < text_length(impact10h, "[00]") && nr <= max)
    {
      if (t) current_training_level = nr;
      else
      {
        current_level = nr;
        CalcScoreString(score_str,
                        hiscore_file.hiscore[current_level].nr[score_pos].name,
                        hiscore_file.hiscore[current_level].nr[score_pos].score);
      }
      strcpy(button[0].text1, menu_message[t || config_file.config.level_available[nr] ?
                                           MSG_MENU_PLAYLEVEL : MSG_MENU_UNLOCKLEVEL].text1);
      actual_button = 0;
      play_sound_sample(snd_click,1000,0,0,0,100);
    }
  }
  lx = mx; ly = my; lb = b;
}

void gf_init_menu()
{
//  read_message_file(LANGUAGE);
/*
  if (allegro_init())
  {
    printf("%s %s\n",error_message[MSG_ERROR_ERROR].text1,allegro_error);
    exit(46);
  }
  install_timer();
  install_keyboard();
  { extern int gf_mouse; gf_mouse = (install_mouse() >= 0); }
  if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,480,0,0))
  {
    printf("%s %s\n",error_message[MSG_ERROR_GRAPHIC_INIT].text1,allegro_error);
    exit(47);
  };
*/
  vscreen = create_bitmap(800,600);
  if (!vscreen)
  {
    error_exit(39,error_message[MSG_ERROR_GRAPHIC_INIT].text1,"not enough ram",39,0);
  };
  clear(vscreen);

  packfile_password("tanja");
  strcpy(tmp_dir,dat_dir);
  data = load_datafile((char*)strcat(tmp_dir,"gfmnu.dat"));
  if (!data) { error_exit(40,error_message[MSG_ERROR_FILE_NOT_FOUND].text1,"gfmnu.dat",40,0); }

  strcpy(tmp_dir,dat_dir);
  odata = load_datafile((char*)strcat(tmp_dir,"gf.dat"));
  packfile_password(NULL);
  if (!odata) { error_exit(41,error_message[MSG_ERROR_FILE_NOT_FOUND].text1,"gf.dat",41,0); }

  setup_global_stuff();

  init_menu_sounds();

  init_title();

  fade_out_active = FALSE; fade_pos = 0; fade_opos = 0; fade_type = 0; fade_count = 0; fade_count_to = 0; fade_speed = 0;
  p_fade_out_active = FALSE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 0; p_fade_count = 0; p_fade_count_to = 0; p_fade_speed = 0;
  set_palette(data[mnu_pal].dat);

  create_rgb_table(&rgb_table,data[mnu_pal].dat,NULL);
  rgb_map = &rgb_table;

#ifdef ALLEGRO_DOS
  antialias_init(NULL);
#endif

  impact10h = odata[impact10].dat;
  impact14h = odata[impact14].dat;
  arial14h = odata[arial14].dat;

  impact10hm = odata[impact10m].dat;
  impact14hm = odata[impact14m].dat;
  arial14hm = odata[arial14m].dat;

  init_colors();
  init_tables();
  init_stars();
}

void exit_menu()
{
  if (gfmod && PLAY_MENU_MUSIC == 1) stop_mod();
  if (gfmod) remove_mod();
  if (gfmod) destroy_mod(gfmod);

  kill_title();
  destroy_bitmap(vscreen);
  unload_datafile(data);
  unload_datafile(odata);
  unload_datafile(sounddata);
  clear_arrays();

//  allegro_exit();
}

void menu_main_init()
{
  int w;
  int h;

  w = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->h;

  button[0].type = TP_NORMAL; button[0].state = ST_SELECTED;
  button[0].xspd = 0; button[0].yspd = 0;
  button[0].xend = 0; button[0].yend = 0;
  button[0].xscl = 0; button[0].yscl = 0;
  button[0].x = middle(SCREEN_W,w);
  button[0].y = 100;
  button[0].w = w; button[0].h = h;
  button[0].draw = TRUE;
  strcpy(button[0].text1,menu_message[MSG_MENU_SINGLEPLAYER].text1);
  button[0].textcol = col_yellow; button[0].font = impact14h;

  button[1].type = TP_NORMAL; button[1].state = ST_OK;
  button[1].xspd = 0; button[1].yspd = 0;
  button[1].xend = 0; button[1].yend = 0;
  button[1].xscl = 0; button[1].yscl = 0;
  button[1].x = middle(SCREEN_W,w);
  button[1].y = 160;
  button[1].w = w; button[1].h = h;
  button[1].draw = TRUE;
  strcpy(button[1].text1,menu_message[MSG_MENU_MULTIPLAYER].text1);
  button[1].textcol = col_yellow; button[1].font = impact14h;

  button[2].type = TP_NORMAL; button[2].state = ST_OK;
  button[2].xspd = 0; button[2].yspd = 0;
  button[2].xend = 0; button[2].yend = 0;
  button[2].xscl = 0; button[2].yscl = 0;
  button[2].x = middle(SCREEN_W,w);
  button[2].y = 220;
  button[2].w = w; button[2].h = h;
  button[2].draw = TRUE;
  strcpy(button[2].text1,menu_message[MSG_MENU_OPTIONS].text1);
  button[2].textcol = col_yellow; button[2].font = impact14h;

  button[3].type = TP_NORMAL; button[3].state = ST_OK;
  button[3].xspd = 0; button[3].yspd = 0;
  button[3].xend = 0; button[3].yend = 0;
  button[3].xscl = 0; button[3].yscl = 0;
  button[3].x = middle(SCREEN_W,w);
  button[3].y = 380;
  button[3].w = w; button[3].h = h;
  button[3].draw = TRUE;
  strcpy(button[3].text1,menu_message[MSG_MENU_QUIT].text1);
  button[3].textcol = col_orange; button[3].font = impact14h;

  button_anz = 4;

  change_title(-1); // no title
}

void menu_sp_sel_init()
{
  int w;
  int h;

  w = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->h;

  button[0].type = TP_NORMAL; button[0].state = ST_SELECTED;
  button[0].xspd = 0; button[0].yspd = 0;
  button[0].xend = 0; button[0].yend = 0;
  button[0].xscl = 0; button[0].yscl = 0;
  button[0].x = middle(SCREEN_W,w);
  button[0].y = 100;
  button[0].w = w; button[0].h = h;
  button[0].draw = TRUE;
  strcpy(button[0].text1,menu_message[MSG_MENU_TRAINING].text1);
  button[0].textcol = col_yellow; button[0].font = impact14h;

  button[1].type = TP_NORMAL; button[1].state = ST_OK;
  button[1].xspd = 0; button[1].yspd = 0;
  button[1].xend = 0; button[1].yend = 0;
  button[1].xscl = 0; button[1].yscl = 0;
  button[1].x = middle(SCREEN_W,w);
  button[1].y = 160;
  button[1].w = w; button[1].h = h;
  button[1].draw = TRUE;
  strcpy(button[1].text1,menu_message[MSG_MENU_CAMPAIGN].text1);
  button[1].textcol = col_yellow; button[1].font = impact14h;

  button[2].type = TP_NORMAL; button[2].state = ST_OK;
  button[2].xspd = 0; button[2].yspd = 0;
  button[2].xend = 0; button[2].yend = 0;
  button[2].xscl = 0; button[2].yscl = 0;
  button[2].x = middle(SCREEN_W,w);
  button[2].y = 220;
  button[2].w = w; button[2].h = h;
  button[2].draw = TRUE;
  strcpy(button[2].text1,menu_message[MSG_MENU_RACE].text1);
  button[2].textcol = col_yellow; button[2].font = impact14h;

  button[3].type = TP_NORMAL; button[3].state = ST_OK;
  button[3].xspd = 0; button[3].yspd = 0;
  button[3].xend = 0; button[3].yend = 0;
  button[3].xscl = 0; button[3].yscl = 0;
  button[3].x = middle(SCREEN_W,w);
  button[3].y = 380;
  button[3].w = w; button[3].h = h;
  button[3].draw = TRUE;
  strcpy(button[3].text1,menu_message[MSG_MENU_BACK].text1);
  button[3].textcol = col_orange; button[3].font = impact14h;

  button_anz = 4;

  change_title(mnu_t_singleplayer);
}


void menu_splayer_init()
{
  int w;
  int h;

  w = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->h;

  button[0].type = TP_NORMAL; button[0].state = ST_SELECTED;
  button[0].xspd = 0; button[0].yspd = 0;
  button[0].xend = 0; button[0].yend = 0;
  button[0].xscl = 0; button[0].yscl = 0;
  button[0].x = 50;
  button[0].y = 90;
  button[0].w = w; button[0].h = h;
  button[0].draw = TRUE;
//  strcpy(button[0].text1,menu_message[MSG_MENU_PLAYLEVEL].text1);
  button[0].textcol = col_yellow; button[0].font = impact14h;
  if (config_file.config.level_available[current_level])
    strcpy(button[0].text1,menu_message[MSG_MENU_PLAYLEVEL].text1);
  else
    strcpy(button[0].text1,menu_message[MSG_MENU_UNLOCKLEVEL].text1);

  button[1].type = TP_NORMAL; button[1].state = ST_DEACTIVATED;
  button[1].xspd = 0; button[1].yspd = 0;
  button[1].xend = 0; button[1].yend = 0;
  button[1].xscl = 0; button[1].yscl = 0;
  button[1].x = 50;
  button[1].y = 150;
  button[1].w = w; button[1].h = h;
  button[1].draw = FALSE;
  strcpy(button[1].text1,menu_message[MSG_MENU_PASSWORD].text1);
  button[1].textcol = col_yellow; button[1].font = impact14h;

  button[2].type = TP_NORMAL; button[2].state = ST_OK;
  button[2].xspd = 0; button[2].yspd = 0;
  button[2].xend = 0; button[2].yend = 0;
  button[2].xscl = 0; button[2].yscl = 0;
  button[2].x = 50;
  button[2].y = 415;
  button[2].w = w; button[2].h = h;
  button[2].draw = TRUE;
  strcpy(button[2].text1,menu_message[MSG_MENU_BACK].text1);
  button[2].textcol = col_orange; button[2].font = impact14h;

  w = ((RLE_SPRITE *)(data[mnu_hiscore].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_hiscore].dat))->h;

  button[3].type = TP_HISCORE; button[3].state = ST_DEACTIVATED;
  button[3].xspd = 0; button[3].yspd = 0;
  button[3].xend = 0; button[3].yend = 0;
  button[3].xscl = 0; button[3].yscl = 0;
  button[3].x = 290;
  button[3].y = 90;
  button[3].w = w; button[3].h = h;
  button[3].draw = TRUE;
  button[3].textcol = col_orange; button[3].font = impact14h;

  button_anz = 4;

  get_lng_name("gf",current_level);
  read_level_text(LANGUAGE);

  change_title(mnu_t_missions);
}

void menu_mp_sel_init()
{
  int w;
  int h;

  w = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->h;

  button[0].type = TP_NORMAL; button[0].state = ST_SELECTED;
  button[0].xspd = 0; button[0].yspd = 0;
  button[0].xend = 0; button[0].yend = 0;
  button[0].xscl = 0; button[0].yscl = 0;
  button[0].x = middle(SCREEN_W,w);
  button[0].y = 100;
  button[0].w = w; button[0].h = h;
  button[0].draw = TRUE;
  strcpy(button[0].text1,menu_message[MSG_MENU_QDOGFIGHT].text1);
  button[0].textcol = col_yellow; button[0].font = impact14h;

  button[1].type = TP_NORMAL; button[1].state = ST_DEACTIVATED;
  button[1].xspd = 0; button[1].yspd = 0;
  button[1].xend = 0; button[1].yend = 0;
  button[1].xscl = 0; button[1].yscl = 0;
  button[1].x = middle(SCREEN_W,w);
  button[1].y = 160;
  button[1].w = w; button[1].h = h;
  button[1].draw = FALSE;
  strcpy(button[1].text1,menu_message[MSG_MENU_DOGFIGHT].text1);
  button[1].textcol = col_yellow; button[1].font = impact14h;

  button[2].type = TP_NORMAL; button[2].state = ST_DEACTIVATED;
  button[2].xspd = 0; button[2].yspd = 0;
  button[2].xend = 0; button[2].yend = 0;
  button[2].xscl = 0; button[2].yscl = 0;
  button[2].x = middle(SCREEN_W,w);
  button[2].y = 220;
  button[2].w = w; button[2].h = h;
  button[2].draw = FALSE;
  strcpy(button[2].text1,menu_message[MSG_MENU_MPRACE].text1);
  button[2].textcol = col_yellow; button[2].font = impact14h;

  button[3].type = TP_NORMAL; button[3].state = ST_OK;
  button[3].xspd = 0; button[3].yspd = 0;
  button[3].xend = 0; button[3].yend = 0;
  button[3].xscl = 0; button[3].yscl = 0;
  button[3].x = middle(SCREEN_W,w);
  button[3].y = 380;
  button[3].w = w; button[3].h = h;
  button[3].draw = TRUE;
  strcpy(button[3].text1,menu_message[MSG_MENU_BACK].text1);
  button[3].textcol = col_orange; button[3].font = impact14h;

  button_anz = 4;

  change_title(mnu_t_multiplayer);
}

void menu_sp_race_init()
{
  int w;
  int h;

  w = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->h;

  button[0].type = TP_NORMAL; button[0].state = ST_SELECTED;
  button[0].xspd = 0; button[0].yspd = 0;
  button[0].xend = 0; button[0].yend = 0;
  button[0].xscl = 0; button[0].yscl = 0;
  button[0].x = 50;
  button[0].y = 90;
  button[0].w = w; button[0].h = h;
  button[0].draw = TRUE;
  button[0].textcol = col_yellow; button[0].font = impact14h;
  strcpy(button[0].text1,menu_message[MSG_MENU_PLAYLEVEL].text1);

  button[1].type = TP_NORMAL; button[1].state = ST_OK;
  button[1].xspd = 0; button[1].yspd = 0;
  button[1].xend = 0; button[1].yend = 0;
  button[1].xscl = 0; button[1].yscl = 0;
  button[1].x = 50;
  button[1].y = 415;
  button[1].w = w; button[1].h = h;
  button[1].draw = TRUE;
  strcpy(button[1].text1,menu_message[MSG_MENU_BACK].text1);
  button[1].textcol = col_orange; button[1].font = impact14h;

  w = ((RLE_SPRITE *)(data[mnu_hiscore].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_hiscore].dat))->h;

  button[2].type = TP_HISCORE; button[2].state = ST_DEACTIVATED;
  button[2].xspd = 0; button[2].yspd = 0;
  button[2].xend = 0; button[2].yend = 0;
  button[2].xscl = 0; button[2].yscl = 0;
  button[2].x = 290;
  button[2].y = 90;
  button[2].w = w; button[2].h = h;
  button[2].draw = TRUE;
  button[2].textcol = col_orange; button[2].font = impact14h;


  button_anz = 3;

  get_lng_name("gfr",current_sprace_level);
  read_level_text(LANGUAGE);

  change_title(mnu_t_sprace);
}

void menu_mp_race_init()
{
  int w;
  int h;

  w = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->h;

  button[0].type = TP_NORMAL; button[0].state = ST_OK;
  button[0].xspd = 0; button[0].yspd = 0;
  button[0].xend = 0; button[0].yend = 0;
  button[0].xscl = 0; button[0].yscl = 0;
  button[0].x = middle(SCREEN_W,w);
  button[0].y = 380;
  button[0].w = w; button[0].h = h;
  button[0].draw = TRUE;
  strcpy(button[0].text1,menu_message[MSG_MENU_BACK].text1);
  button[0].textcol = col_orange; button[0].font = impact14h;

  button_anz = 1;

  change_title(mnu_t_mprace);
}

void menu_dogfight_init()
{
  int w;
  int h;

  w = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->h;

  button[0].type = TP_NORMAL; button[0].state = ST_OK;
  button[0].xspd = 0; button[0].yspd = 0;
  button[0].xend = 0; button[0].yend = 0;
  button[0].xscl = 0; button[0].yscl = 0;
  button[0].x = middle(SCREEN_W,w);
  button[0].y = 380;
  button[0].w = w; button[0].h = h;
  button[0].draw = TRUE;
  strcpy(button[0].text1,menu_message[MSG_MENU_BACK].text1);
  button[0].textcol = col_orange; button[0].font = impact14h;

  button_anz = 1;

  change_title(mnu_t_dogfight);
}

void menu_qdogfight_init()
{
  int w;
  int h;

  w = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->h;

  button[0].type = TP_NORMAL; button[0].state = ST_SELECTED;
  button[0].xspd = 0; button[0].yspd = 0;
  button[0].xend = 0; button[0].yend = 0;
  button[0].xscl = 0; button[0].yscl = 0;
  button[0].x = 50;
  button[0].y = 90;
  button[0].w = w; button[0].h = h;
  button[0].draw = TRUE;
  button[0].textcol = col_yellow; button[0].font = impact14h;
  strcpy(button[0].text1,menu_message[MSG_MENU_PLAYLEVEL].text1);

  button[1].type = TP_OPTIONS2; button[1].state = ST_OK;
  button[1].xspd = 0; button[1].yspd = 0;
  button[1].xend = 0; button[1].yend = 0;
  button[1].xscl = 0; button[1].yscl = 0;
  button[1].x = 50;
  button[1].y = 150;
  button[1].w = w; button[1].h = h;
  button[1].draw = TRUE;
  button[1].textcol = col_yellow; button[1].font = impact10h;
  strcpy(button[1].text1,menu_message[MSG_MENU_FRAG_LIMIT].text1);

  button[2].type = TP_OPTIONS2; button[2].state = ST_OK;
  button[2].xspd = 0; button[2].yspd = 0;
  button[2].xend = 0; button[2].yend = 0;
  button[2].xscl = 0; button[2].yscl = 0;
  button[2].x = 50;
  button[2].y = 190;
  button[2].w = w; button[2].h = h;
  button[2].draw = TRUE;
  button[2].textcol = col_yellow; button[2].font = impact10h;
  strcpy(button[2].text1,menu_message[MSG_MENU_TIME_LIMIT].text1);

  button[3].type = TP_OPTIONS2; button[3].state = ST_OK;
  button[3].xspd = 0; button[3].yspd = 0;
  button[3].xend = 0; button[3].yend = 0;
  button[3].xscl = 0; button[3].yscl = 0;
  button[3].x = 50;
  button[3].y = 230;
  button[3].w = w; button[3].h = h;
  button[3].draw = TRUE;
  button[3].textcol = col_yellow; button[3].font = impact10h;
  strcpy(button[3].text1,menu_message[MSG_MENU_GRAVITY].text1);

  button[4].type = TP_OPTIONS2; button[4].state = ST_OK;
  button[4].xspd = 0; button[4].yspd = 0;
  button[4].xend = 0; button[4].yend = 0;
  button[4].xscl = 0; button[4].yscl = 0;
  button[4].x = 50;
  button[4].y = 270;
  button[4].w = w; button[4].h = h;
  button[4].draw = TRUE;
  button[4].textcol = col_yellow; button[4].font = impact10h;
  strcpy(button[4].text1,menu_message[MSG_MENU_P1WEAPONS].text1);

  button[5].type = TP_OPTIONS2; button[5].state = ST_OK;
  button[5].xspd = 0; button[5].yspd = 0;
  button[5].xend = 0; button[5].yend = 0;
  button[5].xscl = 0; button[5].yscl = 0;
  button[5].x = 50;
  button[5].y = 310;
  button[5].w = w; button[5].h = h;
  button[5].draw = TRUE;
  button[5].textcol = col_yellow; button[5].font = impact10h;
  strcpy(button[5].text1,menu_message[MSG_MENU_P2WEAPONS].text1);

  button[6].type = TP_OPTIONS2; button[6].state = ST_OK;
  button[6].xspd = 0; button[6].yspd = 0;
  button[6].xend = 0; button[6].yend = 0;
  button[6].xscl = 0; button[6].yscl = 0;
  button[6].x = 50;
  button[6].y = 350;
  button[6].w = w; button[6].h = h;
  button[6].draw = TRUE;
  button[6].textcol = col_yellow; button[6].font = impact10h;
  strcpy(button[6].text1,menu_message[MSG_MENU_EXTRAS].text1);

  button[7].type = TP_NORMAL; button[7].state = ST_OK;
  button[7].xspd = 0; button[7].yspd = 0;
  button[7].xend = 0; button[7].yend = 0;
  button[7].xscl = 0; button[7].yscl = 0;
  button[7].x = 50;
  button[7].y = 415;
  button[7].w = w; button[7].h = h;
  button[7].draw = TRUE;
  strcpy(button[7].text1,menu_message[MSG_MENU_BACK].text1);
  button[7].textcol = col_orange; button[7].font = impact14h;

  w = ((RLE_SPRITE *)(data[mnu_hiscore].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_hiscore].dat))->h;

  button[8].type = TP_HISCORE; button[8].state = ST_DEACTIVATED;
  button[8].xspd = 0; button[8].yspd = 0;
  button[8].xend = 0; button[8].yend = 0;
  button[8].xscl = 0; button[8].yscl = 0;
  button[8].x = 290;
  button[8].y = 90;
  button[8].w = w; button[8].h = h;
  button[8].draw = TRUE;
  button[8].textcol = col_orange; button[8].font = impact14h;

  button_anz = 9;

  get_lng_name("gfqd",current_qdogfight_level);
  read_level_text(LANGUAGE);

  change_title(mnu_t_2pdog);
}


void menu_sp_training_init()
{
  int w;
  int h;

  w = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->h;

  button[0].type = TP_NORMAL; button[0].state = ST_SELECTED;
  button[0].xspd = 0; button[0].yspd = 0;
  button[0].xend = 0; button[0].yend = 0;
  button[0].xscl = 0; button[0].yscl = 0;
  button[0].x = 50;
  button[0].y = 90;
  button[0].w = w; button[0].h = h;
  button[0].draw = TRUE;
  button[0].textcol = col_yellow; button[0].font = impact14h;
  strcpy(button[0].text1,menu_message[MSG_MENU_PLAYLEVEL].text1);

  button[1].type = TP_NORMAL; button[1].state = ST_OK;
  button[1].xspd = 0; button[1].yspd = 0;
  button[1].xend = 0; button[1].yend = 0;
  button[1].xscl = 0; button[1].yscl = 0;
  button[1].x = 50;
  button[1].y = 415;
  button[1].w = w; button[1].h = h;
  button[1].draw = TRUE;
  strcpy(button[1].text1,menu_message[MSG_MENU_BACK].text1);
  button[1].textcol = col_orange; button[1].font = impact14h;

  w = ((RLE_SPRITE *)(data[mnu_hiscore].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_hiscore].dat))->h;

  button[2].type = TP_HISCORE; button[2].state = ST_DEACTIVATED;
  button[2].xspd = 0; button[2].yspd = 0;
  button[2].xend = 0; button[2].yend = 0;
  button[2].xscl = 0; button[2].yscl = 0;
  button[2].x = 290;
  button[2].y = 90;
  button[2].w = w; button[2].h = h;
  button[2].draw = TRUE;
  button[2].textcol = col_orange; button[2].font = impact14h;

  button_anz = 3;

  get_lng_name("gft",current_training_level);
  read_level_text(LANGUAGE);

  change_title(mnu_t_training);
}


void menu_options_init()
{
  int w;
  int h;

  w = ((RLE_SPRITE *)(data[mnu_button2_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button2_1].dat))->h;

  button[0].type = TP_OPTIONS; button[0].state = ST_SELECTED;
  button[0].xspd = 0; button[0].yspd = 0;
  button[0].xend = 0; button[0].yend = 0;
  button[0].xscl = 0; button[0].yscl = 0;
  button[0].x = middle(SCREEN_W,w)-w+50;
  button[0].y = 90;
  button[0].w = w; button[0].h = h;
  button[0].draw = TRUE;
  strcpy(button[0].text1,menu_message[MSG_MENU_OPT_LANGUAGE].text1);
  button[0].textcol = col_yellow; button[0].font = impact14h;

  button[1].type = TP_OPTIONS; button[1].state = ST_OK;
  button[1].xspd = 0; button[1].yspd = 0;
  button[1].xend = 0; button[1].yend = 0;
  button[1].xscl = 0; button[1].yscl = 0;
  button[1].x = middle(SCREEN_W,w)-w+50;
  button[1].y = 180;
  button[1].w = w; button[1].h = h;
  button[1].draw = TRUE;
  strcpy(button[1].text1,menu_message[MSG_MENU_OPT_PIXANZ].text1);
  button[1].textcol = col_yellow; button[1].font = impact14h;

  button[2].type = TP_OPTIONS; button[2].state = ST_OK;
  button[2].xspd = 0; button[2].yspd = 0;
  button[2].xend = 0; button[2].yend = 0;
  button[2].xscl = 0; button[2].yscl = 0;
  button[2].x = middle(SCREEN_W,w)-w+50;
  button[2].y = 270;
  button[2].w = w; button[2].h = h;
  button[2].draw = TRUE;
  strcpy(button[2].text1,menu_message[MSG_MENU_OPT_SOUNDVOL].text1);
  button[2].textcol = col_yellow; button[2].font = impact14h;

  button[3].type = TP_OPTIONS; button[3].state = ST_OK;
  button[3].xspd = 0; button[3].yspd = 0;
  button[3].xend = 0; button[3].yend = 0;
  button[3].xscl = 0; button[3].yscl = 0;
  button[3].x = middle(SCREEN_W,w)+w/2+50;
  button[3].y = 90;
  button[3].w = w; button[3].h = h;
  button[3].draw = TRUE;
  strcpy(button[3].text1,menu_message[MSG_MENU_OPT_DEBRIS].text1);
  button[3].textcol = col_yellow; button[3].font = impact14h;

  button[4].type = TP_OPTIONS; button[4].state = ST_OK;
  button[4].xspd = 0; button[4].yspd = 0;
  button[4].xend = 0; button[4].yend = 0;
  button[4].xscl = 0; button[4].yscl = 0;
  button[4].x = middle(SCREEN_W,w)+w/2+50;
  button[4].y = 180;
  button[4].w = w; button[4].h = h;
  button[4].draw = TRUE;
  strcpy(button[4].text1,menu_message[MSG_MENU_OPT_PGRAV].text1);
  button[4].textcol = col_yellow; button[4].font = impact14h;

  button[5].type = TP_OPTIONS; button[5].state = ST_OK;
  button[5].xspd = 0; button[5].yspd = 0;
  button[5].xend = 0; button[5].yend = 0;
  button[5].xscl = 0; button[5].yscl = 0;
  button[5].x = middle(SCREEN_W,w)+w/2+50;
  button[5].y = 270;
  button[5].w = w; button[5].h = h;
  button[5].draw = TRUE;
  strcpy(button[5].text1,menu_message[MSG_MENU_OPT_MMUSIC].text1);
  button[5].textcol = col_yellow; button[5].font = impact14h;

  w = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->h;

  button[6].type = TP_NORMAL; button[6].state = ST_OK;
  button[6].xspd = 0; button[6].yspd = 0;
  button[6].xend = 0; button[6].yend = 0;
  button[6].xscl = 0; button[6].yscl = 0;
  button[6].x = middle(SCREEN_W,w)-105;
  button[6].y = 415;
  button[6].w = w; button[6].h = h;
  button[6].draw = TRUE;
  strcpy(button[6].text1,menu_message[MSG_MENU_BACK].text1);
  button[6].textcol = col_orange; button[6].font = impact14h;

  button[7].type = TP_NORMAL; button[7].state = ST_OK;
  button[7].xspd = 0; button[7].yspd = 0;
  button[7].xend = 0; button[7].yend = 0;
  button[7].xscl = 0; button[7].yscl = 0;
  button[7].x = middle(SCREEN_W,w)+105;
  button[7].y = 415;
  button[7].w = w; button[7].h = h;
  button[7].draw = TRUE;
  strcpy(button[7].text1,menu_message[MSG_KEY_CONTROLS].text1);
  button[7].textcol = col_white; button[7].font = impact14h;

  button_anz = 8;

  change_title(mnu_t_options);
}

void menu_keymap_1p_init()
{
  int w;
  int h;

  w = ((RLE_SPRITE *)(data[mnu_button3_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button3_1].dat))->h;

  button[0].type = TP_OPTIONS2; button[0].state = ST_SELECTED;
  button[0].xspd = 0; button[0].yspd = 0;
  button[0].xend = 0; button[0].yend = 0;
  button[0].xscl = 0; button[0].yscl = 0;
  button[0].x = 80;
  button[0].y = 90;
  button[0].w = w; button[0].h = h;
  button[0].draw = TRUE;
  button[0].textcol = col_yellow; button[0].font = impact10h;
  strcpy(button[0].text1,menu_message[MSG_KEY_THRUST].text1);

  button[1].type = TP_OPTIONS2; button[1].state = ST_OK;
  button[1].xspd = 0; button[1].yspd = 0;
  button[1].xend = 0; button[1].yend = 0;
  button[1].xscl = 0; button[1].yscl = 0;
  button[1].x = 80;
  button[1].y = 130;
  button[1].w = w; button[1].h = h;
  button[1].draw = TRUE;
  button[1].textcol = col_yellow; button[1].font = impact10h;
  strcpy(button[1].text1,menu_message[MSG_KEY_SHOOT].text1);

  button[2].type = TP_OPTIONS2; button[2].state = ST_OK;
  button[2].xspd = 0; button[2].yspd = 0;
  button[2].xend = 0; button[2].yend = 0;
  button[2].xscl = 0; button[2].yscl = 0;
  button[2].x = 80;
  button[2].y = 170;
  button[2].w = w; button[2].h = h;
  button[2].draw = TRUE;
  button[2].textcol = col_yellow; button[2].font = impact10h;
  strcpy(button[2].text1,menu_message[MSG_KEY_LEFT].text1);

  button[3].type = TP_OPTIONS2; button[3].state = ST_OK;
  button[3].xspd = 0; button[3].yspd = 0;
  button[3].xend = 0; button[3].yend = 0;
  button[3].xscl = 0; button[3].yscl = 0;
  button[3].x = 80;
  button[3].y = 210;
  button[3].w = w; button[3].h = h;
  button[3].draw = TRUE;
  button[3].textcol = col_yellow; button[3].font = impact10h;
  strcpy(button[3].text1,menu_message[MSG_KEY_RIGHT].text1);

  button[4].type = TP_OPTIONS2; button[4].state = ST_OK;
  button[4].xspd = 0; button[4].yspd = 0;
  button[4].xend = 0; button[4].yend = 0;
  button[4].xscl = 0; button[4].yscl = 0;
  button[4].x = 80;
  button[4].y = 250;
  button[4].w = w; button[4].h = h;
  button[4].draw = TRUE;
  button[4].textcol = col_yellow; button[4].font = impact10h;
  strcpy(button[4].text1,menu_message[MSG_KEY_SPECIAL].text1);

  button[5].type = TP_OPTIONS2; button[5].state = ST_OK;
  button[5].xspd = 0; button[5].yspd = 0;
  button[5].xend = 0; button[5].yend = 0;
  button[5].xscl = 0; button[5].yscl = 0;
  button[5].x = 80;
  button[5].y = 290;
  button[5].w = w; button[5].h = h;
  button[5].draw = TRUE;
  button[5].textcol = col_yellow; button[5].font = impact10h;
  strcpy(button[5].text1,menu_message[MSG_KEY_NWEAPON].text1);

  button[6].type = TP_OPTIONS2; button[6].state = ST_OK;
  button[6].xspd = 0; button[6].yspd = 0;
  button[6].xend = 0; button[6].yend = 0;
  button[6].xscl = 0; button[6].yscl = 0;
  button[6].x = 80;
  button[6].y = 330;
  button[6].w = w; button[6].h = h;
  button[6].draw = TRUE;
  button[6].textcol = col_yellow; button[6].font = impact10h;
  strcpy(button[6].text1,menu_message[MSG_KEY_PWEAPON].text1);

  button[7].type = TP_OPTIONS2; button[7].state = ST_OK;
  button[7].xspd = 0; button[7].yspd = 0;
  button[7].xend = 0; button[7].yend = 0;
  button[7].xscl = 0; button[7].yscl = 0;
  button[7].x = 360;
  button[7].y = 90;
  button[7].w = w; button[7].h = h;
  button[7].draw = TRUE;
  button[7].textcol = col_yellow; button[7].font = impact10h;
  strcpy(button[7].text1,menu_message[MSG_KEY_W1].text1);

  button[8].type = TP_OPTIONS2; button[8].state = ST_OK;
  button[8].xspd = 0; button[8].yspd = 0;
  button[8].xend = 0; button[8].yend = 0;
  button[8].xscl = 0; button[8].yscl = 0;
  button[8].x = 360;
  button[8].y = 130;
  button[8].w = w; button[8].h = h;
  button[8].draw = TRUE;
  button[8].textcol = col_yellow; button[8].font = impact10h;
  strcpy(button[8].text1,menu_message[MSG_KEY_W2].text1);

  button[9].type = TP_OPTIONS2; button[9].state = ST_OK;
  button[9].xspd = 0; button[9].yspd = 0;
  button[9].xend = 0; button[9].yend = 0;
  button[9].xscl = 0; button[9].yscl = 0;
  button[9].x = 360;
  button[9].y = 170;
  button[9].w = w; button[9].h = h;
  button[9].draw = TRUE;
  button[9].textcol = col_yellow; button[9].font = impact10h;
  strcpy(button[9].text1,menu_message[MSG_KEY_W3].text1);

  button[10].type = TP_OPTIONS2; button[10].state = ST_OK;
  button[10].xspd = 0; button[10].yspd = 0;
  button[10].xend = 0; button[10].yend = 0;
  button[10].xscl = 0; button[10].yscl = 0;
  button[10].x = 360;
  button[10].y = 210;
  button[10].w = w; button[10].h = h;
  button[10].draw = TRUE;
  button[10].textcol = col_yellow; button[10].font = impact10h;
  strcpy(button[10].text1,menu_message[MSG_KEY_W4].text1);

  button[11].type = TP_OPTIONS2; button[11].state = ST_OK;
  button[11].xspd = 0; button[11].yspd = 0;
  button[11].xend = 0; button[11].yend = 0;
  button[11].xscl = 0; button[11].yscl = 0;
  button[11].x = 360;
  button[11].y = 250;
  button[11].w = w; button[11].h = h;
  button[11].draw = TRUE;
  button[11].textcol = col_yellow; button[11].font = impact10h;
  strcpy(button[11].text1,menu_message[MSG_KEY_W5].text1);

  button[12].type = TP_OPTIONS2; button[12].state = ST_OK;
  button[12].xspd = 0; button[12].yspd = 0;
  button[12].xend = 0; button[12].yend = 0;
  button[12].xscl = 0; button[12].yscl = 0;
  button[12].x = 360;
  button[12].y = 290;
  button[12].w = w; button[12].h = h;
  button[12].draw = TRUE;
  button[12].textcol = col_yellow; button[12].font = impact10h;
  strcpy(button[12].text1,menu_message[MSG_KEY_W6].text1);

  w = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->h;

  button[13].type = TP_NORMAL; button[13].state = ST_OK;
  button[13].xspd = 0; button[13].yspd = 0;
  button[13].xend = 0; button[13].yend = 0;
  button[13].xscl = 0; button[13].yscl = 0;
  button[13].x = middle(SCREEN_W,w)-105;
  button[13].y = 415;
  button[13].w = w; button[13].h = h;
  button[13].draw = TRUE;
  button[13].textcol = col_orange; button[13].font = impact14h;
  strcpy(button[13].text1,menu_message[MSG_MENU_BACK].text1);

  button[14].type = TP_NORMAL; button[14].state = ST_OK;
  button[14].xspd = 0; button[14].yspd = 0;
  button[14].xend = 0; button[14].yend = 0;
  button[14].xscl = 0; button[14].yscl = 0;
  button[14].x = middle(SCREEN_W,w)+105;
  button[14].y = 415;
  button[14].w = w; button[14].h = h;
  button[14].draw = TRUE;
  button[14].textcol = col_white; button[14].font = impact14h;
  strcpy(button[14].text1,menu_message[MSG_KEY_2PLAYER].text1);

  // Linux port: mouse control on/off (Enter toggles, see do_menu_keymap_1p)
  button[15] = button[12];
  button[15].state = ST_OK;
  button[15].y = 330;
  strcpy(button[15].text1, LANGUAGE == 1 ? "Maus" : "Mouse");

  button_anz = 16;

  change_title(mnu_t_spkeys);
}

void menu_keymap_2p_init()
{
  int w;
  int h;

  w = ((RLE_SPRITE *)(data[mnu_button3_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button3_1].dat))->h;

  button[0].type = TP_OPTIONS2; button[0].state = ST_SELECTED;
  button[0].xspd = 0; button[0].yspd = 0;
  button[0].xend = 0; button[0].yend = 0;
  button[0].xscl = 0; button[0].yscl = 0;
  button[0].x = 80;
  button[0].y = 90;
  button[0].w = w; button[0].h = h;
  button[0].draw = TRUE;
  button[0].textcol = col_yellow; button[0].font = impact10h;
  strcpy(button[0].text1,menu_message[MSG_KEY_THRUST].text1);

  button[1].type = TP_OPTIONS2; button[1].state = ST_OK;
  button[1].xspd = 0; button[1].yspd = 0;
  button[1].xend = 0; button[1].yend = 0;
  button[1].xscl = 0; button[1].yscl = 0;
  button[1].x = 80;
  button[1].y = 130;
  button[1].w = w; button[1].h = h;
  button[1].draw = TRUE;
  button[1].textcol = col_yellow; button[1].font = impact10h;
  strcpy(button[1].text1,menu_message[MSG_KEY_SHOOT].text1);

  button[2].type = TP_OPTIONS2; button[2].state = ST_OK;
  button[2].xspd = 0; button[2].yspd = 0;
  button[2].xend = 0; button[2].yend = 0;
  button[2].xscl = 0; button[2].yscl = 0;
  button[2].x = 80;
  button[2].y = 170;
  button[2].w = w; button[2].h = h;
  button[2].draw = TRUE;
  button[2].textcol = col_yellow; button[2].font = impact10h;
  strcpy(button[2].text1,menu_message[MSG_KEY_LEFT].text1);

  button[3].type = TP_OPTIONS2; button[3].state = ST_OK;
  button[3].xspd = 0; button[3].yspd = 0;
  button[3].xend = 0; button[3].yend = 0;
  button[3].xscl = 0; button[3].yscl = 0;
  button[3].x = 80;
  button[3].y = 210;
  button[3].w = w; button[3].h = h;
  button[3].draw = TRUE;
  button[3].textcol = col_yellow; button[3].font = impact10h;
  strcpy(button[3].text1,menu_message[MSG_KEY_RIGHT].text1);

  button[4].type = TP_OPTIONS2; button[4].state = ST_OK;
  button[4].xspd = 0; button[4].yspd = 0;
  button[4].xend = 0; button[4].yend = 0;
  button[4].xscl = 0; button[4].yscl = 0;
  button[4].x = 80;
  button[4].y = 250;
  button[4].w = w; button[4].h = h;
  button[4].draw = TRUE;
  button[4].textcol = col_yellow; button[4].font = impact10h;
  strcpy(button[4].text1,menu_message[MSG_KEY_SPECIAL].text1);

  button[5].type = TP_OPTIONS2; button[5].state = ST_OK;
  button[5].xspd = 0; button[5].yspd = 0;
  button[5].xend = 0; button[5].yend = 0;
  button[5].xscl = 0; button[5].yscl = 0;
  button[5].x = 80;
  button[5].y = 290;
  button[5].w = w; button[5].h = h;
  button[5].draw = TRUE;
  button[5].textcol = col_yellow; button[5].font = impact10h;
  strcpy(button[5].text1,menu_message[MSG_KEY_NWEAPON].text1);

  button[6].type = TP_OPTIONS2; button[6].state = ST_OK;
  button[6].xspd = 0; button[6].yspd = 0;
  button[6].xend = 0; button[6].yend = 0;
  button[6].xscl = 0; button[6].yscl = 0;
  button[6].x = 80;
  button[6].y = 330;
  button[6].w = w; button[6].h = h;
  button[6].draw = TRUE;
  button[6].textcol = col_yellow; button[6].font = impact10h;
  strcpy(button[6].text1,menu_message[MSG_KEY_PWEAPON].text1);


  button[7].type = TP_OPTIONS2; button[7].state = ST_OK;
  button[7].xspd = 0; button[7].yspd = 0;
  button[7].xend = 0; button[7].yend = 0;
  button[7].xscl = 0; button[7].yscl = 0;
  button[7].x = 360;
  button[7].y = 90;
  button[7].w = w; button[7].h = h;
  button[7].draw = TRUE;
  button[7].textcol = col_yellow; button[7].font = impact10h;
  strcpy(button[7].text1,menu_message[MSG_KEY_THRUST].text1);

  button[8].type = TP_OPTIONS2; button[8].state = ST_OK;
  button[8].xspd = 0; button[8].yspd = 0;
  button[8].xend = 0; button[8].yend = 0;
  button[8].xscl = 0; button[8].yscl = 0;
  button[8].x = 360;
  button[8].y = 130;
  button[8].w = w; button[8].h = h;
  button[8].draw = TRUE;
  button[8].textcol = col_yellow; button[8].font = impact10h;
  strcpy(button[8].text1,menu_message[MSG_KEY_SHOOT].text1);

  button[9].type = TP_OPTIONS2; button[9].state = ST_OK;
  button[9].xspd = 0; button[9].yspd = 0;
  button[9].xend = 0; button[9].yend = 0;
  button[9].xscl = 0; button[9].yscl = 0;
  button[9].x = 360;
  button[9].y = 170;
  button[9].w = w; button[9].h = h;
  button[9].draw = TRUE;
  button[9].textcol = col_yellow; button[9].font = impact10h;
  strcpy(button[9].text1,menu_message[MSG_KEY_LEFT].text1);

  button[10].type = TP_OPTIONS2; button[10].state = ST_OK;
  button[10].xspd = 0; button[10].yspd = 0;
  button[10].xend = 0; button[10].yend = 0;
  button[10].xscl = 0; button[10].yscl = 0;
  button[10].x = 360;
  button[10].y = 210;
  button[10].w = w; button[10].h = h;
  button[10].draw = TRUE;
  button[10].textcol = col_yellow; button[10].font = impact10h;
  strcpy(button[10].text1,menu_message[MSG_KEY_RIGHT].text1);

  button[11].type = TP_OPTIONS2; button[11].state = ST_OK;
  button[11].xspd = 0; button[11].yspd = 0;
  button[11].xend = 0; button[11].yend = 0;
  button[11].xscl = 0; button[11].yscl = 0;
  button[11].x = 360;
  button[11].y = 250;
  button[11].w = w; button[11].h = h;
  button[11].draw = TRUE;
  button[11].textcol = col_yellow; button[11].font = impact10h;
  strcpy(button[11].text1,menu_message[MSG_KEY_SPECIAL].text1);

  button[12].type = TP_OPTIONS2; button[12].state = ST_OK;
  button[12].xspd = 0; button[12].yspd = 0;
  button[12].xend = 0; button[12].yend = 0;
  button[12].xscl = 0; button[12].yscl = 0;
  button[12].x = 360;
  button[12].y = 290;
  button[12].w = w; button[12].h = h;
  button[12].draw = TRUE;
  button[12].textcol = col_yellow; button[12].font = impact10h;
  strcpy(button[12].text1,menu_message[MSG_KEY_NWEAPON].text1);

  button[13].type = TP_OPTIONS2; button[13].state = ST_OK;
  button[13].xspd = 0; button[13].yspd = 0;
  button[13].xend = 0; button[13].yend = 0;
  button[13].xscl = 0; button[13].yscl = 0;
  button[13].x = 360;
  button[13].y = 330;
  button[13].w = w; button[13].h = h;
  button[13].draw = TRUE;
  button[13].textcol = col_yellow; button[13].font = impact10h;
  strcpy(button[13].text1,menu_message[MSG_KEY_PWEAPON].text1);


  w = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->w;
  h = ((RLE_SPRITE *)(data[mnu_button1_1].dat))->h;

  button[14].type = TP_NORMAL; button[14].state = ST_OK;
  button[14].xspd = 0; button[14].yspd = 0;
  button[14].xend = 0; button[14].yend = 0;
  button[14].xscl = 0; button[14].yscl = 0;
  button[14].x = middle(SCREEN_W,w);
  button[14].y = 415;
  button[14].w = w; button[14].h = h;
  button[14].draw = TRUE;
  button[14].textcol = col_orange; button[14].font = impact14h;
  strcpy(button[14].text1,menu_message[MSG_MENU_BACK].text1);

  button_anz = 15;

  change_title(mnu_t_2pkeys);
}

char *get_key_desc(char f[], int ch)
{
  if (ch >= 1 && ch <= 36)
  {
    f[0] = toupper(scancode_to_ascii(ch));
    f[1] = 0;
  }
  else if (ch >= 37 && ch <= 46)
  {
    f[0] = scancode_to_ascii(ch);
    f[1] = 0;
    strcat(f," [NUM]");
  }
  else if (ch >= 47 && ch <= 58)
  {
    strcpy(f, menu_message[MSG_KEY_F1+ch-47].text1);
  }
  else if (ch >= 60 && ch <= 91)
  {
//    sprintf(f, "%i %s", ch, menu_message[MSG_KEY_TILDE+ch-60].text1);
    strcpy(f, menu_message[MSG_KEY_TILDE+ch-60].text1);
  }
  else if (ch >= 96 && ch <= 101)
  {
    strcpy(f, menu_message[MSG_KEY_LSHIFT+ch-96].text1);
  }
  else if (ch > 1000)
  {
    strcpy(f, menu_message[MSG_JOY0_UP+ch-1001].text1);
  }

  return f;
}

void draw_buttons()
{
  int nr,nr2;
  RLE_SPRITE *tempbut=NULL;
  char f[100], tmp[30];
  int col;
  int startx;

  for (nr=0; nr < button_anz; nr++)
  {
      switch (button[nr].type)
      {
        case TP_NORMAL  :
                 switch (button[nr].state)
                 {
                   case ST_OK :
                                tempbut = data[mnu_button1_1].dat;
                                break;
                   case ST_SELECTED :
                                tempbut = data[mnu_button1_2].dat;
                                break;
                   default :
                                tempbut = data[mnu_button1_1].dat;
                                break;
                 } // switch type
                 break;

        case TP_OPTIONS :
                 switch (button[nr].state)
                 {
                   case ST_OK :
                                tempbut = data[mnu_button2_1].dat;
                                break;
                   case ST_SELECTED :
                                tempbut = data[mnu_button2_2].dat;
                                break;
                   case ST_PRESSED :
                                tempbut = data[mnu_button2_3].dat;
                                break;
                   default :
                                tempbut = data[mnu_button2_1].dat;
                                break;
                 } // switch type
                 break;

        case TP_OPTIONS2 :
                 switch (button[nr].state)
                 {
                   case ST_OK :
                                tempbut = data[mnu_button3_1].dat;
                                break;
                   case ST_SELECTED :
                                tempbut = data[mnu_button3_2].dat;
                                break;
                   case ST_PRESSED :
                                tempbut = data[mnu_button3_3].dat;
                                break;
                   default :
                                tempbut = data[mnu_button3_1].dat;
                                break;
                 } // switch type
                 break;

        case TP_HISCORE :
                 tempbut = data[mnu_hiscore].dat;
                 break;

      } // switch type

      if (button[nr].draw)
      {
        draw_rle_sprite(vscreen,tempbut,button[nr].x,button[nr].y);

        text_mode(-1);

        switch (button[nr].type)
        {
          case TP_NORMAL :
                 jhtextout(vscreen,button[nr].font,button[nr].text1,
                             button[nr].x+middle(tempbut->w,text_length(button[nr].font,button[nr].text1)),
                             button[nr].y+middle(tempbut->h,text_height(button[nr].font)),
                             button[nr].textcol);
                 break;
          case TP_OPTIONS :
                 jhtextout(vscreen,button[nr].font,button[nr].text1,
                             button[nr].x+middle(tempbut->w,text_length(button[nr].font,button[nr].text1)),
                             button[nr].y+5,
                             button[nr].textcol);
                 break;
          case TP_OPTIONS2 :
                 jhtextout(vscreen,button[nr].font,button[nr].text1,
                             button[nr].x+7,
                             button[nr].y+8,
                             button[nr].textcol);
                 break;

        } // switch type
  
        add_2_list(&rl,button[nr].x,button[nr].y,tempbut->w,tempbut->h);
      } // if draw
  } // for nr


  // Menu/Button-specific drawing
  switch (actual_menu)
  {
     case MNU_MAIN    :
                  // print version number
                  #ifdef ALLEGRO_WINDOWS
                    sprintf(f,"GF/Win v%s",VERSION);
                  #endif
                  #ifdef ALLEGRO_LINUX
                    sprintf(f,"GF/Lnx v%s",VERSION);
                  #endif
                  #ifdef ALLEGRO_DOS
                    sprintf(f,"GF/Dos v%s",VERSION);
                  #endif
                  jhtextout(vscreen,impact10h,f,
                            middle(640,text_length(impact10h,f)),
                            450,
                            col_yellow);
                  add_2_list(&rl,middle(640,text_length(impact10h,f)),450,text_length(impact10h,f),text_height(impact10h));

                  // print homepage address
                  sprintf(f,"www.skytrek.de");
                  jhtextout(vscreen,impact10h,f,
                            middle(640,text_length(impact10h,f)),
                            465,
                            col_green);
                  add_2_list(&rl,middle(640,text_length(impact10h,f)),465,text_length(impact10h,f),text_height(impact10h));
                  break;
     case MNU_SPLAYER :
                  tempbut = data[mnu_hiscore].dat;
                  // Hiscore-Table
                  if (current_level != show_level)
                  {
                    get_lng_name("gf",current_level);
                    read_level_text(LANGUAGE);
                    show_level = current_level;
                  }
                  strcpy(f,"Level ");
                  strcat(f,mitoa(tmp,current_level));
                  jhtextout(vscreen,impact10h,f,
                            button[3].x+middle(tempbut->w,text_length(impact10h,f)),
                            button[3].y+50,
                            col_green);

                  strcpy(f,level_info[0].text2);
                  jhtextout(vscreen,impact14h,f,
                            button[3].x+middle(tempbut->w,text_length(impact14h,f)),
                            button[3].y+64,
                            col_yellow);

                  // Hiscore-Table: Levels [locked, available, active]
                  startx = button[3].x+15;
                  for (nr=1; nr <= cur_max_levels; nr++)
                  {
                    sprintf(f,"[%02d]",nr);

                    if (nr == current_level)
                    {
                      if (config_file.config.level_available[nr])
                        col = col_yellow;
                      else
                        col = col_orange;
                    }
                    else if (config_file.config.level_available[nr])
                      col = col_green;
                    else
                      col = col_red;

                    jhtextout(vscreen,impact10h,f,
                              startx+28*(nr-1),
                              button[3].y+10,
                              col);
                  }

                  // Hiscore-Table: Hiscores!
                  for (nr=0; nr < 10; nr++)
                  {
                    // place
                    if (nr < 9)
                    {
                      strcpy(f," ");
                      strcat(f,mitoa(tmp,nr+1));
                    }
                    else
                      strcpy(f,mitoa(tmp,nr+1));
                    strcat(f,".");

                    if (nr == score_pos)
                      jhtextout(vscreen,impact10h,f,
                                button[3].x+15,
                                button[3].y+100+nr*20,
                                col_orange);
                    else
                      jhtextout(vscreen,impact10h,f,
                                button[3].x+15,
                                button[3].y+100+nr*20,
                                col_yellow);
                    // name
                    strcpy(f,hiscore_file.hiscore[current_level].nr[nr].name);
                    jhtextout(vscreen,impact10h,f,
                              button[3].x+40,
                              button[3].y+100+nr*20,
                              col_white);
                    // score
                    mitoa(f,hiscore_file.hiscore[current_level].nr[nr].score);
                    if (nr == score_pos)
                      jhtextout(vscreen,impact10h,f,
                                button[3].x+tempbut->w-15-text_length(impact10h,f),
                                button[3].y+100+nr*20,
                                col_orange);
                    else
                      jhtextout(vscreen,impact10h,f,
                                button[3].x+tempbut->w-15-text_length(impact10h,f),
                                button[3].y+100+nr*20,
                                col_yellow);
                  } // for

                  // Password
                  strcpy(tmp,level_passwords[current_level]);
                  sprintf(f, "%s: %s", menu_message[MSG_MENU_PASSWORD].text1, decrypt_password(tmp));
                  if (config_file.config.level_available[current_level])
                  {
                    jhtextout(vscreen,impact10h,f,
                              button[3].x+middle(tempbut->w,text_length(impact10h,f)),
                              button[3].y+333,
                              col_orange);
                  }

                  // Highscore Code
                  strcpy(tmp,score_str);
                  sprintf(f, "%s: %s", "Code", tmp);
                  jhtextout(vscreen,impact10h,f,
                            button[3].x+middle(tempbut->w,text_length(impact10h,f)),
                            button[3].y+350,
                            col_orange);
                  break;

     case MNU_SP_RACE :

                  tempbut = data[mnu_hiscore].dat;
                  // Hiscore-Table
                  if (current_sprace_level != show_level)
                  {
                    get_lng_name("gfr",current_sprace_level);
                    read_level_text(LANGUAGE);
                    show_level = current_sprace_level;
                  }
                  strcpy(f,"Race Level ");
                  strcat(f,mitoa(tmp,current_sprace_level));
                  jhtextout(vscreen,impact10h,f,
                            button[2].x+middle(tempbut->w,text_length(impact10h,f)),
                            button[2].y+50,
                            col_green);

                  strcpy(f,level_info[0].text2);
                  jhtextout(vscreen,impact14h,f,
                            button[2].x+middle(tempbut->w,text_length(impact14h,f)),
                            button[2].y+64,
                            col_yellow);

                  // Hiscore-Table: Levels [locked, available, active]
                  startx = button[2].x+112;
                  for (nr=1; nr <= cur_max_race_levels; nr++)
                  {
                    sprintf(f,"[%02d]",nr);

                    if (nr == current_sprace_level)
                      col = col_yellow;
                    else
                      col = col_green;

                    jhtextout(vscreen,impact10h,f,
                              startx+28*(nr-1),
                              button[2].y+10,
                              col);
                  }

                  // Hiscore-Table: Hiscores!
                  for (nr=0; nr < 10; nr++)
                  {
                    // place
                    if (nr < 9)
                    {
                      strcpy(f," ");
                      strcat(f,mitoa(tmp,nr+1));
                    }
                    else
                      strcpy(f,mitoa(tmp,nr+1));
                    strcat(f,".");

                    if (nr == score_pos)
                      jhtextout(vscreen,impact10h,f,
                                button[2].x+15,
                                button[2].y+100+nr*20,
                                col_orange);
                    else
                      jhtextout(vscreen,impact10h,f,
                                button[2].x+15,
                                button[2].y+100+nr*20,
                                col_yellow);
                    // name
                    strcpy(f,hiscore_file.race_hiscore[current_sprace_level].nr[nr].name);
                    jhtextout(vscreen,impact10h,f,
                              button[2].x+40,
                              button[2].y+100+nr*20,
                              col_white);
                    // score (time)
                    sprintf(f,"%02d:%02d:%02d",
                              hiscore_file.race_hiscore[current_sprace_level].nr[nr].m,
                              hiscore_file.race_hiscore[current_sprace_level].nr[nr].s,
                              hiscore_file.race_hiscore[current_sprace_level].nr[nr].hs);

                    if (nr == score_pos)
                      jhtextout(vscreen,impact10h,f,
                                button[2].x+tempbut->w-15-text_length(impact10h,f),
                                button[2].y+100+nr*20,
                                col_orange);
                    else
                      jhtextout(vscreen,impact10h,f,
                                button[2].x+tempbut->w-15-text_length(impact10h,f),
                                button[2].y+100+nr*20,
                                col_yellow);
                  } // for

                  // Highscore Code
                  strcpy(tmp,score_str);
                  sprintf(f, "%s: %s", "Code", tmp);
                  jhtextout(vscreen,impact10h,f,
                            button[2].x+middle(tempbut->w,text_length(impact10h,f)),
                            button[2].y+350,
                            col_orange);
                  break;

     case MNU_SP_TRAINING :

                  tempbut = data[mnu_hiscore].dat;
                  // Hiscore-Table
                  if (current_training_level != show_level)
                  {
                    get_lng_name("gft",current_training_level);
                    read_level_text(LANGUAGE);
                    show_level = current_training_level;
                  }
                  strcpy(f,"Training Level ");
                  strcat(f,mitoa(tmp,current_training_level));
                  jhtextout(vscreen,impact10h,f,
                            button[2].x+middle(tempbut->w,text_length(impact10h,f)),
                            button[2].y+50,
                            col_green);

                  strcpy(f,level_info[0].text2);
                  jhtextout(vscreen,impact14h,f,
                            button[2].x+middle(tempbut->w,text_length(impact14h,f)),
                            button[2].y+64,
                            col_yellow);

                  // Hiscore-Table: Levels [locked, available, active]
                  startx = button[2].x+79;
                  for (nr=1; nr <= cur_max_training_levels; nr++)
                  {
                    sprintf(f,"[%02d]",nr);

                    if (nr == current_training_level)
                      col = col_yellow;
                    else
                      col = col_green;

                    jhtextout(vscreen,impact10h,f,
                              startx+28*(nr-1),
                              button[2].y+10,
                              col);
                  }
                  break;

     case MNU_QDOGFIGHT :

                  tempbut = data[mnu_hiscore].dat;

                  // Frag Limit
                  sprintf(f, "%i kills", frag_limit);
                  jhtextout(vscreen,impact10h,f,
                            button[1].x+130,
                            button[1].y+8,
                            col_green);

                  // Time Limit
                  sprintf(f, "%i min", time_limit);
                  jhtextout(vscreen,impact10h,f,
                            button[2].x+130,
                            button[2].y+8,
                            col_green);

                  // Gravity
                  sprintf(f, "%i%%", config_file.config.def_gravity);
                  jhtextout(vscreen,impact10h,f,
                            button[3].x+130,
                            button[3].y+8,
                            col_green);

                  // P1 Weapons
                  sprintf(f, "%i", config_file.config.p1weapons);
                  jhtextout(vscreen,impact10h,f,
                            button[4].x+130,
                            button[4].y+8,
                            col_green);

                  // P2 Weapons
                  sprintf(f, "%i", config_file.config.p2weapons);
                  jhtextout(vscreen,impact10h,f,
                            button[5].x+130,
                            button[5].y+8,
                            col_green);

                  // Extras
                  switch(config_file.config.extras)
                  {
                    case 0 :
                             sprintf(f, "%s", menu_message[MSG_MENU_NONE].text1);
                             break;
                    case 1 :
                             sprintf(f, "%s", menu_message[MSG_MENU_RARE].text1);
                             break;
                    case 2 :
                             sprintf(f, "%s", menu_message[MSG_MENU_MEDIUM].text1);
                             break;
                    case 3 :
                             sprintf(f, "%s", menu_message[MSG_MENU_MUCH].text1);
                             break;
                  }
                  jhtextout(vscreen,impact10h,f,
                            button[6].x+130,
                            button[6].y+8,
                            col_green);

                  // Hiscore-Table
                  if (current_qdogfight_level != show_level)
                  {
                    get_lng_name("gfqd",current_qdogfight_level);
                    read_level_text(LANGUAGE);
                    show_level = current_qdogfight_level;
                  }
                  strcpy(f,"Dogfight Arena ");
                  strcat(f,mitoa(tmp,current_qdogfight_level));
                  jhtextout(vscreen,impact10h,f,
                            button[8].x+middle(tempbut->w,text_length(impact10h,f)),
                            button[8].y+50,
                            col_green);

                  strcpy(f,level_info[0].text2);
                  jhtextout(vscreen,impact14h,f,
                            button[8].x+middle(tempbut->w,text_length(impact14h,f)),
                            button[8].y+64,
                            col_yellow);

                  // Hiscore-Table: Levels [locked, available, active]
                  startx = button[8].x+79;
                  for (nr=1; nr <= cur_max_qdogfight_levels; nr++)
                  {
                    sprintf(f,"[%02d]",nr);

                    if (nr == current_qdogfight_level)
                      col = col_yellow;
                    else
                      col = col_green;

                    jhtextout(vscreen,impact10h,f,
                              startx+28*(nr-1),
                              button[8].y+10,
                              col);
                  }
                  break;

     case MNU_MPLAYER :
                  break;
     case MNU_DEMO    :
                  break;
     case MNU_OPTIONS :
                  tempbut = data[mnu_button2_1].dat;
                  // Button 0 - Language
                  switch (LANGUAGE)
                  {
                    case 0 :
                             strcpy(f,menu_message[MSG_MENU_OPT_ENGLISH].text1);
                             break;
                    case 1 :
                             strcpy(f,menu_message[MSG_MENU_OPT_GERMAN].text1);
                             break;
                  }
                  jhtextout(vscreen,impact14h,f,
                            button[0].x+middle(tempbut->w,text_length(button[0].font,f)),
                            button[0].y+37,
                            col_green);

                  // Button 1 - Pixanz
                  for (nr=0; nr < 100; nr++)
                    vline(vscreen,button[1].x+10+nr*2,button[1].y+37,button[1].y+56,col_black);
                  for (nr=0; nr < PIXEL_DIVISOR/2; nr++)
                    vline(vscreen,button[1].x+11+nr*2,button[1].y+37,button[1].y+56,col_blue);

                  mitoa(f,PIXEL_DIVISOR);
                  strcat(f,"%");

                  jhtextout(vscreen,impact14h,f,
                            button[1].x+middle(tempbut->w,text_length(button[1].font,f)),
                            button[1].y+36,
                            col_green);

                  // Button 2 - Volume
                  for (nr=0; nr < 100; nr++)
                    vline(vscreen,button[2].x+10+nr*2,button[2].y+37,button[2].y+56,col_black);
                  for (nr=0; nr < SOUND_VOLUME; nr++)
                    vline(vscreen,button[2].x+11+nr*2,button[2].y+37,button[2].y+56,col_blue);

                  mitoa(f,SOUND_VOLUME);
                  strcat(f,"%");

                  jhtextout(vscreen,impact14h,f,
                            button[2].x+middle(tempbut->w,text_length(button[2].font,f)),
                            button[2].y+36,
                            col_green);

                  // Button 3 - Debris?
                  switch (SHOW_DEBRIS)
                  {
                    case 0 :
                             strcpy(f,menu_message[MSG_MENU_OPT_OFF].text1);
                             break;
                    case 1 :
                             strcpy(f,menu_message[MSG_MENU_OPT_ON].text1);
                             break;
                  }
                  jhtextout(vscreen,impact14h,f,
                            button[3].x+middle(tempbut->w,text_length(button[3].font,f)),
                            button[3].y+37,
                            col_green);

                  // Button 4 - Pixel deviation?
                  switch (GRAV_4_PIXELS)
                  {
                    case 0 :
                             strcpy(f,menu_message[MSG_MENU_OPT_OFF].text1);
                             break;
                    case 1 :
                             strcpy(f,menu_message[MSG_MENU_OPT_ON].text1);
                             break;
                  }
                  jhtextout(vscreen,impact14h,f,
                            button[4].x+middle(tempbut->w,text_length(button[4].font,f)),
                            button[4].y+37,
                            col_green);

                  // Button 5 - Menu Music?
                  switch (PLAY_MENU_MUSIC)
                  {
                    case 0 :
                             strcpy(f,menu_message[MSG_MENU_OPT_OFF].text1);
                             break;
                    case 1 :
                             strcpy(f,menu_message[MSG_MENU_OPT_ON].text1);
                             break;
                    case 2 :
                             strcpy(f,menu_message[MSG_MENU_OPT_HATEIT].text1);
                             break;
                  }
                  jhtextout(vscreen,impact14h,f,
                            button[5].x+middle(tempbut->w,text_length(button[5].font,f)),
                            button[5].y+37,
                            col_green);

                  // Descriptions
                  switch (actual_button)
                  {
                    case 0 :
                             strcpy(f,menu_message[MSG_MENU_OPT_LANGUAGE_DESC].text1);
                             break;
                    case 1 :
                             strcpy(f,menu_message[MSG_MENU_OPT_PIXANZ_DESC].text1);
                             break;
                    case 2 :
                             strcpy(f,menu_message[MSG_MENU_OPT_SOUNDVOL_DESC].text1);
                             break;
                    case 3 :
                             strcpy(f,menu_message[MSG_MENU_OPT_DEBRIS_DESC].text1);
                             break;
                    case 4 :
                             strcpy(f,menu_message[MSG_MENU_OPT_PGRAV_DESC].text1);
                             break;
                    case 5 :
                             strcpy(f,menu_message[MSG_MENU_OPT_MMUSIC_DESC].text1);
                             break;
                    default :
                             strcpy(f,"");
                             break;
                  }
                  jhtextout(vscreen,impact10h,f,
                            button[0].x+middle(640,text_length(impact10h,f))-38,
                            375,
                            col_white);
                  add_2_list(&rl,button[0].x+middle(640,text_length(impact10h,f))-38,375,text_length(impact10h,f),text_height(impact10h));
                  break;

     case MNU_KEYMAP_1P :
                  tempbut = data[mnu_button3_1].dat;

                  // Buttons
                  for (nr2=0; nr2 <= 12; nr2++)
                  {
                    switch (nr2)
                    {
                      case 0 : get_key_desc(f,pcontrol[0].thrust); break;
                      case 1 : get_key_desc(f,pcontrol[0].shoot); break;
                      case 2 : get_key_desc(f,pcontrol[0].left); break;
                      case 3 : get_key_desc(f,pcontrol[0].right); break;
                      case 4 : get_key_desc(f,pcontrol[0].special1); break;
                      case 5 : get_key_desc(f,pcontrol[0].nw); break;
                      case 6 : get_key_desc(f,pcontrol[0].pw); break;
                      case 7 : get_key_desc(f,pcontrol[0].w1); break;
                      case 8 : get_key_desc(f,pcontrol[0].w2); break;
                      case 9 : get_key_desc(f,pcontrol[0].w3); break;
                      case 10: get_key_desc(f,pcontrol[0].w4); break;
                      case 11: get_key_desc(f,pcontrol[0].w5); break;
                      case 12: get_key_desc(f,pcontrol[0].w6); break;
                    }
                    jhtextout(vscreen,impact10h,f,
                              button[nr2].x+120,
                              button[nr2].y+8,
                              col_green);
                    add_2_list(&rl,button[nr2].x+middle(640,text_length(impact10h,f))-38,375,text_length(impact10h,f),text_height(impact10h));
                  }
                  jhtextout(vscreen,impact10h,
                            menu_message[config_file.config.no_mouse ? MSG_MENU_OPT_OFF : MSG_MENU_OPT_ON].text1,
                            button[15].x+120, button[15].y+8, col_green);
                  break;

     case MNU_KEYMAP_2P :
                  tempbut = data[mnu_button3_1].dat;

                  // Buttons
                  for (nr2=0; nr2 <= 13; nr2++)
                  {
                    switch (nr2)
                    {
                      case 0 : get_key_desc(f,pcontrol[1].thrust); break;
                      case 1 : get_key_desc(f,pcontrol[1].shoot); break;
                      case 2 : get_key_desc(f,pcontrol[1].left); break;
                      case 3 : get_key_desc(f,pcontrol[1].right); break;
                      case 4 : get_key_desc(f,pcontrol[1].special1); break;
                      case 5 : get_key_desc(f,pcontrol[1].nw); break;
                      case 6 : get_key_desc(f,pcontrol[1].pw); break;

                      case 7 : get_key_desc(f,pcontrol[2].thrust); break;
                      case 8 : get_key_desc(f,pcontrol[2].shoot); break;
                      case 9 : get_key_desc(f,pcontrol[2].left); break;
                      case 10: get_key_desc(f,pcontrol[2].right); break;
                      case 11: get_key_desc(f,pcontrol[2].special1); break;
                      case 12: get_key_desc(f,pcontrol[2].nw); break;
                      case 13: get_key_desc(f,pcontrol[2].pw); break;
                    }
                    jhtextout(vscreen,impact10h,f,
                              button[nr2].x+120,
                              button[nr2].y+8,
                              col_green);
                    add_2_list(&rl,button[nr2].x+middle(640,text_length(impact10h,f))-38,375,text_length(impact10h,f),text_height(impact10h));
                  }
                  break;

  } // sw actual_menu

}

void do_button_explosion(BUTTONTYPE *but)
{
  add_pixels(300,but->x+but->w/4,but->y+but->h/2,-1,0,10,tbl_blue,10,50,1100,800,0);
  add_pixels(150,but->x+but->w/2,but->y+but->h/2,0,0,8,tbl_blue,10,50,1100,800,0);
  add_pixels(300,but->x+but->w-but->w/4,but->y+but->h/2,1,0,10,tbl_blue,10,50,1100,800,0);
}

void moveout_menu()
{
  int nr;

  srandom(time(0));

  for (nr=0; nr < button_anz; nr++)
  {
    button[nr].xend = button[nr].x-SCREEN_W;
    button[nr].yend = button[nr].y;
    button[nr].xspd = 2;
    button[nr].xacc = 0.5;
    button[nr].yspd = 0;
    button[nr].yacc = 0;
  } // for nr

  if (title_in == TRUE)
  {
    title_dx = title_ax;
    title_dy = title_ay;
    title_sx = 10;
  }

  lock_keyboard = TRUE;
}

void movein_menu()
{
  int nr;

  srandom(time(0));

  actual_button = 0;

  for (nr=0; nr < button_anz; nr++)
  {
    button[nr].xend = button[nr].x;
    button[nr].yend = button[nr].y;
    button[nr].x += SCREEN_W;
    button[nr].y += 0;
    button[nr].xspd = 12;
    button[nr].xacc = -0.1;
    button[nr].yspd = 0;
    button[nr].yacc = 0;
  } // for nr

  if (title_in == FALSE)
  {
    title_dx = title_ex;
    title_dy = title_ey;
    title_sx = -5;
  }

  lock_keyboard = TRUE;
}

int move_menu()
{
  int nr;
  int ok = FALSE;

  for (nr=0; nr < button_anz; nr++)
  {
    if ((button[nr].xend != 0) || (button[nr].yend != 0))
    {
      button[nr].xspd += button[nr].xacc;
      button[nr].yspd += button[nr].yacc;

      if (abs(button[nr].xend - button[nr].x) < button[nr].xspd)
        button[nr].x = button[nr].xend;

      if (abs(button[nr].yend - button[nr].y) < button[nr].yspd)
        button[nr].y = button[nr].yend;

      if (button[nr].x > button[nr].xend) button[nr].x -= button[nr].xspd;
        else if (button[nr].x < button[nr].xend) button[nr].x += button[nr].xspd;

      if (button[nr].y > button[nr].yend) button[nr].y -= button[nr].yspd;
        else if (button[nr].y < button[nr].yend) button[nr].y += button[nr].yspd;

    } // if move
    if ((button[nr].x == button[nr].xend) && (button[nr].y == button[nr].yend))
      ok = TRUE;
    else
      ok = FALSE;
  } // for nr

  if (title_in == TRUE && title_sx != 0)
  {
    if (title_x < title_dx) title_x += title_sx; else if (ok) title_in = FALSE;
    if (title_y < title_dy) title_y += title_sy;
  }
  else if (title_in == FALSE && title_sx != 0)
  {
    if (title_x > title_dx) title_x += title_sx; else if (ok) title_in = TRUE;
    if (title_y > title_dy) title_y += title_sy;
  }


  if (ok) { lock_keyboard = FALSE; return 1; }
  else return 0;
}

void do_menu_main()
{
  int nr;

  // Tastendruecke
  if (!menu_change)
  {

    if ((getctrl(KEY_DOWN,1) || getctrl(JOY0_DOWN,1)) && !lock_keyboard && !quit_menu)
    {
      do
      {
        if (actual_button < button_anz-1)
        {
          actual_button++;
        }
        else actual_button = 0;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_UP,1) || getctrl(JOY0_UP,1)) && !lock_keyboard && !quit_menu)
    {
      do
      {
        if (actual_button > 0)
        {
          actual_button--;
        }
        else actual_button = button_anz-1;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_ENTER,1) || getctrl(JOY0_B1,1)) && !lock_keyboard && !quit_menu)
    {
      if (button[actual_button].type == TP_NORMAL) button[actual_button].draw = FALSE;
      button[actual_button].state = ST_PRESSED;
      clear_keybuf();
    }

    for (nr=0; nr < button_anz; nr++)
    {
      if (button[nr].state == ST_PRESSED)
      {
        if (button[nr].state != ST_DEACTIVATED)
        {
          play_sound_sample(snd_menu_select,1000,0,0,0,100);
          do_button_explosion(&button[nr]);
          button[nr].state = ST_DEACTIVATED;
          menu_change = TRUE;
     
          switch (nr)
          {
            case 0 : // Single Player
                 new_menu = MNU_SPLAYER_SEL;
                 moveout_menu();
                 break;
            case 1 : // Multi Player
                 new_menu = MNU_MPLAYER_SEL;
                 moveout_menu();
                 break;
            case 2 : // Options
                 new_menu = MNU_OPTIONS;
                 moveout_menu();
                 break;
            case 3 : // Quit
                 quit_menu = TRUE;
                 fade_out_active = TRUE;
                 fade_pos = 0; fade_opos = 0; fade_type = 1; fade_count = 0; fade_count_to = 200; fade_speed = 0.3;
                 break;
          } // switch nr
        } // if !deactivated
      } // if pressed
  
      if (nr == actual_button) button[nr].state = ST_SELECTED;
        else if (button[nr].state == ST_SELECTED) button[nr].state = ST_OK;
  
    } // for nr

  } // if not change

}

void do_menu_sp_sel()
{
  int nr;

  // Tastendruecke
  if (!menu_change)
  {

    if ((getctrl(KEY_DOWN,1) || getctrl(JOY0_DOWN,1)) && !lock_keyboard && !quit_menu)
    {
      do
      {
        if (actual_button < button_anz-1)
        {
          actual_button++;
        }
        else actual_button = 0;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_UP,1) || getctrl(JOY0_UP,1)) && !lock_keyboard && !quit_menu)
    {
      do
      {
        if (actual_button > 0)
        {
          actual_button--;
        }
        else actual_button = button_anz-1;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_ENTER,1) || getctrl(JOY0_B1,1)) && !lock_keyboard && !quit_menu)
    {
      if (button[actual_button].type == TP_NORMAL) button[actual_button].draw = FALSE;
      button[actual_button].state = ST_PRESSED;
      clear_keybuf();
    }
    else if (key[KEY_ESC])
    {
       menu_change = TRUE;
       new_menu = MNU_MAIN;
       moveout_menu();
    }

    for (nr=0; nr < button_anz; nr++)
    {
      if (button[nr].state == ST_PRESSED)
      {
        if (button[nr].state != ST_DEACTIVATED)
        {
          play_sound_sample(snd_menu_select,1000,0,0,0,100);
          do_button_explosion(&button[nr]);
          button[nr].state = ST_DEACTIVATED;
          menu_change = TRUE;
     
          switch (nr)
          {
            case 0 : // Training
                 new_menu = MNU_SP_TRAINING;
                 moveout_menu();
                 break;
            case 1 : // Campaign
                 new_menu = MNU_SPLAYER;
                 moveout_menu();
                 break;
            case 2 : // Race
                 new_menu = MNU_SP_RACE;
                 moveout_menu();
                 break;
            case 3 : // Back
                 new_menu = MNU_MAIN;
                 moveout_menu();
                 break;
          } // switch nr
        } // if !deactivated
      } // if pressed
  
      if (nr == actual_button) button[nr].state = ST_SELECTED;
        else if (button[nr].state == ST_SELECTED) button[nr].state = ST_OK;
  
    } // for nr

  } // if not change

}

void do_menu_splayer()
{
  int nr;

  // Tastendruecke
  if (!menu_change)
  {

    if ((getctrl(KEY_DOWN,1) || getctrl(JOY0_DOWN,1)) && !lock_keyboard)
    {
      do
      {
        if (actual_button < button_anz-1) actual_button++;
          else actual_button = 0;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_UP,1) || getctrl(JOY0_UP,1)) && !lock_keyboard)
    {
      do
      {
        if (actual_button > 0) actual_button--;
          else actual_button = button_anz-1;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_RIGHT,1) || getctrl(JOY0_RIGHT,1)) && !lock_keyboard)
    {
      if (actual_button == 0)
      {
        if (current_level < cur_max_levels)
        {
          current_level++;
          CalcScoreString(score_str,
                          hiscore_file.hiscore[current_level].nr[score_pos].name,
                          hiscore_file.hiscore[current_level].nr[score_pos].score);

          play_sound_sample(snd_click,1000,0,0,0,100);

          if (config_file.config.level_available[current_level])
            strcpy(button[0].text1,menu_message[MSG_MENU_PLAYLEVEL].text1);
          else
            strcpy(button[0].text1,menu_message[MSG_MENU_UNLOCKLEVEL].text1);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      clear_keybuf();
    }
    else if ((getctrl(KEY_LEFT,1) || getctrl(JOY0_LEFT,1)) && !lock_keyboard)
    {
      if (actual_button == 0)
      {
        if (current_level > 1)
        {
          current_level--;
          CalcScoreString(score_str,
                          hiscore_file.hiscore[current_level].nr[score_pos].name,
                          hiscore_file.hiscore[current_level].nr[score_pos].score);
          play_sound_sample(snd_click,1000,0,0,0,100);
          if (config_file.config.level_available[current_level])
            strcpy(button[0].text1,menu_message[MSG_MENU_PLAYLEVEL].text1);
          else
            strcpy(button[0].text1,menu_message[MSG_MENU_UNLOCKLEVEL].text1);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      clear_keybuf();
    }
    else if ((getctrl(KEY_ENTER,1) || getctrl(JOY0_B1,1)) && !lock_keyboard)
    {
      if ( (actual_button == 0) || (actual_button == 2) )
      {
        if (button[actual_button].type == TP_NORMAL) button[actual_button].draw = FALSE;
      }
      button[actual_button].state = ST_PRESSED;
      clear_keybuf();
    }
    else if (key[KEY_PGUP] && !lock_keyboard)
    {
      if (score_pos > 0) score_pos--;
      CalcScoreString(score_str,
                      hiscore_file.hiscore[current_level].nr[score_pos].name,
                      hiscore_file.hiscore[current_level].nr[score_pos].score);
      clear_keybuf();
    }
    else if (key[KEY_PGDN] && !lock_keyboard)
    {
      if (score_pos < 9) score_pos++;
      CalcScoreString(score_str,
                      hiscore_file.hiscore[current_level].nr[score_pos].name,
                      hiscore_file.hiscore[current_level].nr[score_pos].score);
      clear_keybuf();
    }
    else if (key[KEY_ESC])
    {
       menu_change = TRUE;
       new_menu = MNU_SPLAYER_SEL;
       moveout_menu();
    }

    for (nr=0; nr < button_anz; nr++)
    {
      if (button[nr].state == ST_PRESSED)
      {
        if (button[nr].state != ST_DEACTIVATED)
        {
          switch (nr)
          {
            case 0 : // Play Level
                 if (config_file.config.level_available[current_level])
                 {
                   play_sound_sample(snd_menu_select,1000,0,0,0,100);
                   do_button_explosion(&button[nr]);
                   button[nr].state = ST_DEACTIVATED;
                   begin_play = TRUE;
                   game_mode = SP_CAMPAIGN;
                   fade_out_active = TRUE;
                   fade_pos = 0; fade_opos = 0; fade_type = 1; fade_count = 0; fade_count_to = 200; fade_speed = 0.5;
                   lock_keyboard = TRUE;
                 }
                 else
                 {
                   strcpy(input_string,"");
                   input_active = TRUE;
                   play_sound_sample(snd_noweap,1000,0,0,0,100);
                   button[nr].state = ST_SELECTED;
                   button[nr].draw = TRUE;
                 }
                 break;
            case 1 : // Password
                 break;
            case 2 : // Back
                 play_sound_sample(snd_menu_select,1000,0,0,0,100);
                 do_button_explosion(&button[nr]);
                 button[nr].state = ST_DEACTIVATED;
                 menu_change = TRUE;
                 new_menu = MNU_SPLAYER_SEL;
                 moveout_menu();
                 break;
          } // switch nr
        } // if !deactivated
      } // if pressed
  
      if (nr == actual_button) button[nr].state = ST_SELECTED;
        else if (button[nr].state == ST_SELECTED) button[nr].state = ST_OK;
  
    } // for nr

  } // if not change
}

void do_menu_mp_sel()
{
  int nr;

  // Tastendruecke
  if (!menu_change)
  {

    if ((getctrl(KEY_DOWN,1) || getctrl(JOY0_DOWN,1)) && !lock_keyboard && !quit_menu)
    {
      do
      {
        if (actual_button < button_anz-1)
        {
          actual_button++;
        }
        else actual_button = 0;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_UP,1) || getctrl(JOY0_UP,1)) && !lock_keyboard && !quit_menu)
    {
      do
      {
        if (actual_button > 0)
        {
          actual_button--;
        }
        else actual_button = button_anz-1;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_ENTER,1) || getctrl(JOY0_B1,1)) && !lock_keyboard && !quit_menu)
    {
      if (button[actual_button].type == TP_NORMAL) button[actual_button].draw = FALSE;
      button[actual_button].state = ST_PRESSED;
      clear_keybuf();
    }
    else if (key[KEY_ESC])
    {
       menu_change = TRUE;
       new_menu = MNU_MAIN;
       moveout_menu();
    }

    for (nr=0; nr < button_anz; nr++)
    {
      if (button[nr].state == ST_PRESSED)
      {
        if (button[nr].state != ST_DEACTIVATED)
        {
          play_sound_sample(snd_menu_select,1000,0,0,0,100);
          do_button_explosion(&button[nr]);
          button[nr].state = ST_DEACTIVATED;
          menu_change = TRUE;
     
          switch (nr)
          {
            case 0 : // Quick Dogfight
                 new_menu = MNU_QDOGFIGHT;
                 moveout_menu();
                 break;
            case 1 : // Dogfight
                 new_menu = MNU_DOGFIGHT;
                 moveout_menu();
                 break;
            case 2 : // Race
                 new_menu = MNU_MP_RACE;
                 moveout_menu();
                 break;
            case 3 : // Back
                 new_menu = MNU_MAIN;
                 moveout_menu();
                 break;
          } // switch nr
        } // if !deactivated
      } // if pressed
  
      if (nr == actual_button) button[nr].state = ST_SELECTED;
        else if (button[nr].state == ST_SELECTED) button[nr].state = ST_OK;
  
    } // for nr

  } // if not change

}

void do_menu_sp_race()
{
  int nr;

  // Tastendruecke
  if (!menu_change)
  {

    if ((getctrl(KEY_DOWN,1) || getctrl(JOY0_DOWN,1)) && !lock_keyboard && !quit_menu)
    {
      do
      {
        if (actual_button < button_anz-1)
        {
          actual_button++;
        }
        else actual_button = 0;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_UP,1) || getctrl(JOY0_UP,1)) && !lock_keyboard && !quit_menu)
    {
      do
      {
        if (actual_button > 0)
        {
          actual_button--;
        }
        else actual_button = button_anz-1;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_RIGHT,1) || getctrl(JOY0_RIGHT,1)) && !lock_keyboard)
    {
      if (actual_button == 0)
      {
        if (current_sprace_level < cur_max_race_levels)
        {
          current_sprace_level++;
          CalcRaceScoreString(score_str,
                          hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].name,
                          hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].m,
                          hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].s,
                          hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].hs);

          play_sound_sample(snd_click,1000,0,0,0,100);

          strcpy(button[0].text1,menu_message[MSG_MENU_PLAYLEVEL].text1);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      clear_keybuf();
    }
    else if ((getctrl(KEY_LEFT,1) || getctrl(JOY0_LEFT,1)) && !lock_keyboard)
    {
      if (actual_button == 0)
      {
        if (current_sprace_level > 1)
        {
          current_sprace_level--;
          CalcRaceScoreString(score_str,
                          hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].name,
                          hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].m,
                          hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].s,
                          hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].hs);

          play_sound_sample(snd_click,1000,0,0,0,100);
          strcpy(button[0].text1,menu_message[MSG_MENU_PLAYLEVEL].text1);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      clear_keybuf();
    }
    else if ((getctrl(KEY_ENTER,1) || getctrl(JOY0_B1,1)) && !lock_keyboard && !quit_menu)
    {
      if (button[actual_button].type == TP_NORMAL) button[actual_button].draw = FALSE;
      button[actual_button].state = ST_PRESSED;
      clear_keybuf();
    }
    else if (key[KEY_PGUP] && !lock_keyboard)
    {
      if (score_pos > 0) score_pos--;
      CalcRaceScoreString(score_str,
                      hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].name,
                      hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].m,
                      hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].s,
                      hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].hs);
      clear_keybuf();
    }
    else if (key[KEY_PGDN] && !lock_keyboard)
    {
      if (score_pos < 9) score_pos++;
      CalcRaceScoreString(score_str,
                      hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].name,
                      hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].m,
                      hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].s,
                      hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].hs);
      clear_keybuf();
    }
    else if (key[KEY_ESC])
    {
       menu_change = TRUE;
       new_menu = MNU_SPLAYER_SEL;
       moveout_menu();
    }

    for (nr=0; nr < button_anz; nr++)
    {
      if (button[nr].state == ST_PRESSED)
      {
        if (button[nr].state != ST_DEACTIVATED)
        {
          switch (nr)
          {
            case 0 : // Play Level
                 play_sound_sample(snd_menu_select,1000,0,0,0,100);
                 do_button_explosion(&button[nr]);
                 button[nr].state = ST_DEACTIVATED;
                 begin_play = TRUE;
                 game_mode = SP_RACE;
                 fade_out_active = TRUE;
                 fade_pos = 0; fade_opos = 0; fade_type = 1; fade_count = 0; fade_count_to = 200; fade_speed = 0.5;
                 lock_keyboard = TRUE;
                 break;
            case 1 : // Back
                 play_sound_sample(snd_menu_select,1000,0,0,0,100);
                 do_button_explosion(&button[nr]);
                 button[nr].state = ST_DEACTIVATED;
                 menu_change = TRUE;
                 new_menu = MNU_SPLAYER_SEL;
                 moveout_menu();
                 break;
          } // switch nr
        } // if !deactivated
      } // if pressed
  
      if (nr == actual_button) button[nr].state = ST_SELECTED;
        else if (button[nr].state == ST_SELECTED) button[nr].state = ST_OK;
  
    } // for nr

  } // if not change
}

void do_menu_mp_race()
{
  int nr;

  // Tastendruecke
  if (!menu_change)
  {

    if ((getctrl(KEY_DOWN,1) || getctrl(JOY0_DOWN,1)) && !lock_keyboard && !quit_menu)
    {
      do
      {
        if (actual_button < button_anz-1)
        {
          actual_button++;
        }
        else actual_button = 0;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_UP,1) || getctrl(JOY0_UP,1)) && !lock_keyboard && !quit_menu)
    {
      do
      {
        if (actual_button > 0)
        {
          actual_button--;
        }
        else actual_button = button_anz-1;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_ENTER,1) || getctrl(JOY0_B1,1)) && !lock_keyboard && !quit_menu)
    {
      if (button[actual_button].type == TP_NORMAL) button[actual_button].draw = FALSE;
      button[actual_button].state = ST_PRESSED;
      clear_keybuf();
    }

    for (nr=0; nr < button_anz; nr++)
    {
      if (button[nr].state == ST_PRESSED)
      {
        if (button[nr].state != ST_DEACTIVATED)
        {
          play_sound_sample(snd_menu_select,1000,0,0,0,100);
          do_button_explosion(&button[nr]);
          button[nr].state = ST_DEACTIVATED;
          menu_change = TRUE;
     
          switch (nr)
          {
            case 0 : // Back
                 new_menu = MNU_MAIN;
                 moveout_menu();
                 break;
          } // switch nr
        } // if !deactivated
      } // if pressed
  
      if (nr == actual_button) button[nr].state = ST_SELECTED;
        else if (button[nr].state == ST_SELECTED) button[nr].state = ST_OK;
  
    } // for nr

  } // if not change
}

void do_menu_dogfight()
{
  int nr;

  // Tastendruecke
  if (!menu_change)
  {

    if ((getctrl(KEY_DOWN,1) || getctrl(JOY0_DOWN,1)) && !lock_keyboard && !quit_menu)
    {
      do
      {
        if (actual_button < button_anz-1)
        {
          actual_button++;
        }
        else actual_button = 0;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_UP,1) || getctrl(JOY0_UP,1)) && !lock_keyboard && !quit_menu)
    {
      do
      {
        if (actual_button > 0)
        {
          actual_button--;
        }
        else actual_button = button_anz-1;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_ENTER,1) || getctrl(JOY0_B1,1)) && !lock_keyboard && !quit_menu)
    {
      if (button[actual_button].type == TP_NORMAL) button[actual_button].draw = FALSE;
      button[actual_button].state = ST_PRESSED;
      clear_keybuf();
    }

    for (nr=0; nr < button_anz; nr++)
    {
      if (button[nr].state == ST_PRESSED)
      {
        if (button[nr].state != ST_DEACTIVATED)
        {
          play_sound_sample(snd_menu_select,1000,0,0,0,100);
          do_button_explosion(&button[nr]);
          button[nr].state = ST_DEACTIVATED;
          menu_change = TRUE;
     
          switch (nr)
          {
            case 0 : // Back
                 new_menu = MNU_MAIN;
                 moveout_menu();
                 break;
          } // switch nr
        } // if !deactivated
      } // if pressed
  
      if (nr == actual_button) button[nr].state = ST_SELECTED;
        else if (button[nr].state == ST_SELECTED) button[nr].state = ST_OK;
  
    } // for nr

  } // if not change
}

void do_menu_qdogfight()
{
  int nr;

  // Tastendruecke
  if (!menu_change)
  {

    if ((getctrl(KEY_DOWN,1) || getctrl(JOY0_DOWN,1)) && !lock_keyboard && !quit_menu && !set_option)
    {
      do
      {
        if (actual_button < button_anz-1)
        {
          actual_button++;
        }
        else actual_button = 0;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_UP,1) || getctrl(JOY0_UP,1)) && !lock_keyboard && !quit_menu && !set_option)
    {
      do
      {
        if (actual_button > 0)
        {
          actual_button--;
        }
        else actual_button = button_anz-1;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_RIGHT,1) || getctrl(JOY0_RIGHT,1)) && !lock_keyboard)
    {
      if (actual_button == 0 && !set_option)
      {
        if (current_qdogfight_level < cur_max_qdogfight_levels)
        {
          current_qdogfight_level++;
          play_sound_sample(snd_click,1000,0,0,0,100);

          strcpy(button[0].text1,menu_message[MSG_MENU_PLAYLEVEL].text1);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      else if (actual_button == 1 && set_option)
      {
        if (frag_limit < 99)
        {
          frag_limit++;
          play_sound_sample(snd_click,1000,0,0,0,100);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      else if (actual_button == 2 && set_option)
      {
        if (time_limit < 99)
        {
          time_limit++;
          play_sound_sample(snd_click,1000,0,0,0,100);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      else if (actual_button == 3 && set_option)
      {
        if (config_file.config.def_gravity < 500)
        {
          config_file.config.def_gravity += 10;
          play_sound_sample(snd_click,1000,0,0,0,100);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      else if (actual_button == 4 && set_option)
      {
        if (config_file.config.p1weapons < 10)
        {
          config_file.config.p1weapons++;
          play_sound_sample(snd_click,1000,0,0,0,100);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      else if (actual_button == 5 && set_option)
      {
        if (config_file.config.p2weapons < 10)
        {
          config_file.config.p2weapons++;
          play_sound_sample(snd_click,1000,0,0,0,100);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      else if (actual_button == 6 && set_option)
      {
        if (config_file.config.extras < 3)
        {
          config_file.config.extras++;
          play_sound_sample(snd_click,1000,0,0,0,100);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }

      clear_keybuf();
    }
    else if ((getctrl(KEY_LEFT,1) || getctrl(JOY0_LEFT,1)) && !lock_keyboard)
    {
      if (actual_button == 0  && !set_option)
      {
        if (current_qdogfight_level > 1)
        {
          current_qdogfight_level--;
          play_sound_sample(snd_click,1000,0,0,0,100);
          strcpy(button[0].text1,menu_message[MSG_MENU_PLAYLEVEL].text1);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      else if (actual_button == 1 && set_option)
      {
        if (frag_limit > 1)
        {
          frag_limit--;
          play_sound_sample(snd_click,1000,0,0,0,100);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      else if (actual_button == 2 && set_option)
      {
        if (time_limit > 1)
        {
          time_limit--;
          play_sound_sample(snd_click,1000,0,0,0,100);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      else if (actual_button == 3 && set_option)
      {
        if (config_file.config.def_gravity > -400)
        {
          config_file.config.def_gravity -= 10;
          play_sound_sample(snd_click,1000,0,0,0,100);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      else if (actual_button == 4 && set_option)
      {
        if (config_file.config.p1weapons > 0)
        {
          config_file.config.p1weapons--;
          play_sound_sample(snd_click,1000,0,0,0,100);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      else if (actual_button == 5 && set_option)
      {
        if (config_file.config.p2weapons > 0)
        {
          config_file.config.p2weapons--;
          play_sound_sample(snd_click,1000,0,0,0,100);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      else if (actual_button == 6 && set_option)
      {
        if (config_file.config.extras > 0)
        {
          config_file.config.extras--;
          play_sound_sample(snd_click,1000,0,0,0,100);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }


      clear_keybuf();
    }
    else if ((getctrl(KEY_ENTER,1) || getctrl(JOY0_B1,1)) && !lock_keyboard)
    {
      if (button[actual_button].type == TP_NORMAL) button[actual_button].draw = FALSE;

      if (button[actual_button].state == ST_PRESSED)
      {
        button[actual_button].state = ST_SELECTED;
        if (button[actual_button].type == TP_OPTIONS2) set_option = FALSE;
      }
      else
      {
        button[actual_button].state = ST_PRESSED;
        if (button[actual_button].type == TP_OPTIONS2) set_option = TRUE;
      }

      clear_keybuf();
    }
    else if (key[KEY_ESC])
    {
       menu_change = TRUE;
       new_menu = MNU_MPLAYER_SEL;
       moveout_menu();
    }

    for (nr=0; nr < button_anz; nr++)
    {
      if (button[nr].state == ST_PRESSED)
      {
        if (button[nr].state != ST_DEACTIVATED)
        {
          switch (nr)
          {
            case 0 : // Play Level
                 play_sound_sample(snd_menu_select,1000,0,0,0,100);
                 do_button_explosion(&button[nr]);
                 button[nr].state = ST_DEACTIVATED;
                 begin_play = TRUE;
                 game_mode = MP_2PDOGFIGHT;
                 fade_out_active = TRUE;
                 fade_pos = 0; fade_opos = 0; fade_type = 1; fade_count = 0; fade_count_to = 200; fade_speed = 0.5;
                 lock_keyboard = TRUE;
                 break;
            case 7 : // Back
                 play_sound_sample(snd_menu_select,1000,0,0,0,100);
                 do_button_explosion(&button[nr]);
                 button[nr].state = ST_DEACTIVATED;
                 menu_change = TRUE;
                 new_menu = MNU_MPLAYER_SEL;
                 moveout_menu();
                 break;
          } // switch nr
        } // if !deactivated
      } // if pressed

      if (button[nr].state != ST_PRESSED)
      {
        if (nr == actual_button) button[nr].state = ST_SELECTED;
          else if (button[nr].state == ST_SELECTED) button[nr].state = ST_OK;
      }
      
    } // for nr

  } // if not change

}


void do_menu_sp_training()
{
  int nr;

  // Tastendruecke
  if (!menu_change)
  {

    if ((getctrl(KEY_DOWN,1) || getctrl(JOY0_DOWN,1)) && !lock_keyboard && !quit_menu)
    {
      do
      {
        if (actual_button < button_anz-1)
        {
          actual_button++;
        }
        else actual_button = 0;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_UP,1) || getctrl(JOY0_UP,1)) && !lock_keyboard && !quit_menu)
    {
      do
      {
        if (actual_button > 0)
        {
          actual_button--;
        }
        else actual_button = button_anz-1;
      } while (button[actual_button].state == ST_DEACTIVATED);
      play_sound_sample(snd_menu_change,1000,0,0,0,100);
      clear_keybuf();
    }
    else if ((getctrl(KEY_RIGHT,1) || getctrl(JOY0_RIGHT,1)) && !lock_keyboard)
    {
      if (actual_button == 0)
      {
        if (current_training_level < cur_max_training_levels)
        {
          current_training_level++;
//          CalcScoreString(score_str,
//                          hiscore_file.hiscore[current_level].nr[score_pos].name,
//                          hiscore_file.hiscore[current_level].nr[score_pos].score);

          play_sound_sample(snd_click,1000,0,0,0,100);

          strcpy(button[0].text1,menu_message[MSG_MENU_PLAYLEVEL].text1);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      clear_keybuf();
    }
    else if ((getctrl(KEY_LEFT,1) || getctrl(JOY0_LEFT,1)) && !lock_keyboard)
    {
      if (actual_button == 0)
      {
        if (current_training_level > 1)
        {
          current_training_level--;
//          CalcScoreString(score_str,
//                          hiscore_file.hiscore[current_level].nr[score_pos].name,
//                          hiscore_file.hiscore[current_level].nr[score_pos].score);
          play_sound_sample(snd_click,1000,0,0,0,100);
          strcpy(button[0].text1,menu_message[MSG_MENU_PLAYLEVEL].text1);
        }
        else
          play_sound_sample(snd_noweap,1000,0,0,0,100);
      }
      clear_keybuf();
    }
    else if ((getctrl(KEY_ENTER,1) || getctrl(JOY0_B1,1)) && !lock_keyboard && !quit_menu)
    {
      if (button[actual_button].type == TP_NORMAL) button[actual_button].draw = FALSE;
      button[actual_button].state = ST_PRESSED;
      clear_keybuf();
    }
    else if (key[KEY_ESC])
    {
       menu_change = TRUE;
       new_menu = MNU_SPLAYER_SEL;
       moveout_menu();
    }

    for (nr=0; nr < button_anz; nr++)
    {
      if (button[nr].state == ST_PRESSED)
      {
        if (button[nr].state != ST_DEACTIVATED)
        {
          switch (nr)
          {
            case 0 : // Play Level
                 play_sound_sample(snd_menu_select,1000,0,0,0,100);
                 do_button_explosion(&button[nr]);
                 button[nr].state = ST_DEACTIVATED;
                 begin_play = TRUE;
                 game_mode = SP_TRAINING;
                 fade_out_active = TRUE;
                 fade_pos = 0; fade_opos = 0; fade_type = 1; fade_count = 0; fade_count_to = 200; fade_speed = 0.5;
                 lock_keyboard = TRUE;
                 break;
            case 1 : // Back
                 play_sound_sample(snd_menu_select,1000,0,0,0,100);
                 do_button_explosion(&button[nr]);
                 button[nr].state = ST_DEACTIVATED;
                 menu_change = TRUE;
                 new_menu = MNU_SPLAYER_SEL;
                 moveout_menu();
                 break;
          } // switch nr
        } // if !deactivated
      } // if pressed
  
      if (nr == actual_button) button[nr].state = ST_SELECTED;
        else if (button[nr].state == ST_SELECTED) button[nr].state = ST_OK;
  
    } // for nr

  } // if not change
}


void do_menu_options()
{
  int nr;

  // Tastendruecke
  if (!menu_change)
  {

    if ((getctrl(KEY_DOWN,1) || getctrl(JOY0_DOWN,1)) && !lock_keyboard)
    {
      if (!set_option)
      {
        if (actual_button == 2) actual_button = 6;
        else if (actual_button == 7) actual_button = 3;
        else if (actual_button == 5) actual_button = 7;
        else if (actual_button == 6) actual_button = 0;
        else
        do
        {
          if (actual_button < button_anz-1) actual_button++;
            else actual_button = 0;
        } while (button[actual_button].state == ST_DEACTIVATED);
        play_sound_sample(snd_menu_change,1000,0,0,0,100);
      } // if set_option
      else
      {
      }
      clear_keybuf();
    }
    else if ((getctrl(KEY_UP,1) || getctrl(JOY0_UP,1)) && !lock_keyboard)
    {
      if (!set_option)
      {
        if (actual_button == 3) actual_button = 7;
        else if (actual_button == 6) actual_button = 2;
        else if (actual_button == 7) actual_button = 5;
        else if (actual_button == 0) actual_button = 6;
        else
        do
        {
          if (actual_button > 0) actual_button--;
            else actual_button = button_anz-1;
        } while (button[actual_button].state == ST_DEACTIVATED);
        play_sound_sample(snd_menu_change,1000,0,0,0,100);
      } // if set_option
      else
      {
      }
      clear_keybuf();
    }
    else if (((getctrl(KEY_RIGHT,1) || getctrl(JOY0_RIGHT,1)) || (getctrl(KEY_LEFT,1) || getctrl(JOY0_LEFT,1))) && !lock_keyboard)
    {
      if (!set_option)
      {
        play_sound_sample(snd_menu_change,1000,0,0,0,100);
        switch (actual_button)
        {
          case  0  :  if (button[3].state != ST_DEACTIVATED) actual_button = 3;
                      break;
          case  1  :  if (button[4].state != ST_DEACTIVATED) actual_button = 4;
                      break;
          case  2  :  if (button[5].state != ST_DEACTIVATED) actual_button = 5;
                      break;
          case  3  :  if (button[0].state != ST_DEACTIVATED) actual_button = 0;
                      break;
          case  4  :  if (button[1].state != ST_DEACTIVATED) actual_button = 1;
                      break;
          case  5  :  if (button[2].state != ST_DEACTIVATED) actual_button = 2;
                      break;
          case  6  :  actual_button = 7;
                      break;
          case  7  :  actual_button = 6;
                      break;
        } // switch actual
      } // if set_option
      else
      {
        switch (actual_button)
        {
          case 0  : // Language
                    play_sound_sample(snd_menu_change,1000,0,0,0,100);
                    switch (LANGUAGE)
                    {
                      case 0 : LANGUAGE = 1;
                               break;
                      case 1 : LANGUAGE = 0;
                               break;
                    } // sw lang
                    break;
          case 1  : // Pixanz
                    play_sound_sample(snd_click,1000,0,0,0,100);
                    if ((getctrl(KEY_RIGHT,0) || getctrl(JOY0_RIGHT,0)))
                      if (PIXEL_DIVISOR < 200) PIXEL_DIVISOR+=2;
                    if ((getctrl(KEY_LEFT,0) || getctrl(JOY0_LEFT,0)))
                      if (PIXEL_DIVISOR > 0) PIXEL_DIVISOR-=2;
                    break;
          case 2  : // Volume
                    play_sound_sample(snd_click,1000,0,0,0,100);
                    if ((getctrl(KEY_RIGHT,0) || getctrl(JOY0_RIGHT,0)))
                      if (SOUND_VOLUME < 100) SOUND_VOLUME+=2;
                    if ((getctrl(KEY_LEFT,0) || getctrl(JOY0_LEFT,0)))
                      if (SOUND_VOLUME > 0) SOUND_VOLUME-=2;
                    break;
          case 3  : // Debris
                    play_sound_sample(snd_menu_change,1000,0,0,0,100);
                    switch (SHOW_DEBRIS)
                    {
                      case 0 : SHOW_DEBRIS = 1;
                               break;
                      case 1 : SHOW_DEBRIS = 0;
                               break;
                    } // sw lang
                    break;
          case 4  : // PGrav
                    play_sound_sample(snd_menu_change,1000,0,0,0,100);
                    switch (GRAV_4_PIXELS)
                    {
                      case 0 : GRAV_4_PIXELS = 1;
                               break;
                      case 1 : GRAV_4_PIXELS = 0;
                               break;
                    } // sw lang
                    break;
          case 5  : // Menu Music
                    play_sound_sample(snd_menu_change,1000,0,0,0,100);
                    if ((getctrl(KEY_RIGHT,0) || getctrl(JOY0_RIGHT,0)))
                      switch (PLAY_MENU_MUSIC)
                      {
                        case 0 : PLAY_MENU_MUSIC = 2;
                                 stop_mod();
                                 break;
                        case 1 : PLAY_MENU_MUSIC = 0;
                                 stop_mod();
                                 break;
                        case 2 : PLAY_MENU_MUSIC = 1;
                                 play_mod(gfmod,1);
                                 break;
                      } // sw lang
                    else if ((getctrl(KEY_LEFT,0) || getctrl(JOY0_LEFT,0)))
                      switch (PLAY_MENU_MUSIC)
                      {
                        case 0 : PLAY_MENU_MUSIC = 1;
                                 play_mod(gfmod,1);
                                 break;
                        case 1 : PLAY_MENU_MUSIC = 2;
                                 stop_mod();
                                 break;
                        case 2 : PLAY_MENU_MUSIC = 0;
                                 stop_mod();
                                 break;
                      } // sw lang

                    break;

        } // switch actualbut
      } // else

      clear_keybuf();
    }
    else if ((getctrl(KEY_ENTER,1) || getctrl(JOY0_B1,1)) && !lock_keyboard)
    {
      if (button[actual_button].type == TP_NORMAL) button[actual_button].draw = FALSE;

      if (button[actual_button].state == ST_PRESSED)
      {
        button[actual_button].state = ST_SELECTED;
        if (button[actual_button].type == TP_OPTIONS) set_option = FALSE;

        if (actual_button == 0)
        {
          read_message_file(LANGUAGE);
          menu_options_init();
        }

      }
      else
      {
        button[actual_button].state = ST_PRESSED;
        if (button[actual_button].type == TP_OPTIONS) set_option = TRUE;
      }

      clear_keybuf();
    }
    else if ((key[KEY_ESC]) && (button[actual_button].state == ST_PRESSED) && !lock_keyboard)
    {
      button[actual_button].state = ST_SELECTED;
      if (button[actual_button].type == TP_OPTIONS) set_option = FALSE;
      clear_keybuf();
    }
    else if (key[KEY_ESC])
    {
       menu_change = TRUE;
       new_menu = MNU_MAIN;
       moveout_menu();
    }


    for (nr=0; nr < button_anz; nr++)
    {
      if (button[nr].state == ST_PRESSED)
      {
        if (button[nr].state != ST_DEACTIVATED)
        {
          if (button[nr].type == TP_NORMAL)
          {
            play_sound_sample(snd_menu_select,1000,0,0,0,100);
            do_button_explosion(&button[nr]);
            button[nr].state = ST_DEACTIVATED;
            menu_change = TRUE;

            switch (nr)
            {
              case 6 : // Back
                   new_menu = MNU_MAIN;
                   moveout_menu();
                   break;
              case 7 : // Keymap
                   new_menu = MNU_KEYMAP_1P;
                   moveout_menu();
                   break;
            } // switch nr
          } // if normal
        } // if !deactivated
      } // if pressed

      if (button[nr].state != ST_PRESSED)
      {
        if (nr == actual_button) button[nr].state = ST_SELECTED;
          else if (button[nr].state == ST_SELECTED) button[nr].state = ST_OK;
      }
    } // for nr

  } // if not change
}

void do_menu_keymap_1p()
{
  int nr,keynum;

  // Tastendruecke
  if (!menu_change)
  {

    if ((getctrl(KEY_DOWN,1) || getctrl(JOY0_DOWN,1)) && !lock_keyboard && !set_option)
    {
      if (!set_option)
      {
        if (actual_button == 6) actual_button = 13;
        else if (actual_button == 13) actual_button = 0;
        else if (actual_button == 14) actual_button = 7;
        else if (actual_button == 12) actual_button = 15;
        else if (actual_button == 15) actual_button = 14;
        else
        do
        {
          if (actual_button < button_anz-1) actual_button++;
            else actual_button = 0;
        } while (button[actual_button].state == ST_DEACTIVATED);
        play_sound_sample(snd_menu_change,1000,0,0,0,100);
      } // if set_option
      else
      {
      }
      clear_keybuf();
    }
    else if ((getctrl(KEY_UP,1) || getctrl(JOY0_UP,1)) && !lock_keyboard && !set_option)
    {
      if (!set_option)
      {
        if (actual_button == 14) actual_button = 15;
        else if (actual_button == 15) actual_button = 12;
        else if (actual_button == 0) actual_button = 13;
        else if (actual_button == 13) actual_button = 6;
        else if (actual_button == 7) actual_button = 14;
        else
        do
        {
          if (actual_button > 0) actual_button--;
            else actual_button = button_anz-1;
        } while (button[actual_button].state == ST_DEACTIVATED);
        play_sound_sample(snd_menu_change,1000,0,0,0,100);
      } // if set_option
      else
      {
      }
      clear_keybuf();
    }
    else if ((getctrl(KEY_ENTER,1) || getctrl(JOY0_B1,1)) && !lock_keyboard && !set_option)
    {
      if (button[actual_button].type == TP_NORMAL) button[actual_button].draw = FALSE;

      if (actual_button == 15)   // mouse on/off: toggle, no key to wait for
      {
        config_file.config.no_mouse = !config_file.config.no_mouse;
        play_sound_sample(snd_menu_change,1000,0,0,0,100);
      }
      else if (button[actual_button].state == ST_PRESSED)
      {
        button[actual_button].state = ST_SELECTED;
        if (button[actual_button].type == TP_OPTIONS2) set_option = FALSE;
      }
      else
      {
        button[actual_button].state = ST_PRESSED;
        if (button[actual_button].type == TP_OPTIONS2) set_option = TRUE;
      }

      clear_keybuf();
    }
    else if ((keypressed() || joy_activity() ||
             (key_shifts & KB_SHIFT_FLAG) ||
             (key_shifts & KB_CTRL_FLAG) ||
             (key_shifts & KB_ALT_FLAG)) && !lock_keyboard && !key[KEY_ESC]
             && get_key_okay())
    {
      if (!set_option && ((getctrl(KEY_LEFT,1) || getctrl(JOY0_LEFT,1)) || (getctrl(KEY_RIGHT,1) || getctrl(JOY0_RIGHT,1))))
      {
        play_sound_sample(snd_menu_change,1000,0,0,0,100);
        switch (actual_button)
        {
          case  0  :  actual_button = 7;
                      break;
          case  1  :  actual_button = 8;
                      break;
          case  2  :  actual_button = 9;
                      break;
          case  3  :  actual_button = 10;
                      break;
          case  4  :  actual_button = 11;
                      break;
          case  5  :  actual_button = 12;
                      break;
          case  6  :  actual_button = 15;
                      break;
          case  7  :  actual_button = 0;
                      break;
          case  8  :  actual_button = 1;
                      break;
          case  9  :  actual_button = 2;
                      break;
          case  10 :  actual_button = 3;
                      break;
          case  11 :  actual_button = 4;
                      break;
          case  12 :  actual_button = 5;
                      break;
          case  13 :  actual_button = 14;
                      break;
          case  14 :  actual_button = 13;
                      break;
          case  15 :  actual_button = 6;
                      break;
        } // switch actual
      } // if set_option
      else if (set_option)
      {
        keynum = get_pressed_key();
        if (keynum == 0)
        {
          keynum = readkey() >> 8;
          if (keynum==0) keynum = readkey();
        }

        play_sound_sample(snd_menu_change,1000,0,0,0,100);
        set_option = FALSE;
        button[actual_button].state = ST_OK;

        switch (actual_button)
        {
          case 0  : pcontrol[0].thrust = keynum; break;
          case 1  : pcontrol[0].shoot = keynum; break;
          case 2  : pcontrol[0].left = keynum; break;
          case 3  : pcontrol[0].right = keynum; break;
          case 4  : pcontrol[0].special1 = keynum; break;
          case 5  : pcontrol[0].nw = keynum; break;
          case 6  : pcontrol[0].pw = keynum; break;
          case 7  : pcontrol[0].w1 = keynum; break;
          case 8  : pcontrol[0].w2 = keynum; break;
          case 9  : pcontrol[0].w3 = keynum; break;
          case 10 : pcontrol[0].w4 = keynum; break;
          case 11 : pcontrol[0].w5 = keynum; break;
          case 12 : pcontrol[0].w6 = keynum; break;
        } // switch actualbut
      } // else

      clear_keybuf();
    }
    else if ((key[KEY_ESC] || !get_key_okay()) && button[actual_button].state == ST_PRESSED && !lock_keyboard)
    {
      button[actual_button].state = ST_SELECTED;
      if (button[actual_button].type == TP_OPTIONS2) set_option = FALSE;
      clear_keybuf();
    }
    else if (key[KEY_ESC])
    {
       menu_change = TRUE;
       new_menu = MNU_OPTIONS;
       moveout_menu();
    }

    for (nr=0; nr < button_anz; nr++)
    {
      if (button[nr].state == ST_PRESSED)
      {
        if (button[nr].state != ST_DEACTIVATED)
        {
          if (button[nr].type == TP_NORMAL)
          {
            play_sound_sample(snd_menu_select,1000,0,0,0,100);
            do_button_explosion(&button[nr]);
            button[nr].state = ST_DEACTIVATED;
            menu_change = TRUE;

            switch (nr)
            {
              case 13 : // Back
                   new_menu = MNU_OPTIONS;
                   moveout_menu();
                   break;
              case 14 : // 2nd keymap
                   new_menu = MNU_KEYMAP_2P;
                   moveout_menu();
                   break;
            } // switch nr
          } // if normal
        } // if !deactivated
      } // if pressed

      if (button[nr].state != ST_PRESSED)
      {
        if (nr == actual_button) button[nr].state = ST_SELECTED;
          else if (button[nr].state == ST_SELECTED) button[nr].state = ST_OK;
      }
    } // for nr

  } // if not change

}

void do_menu_keymap_2p()
{
  int nr,keynum;

  // Tastendruecke
  if (!menu_change)
  {

    if ((getctrl(KEY_DOWN,1) || getctrl(JOY0_DOWN,1)) && !lock_keyboard && !set_option)
    {
      if (!set_option)
      {
        if (actual_button == 6) actual_button = 14;
        else if (actual_button == 13) actual_button = 14;
        else
        do
        {
          if (actual_button < button_anz-1) actual_button++;
            else actual_button = 0;
        } while (button[actual_button].state == ST_DEACTIVATED);
        play_sound_sample(snd_menu_change,1000,0,0,0,100);
      } // if set_option
      else
      {
      }
      clear_keybuf();
    }
    else if ((getctrl(KEY_UP,1) || getctrl(JOY0_UP,1)) && !lock_keyboard && !set_option)
    {
      if (!set_option)
      {
        if (actual_button == 14) actual_button = 6;
        else if (actual_button == 7) actual_button = 14;
        else
        do
        {
          if (actual_button > 0) actual_button--;
            else actual_button = button_anz-1;
        } while (button[actual_button].state == ST_DEACTIVATED);
        play_sound_sample(snd_menu_change,1000,0,0,0,100);
      } // if set_option
      else
      {
      }
      clear_keybuf();
    }
    else if ((getctrl(KEY_ENTER,1) || getctrl(JOY0_B1,1)) && !lock_keyboard && !set_option)
    {
      if (button[actual_button].type == TP_NORMAL) button[actual_button].draw = FALSE;

      if (button[actual_button].state == ST_PRESSED)
      {
        button[actual_button].state = ST_SELECTED;
        if (button[actual_button].type == TP_OPTIONS2) set_option = FALSE;
      }
      else
      {
        button[actual_button].state = ST_PRESSED;
        if (button[actual_button].type == TP_OPTIONS2) set_option = TRUE;
      }

      clear_keybuf();
    }
    else if ((keypressed() || joy_activity() ||
             (key_shifts & KB_SHIFT_FLAG) ||
             (key_shifts & KB_CTRL_FLAG) ||
             (key_shifts & KB_ALT_FLAG)) && !lock_keyboard && !key[KEY_ESC]
             && get_key_okay())
    {
      if (!set_option && ((getctrl(KEY_LEFT,1) || getctrl(JOY0_LEFT,1)) || (getctrl(KEY_RIGHT,1) || getctrl(JOY0_RIGHT,1))))
      {
        play_sound_sample(snd_menu_change,1000,0,0,0,100);
        switch (actual_button)
        {
          case  0  :  actual_button = 7;
                      break;
          case  1  :  actual_button = 8;
                      break;
          case  2  :  actual_button = 9;
                      break;
          case  3  :  actual_button = 10;
                      break;
          case  4  :  actual_button = 11;
                      break;
          case  5  :  actual_button = 12;
                      break;
          case  6  :  actual_button = 13;
                      break;
          case  7  :  actual_button = 0;
                      break;
          case  8  :  actual_button = 1;
                      break;
          case  9  :  actual_button = 2;
                      break;
          case  10 :  actual_button = 3;
                      break;
          case  11 :  actual_button = 4;
                      break;
          case  12 :  actual_button = 5;
                      break;
          case  13 :  actual_button = 6;
                      break;
        } // switch actual
      } // if set_option
      else if (set_option)
      {
        keynum = get_pressed_key();
        if (keynum == 0)
        {
          keynum = readkey() >> 8;
          if (keynum==0) keynum = readkey();
        }

        play_sound_sample(snd_menu_change,1000,0,0,0,100);
        set_option = FALSE;
        button[actual_button].state = ST_OK;

        switch (actual_button)
        {
          case 0  : pcontrol[1].thrust = keynum; break;
          case 1  : pcontrol[1].shoot = keynum; break;
          case 2  : pcontrol[1].left = keynum; break;
          case 3  : pcontrol[1].right = keynum; break;
          case 4  : pcontrol[1].special1 = keynum; break;
          case 5  : pcontrol[1].nw = keynum; break;
          case 6  : pcontrol[1].pw = keynum; break;

          case 7  : pcontrol[2].thrust = keynum; break;
          case 8  : pcontrol[2].shoot = keynum; break;
          case 9  : pcontrol[2].left = keynum; break;
          case 10 : pcontrol[2].right = keynum; break;
          case 11 : pcontrol[2].special1 = keynum; break;
          case 12 : pcontrol[2].nw = keynum; break;
          case 13 : pcontrol[2].pw = keynum; break;
        } // switch actualbut
      } // else

      clear_keybuf();
    }
    else if ((key[KEY_ESC] || !get_key_okay()) && (button[actual_button].state == ST_PRESSED) && !lock_keyboard)
    {
      button[actual_button].state = ST_SELECTED;
      if (button[actual_button].type == TP_OPTIONS2) set_option = FALSE;
      clear_keybuf();
    }
    else if (key[KEY_ESC])
    {
       menu_change = TRUE;
       new_menu = MNU_KEYMAP_1P;
       moveout_menu();
    }

    for (nr=0; nr < button_anz; nr++)
    {
      if (button[nr].state == ST_PRESSED)
      {
        if (button[nr].state != ST_DEACTIVATED)
        {
          if (button[nr].type == TP_NORMAL)
          {
            play_sound_sample(snd_menu_select,1000,0,0,0,100);
            do_button_explosion(&button[nr]);
            button[nr].state = ST_DEACTIVATED;
            menu_change = TRUE;

            switch (nr)
            {
              case 14 : // Back
                   new_menu = MNU_KEYMAP_1P;
                   moveout_menu();
                   break;
            } // switch nr
          } // if normal
        } // if !deactivated
      } // if pressed

      if (button[nr].state != ST_PRESSED)
      {
        if (nr == actual_button) button[nr].state = ST_SELECTED;
          else if (button[nr].state == ST_SELECTED) button[nr].state = ST_OK;
      }
    } // for nr

  } // if not change


}


int show_menu()
{
  int actual_time;
  int update_graphics=FALSE;
  static int endthis=FALSE;

  gf_init_menu();
  playship[0].xpos = playship[0].ypos = 0;

  LOCK_VARIABLE(mgame_time);
  LOCK_FUNCTION(mgame_timer);
  install_int_ex(mgame_timer,BPS_TO_TIMER(60));

  mgame_time = actual_time = 0;
  begin_play = FALSE;
//  actual_menu = MNU_MAIN;
  actual_button = 0;

  menu_main_init();
  text_mode(-1);

  save_screen = FALSE;

  endthis = FALSE;
  clear_keybuf();

  lock_keyboard = FALSE;

  switch (actual_menu)
  {
    case MNU_MAIN :
      menu_main_init();
      break;
    case MNU_SPLAYER_SEL :
      menu_sp_sel_init();
      break;
    case MNU_MPLAYER_SEL :
      menu_mp_sel_init();
      break;
    case MNU_SPLAYER :
      menu_splayer_init();
      break;
    case MNU_SP_RACE :
      menu_sp_race_init();
      break;
    case MNU_MP_RACE :
      menu_mp_race_init();
      break;
    case MNU_QDOGFIGHT :
      menu_qdogfight_init();
      break;
    case MNU_DOGFIGHT :
      menu_dogfight_init();
      break;
    case MNU_SP_TRAINING :
      menu_sp_training_init();
      break;
    case MNU_OPTIONS :
      menu_options_init();
      break;
    case MNU_KEYMAP_1P :
      menu_keymap_1p_init();
      break;
    case MNU_KEYMAP_2P :
      menu_keymap_2p_init();
      break;
  } // switch

  title_x = title_ex; title_y = title_ey; title_in = TRUE;

  if (gfmod && PLAY_MENU_MUSIC == 1) play_mod(gfmod,1);

  while ( (!endthis) )
  {

    while (mgame_time > actual_time)
    {
      actual_time++;

      CheckFade();
      if (fade_count && fade_count_to && gfmod)
        set_mod_volume(150.0*(1.0-((float)fade_count/(float)fade_count_to)));

      if (input_active) do_input_box();

//      if (use_joystick)
      poll_joystick();

      if (begin_play || quit_menu ) if (fade_out_active && (fade_type == 1)) if (fade_count >= fade_count_to) endthis = TRUE;

      menu_mouse();
      switch (actual_menu)
      {
        case MNU_MAIN :
               do_menu_main();
               break;
        case MNU_SPLAYER_SEL :
               CalcScoreString(score_str,
                  hiscore_file.hiscore[current_level].nr[score_pos].name,
                  hiscore_file.hiscore[current_level].nr[score_pos].score);
               do_menu_sp_sel();
               break;
        case MNU_MPLAYER_SEL :
               do_menu_mp_sel();
               break;
        case MNU_SPLAYER :
               do_menu_splayer();
               break;
        case MNU_SP_RACE :
               CalcRaceScoreString(score_str,
                 hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].name,
                 hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].m,
                 hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].s,
                 hiscore_file.race_hiscore[current_sprace_level].nr[score_pos].hs);
               do_menu_sp_race();
               break;
        case MNU_MP_RACE :
               do_menu_mp_race();
               break;
        case MNU_QDOGFIGHT :
               do_menu_qdogfight();
               break;
        case MNU_DOGFIGHT :
               do_menu_dogfight();
               break;
        case MNU_SP_TRAINING :
               do_menu_sp_training();
               break;
        case MNU_OPTIONS :
               do_menu_options();
               break;
        case MNU_KEYMAP_1P :
               do_menu_keymap_1p();
               break;
        case MNU_KEYMAP_2P :
               do_menu_keymap_2p();
               break;
      } // switch menu

      if (menu_change)
        retval = move_menu();

      if (key[KEY_F12]) { save_screen = TRUE; }
      if (key[KEY_ESC] && actual_menu == MNU_MAIN) { quit_menu = TRUE; endthis = TRUE; }
      // fast exit (Shift+ESC)
      if (key[KEY_ESC] && (key_shifts & KB_SHIFT_FLAG)) { quit_menu = TRUE; endthis = TRUE; }

      if (retval && (new_menu != actual_menu))
      {
        actual_menu = new_menu;
        switch (new_menu)
        {
          case MNU_MAIN :
                menu_main_init();
                movein_menu();
                break;
          case MNU_SPLAYER_SEL :
                menu_sp_sel_init();
                movein_menu();
                break;
          case MNU_MPLAYER_SEL :
                menu_mp_sel_init();
                movein_menu();
                break;
          case MNU_SPLAYER :
                menu_splayer_init();
                movein_menu();
                break;
          case MNU_SP_RACE :
                menu_sp_race_init();
                movein_menu();
                break;
          case MNU_MP_RACE :
                menu_mp_race_init();
                movein_menu();
                break;
          case MNU_QDOGFIGHT :
                menu_qdogfight_init();
                movein_menu();
                break;
          case MNU_DOGFIGHT :
                menu_dogfight_init();
                movein_menu();
                break;
          case MNU_SP_TRAINING :
                menu_sp_training_init();
                movein_menu();
                break;
          case MNU_OPTIONS :
                menu_options_init();
                movein_menu();
                break;
          case MNU_KEYMAP_1P :
                menu_keymap_1p_init();
                movein_menu();
                break;
          case MNU_KEYMAP_2P :
                menu_keymap_2p_init();
                movein_menu();
                break;
        } // switch
      }
      else if (retval)
      {
        menu_change = FALSE;
        retval = 0;
      }
//      add_pixels(1,120+random() % 400,35+random() % 40,-0.7,0,0,tbl_green,10,70,1100,800,NULL);
      do_pixels(0);

      update_graphics = TRUE;
    } // while game_time>actual_time

    if (update_graphics)
    {
      update_stars();

      draw_stars();
      draw_pixels(vscreen,0);

      // Gravity Force
      draw_rle_sprite(vscreen,data[mnu_title].dat,100,20);

      // Title
      draw_sprite(vscreen,title,title_x,title_y);
      add_2_list(&rl,title_x,title_y,SCREEN_W-title_x+5,12);

      draw_buttons();

      if (input_active) draw_input_box();

//      menu_draw_panel_text();

      blit(vscreen,screen,0,0,0,0,640,480);

      mnu_clear_screen();
      update_graphics = FALSE;

      // "I hate this music!" -- give me beeps :-)
      if (PLAY_MENU_MUSIC == 2 && actual_menu != MNU_OPTIONS)
      {
        if (rand() % 100 < 40)
          play_sound_sample(snd_menu_select,rand() % 10000,0,0,0,100);
      }

    } // if update_graphics

    if (save_screen)
    {
      make_screenshot();
      save_screen = FALSE;
    }

  } // while !end

  remove_int(mgame_timer);
  exit_menu();

  write_config_file();

  if (quit_menu) {  close_log(); exit(0); }
  return 0;
}


