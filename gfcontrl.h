/*
 *    GRAVITY FORCE, gfcontrl.h
 *
 *    -- control definitions
 *
 *
 *    see source.txt for further information
 */

#define  JOY0_UP       1001
#define  JOY0_DOWN     1002
#define  JOY0_LEFT     1003
#define  JOY0_RIGHT    1004
#define  JOY0_B1       1005
#define  JOY0_B2       1006
#define  JOY0_B3       1007
#define  JOY0_B4       1008

#define  JOY1_UP       1011
#define  JOY1_DOWN     1012
#define  JOY1_LEFT     1013
#define  JOY1_RIGHT    1014
#define  JOY1_B1       1015
#define  JOY1_B2       1016
#define  JOY1_B3       1017
#define  JOY1_B4       1018

// Player Controls array
typedef struct {
  int shoot,thrust,left,right;
  int special1,special2,special3;
  int w1,w2,w3,w4,w5,w6,nw,pw;
} PLAYER_CONTROL;

PLAYER_CONTROL pcontrol[MAX_PLAYERS];

int use_joystick;

// Header aus gfcontrl.c

void open_demo_file(char *file, int mode);
void close_demo_file();
void read_keys(int c);
void check_collisions(BITMAP *scr, int c);
void lay_extra(int c);
int getctrl(int c, int reset);

