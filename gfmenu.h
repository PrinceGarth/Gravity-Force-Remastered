/*
 *    GRAVITY FORCE, gfmenu.h
 *
 *
 *
 *    see source.txt for further information
 */

enum MENUS {
  MNU_MAIN=0,
  MNU_SPLAYER_SEL,
  MNU_SPLAYER,
  MNU_MPLAYER_SEL,
  MNU_MPLAYER,
  MNU_DEMO,
  MNU_OPTIONS,
  MNU_KEYMAP_1P,
  MNU_KEYMAP_2P,
  MNU_SP_RACE,
  MNU_MP_RACE,
  MNU_DOGFIGHT,
  MNU_QDOGFIGHT,
  MNU_SP_TRAINING
};

enum STATES {
  ST_DEACTIVATED,
  ST_OK,
  ST_SELECTED,
  ST_PRESSED
};

enum TYPES {
  TP_NORMAL,
  TP_OPTIONS,
  TP_OPTIONS2,
  TP_HISCORE
};

/*
enum MNU_MAIN {
  MAIN_SPLAYER,
  MAIN_MPLAYER,
  MAIN_DEMO,
  MAIN_OPTIONS,
  MAIN_QUIT
};

enum MNU_SPLAYER {
  SPL_LEVEL,
  SPL_PLAY,
  SPL_PASSWORD,
  SPL_BACK,
  SPL_DEMREC
};

enum MNU_MPLAYER {
  MPL_LEVEL,
  MPL_PLAY,
  MPL_PASSWORD,
  MPL_BACK,
  MPL_DEMREC
};

enum MNU_OPTIONS {
  OPT_PIXANZ,
  OPT_BACK
};
*/

typedef struct {
  int w,h;
  float x,y;
  float xspd,yspd;
  float xacc,yacc;
  float xend,yend;
  int xscl,yscl;
  int type;  // 0 = normal, 1 = options, 2 = hiscore
  int state; // 0 = deactivated, 1 = normal, 2 = selected, 3 = pressed
  int draw;
  char text1[30];
  char text2[5];
  int textcol;
  FONT *font;
  int retval;
} BUTTONTYPE;

typedef struct {
  float x,y;
  float xs,ys;
  int col;
} STARTYPE;


// Header aus gfmenu.c

//void mgame_timer();
void mnu_clear_screen();
void menu_draw_panel_text();
//void init_stars();
//void update_stars();
//void draw_stars();
void init_menu();
void exit_menu();
void menu_main_init();
void menu_splayer_init();
void menu_options_init();
void draw_buttons();
void do_button_explosion(BUTTONTYPE *but);
void moveout_menu();
void movein_menu();
int move_menu();
void do_menu_main();
void do_menu_splayer();
void do_menu_options();
int show_menu();
void change_menu(int ret);
void do_input_box();
void draw_input_box();

