/*
 *    GRAVITY FORCE, gfhead.h
 *
 *    -- main header file
 *
 *
 *    see source.txt for further information
 */

#define  TRUE  1
#define  FALSE 0

#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <jgmod.h>
#include "allegttf.h"
#include "gfd.h"

#ifdef ALLEGRO_WINDOWS
  #define srandom srand
  #define random rand
#endif

#define  MAX_PLAYERS         5
#define  MAX_NETWORK_PLAYERS 8

#define  MAX_OBJECTS         100
#define  MAX_ENEMIES         200
#define  MAX_BASES           40

int      DEFDOUBLE_POWER;

char     VERSION[5];

int      MAP_WIDTH;
int      MAP_HEIGHT;

int      SCORE_WIDTH;
int      SCORE_HEIGHT;
int      USCORE_HEIGHT;

int      PLAYSCREEN_XSTART;
int      PLAYSCREEN_YSTART;
int      PLAYSCREEN_WIDTH;
int      PLAYSCREEN_HEIGHT;
int      PLAYSCREEN1_XSTART;
int      PLAYSCREEN1_YSTART;
int      PLAYSCREEN1_WIDTH;
int      PLAYSCREEN1_HEIGHT;
int      PLAYSCREEN2_XSTART;
int      PLAYSCREEN2_YSTART;
int      PLAYSCREEN2_WIDTH;
int      PLAYSCREEN2_HEIGHT;

int      PLUS_X;
int      PLUS_Y;

int      BOMB_SHOOT_TIME;
int      NEW_SHIP_TIME;

//int      REACT_W;
//int      REACT_H;

float    PARTICLE_GRAVITY;
float    LEVEL_GRAVITY;
float    BOMB_GRAVITY;
float    PLAYER_ACC;

float    MAX_SHIP_WEIGHT;
float    CARGO_WEIGHT;
float    SHOOT_SPEED;
float    SHOOT_SPEED_BOMB;
float    TURN_SPEED;
int      MAX_FUEL;
int      MAX_SHIELD;
float    FUEL_DEC;
float    ROCKET_TURN;
int      SHOW_RADAR;
int      NO_CLIP;

int      LANGUAGE;

int      PIXEL_DIVISOR;

/* begin GRAFIK_DEFINES */
#define TILE_W          48
#define TILE_H          48
#define PLBASE_TOWER_W  22
#define BASE_LANDING_W  75
#define BASE_LANDING_H   6
#define BASE_LANDOFF    10
#define BASE_IN_GRASS    5

#define MAX_RECTANGLES  10000
/* end   GRAFIK_DEFINES */

enum timing_methods {
  TIMER,
  LOGIC_TIMER,
  VSYNC
};

enum base_types {
  PLAYER_BASE   =  1,
  CARGO_BASE,
  ENEMY_S_BASE,
  ENEMY_L_BASE,
  CUSTOM_BASE
};

enum bullet_types {
  B_OWN         =  1,
  B_ENEMY,
  B_NEUTRAL
};

enum weapon_types {
  W_DEFAULT     = 0, // weiss
  W_DEFAULT2,        // gelb
  W_DEFAULT3,        // rot (neutral)
  W_DEFDOUBLE,
  W_BOMB,
  W_BOMB2,
  W_BOMB3,
  W_MG,
  W_BIGBOMB,
  W_BIGBOMB2,
  W_BIGBOMB3,
  W_ROCKET,
  W_ROCKET2,
  W_ROCKET3,
  W_MINES,
  W_MINE1,
  W_MINE2,
  W_MINE3
};

enum item_types {
  I_FULLSTOP    = 0
};

enum object_types {
  PILE          =  1,
  TUNNEL1,
  BUILDING1,
  BUILDING2,
  BUILDING3,
  BUILDING4,
  POOL1,
  POOL2,
  PPILE1,
  PPILE2,
  MAN1,
  DOORH,
  DOORV,
  SWITCHR,
  SWITCHL,
  SWITCHU,
  SWITCHD,
  DOORH2,
  DOORV2,
  BUILDING4_2,
  BUILDING_GRAV,
  RPOLER,
  RPOLEL,
  RPOLEU,
  RPOLED,
  TREE
};

enum spobject_types {
  TELEPORT      =  1,
  TELEPORT2,
  GRAVTRAP,
  GWALL_L,
  GWALL_R,
  GWALL_U,
  GWALL_D,
  BUMPL,
  BUMPR,
  BUMPU,
  BUMPD,
  EXTRA1
};

enum enemy_types {
  SHOOTER_UP    =  1,
  SHOOTER_DOWN  =  2,
  SHOOTER_RIGHT =  3,
  SHOOTER_LEFT  =  4,
  THREE_SHOOTER =  5,
  PULSE_SHOOTER =  6,
  ENEMY_SHIP_1  =  10,
  ENEMY_SHIP_2  =  11,
  ENEMY_SHIP_3  =  12,
  ENEMY_SHIP_4  =  13,
  ENEMY_BOSS_1  =  14,
  ENEMY_NSHIP_1,
  ENEMY_SHIP_5,
  ENEMY_SHIP_6,
  GSTONE_B1,
  GSTONE_S1,
  GSTONE_S2,
  CUSTOM_ENEMY  = 100
};

enum cargo_types {
  FUEL          =  10,
  LIFE          =  11,
  CMAN1         =  12,
  CSHIELD       =  13,
  CBOMBS10      =  14,
  CBOMBS20      =  15,
  CBIGBOMB1     =  16,
  CBIGBOMB3     =  17,
  CMGBULLTS     =  18,
  CBOMBS,
  CBIGBOMBS,
  CROCKETS,
  CMINES1,
  CMINES2,
  CMINES3
};

enum level_ends {
  END_GOOD      =  5,
  END_BAD
};

int redraw;

// Dirty Rectangle Zeug
typedef struct {
  int x,y,w,h;
} RECT;

typedef struct {
  int count;
  RECT rect[MAX_RECTANGLES];
} RLIST;

RLIST rl,orl;


typedef struct RECTNODE {
  RECT dat;
  struct RECTNODE *prev, *next;
} RECTNODE;

RECTNODE *first_rect, *last_rect;

PALETTE mainpal;
FONT *impact10h;
FONT *impact14h;
FONT *arial14h;
FONT *impact10hm;
FONT *impact14hm;
FONT *arial14hm;
DATAFILE *paneldat;

RGB_MAP rgb_table;
COLOR_MAP light_table;
COLOR_MAP trans_table;

BITMAP *vscreen;
BITMAP *vscreen2;
BITMAP *black_player;
BITMAP *score_table,*uscore_table;
BITMAP *osd,*osdb;
DATAFILE *data;
DATAFILE *sounddata;

// Color-Tables
int tbl_blue[21];
int tbl_white[21];
int tbl_bluewhite[21];
int tbl_green[21];
int tbl_yellow[21];
int tbl_red[21];
int tbl_all[256];

/* Sounds
SAMPLE *snd_thruster;
SAMPLE *snd_shot1;
SAMPLE *snd_shot2;
SAMPLE *snd_shot3;
SAMPLE *snd_expl1;
SAMPLE *snd_expl2;
SAMPLE *snd_expl3;
SAMPLE *snd_wiep;
SAMPLE *snd_bing;
SAMPLE *snd_hops;
SAMPLE *snd_bonk;
*/

MIDI   *mid_music;
JGMOD  *gfmod;

PALETTE white_palette,red_palette;

char cfg_name[50];
char gfx_name[50];
char map_name[50];
char lng_name[50];
char tmp_name[50];
char map_dir[50];
char lng_dir[50];
char dem_dir[50];
char dat_dir[50];
char snd_dir[50];
char tmp_dir[50];
FILE *tmp_file;
int load_files,write_files;

long int counter,countdown;
int play_sound,use_countdown;
int cur_player;
int drop_points;
int message;
int show_panel;
int  osd_counter;
int player_dead_counter;
int scroll_x[MAX_PLAYERS],scroll_y[MAX_PLAYERS];
int map_x[MAX_PLAYERS],map_y[MAX_PLAYERS];
int max_scroll_x,max_scroll_y;
int temp_var;
double global_weight;
int global_saved,global_active;

int USE_TIMER;

int base_anz, object_anz, spobject_anz, enemy_anz;

int player_anz;
int PLAYER_WIDTH, PLAYER_HEIGHT;
int save_screen;

int col_red,
    col_bred,
    col_green,
    col_blue,
    col_black,
    col_white,
    col_yellow,
    col_orange,
    col_grey,
    col_darkgrey,
    col_lightblue;

typedef struct {
  int type;
  int type2;
  int bullets[30];
} WTYPE;

typedef struct {
  int type;
  int anz[10];
} ITYPE;

// Schiffsdaten
typedef struct {
  BITMAP *player_temp;
  float xpos,ypos,oxpos,oypos;
  float axpos,aypos;
  float head;
  float xspd,yspd;
  float xacc,yacc;
  float xacc2,yacc2;
  float wght;
  int  land;
  int   ldur;
  int  dead;
  int  maxdead;
  int  base;
  int  powr;
  int  shot;
  int   shott;
  int  lifes;
  WTYPE weapon;
  ITYPE item;
  int  thrst;
  float fuel;
  int   thrust;
  int   shield;
  int   shield_active;
  int   pshield;
  int   pshield_active;
  int   onehit;
  int   tele,telet;
  int   hide;
  int   SHIELD_RECHARGE;
  int   AUTOFIRE_RATE;
  int   START_BASE;
  int   foe; // Enemy player?
} ship_type;

ship_type playship[MAX_PLAYERS];


// Kollisionsdaten
typedef struct {
  int x1,y;
  int x2;
  int cargo;
  int cargo_obj;
  int type;
  int  anz;
} coord_type;

typedef struct {
  int x1,y1;
  int x2,y2;
} c_coord_type;

coord_type base[MAX_BASES];

#define MAX_NODE_CONNECTIONS  8
#define MAX_PATH_NODES        250
#define MAX_NODE_POINTS       470

typedef struct NODE
{
   int x, y;
   fixed tangent;
   int connect[MAX_NODE_CONNECTIONS];
} NODE;

NODE path_node[MAX_PATH_NODES];
int node_anz;

// Objektdaten
typedef struct {
  BITMAP *data;
  int width,height;
  int boundxa,boundya;
  int boundxe,boundye;
} SPRB_TYPE;

typedef struct {
  int data; // position in datafile
  int width,height;
  int boundxa,boundya;
  int boundxe,boundye;
} SPR_TYPE;

SPRB_TYPE bullet_sprite[15];
int bullet_anz;

typedef struct {
  float x,y;
  int c,tc,dspd;
  int *tbl;
  float xspd,yspd,xspd2,yspd2;
  int t,mt;
  int startx,starty,max_xab,max_yab;
  BITMAP *image;
  int user;
} PIXEL_TYPE;

int pixel_anz,explosion_anz;

typedef struct {
  short int xoff,yoff;
  float xspd,yspd;
  int min_x,max_x,min_y,max_y;
  int type;
  int way;
} SHOOT;

typedef struct {
  int x,y;
  float s;
} WAYPOINT;

typedef struct {
  int xw1,yw1,xw2,yw2;
} COORDS;

typedef struct {
  BITMAP *tempbit;
  int  type;
  int  x,y;
  int starttime;
  int curframe;
  int maxframe;
  int curframet;
  int maxframet;
  int nr;
} EXPLOSION_TYPE;

typedef struct {
  SPR_TYPE image;
  float xpos,ypos;
  float xspd,yspd;
  float xacc,yacc;
  WAYPOINT waypnt[20];
  int max_waypnt;
  int cur_waypnt;
  int start_waypnt;
  int real_waypnt;
  int  nwaypnt[20];
  int  path_x[MAX_NODE_POINTS];
  int  path_y[MAX_NODE_POINTS];
  int  cur_point;
  int  anz_points;
  int  dest_point;
  int bezier;
  int chase;
  int chase_now;
  fixed curviness;
  int type;
  int type2;
  int frame;
  int maxframe;
  int framet;
  int startframe;
  int maxframet;
  int dead;
  int maxdead;
  int powr;
  int grass;
  int active;
  SHOOT shot[10];
  int sactive;
  int maxshot;
  int  shoott;
  int  shootf;
  int shootr;
  int connect;
  int onehit;
  int  hit;
  int maxhit;
  int min_x,max_x,min_y,max_y;
  COORDS react;
  int user1,user2,user3;
} SPRITE_TYPE;

typedef struct {
  SPRB_TYPE image;
  float xpos,ypos;
  float xspd,yspd,dxspd,dyspd;
  float xacc,yacc;
  int type;
  int type2;
  int frame;
  int maxframe;
  int framet;
  int startframe;
  int maxframet;
  int active;
  float drot;
  int user1,user2;
  int min_x,max_x,min_y,max_y;
  int nr; // nr = enemy || nr = player (depending on type)
  int ttl; // time to live (frames)
} SPRITEB_TYPE;

SPRITE_TYPE spobject[MAX_OBJECTS/2];
SPRITE_TYPE object[MAX_OBJECTS];
SPRITE_TYPE enemy[MAX_ENEMIES];


// Linked-List-Data
typedef struct BNODE {
  SPRITEB_TYPE dat;
  struct BNODE *prev, *next;
} BNODE;

BNODE *first_bullet, *last_bullet;

typedef struct PNODE {
  PIXEL_TYPE dat;
  struct PNODE *prev, *next;
} PNODE;

PNODE *first_pixel, *last_pixel;

typedef struct ENODE {
  EXPLOSION_TYPE dat;
  struct ENODE *prev, *next;
} ENODE;

ENODE *first_explosion, *last_explosion;

