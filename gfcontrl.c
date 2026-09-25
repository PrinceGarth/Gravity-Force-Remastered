/*
 *    GRAVITY FORCE, gfcontrl.c
 *
 *    -- handles player input, movement, collision checking in
 *       TOO large functions...
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfcontrl.h"
#include "gf.h"
#include "gfdraw.h"
#include "gfinit.h"
#include "gfpanel.h"
#include "gfosd.h"
#include "gflink.h"
#include "gfmisc.h"
#include "gfmap.h"
#include "gfspln.h"
#include "gfsprt.h"
#include "gfpal.h"
#include "gfmes.h"
#include "gfstat.h"
#include "gfspibul.h"
#include "sin.h"
#include "cos.h"
#include "gfsound.h"
#include "gflevel.h"
#include "gfg2pdog.h"
#include "mappyal.h"

int radar_zoom;
//extern int osd_update_counter; (hm)
extern int seed;
extern int abort_start_message, level_end;
extern int KAMIKAZE_HITS;
extern int MAX_LANDING_ANGLE,MAX_LANDING_SPEED;

FILE *demo_file;
static int demo_write_code=0,demo_read_code=0,demo_mode=0;
int demo_time_code=0;
int lock_kb;

void open_demo_file(char *file, int mode)
{
  if (mode == 1)
  {
    demo_file = fopen((char*)strcat(dem_dir,file),"rb");
    seed = getw(demo_file);
    srand(seed);
    srandom(seed);
    lock_kb = TRUE;
  }
  else if (mode == 2)
  {
    demo_file = fopen((char*)strcat(dem_dir,file),"wb");
    putw(seed,demo_file);
  }

  demo_mode = mode;
}

void close_demo_file()
{
  if (demo_mode)
    fclose(demo_file);

  lock_kb = FALSE;
  demo_mode = 0;
}

int getctrl(int c, int reset)
{
  static int j0up = FALSE, j0down = FALSE, j0left = FALSE, j0right = FALSE,
             j0b1 = FALSE, j0b2 = FALSE, j0b3 = FALSE, j0b4 = FALSE;
  static int j1up = FALSE, j1down = FALSE, j1left = FALSE, j1right = FALSE,
             j1b1 = FALSE, j1b2 = FALSE, j1b3 = FALSE, j1b4 = FALSE;

  // c < 1000? keyboard
  if (c < 1000)
  {
    if (key[c]) { if (reset) key[c] = 0; return 1; }
  }

  // released Joystick buttons?
  if (j0up && !joy[0].stick[0].axis[1].d1) j0up = FALSE;
  if (j0down && !joy[0].stick[0].axis[1].d2) j0down = FALSE;
  if (j0left && !joy[0].stick[0].axis[0].d1) j0left = FALSE;
  if (j0right && !joy[0].stick[0].axis[0].d2) j0right = FALSE;
  if (j0b1 && !joy[0].button[0].b) j0b1 = FALSE;
  if (j0b2 && !joy[0].button[1].b) j0b2 = FALSE;
  if (j0b3 && !joy[0].button[2].b) j0b3 = FALSE;
  if (j0b4 && !joy[0].button[3].b) j0b4 = FALSE;

  // c > 1000? joystick
  if (c > 1000)
  {
    switch(c)
    {
      case JOY0_UP      : if (joy[0].stick[0].axis[1].d1) { if (reset) { if (!j0up) { j0up = TRUE; return 1; } } else return 1; } break;
      case JOY0_DOWN    : if (joy[0].stick[0].axis[1].d2) { if (reset) { if (!j0down) { j0down = TRUE; return 1; } } else return 1; } break;
      case JOY0_LEFT    : if (joy[0].stick[0].axis[0].d1) { if (reset) { if (!j0left) { j0left = TRUE; return 1; } } else return 1; } break;
      case JOY0_RIGHT   : if (joy[0].stick[0].axis[0].d2) { if (reset) { if (!j0right) { j0right = TRUE; return 1; } } else return 1; } break;
      case JOY0_B1      : if (joy[0].button[0].b) { if (reset) { if (!j0b1) { j0b1 = TRUE; return 1; } } else return 1; } break;
      case JOY0_B2      : if (joy[0].button[1].b) { if (reset) { if (!j0b2) { j0b2 = TRUE; return 1; } } else return 1; } break;
      case JOY0_B3      : if (joy[0].button[2].b) { if (reset) { if (!j0b3) { j0b3 = TRUE; return 1; } } else return 1; } break;
      case JOY0_B4      : if (joy[0].button[3].b) { if (reset)  { if (!j0b4) { j0b4 = TRUE; return 1; } } else return 1; } break;

      case JOY1_UP      : if (joy[1].stick[0].axis[1].d1) { if (reset) { if (!j1up) { j1up = TRUE; return 1; } } else return 1; } break;
      case JOY1_DOWN    : if (joy[1].stick[0].axis[1].d2) { if (reset) { if (!j1down) { j1down = TRUE; return 1; } } else return 1; } break;
      case JOY1_LEFT    : if (joy[1].stick[0].axis[0].d1) { if (reset) { if (!j1left) { j1left = TRUE; return 1; } } else return 1; } break;
      case JOY1_RIGHT   : if (joy[1].stick[0].axis[0].d2) { if (reset) { if (!j1right) { j1right = TRUE; return 1; } } else return 1; } break;
      case JOY1_B1      : if (joy[1].button[0].b) { if (reset) { if (!j1b1) { j1b1 = TRUE; return 1; } } else return 1; } break;
      case JOY1_B2      : if (joy[1].button[1].b) { if (reset) { if (!j1b2) { j1b2 = TRUE; return 1; } } else return 1; } break;
      case JOY1_B3      : if (joy[1].button[2].b) { if (reset) { if (!j1b3) { j1b3 = TRUE; return 1; } } else return 1; } break;
      case JOY1_B4      : if (joy[1].button[3].b) { if (reset)  { if (!j1b4) { j1b4 = TRUE; return 1; } } else return 1; } break;
    }
  }
  return 0;
}

void choose_prev_weapon(int c, int wnr)
{
  switch(wnr)
  {
    case W_MG :
                playship[c].weapon.type = W_DEFAULT;
                update_panel_weapons();
                break;
    case W_BOMB :
                if (playship[c].weapon.bullets[W_MG])
                  playship[c].weapon.type = W_MG;
                else
                  playship[c].weapon.type = W_DEFAULT;
                update_panel_weapons();
                break;
    case W_BIGBOMB :
                if (playship[c].weapon.bullets[W_BOMB])
                  playship[c].weapon.type = W_BOMB;
                else if (playship[c].weapon.bullets[W_MG])
                  playship[c].weapon.type = W_MG;
                else
                  playship[c].weapon.type = W_DEFAULT;
                update_panel_weapons();
                break;
    case W_ROCKET :
                if (playship[c].weapon.bullets[W_BIGBOMB])
                  playship[c].weapon.type = W_BIGBOMB;
                else if (playship[c].weapon.bullets[W_BOMB])
                  playship[c].weapon.type = W_BOMB;
                else if (playship[c].weapon.bullets[W_MG])
                  playship[c].weapon.type = W_MG;
                else
                  playship[c].weapon.type = W_DEFAULT;
                update_panel_weapons();
                break;
    case W_MINES :
                if (playship[c].weapon.bullets[W_ROCKET])
                  playship[c].weapon.type = W_ROCKET;
                else if (playship[c].weapon.bullets[W_BIGBOMB])
                  playship[c].weapon.type = W_BIGBOMB;
                else if (playship[c].weapon.bullets[W_BOMB])
                  playship[c].weapon.type = W_BOMB;
                else if (playship[c].weapon.bullets[W_MG])
                  playship[c].weapon.type = W_MG;
                else
                  playship[c].weapon.type = W_DEFAULT;
                update_panel_weapons();
                break;
  }
}

void choose_next_weapon(int c, int wnr)
{
  switch(wnr)
  {
    case W_DEFAULT :
                if (playship[c].weapon.bullets[W_MG])
                  playship[c].weapon.type = W_MG;
                else if (playship[c].weapon.bullets[W_BOMB])
                  playship[c].weapon.type = W_BOMB;
                else if (playship[c].weapon.bullets[W_BIGBOMB])
                  playship[c].weapon.type = W_BIGBOMB;
                else if (playship[c].weapon.bullets[W_ROCKET])
                  playship[c].weapon.type = W_ROCKET;
                else if (playship[c].weapon.bullets[W_MINES])
                  playship[c].weapon.type = W_MINES;
                update_panel_weapons();
                break;
    case W_MG :
                if (playship[c].weapon.bullets[W_BOMB])
                  playship[c].weapon.type = W_BOMB;
                else if (playship[c].weapon.bullets[W_BIGBOMB])
                  playship[c].weapon.type = W_BIGBOMB;
                else if (playship[c].weapon.bullets[W_ROCKET])
                  playship[c].weapon.type = W_ROCKET;
                else if (playship[c].weapon.bullets[W_MINES])
                  playship[c].weapon.type = W_MINES;

                update_panel_weapons();
                break;
    case W_BOMB :
                if (playship[c].weapon.bullets[W_BIGBOMB])
                  playship[c].weapon.type = W_BIGBOMB;
                else if (playship[c].weapon.bullets[W_ROCKET])
                  playship[c].weapon.type = W_ROCKET;
                else if (playship[c].weapon.bullets[W_MINES])
                  playship[c].weapon.type = W_MINES;

                update_panel_weapons();
                break;
    case W_BIGBOMB :
                if (playship[c].weapon.bullets[W_ROCKET])
                  playship[c].weapon.type = W_ROCKET;
                else if (playship[c].weapon.bullets[W_MINES])
                  playship[c].weapon.type = W_MINES;

                update_panel_weapons();
                break;
    case W_ROCKET :
                if (playship[c].weapon.bullets[W_MINES])
                  playship[c].weapon.type = W_MINES;

                update_panel_weapons();
                break;
    case W_MINES :
                update_panel_weapons();
                break;
  }
}


void read_keys(int c)
{
  float scale_x,scale_y;
  float xp,yp,cx,cy;
  float xrot,yrot,angle;
  float dx,dy;
  int km = (game_mode == MP_2PDOGFIGHT) ? 1 : 0;

  demo_write_code = 0;
//  demo_read_code = 0;
  demo_time_code = 0;

  if (demo_mode==1)
  {
//      demo_time_code = getw(demo_file);
    if (demo_read_code == 0xFFFF)
    {
      if (playship[c].land)
        demo_read_code = 0;
    }
    else if (!player_dead_counter)
      demo_read_code = getw(demo_file);

    if (feof(demo_file))
    {
      close_demo_file();
    }
  }

  if (keyboard_needs_poll()) poll_keyboard();

  // thruster
 if (!playship[c].dead)
 {
  if ( ((getctrl(pcontrol[c+km].thrust,0) && !lock_kb) || demo_read_code & 1) && (playship[c].fuel > 0) )
  {
    demo_write_code |= 1;

    if (!abort_start_message)
    {
      abort_start_message = TRUE;
      clear_osd(0);
      osd_update_counter = FALSE;
      message = FALSE;
    }

    scale_x = sinus[(int)playship[c].head];
    scale_y = cosinus[(int)playship[c].head];

    playship[c].xacc = scale_x * PLAYER_ACC;
    if (!drop_points)
      playship[c].yacc = scale_y * PLAYER_ACC - (LEVEL_GRAVITY+playship[c].wght);
    else
      playship[c].yacc = scale_y * PLAYER_ACC;

    playship[c].powr = 2;
    playship[c].fuel -= FUEL_DEC;
    playship[c].land = FALSE;

    update_panel_fuel();

    /* Pixelausstoss in Gegenflugrichtung
    angle = 360 - playship[c].head;
    xrot = sinus[(int)angle];
    yrot = cosinus[(int)angle];

    dx = xrot/10;
    dy = yrot/10;

    add_pixels(20,
               playship[c].xpos+3+random()%5,playship[c].ypos+3+random()%5,
               dx, dy - PARTICLE_GRAVITY,
               0.4,
               tbl_red, 10, 45, 0, 0, NULL);
    */


    if (!playship[c].thrust) { play_sound_sample(snd_thrusters,1000,1,playship[c].xpos,playship[c].ypos,100); playship[c].thrust = TRUE; }
  }
  else
  {
    playship[c].xacc = 0;
    if (!drop_points)
      playship[c].yacc = 0-(LEVEL_GRAVITY+playship[c].wght);
    else
      playship[c].yacc = 0;

    playship[c].powr = 1;

    if (playship[c].thrust) { stop_sound_sample(snd_thrusters); playship[c].thrust = FALSE; }
  }


  playship[c].shott++;

  // Schiessen
  if (((getctrl(pcontrol[c+km].shoot,0) && !lock_kb) || (demo_read_code & 2)) && !playship_noweap)
  {
    demo_write_code |= 2;

    if (!playship[c].shot)
    {
      // Spitze finden
      xp = PLAYER_WIDTH/2.0; yp = PLAYER_HEIGHT/2.0;
      cx = xp; cy = yp;
      xrot = sinus[(int)playship[c].head];
      yrot = cosinus[(int)playship[c].head];
      dx = xp + xrot*yp;
      dy = yp - yrot*yp;

      switch (playship[c].weapon.type)
      {
        case W_DEFAULT : playship[c].shott = 0;
                         scale_x = xrot * SHOOT_SPEED;
                         scale_y = yrot * SHOOT_SPEED;
                         add_bullet(c,B_OWN,playship[c].weapon.type,playship[c].xpos+dx,playship[c].ypos+dy,scale_x,-scale_y,
                                    playship[c].xpos-PLAYSCREEN_WIDTH*2,playship[c].ypos-PLAYSCREEN_HEIGHT*2,
                                    playship[c].xpos+PLAYSCREEN_WIDTH*2,playship[c].ypos+PLAYSCREEN_HEIGHT*2);
                         play_sound_sample(snd_shot_single,1000,0,playship[c].xpos,playship[c].ypos,100);
                         player_stat[c].anz_shot++;
                         break;
        case W_MG      : if (playship[c].weapon.bullets[W_MG] > 0)
                         {
                           playship[c].shott = 0;
                           scale_x = xrot * SHOOT_SPEED;
                           scale_y = yrot * SHOOT_SPEED;

                           dx = xp - yrot*3;
                           dy = yp - xrot*3;

                           add_bullet(c,B_OWN,playship[c].weapon.type,playship[c].xpos+dx,playship[c].ypos+dy,scale_x,-scale_y,
                                      playship[c].xpos-PLAYSCREEN_WIDTH*2,playship[c].ypos-PLAYSCREEN_HEIGHT*2,
                                      playship[c].xpos+PLAYSCREEN_WIDTH*2,playship[c].ypos+PLAYSCREEN_HEIGHT*2);

                           dx = xp + yrot*3;
                           dy = yp + xrot*3;

                           add_bullet(c,B_OWN,playship[c].weapon.type,playship[c].xpos+dx,playship[c].ypos+dy,scale_x,-scale_y,
                                      playship[c].xpos-PLAYSCREEN_WIDTH*2,playship[c].ypos-PLAYSCREEN_HEIGHT*2,
                                      playship[c].xpos+PLAYSCREEN_WIDTH*2,playship[c].ypos+PLAYSCREEN_HEIGHT*2);
                           play_sound_sample(snd_shot_double,1000,0,playship[c].xpos,playship[c].ypos,100);
                           playship[c].weapon.bullets[W_MG]-=2;

                           player_stat[c].anz_shot+=2;
                         }
                         else
                         {
                           if (!playship[c].weapon.bullets[W_MG])
                             choose_prev_weapon(c,W_MG);
                           // "Klack-Sound" spielen :)
                           play_sound_sample(snd_noweap,1000,0,playship[c].xpos,playship[c].ypos,100);
                         }
                         break;
        case W_BOMB    : if ( (playship[c].shott > BOMB_SHOOT_TIME) && (playship[c].weapon.bullets[W_BOMB] > 0))
                         {
                           playship[c].shott = 0;
                           playship[c].weapon.bullets[W_BOMB]--;
                           scale_x = xrot * SHOOT_SPEED_BOMB + playship[c].xspd;
                           scale_y = yrot * SHOOT_SPEED_BOMB + playship[c].yspd;
                           add_bullet(c,B_OWN,playship[c].weapon.type,playship[c].xpos+dx,playship[c].ypos+dy,scale_x,-scale_y,
                                      playship[c].xpos-PLAYSCREEN_WIDTH*2,playship[c].ypos-PLAYSCREEN_HEIGHT*2,
                                      playship[c].xpos+PLAYSCREEN_WIDTH*2,playship[c].ypos+PLAYSCREEN_HEIGHT*2);
                           play_sound_sample(snd_shot_single,1000,0,playship[c].xpos,playship[c].ypos,100);
//                           player_stat.anz_shot++;
                         }
                         else
                         {
                           if (!playship[c].weapon.bullets[W_BOMB])
                             choose_prev_weapon(c,W_BOMB);
                           // "Klack-Sound" spielen :)
                           play_sound_sample(snd_noweap,1000,0,playship[c].xpos,playship[c].ypos,100);
                         }
                         break;
        case W_BIGBOMB : if ( (playship[c].shott > BOMB_SHOOT_TIME) && (playship[c].weapon.bullets[W_BIGBOMB] > 0))
                         {
                           playship[c].shott = 0;
                           playship[c].weapon.bullets[W_BIGBOMB]--;
                           scale_x = playship[c].xspd;
                           scale_y = playship[c].yspd;
                           add_bullet(c,B_OWN,playship[c].weapon.type,playship[c].xpos+dx,playship[c].ypos+dy,scale_x,-scale_y,
                                      playship[c].xpos-PLAYSCREEN_WIDTH*2,playship[c].ypos-PLAYSCREEN_HEIGHT*2,
                                      playship[c].xpos+PLAYSCREEN_WIDTH*2,playship[c].ypos+PLAYSCREEN_HEIGHT*2);
                           play_sound_sample(snd_shot_single,1000,0,playship[c].xpos,playship[c].ypos,100);
                           player_stat[c].anz_shot++;
                         }
                         else
                         {
                           if (!playship[c].weapon.bullets[W_BIGBOMB])
                             choose_prev_weapon(c,W_BIGBOMB);
                           // "Klack-Sound" spielen :)
                           play_sound_sample(snd_noweap,1000,0,playship[c].xpos,playship[c].ypos,100);
                         }
                         break;
        case W_ROCKET  : if ( (playship[c].shott > BOMB_SHOOT_TIME/2) && (playship[c].weapon.bullets[W_ROCKET] > 0))
                         {
                           playship[c].shott = 0;
                           playship[c].weapon.bullets[W_ROCKET]--;
                           scale_x = xrot * SHOOT_SPEED_BOMB + playship[c].xspd;
                           scale_y = yrot * SHOOT_SPEED_BOMB + playship[c].yspd;
                           add_bullet(c,B_OWN,playship[c].weapon.type,playship[c].xpos+dx-2,playship[c].ypos+dy,scale_x,-scale_y,
                                      playship[c].xpos-PLAYSCREEN_WIDTH*2,playship[c].ypos-PLAYSCREEN_HEIGHT*2,
                                      playship[c].xpos+PLAYSCREEN_WIDTH*2,playship[c].ypos+PLAYSCREEN_HEIGHT*2);
                           play_sound_sample(snd_shot_rocket,1000,0,playship[c].xpos,playship[c].ypos,100);
                           player_stat[c].anz_shot++;
                         }
                         else
                         {
                           if (!playship[c].weapon.bullets[W_ROCKET])
                             choose_prev_weapon(c,W_ROCKET);
                           // "Klack-Sound" spielen :)
                           play_sound_sample(snd_noweap,1000,0,playship[c].xpos,playship[c].ypos,100);
                         }
                         break;
        case W_MINES   : if (playship[c].weapon.bullets[W_MINES] > 0)
                         {
                           playship[c].weapon.bullets[W_MINES]--;
                           switch (playship[c].weapon.type2)
                           {
                             case W_MINE1 : case W_MINE2 : case W_MINE3 :
                                        add_bullet(c,B_OWN,playship[c].weapon.type2,
                                                   playship[c].xpos+xp,playship[c].ypos+yp,
                                                   0,0,
                                                   0,0,5000,5000);
                                        play_sound_sample(snd_noweap,1000,0,playship[c].xpos,playship[c].ypos,100);
                                        player_stat[c].anz_shot++;
                                        break;
                           } // switch type2
                         }
                         else
                         {
                           if (!playship[c].weapon.bullets[W_MINES])
                             choose_prev_weapon(c,W_MINES);
                           // "Klack-Sound" spielen :)
                           play_sound_sample(snd_noweap,1000,0,playship[c].xpos,playship[c].ypos,100);
                         }
                         break;
      } // switch player

      playship[c].shot = 1;
      update_panel_weapons_ammo();
    }
  }
  else
    playship[c].shot = FALSE;

  if (playship[c].weapon.type == W_DEFAULT || playship[c].weapon.type == W_MG)
  {
    if (playship[c].shot) playship[c].shot++;
    if (playship[c].shot > playship[c].AUTOFIRE_RATE) playship[c].shot = FALSE;
  }

  // Lenken
  if (!playship[c].land)
  {
    if ((getctrl(pcontrol[c+km].left,0) && !lock_kb) || (demo_read_code & 4))  { playship[c].head-=TURN_SPEED; demo_write_code |= 4; }
    if ((getctrl(pcontrol[c+km].right,0) && !lock_kb)  || (demo_read_code & 8)) { playship[c].head+=TURN_SPEED; demo_write_code |= 8; }
    if (playship[c].head > 360) playship[c].head = 1;
    if (playship[c].head <   0) playship[c].head = 359;

    if (abs(playship[c].yspd+playship[c].yacc+playship[c].yacc2) < 6)
      playship[c].yspd += playship[c].yacc + playship[c].yacc2;
    if (abs(playship[c].xspd+playship[c].xacc+playship[c].xacc2) < 6)
      playship[c].xspd += playship[c].xacc + playship[c].xacc2;

    playship[c].ypos -= playship[c].yspd;
    playship[c].xpos += playship[c].xspd;
  }

  if ((getctrl(pcontrol[c+km].w1,0) && !lock_kb) || (demo_read_code & 16)) { playship[c].weapon.type = W_DEFAULT; update_panel_weapons(); demo_write_code |= 16; }
  if ((getctrl(pcontrol[c+km].w2,0) && !lock_kb) || (demo_read_code & 32)) { if (playship[c].weapon.bullets[W_MG]) playship[c].weapon.type = W_MG; update_panel_weapons();  demo_write_code |= 32; }
  if ((getctrl(pcontrol[c+km].w3,0) && !lock_kb) || (demo_read_code & 64)) { if (playship[c].weapon.bullets[W_BOMB]) playship[c].weapon.type = W_BOMB; update_panel_weapons();  demo_write_code |= 64; }
  if ((getctrl(pcontrol[c+km].w4,0) && !lock_kb) || (demo_read_code & 128)) { if (playship[c].weapon.bullets[W_BIGBOMB]) playship[c].weapon.type = W_BIGBOMB; update_panel_weapons(); demo_write_code |= 128; }
  if ((getctrl(pcontrol[c+km].w5,0) && !lock_kb) || (demo_read_code & 256)) { if (playship[c].weapon.bullets[W_ROCKET]) playship[c].weapon.type = W_ROCKET; update_panel_weapons(); demo_write_code |= 256; }
  if ((getctrl(pcontrol[c+km].w6,0) && !lock_kb) || (demo_read_code & 512)) { if (playship[c].weapon.bullets[W_MINES]) playship[c].weapon.type = W_MINES; update_panel_weapons(); demo_write_code |= 512; }
  if ((getctrl(pcontrol[c+km].nw,1) && !lock_kb)) { choose_next_weapon(c,playship[c].weapon.type); update_panel_weapons(); }
  if ((getctrl(pcontrol[c+km].pw,1) && !lock_kb)) { choose_prev_weapon(c,playship[c].weapon.type); update_panel_weapons(); }

  /*
  if (key[KEY_9])
  {
    for (nr=0; nr < 10; nr++)
    {
      add_explosion(expl_big_1,5,4,nr*5, 100+(nr*10),100);
      add_explosion(expl_sm1_01,10,2,rand() % 50, rand() % 500, rand() % 300);
    }

    clear_keybuf();
  }
*/
  // Radar verstellen
  if ((key[KEY_R] || (demo_read_code & 2048)) && game_mode != MP_2PDOGFIGHT)
  {
    radar_zoom--;
    if (radar_zoom < 1) radar_zoom = 5;
    key[KEY_R] = 0;
    demo_write_code |= 2048;
  }

  // Enter - Detonate Mines
  if ((getctrl(pcontrol[c+km].special1,1) && !lock_kb && game_mode != MP_2PDOGFIGHT) || (demo_read_code & 4096)) { detonate_mines(); demo_write_code |= 4096;  }

//  if (key[KEY_0]) add_pixels(100,100,100,0,0,20,tbl_all,256,10,1000,1000,debris_1_01);
//  if (key[KEY_0]) add_pixels(10,100+random()%20,100+random()%20,0,-PARTICLE_GRAVITY-0.001,0,tbl_white,10,20,0,0,NULL);

//  if (USE_CHEATS) if ((key[KEY_ENTER] && !lock_kb) || (demo_read_code & 4096)) { playship[c].xspd = 0; playship[c].yspd = 0; clear_keybuf(); demo_write_code |= 4096; }
//  if (key[KEY_D]) { show_normal_osd_message(impact10h,impact14h,"WELCOME TO","GRAVITY FORCE",15,col_yellow,col_white,1); clear_keybuf(); }
//  if (key[KEY_E]) { show_typewriter_osd_message(impact10h,"WELCOME TO\nGRAVITY FORCE!\nJUST PLAY A BIT AND HAVE FUN!",60,5,col_orange,50); clear_keybuf(); }
  if (key[KEY_P]) { if (show_panel) { show_panel = FALSE; clear(score_table); } else { show_panel = TRUE; draw_panel(); } clear_keybuf(); }
  if (key[KEY_F]) { if (show_debug_info) {show_debug_info = FALSE; clear(score_table); draw_panel(); } else { show_debug_info = TRUE; clear(score_table); draw_panel_text(); } clear_keybuf(); }

//  if (key[KEY_D]) { if (drop_points) { drop_points = FALSE; redraw = TRUE; } else { drop_points = TRUE; draw_splines(); } clear_keybuf(); }
  if (USE_CHEATS) if (key[KEY_C]) { if (NO_CLIP) { NO_CLIP = FALSE; } else NO_CLIP = TRUE; clear_keybuf(); }

  if (key[KEY_F12]) save_screen = TRUE;

  // sonstiges (Test)
//  if (key[KEY_F]) { fade_out_active = TRUE; fade_pos = 0; fade_opos = 0; fade_type = 1; fade_count = 0; fade_count_to = 40; fade_speed = 0.2; clear_keybuf(); }
//  if (key[KEY_G]) { fade_out_active = TRUE; fade_pos = 0; fade_opos = 0; fade_type = 2; fade_count = 0; fade_count_to = 150; fade_speed = 0.5; clear_keybuf(); }
//  if (key[KEY_H]) { fade_out_active = TRUE; fade_pos = 0; fade_opos = 0; fade_type = 3; fade_count = 0; fade_count_to = 150; fade_speed = 0.5; clear_keybuf(); }

  if (USE_CHEATS) if (key[KEY_U]) { level_end = END_GOOD; clear_keybuf(); }

//  if (key[KEY_F5]) { make_levelmap(); }
//  if (key[KEY_F8]) { generate_statistics(); }

 } // if !dead

  if (demo_mode==2)
  {
//    putw(overall_counter,demo_file);
    if (player_dead_counter == 1) { putw(0xFFFF,demo_file); }
    if (!player_dead_counter)
      putw(demo_write_code,demo_file);
  }

}

void check_collisions(BITMAP *scr, int c)
{
  int nr;
  int xp,yp;
  int mid;
  int e1,e2,e3,e4,e5,e6,e7,e8;
  int kamikaze = FALSE;
  int hitwall = FALSE;
  char tmpt[100];
  char tmpt2[50];
  extern int REFUEL_TIME;
  extern int update_graphics;

  // Mittel-Koordinaten der Landekufen des Spielers
  xp = playship[c].xpos + (PLAYER_WIDTH / 2)-map_x[1];
  yp = playship[c].ypos + PLAYER_HEIGHT-map_y[1];

  if (!playship[c].land) playship[c].ldur = 0;

  // Checken, ob Spieler mit was kollidiert ist -> UEBERARBEITEN!!
  if (!playship[c].dead)
  {

    // Spieler ausserhalb Karte? Killen!
    if ((playship[c].xpos+PLAYER_WIDTH < 0) || (playship[c].ypos+PLAYER_HEIGHT < 0))
    {
      playship[c].dead = TRUE;
    }

    // Auf Kollision mit Spielern checken (Kamikaze!)
    if (game_mode == MP_2PDOGFIGHT)
    for (nr=0; nr < player_anz; nr++)
      if (nr != c && !playship[nr].dead &&
          collision(playship[c].xpos,playship[c].ypos,PLAYER_WIDTH,PLAYER_HEIGHT,
                    playship[nr].xpos,playship[nr].ypos,PLAYER_WIDTH,PLAYER_HEIGHT))
      {
        playship[nr].dead = 1;
        kamikaze = TRUE;
      }

    // Spieler mit Landschaft kollidiert?
    e1 = e2 = e3 = e4 = e5 = e6 = e7 = e8 = FALSE;
    if (getpixel(scr,xp  ,yp-2) > 0) e1 = TRUE;
    if (getpixel(scr,xp-3,yp-2) > 0) e2 = TRUE;
    if (getpixel(scr,xp+3,yp-2) > 0) e3 = TRUE;
    if (getpixel(scr,xp  ,yp-6) > 0) e4 = TRUE;
    if (getpixel(scr,xp+3,yp-3) > 0) e5 = TRUE;
    if (getpixel(scr,xp-3,yp-3) > 0) e6 = TRUE;
    if (getpixel(scr,xp-4,yp+2) > 0) e7 = TRUE;
    if (getpixel(scr,xp+4,yp+2) > 0) e8 = TRUE;

    if (e1 || e2 || e3 || e4 || e5 || e6)
    {
      // Auf Kollision mit Gegnern checken (Kamikaze!)
      for (nr=0; nr < enemy_anz; nr++)
        if (collision(playship[c].xpos,playship[c].ypos,PLAYER_WIDTH,PLAYER_HEIGHT,
                      enemy[nr].xpos,
                      enemy[nr].ypos,
                      enemy[nr].image.width,
                      enemy[nr].image.height) )
        {
          enemy[nr].hit += KAMIKAZE_HITS;
          upanel_bar_hit = enemy[nr].hit;
          upanel_bar_maxhit = enemy[nr].maxhit;
          update_upanel_bar();
          kamikaze = TRUE;

          switch (enemy[nr].type)
          {
            case GSTONE_B1 : case GSTONE_S1 : case GSTONE_S2 :
              hitwall = TRUE;
              break;
          }
        }

       // Auf Gruenflaeche landen?
       if (!kamikaze)
       {
         if ( ((playship[c].head > 360-MAX_LANDING_ANGLE) || (playship[c].head < MAX_LANDING_ANGLE)) &&
              (abs(playship[c].xspd) < 1) && (playship[c].yspd > -1.0) && e7 && e8 && !(e4 || e5 || e6))
         {
           playship[c].yspd = 0; playship[c].xspd = 0;
           playship[c].land = TRUE;
         }
         else
         if (playship[c].pshield_active && playship[c].pshield > 0 && !playship[c].land)
         {
           if ( sqrt((pow(playship[c].xspd,2)+pow(playship[c].yspd,2))) < 0.8)
           {
             if (!e1 && !e4)
             {
               playship[c].yspd = -playship[c].yspd*1.5;
               playship[c].xspd = -playship[c].xspd;
             }
             else
               playship[c].yspd = -playship[c].yspd*1.5;
           }
           else if (sqrt((pow(playship[c].xspd,2)+pow(playship[c].yspd,2))) < 10)
           {
//             playship[c].xpos -= playship[c].xspd;
//             playship[c].ypos += playship[c].yspd;
  
             if (!e1 && !e4)
             {
               playship[c].yspd = -playship[c].yspd;
               playship[c].xspd = -playship[c].xspd;
             }
             else
               playship[c].yspd = -playship[c].yspd;
  
             playship[c].pshield--;
             playship[c].onehit = TRUE;
//             if (playship[c].shield > 0) add_explosion(c,effect_shield_1,3,6,0,playship[c].xpos-6,playship[c].ypos-5);
             play_sound_sample(snd_bonk,1000,0,playship[c].xpos,playship[c].ypos,100);
             update_panel_shield();
           }
           else
           {
             playship[c].dead = 1;
             hitwall = TRUE;
           }
         } // pshield?
         else
         {
           playship[c].dead = 1;
           hitwall = TRUE;
         }
       }
    }

   if (kamikaze) playship[c].dead = 1;

//    if (playship[c].dead)
//      add_explosion(c,expl_big_1,5,2,0,playship[c].xpos-2,playship[c].ypos-2);
    if (!kamikaze && playship[c].dead) add_explosion(c,expl_big_1,5,2,0,playship[c].xpos-2,playship[c].ypos-2);

    if (playship[c].land && (!e7 || !e8))
      playship[c].land = 0;

    xp = playship[c].xpos-map_x[1];
    yp = playship[c].ypos + PLAYER_HEIGHT-map_y[1];

    // Checken, ob Spieler gelandet ist
    if (!playship[c].dead)
    for (nr=0; nr < base_anz; nr++)
    {
      if ( (yp == base[nr].y-map_y[1]) && (xp >= base[nr].x1-map_x[1]) && (xp+PLAYER_WIDTH <= base[nr].x2-map_x[1]) )
      {
        // Checken, ob Anflugwinkel korrekt ist
        if ( ((playship[c].head > 360-MAX_LANDING_ANGLE) || (playship[c].head < MAX_LANDING_ANGLE)) &&
             (playship[c].yspd > MAX_LANDING_SPEED) )
        {
          // Wenn XSPD/YSPD zu gross, Schiff durch Huepfen auf 0 bringen
          if ((playship[c].xspd < -0.02) || (playship[c].xspd > 0.02) || (playship[c].yspd < -0.1))
          {
            play_sound_sample(snd_land,1000,0,playship[c].xpos,playship[c].ypos,100);
            playship[c].head = 0;
            playship[c].yspd = -playship[c].yspd + (playship[c].yspd/2);
            if (playship[c].xspd > 0)
            {
              if (playship[c].xspd > 0.05) playship[c].xspd -= 0.05;
                else playship[c].xspd = 0;
            }
            else if (playship[c].xspd < 0)
            {
              if (playship[c].xspd < -0.05) playship[c].xspd += 0.05;
                else playship[c].xspd = 0;
            }
          } // xspd <> 0
          else
          {
            playship[c].land = TRUE;
            playship[c].ldur++;
            playship[c].base = nr;
            playship[c].ypos = base[nr].y-PLAYER_HEIGHT;
            playship[c].head = 0;
            playship[c].xspd = 0;
            playship[c].yspd = 0;

            if (playship[c].ldur % REFUEL_TIME == 0 && playship[c].base == playship[c].START_BASE)
            {
              if (playship[c].fuel < 99) playship[c].fuel++;
            }

            // Einladen
            if ((base[nr].cargo > 0) && (playship[c].ldur > 20))
            {
              playship[c].ldur = 0;
              mid = (base[nr].x1-map_x[1]) + ((base[nr].x2 - base[nr].x1) / 2);
              xp = (playship[c].xpos-map_x[1]) + (PLAYER_WIDTH / 2);
              if ( (xp > mid-10) && (xp < mid+10) )
              {
                play_sound_sample(snd_cargoin,1000,0,playship[c].xpos,playship[c].ypos,100);
                abort_start_message = FALSE;
                switch (base[nr].cargo)
                {
                  case FUEL       : playship[c].fuel = MAX_FUEL;
                                    base[nr].cargo = 0;
                                    show_normal_osd_message(impact14h,impact10h,cargo_message[MSG_CARGO_FUEL].text1,"",3,col_white,col_yellow,0);
                                    break;
                  case LIFE       : playship[c].lifes++;
                                    base[nr].cargo = 0;
                                    show_normal_osd_message(impact14h,impact10h,cargo_message[MSG_CARGO_LIFE].text1,cargo_message[MSG_CARGO_LIFE].text2,3,col_white,col_yellow,0);
                                    update_panel_lifes();
                                    break;
                  case CMAN1      : if (playship[c].wght + CARGO_WEIGHT <= MAX_SHIP_WEIGHT)
                                    {
                                      playship[c].wght += CARGO_WEIGHT;
                                      base[nr].cargo = 0;
                                    }
                                    show_normal_osd_message(impact14h,impact10h,cargo_message[MSG_CARGO_MAN].text1,cargo_message[MSG_CARGO_MAN].text2,3,col_white,col_yellow,0);
                                    update_panel_cargo();
                                    break;
                  case CSHIELD    : base[nr].cargo = 0;
                                    if (!playship[c].shield_active)
                                    {
                                      playship[c].shield_active = TRUE;
                                      show_normal_osd_message(impact14h,impact10h,cargo_message[MSG_CARGO_SHIELD1].text1,cargo_message[MSG_CARGO_SHIELD1].text2,3,col_white,col_yellow,0);
                                    }
                                    else
                                    {
                                      playship[c].shield = MAX_SHIELD;
                                      show_normal_osd_message(impact14h,impact10h,cargo_message[MSG_CARGO_SHIELD2].text1,cargo_message[MSG_CARGO_SHIELD2].text2,3,col_white,col_yellow,0);
                                    }
                                    update_panel_shield();
                                    break;
                  case CBOMBS10   : base[nr].cargo = 0;
                                    if (base[nr].anz)
                                    {
                                      playship[c].weapon.bullets[W_BOMB]+=base[nr].anz;
                                      mitoa(tmpt,base[nr].anz);
                                      strcat(tmpt," ");
                                      strcat(tmpt,cargo_message[MSG_CARGO_BOMBS].text1);
                                    }
                                    else
                                    {
                                      playship[c].weapon.bullets[W_BOMB]+=20;
                                      strcpy(tmpt,"20 BOMBS!");
                                    }
                                    show_normal_osd_message(impact14h,impact10h,tmpt,"",3,col_white,col_yellow,0);
                                    update_panel_weapons();
                                    break;
                  case CBOMBS20   : base[nr].cargo = 0;
                                    if (base[nr].anz)
                                    {
                                      playship[c].weapon.bullets[W_BOMB]+=base[nr].anz;
                                      mitoa(tmpt,base[nr].anz);
                                      strcat(tmpt," ");
                                      strcat(tmpt,cargo_message[MSG_CARGO_BOMBS].text1);
                                    }
                                    else
                                    {
                                      playship[c].weapon.bullets[W_BOMB]+=20;
                                      strcpy(tmpt,"20 BOMBS!");
                                    }
                                    show_normal_osd_message(impact14h,impact10h,tmpt,"",3,col_white,col_yellow,0);
                                    update_panel_weapons();
                                    break;
                  case CBIGBOMB1  : base[nr].cargo = 0;
                                    if (base[nr].anz)
                                    {
                                      playship[c].weapon.bullets[W_BIGBOMB]+=base[nr].anz;
                                      mitoa(tmpt,base[nr].anz);
                                      strcat(tmpt," ");
                                      strcat(tmpt,cargo_message[MSG_CARGO_BIGBOMBS].text1);
                                    }
                                    else
                                    {
                                      playship[c].weapon.bullets[W_BIGBOMB]+=1;
                                      strcpy(tmpt,"1 BIGBOMB!");
                                    }
                                    show_normal_osd_message(impact14h,impact10h,tmpt,cargo_message[MSG_CARGO_BIGBOMBS].text2,3,col_white,col_yellow,0);
                                    update_panel_weapons();
                                    break;
                  case CBIGBOMB3  : base[nr].cargo = 0;
                                    if (base[nr].anz)
                                    {
                                      playship[c].weapon.bullets[W_BIGBOMB]+=base[nr].anz;
                                      mitoa(tmpt,base[nr].anz);
                                      strcat(tmpt," ");
                                      strcat(tmpt,cargo_message[MSG_CARGO_BIGBOMBS].text1);
                                    }
                                    else
                                    {
                                      playship[c].weapon.bullets[W_BIGBOMB]+=3;
                                      strcpy(tmpt,"3 BIGBOMBS!");
                                    }
                                    show_normal_osd_message(impact14h,impact10h,tmpt,cargo_message[MSG_CARGO_BIGBOMBS].text2,3,col_white,col_yellow,0);
                                    update_panel_weapons();
                                    break;
                  case CMGBULLTS  : base[nr].cargo = 0;
                                    if (base[nr].anz)
                                    {
                                      playship[c].weapon.bullets[W_MG]+=base[nr].anz;
                                      mitoa(tmpt,base[nr].anz);
                                      strcat(tmpt," ");
                                      strcat(tmpt,cargo_message[MSG_CARGO_BULLETS].text2);
                                    }
                                    else
                                    {
                                      playship[c].weapon.bullets[W_MG]+=200;
                                      strcpy(tmpt,"200 BULLETS");
                                    }
                                    show_normal_osd_message(impact14h,impact10h,cargo_message[MSG_CARGO_BULLETS].text1,tmpt,3,col_white,col_yellow,0);
                                    update_panel_weapons();
                                    break;
                  case CROCKETS   : base[nr].cargo = 0;
                                    if (base[nr].anz)
                                    {
                                      playship[c].weapon.bullets[W_ROCKET]+=base[nr].anz;
                                      mitoa(tmpt,base[nr].anz);
                                      strcat(tmpt," ");
                                      strcat(tmpt,cargo_message[MSG_CARGO_MISSILES].text1);
                                    }
                                    show_normal_osd_message(impact14h,impact10h,tmpt,cargo_message[MSG_CARGO_MISSILES].text2,3,col_white,col_yellow,0);
                                    update_panel_weapons();
                                    break;
                  case CMINES1    :
                                    if (base[nr].anz && (playship[c].weapon.type2 == W_MINE1 ||
                                        playship[c].weapon.bullets[W_MINES] == 0))
                                    {
                                      base[nr].cargo = 0;
                                      playship[c].weapon.bullets[W_MINES]+=base[nr].anz;
                                      playship[c].weapon.type2 = W_MINE1;
                                      mitoa(tmpt,base[nr].anz);
                                      strcat(tmpt," ");
                                      strcat(tmpt,cargo_message[MSG_CARGO_MINES1].text1);
                                      show_normal_osd_message(impact14h,impact10h,tmpt,cargo_message[MSG_CARGO_MINES1].text2,3,col_white,col_yellow,0);
                                    }
                                    else
                                    {
                                      show_normal_osd_message(impact14h,impact10h,cargo_message[MSG_CARGO_SLOT6NA].text1,cargo_message[MSG_CARGO_SLOT6NA].text2,3,col_white,col_yellow,0);
                                    }
                                    update_panel_weapons();
                                    break;
                  case CMINES2    :
                                    if (base[nr].anz && (playship[c].weapon.type2 == W_MINE2 ||
                                        playship[c].weapon.bullets[W_MINES] == 0))
                                    {
                                      base[nr].cargo = 0;
                                      playship[c].weapon.bullets[W_MINES]+=base[nr].anz;
                                      playship[c].weapon.type2 = W_MINE2;
                                      mitoa(tmpt,base[nr].anz);
                                      strcat(tmpt," ");
                                      strcat(tmpt,cargo_message[MSG_CARGO_MINES2].text1);
                                      show_normal_osd_message(impact14h,impact10h,tmpt,cargo_message[MSG_CARGO_MINES2].text2,3,col_white,col_yellow,0);
                                    }
                                    else
                                    {
                                      show_normal_osd_message(impact14h,impact10h,cargo_message[MSG_CARGO_SLOT6NA].text1,cargo_message[MSG_CARGO_SLOT6NA].text2,3,col_white,col_yellow,0);
                                    }
                                    update_panel_weapons();
                                    break;
                  case CMINES3    :
                                    if (base[nr].anz && (playship[c].weapon.type2 == W_MINE3 ||
                                        playship[c].weapon.bullets[W_MINES] == 0))
                                    {
                                      base[nr].cargo = 0;
                                      playship[c].weapon.bullets[W_MINES]+=base[nr].anz;
                                      playship[c].weapon.type2 = W_MINE3;
                                      mitoa(tmpt,base[nr].anz);
                                      strcat(tmpt," ");
                                      strcat(tmpt,cargo_message[MSG_CARGO_MINES3].text1);
                                      show_normal_osd_message(impact14h,impact10h,tmpt,cargo_message[MSG_CARGO_MINES3].text2,3,col_white,col_yellow,0);
                                    }
                                    else
                                    {
                                      show_normal_osd_message(impact14h,impact10h,cargo_message[MSG_CARGO_SLOT6NA].text1,cargo_message[MSG_CARGO_SLOT6NA].text2,3,col_white,col_yellow,0);
                                    }
                                    update_panel_weapons();
                                    break;
                  default         : if ((float)(playship[c].wght + base[nr].cargo * CARGO_WEIGHT) <= MAX_SHIP_WEIGHT)
                                    {
                                      (float)playship[c].wght += base[nr].cargo * CARGO_WEIGHT;
                                      strcpy(tmpt,cargo_message[MSG_CARGO_CARGO1].text1);
                                      strcat(tmpt," (");
                                      sprintf(tmpt2,"%.0f",(float)(base[nr].cargo*(CARGO_WEIGHT*1000.0)));
                                      strcat(tmpt,tmpt2);
                                      strcat(tmpt,")");
                                      show_normal_osd_message(impact14h,impact10h,tmpt,cargo_message[MSG_CARGO_CARGO1].text2,3,col_white,col_yellow,0);
                                      base[nr].cargo = 0;
                                      global_active--;
                                      update_panel_cargo();
                                    }
                                    else
                                    {
                                      sprintf(tmpt,"%s (%.0f)", cargo_message[MSG_CARGO_CARGO2].text1, (float)(base[nr].cargo*(CARGO_WEIGHT*1000.0)));
                                      show_normal_osd_message(impact14h,impact10h,tmpt,cargo_message[MSG_CARGO_CARGO2].text2,3,col_white,col_orange,0);
                                    }
                 } // switch
              }
            } // if cargo

            // Ausladen
            if ((base[nr].type == PLAYER_BASE) && (playship[c].wght != 0) && (playship[c].ldur > 50))
            {
              playship[c].ldur++;
              mid = (base[nr].x1-map_x[1]) + ((base[nr].x2 - base[nr].x1) / 2);
              xp = (playship[c].xpos-map_x[1]) + (PLAYER_WIDTH / 2);
              if (playship[c].wght > 0)
              {
                if (playship[c].ldur % 100 == 0)
                {
                  playship[c].wght -= CARGO_WEIGHT;
                  global_saved++;
                  global_weight += CARGO_WEIGHT;
                  play_sound_sample(snd_cargoout,1000,0,playship[c].xpos,playship[c].ypos,100);
                } // ldur
                if (!playship[c].hide)
                {
                  strcpy(tmpt,cargo_message[MSG_CARGO_RECEIVED].text2);
//                strcat(tmpt,mitoa((int)((float)global_saved*1000.0)));
                  sprintf(tmpt2,"%i",global_saved*5);
                  strcat(tmpt,tmpt2);
                  strcat(tmpt,cargo_message[MSG_CARGO_RECEIVED].text3);
                  show_normal_osd_message(impact14h,impact10h,cargo_message[MSG_CARGO_RECEIVED].text1,tmpt,3,col_white,col_yellow,0);
                }
                update_panel_cargo();
              } // if wght>0
              else
              {
                playship[c].ldur = 0;
              }
            } // if basetype

            if (playship[c].fuel <= FUEL_DEC)
            {
              if (playship[c].ldur == 100) show_normal_osd_message(impact10h,impact14h,other_message[MSG_OTHER_NOFUEL].text1,other_message[MSG_OTHER_NOFUEL].text2,15,col_yellow,col_white,1);
              if (playship[c].ldur == 180) show_typewriter_osd_message(impact10h,other_message[MSG_OTHER_NOFUEL].text3,90,4,col_orange,45);
              if ((playship[c].ldur > 130) && (!message)) { playship[c].dead = TRUE; }
            } // if no fuel

          } // wenn gelandet
        } // if aufgesetzt
        else
        {
          // Tot
          playship[c].dead = 1;
        }
      } // if base
    } // for base
  } // if !dead

  // Player-Deads erhoehen
  if (playship[c].dead)
  {
    if (playship[c].dead == 1)
    {
      global_weight += playship[c].wght;
      player_stat[c].cargo_lost += playship[c].wght;
      stop_sound_sample(snd_thrusters);
      play_sound_sample(snd_exp_mid_1,1000,0,playship[c].xpos,playship[c].ypos,100);
      add_pixels(400,playship[c].xpos+PLAYER_WIDTH/2,playship[c].ypos+PLAYER_HEIGHT/20,0,0,25,tbl_all,256,0,0,0,0);
      if (playship[c].lifes > 0) playship[c].lifes--;
      player_dead_counter = 0;

      if (use_extras == 1 || (use_extras > 0 && rand() % use_extras == 1))
        if (!hitwall && !kamikaze) lay_extra(c);

      if (hitwall) { player_stat[c].suicides++; }
    }

    if (playship[c].dead < playship[c].maxdead) playship[c].dead++;

      if (player_dead_counter < NEW_SHIP_TIME) player_dead_counter++;
      else if (player_dead_counter == NEW_SHIP_TIME)
      {
        if (playship[c].lifes && !level_end && game_mode != MP_2PDOGFIGHT)
        {
          p_fade_out_active = TRUE;
          p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 60; p_fade_speed = 1;
        }
        else if (!level_end && playship[c].lifes <= 0)
        {
          p_fade_out_active = TRUE;
          p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 250; p_fade_speed = 0.25;
        }
        player_dead_counter++;
      }
      else if ((p_fade_count == p_fade_count_to) && (player_dead_counter > NEW_SHIP_TIME))
      {
        if (playship[c].lifes > 0)
        {
          clear(playship[c].player_temp);
          redraw = TRUE;
          init_ship(c);
          if (game_mode != MP_2PDOGFIGHT)
          {
            scroll_x[1] = scroll_y[1] = map_x[1] = map_y[1] = 0;
            map_stuff(0);
            MapDraw8BG(scr,map_x[1],map_y[1],0,0,PLAYSCREEN_WIDTH+PLUS_X,PLAYSCREEN_HEIGHT+PLUS_Y);
            make_base_room(vscreen,0);
          }
          else
          {
//            scroll_to(l_scroll_x[cur_l],l_scroll_y[cur_l],1000);
//            do_scroll();
          }
          update_graphics = TRUE;
          if (!playship[c].dead) blit(scr,playship[c].player_temp,playship[c].xpos-5-map_x[1],playship[c].ypos-2-map_y[1],0,0,19,18);
          draw_player(vscreen,c);
          rl.count = 0; // Clear-List loeschen
          p_fade_out_active = TRUE;
          p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 2; p_fade_count = 0; p_fade_count_to = 60; p_fade_speed = 1;

          player_dead_counter = 0;
          draw_bases(vscreen,0);
//          make_base_room(vscreen,0);
//          check_collisions(vscreen,0);
        }
      }
  } // if dead

}


void lay_extra(int c)
{
  int nr=-1;

  // search free slot
  while (nr < spobject_anz && !(spobject[nr].type==EXTRA1 && spobject[nr].active==FALSE)) nr++;

  if (nr >= 0)
  {
    spobject[nr].type = EXTRA1;
    spobject[nr].xpos = playship[c].xpos+1; spobject[nr].ypos = playship[c].ypos+2;
    spobject[nr].xspd = 0; spobject[nr].yspd = 0;
    spobject[nr].xacc = 0.02; spobject[nr].yacc = 0.02;
    spobject[nr].startframe = 0; spobject[nr].maxframe = 2; spobject[nr].maxframet = 5;
    spobject[nr].image.data = obj_extra1_1;
    spobject[nr].image.width = 11; spobject[nr].image.height = 11;
    spobject[nr].active = TRUE;
  }
}
