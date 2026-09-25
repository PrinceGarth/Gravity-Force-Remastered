/*
 *    GRAVITY FORCE, gfmes.c
 *
 *    -- Parsing of "language files" and some other message related stuff
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfmes.h"
#include "gfosd.h"
#include "gfsound.h"
#include "gfinit.h"
#include "gflog.h"

//#include <ctype.h>

void jhtextout(BITMAP *bmp, FONT *f, unsigned char *s, int x, int y, int color)
{

#ifdef ALLEGRO_DOS
  aatextout(bmp,f,s,x,y,color);
#else
  FONT *newfont;

  newfont = f;

  if ((FONT *)f == (FONT *)impact14h)
    newfont = impact14hm;
  else if ((FONT *)f == (FONT *)impact10h)
    newfont = impact10hm;
  else if ((FONT *)f == (FONT *)arial14h)
    newfont = arial14hm;

  textout(bmp,newfont,s,x,y,color);
#endif

}

void read_message_file(int language_setting)
{
  char tmp[250];
  PACKFILE *f;
  char *tok;
  int line=0,pos=0;
  int mode=0,number=0,ok=FALSE;

//  strcpy(tmp_dir,lng_dir);
//  f = fopen((char*)strcat(tmp_dir,"gf.lng"),"rt");

  memset(error_message,0,sizeof(error_message)); memset(cargo_message,0,sizeof(cargo_message));
  memset(mission_message,0,sizeof(mission_message)); memset(other_message,0,sizeof(other_message));
  memset(menu_message,0,sizeof(menu_message)); memset(stat_message,0,sizeof(stat_message));
  packfile_password("tanja");
  sprintf(tmp_dir,"%s%s#%s",dat_dir,"gflng.dat","gf.lng");
  f = pack_fopen(tmp_dir,"r");

  if (!f) { error_exit(42,"Error reading general language file","gf.lng",42,0); }

  while (!pack_feof(f))
  {
    // read line from file
    pack_fgets(tmp,250,f);

    // kill last character (newline)
//    if (strlen(tmp) > 1)
//      tmp[strlen(tmp)-1] = 0;

    // correct language section?
    switch (language_setting)
    {
      case 0  : if (strstr(tmp,"%%english")) { if (!ok) ok = TRUE; }
                else if (strstr(tmp,"%%")) { if (ok) ok = FALSE; }
                break;
      case 1  : if (strstr(tmp,"%%german")) { if (!ok) ok = TRUE; }
                else if (strstr(tmp,"%%")) { if (ok) ok = FALSE; }
                break;
    }

    if (strstr(tmp,"%errors")) { line = 0; mode = 1; }
    else if (strstr(tmp,"%cargo")) { line = 0; mode = 2; }
    else if (strstr(tmp,"%mission")) { line = 0; mode = 3; }
    else if (strstr(tmp,"%other")) { line = 0; mode = 4; }
    else if (strstr(tmp,"%menu")) { line = 0; mode = 5; }
    else if (strstr(tmp,"%stat")) { line = 0; mode = 6; }

    // Zeile auswerten
    if ( (tmp[0]=='*') && ok && mode)
    {
       pos = 0;
       for (tok = (char *)strtok(tmp,";"); tok; tok = (char *)strtok(0,";"))
       {
         if (pos==0)
         {
           number = -1;
           switch (mode)
           {
             case 1  :
                       if (strstr(tok,"*error_graphic_init")) number = MSG_ERROR_GRAPHIC_INIT;
                       else if (strstr(tok,"*error_sound_init2")) number = MSG_ERROR_SOUND_INIT2;
                       else if (strstr(tok,"*error_sound_init")) number = MSG_ERROR_SOUND_INIT;
                       else if (strstr(tok,"*error_filenotfound")) number = MSG_ERROR_FILE_NOT_FOUND;
                       else if (strstr(tok,"*error_gfxfile")) number = MSG_ERROR_GFXFILE;
                       else if (strstr(tok,"*error_lngfile")) number = MSG_ERROR_LNGFILE;
                       else if (strstr(tok,"*error_cfgfile")) number = MSG_ERROR_CFGFILE;
                       else if (strstr(tok,"*error_mapfile")) number = MSG_ERROR_MAPFILE;
                       else if (strstr(tok,"*error_error")) number = MSG_ERROR_ERROR;
                       break;
             case 2  :
                       if (strstr(tok,"*cargo_fuel")) number = MSG_CARGO_FUEL;
                       else if (strstr(tok,"*cargo_life")) number = MSG_CARGO_LIFE;
                       else if (strstr(tok,"*cargo_man")) number = MSG_CARGO_MAN;
                       else if (strstr(tok,"*cargo_shield1")) number = MSG_CARGO_SHIELD1;
                       else if (strstr(tok,"*cargo_shield2")) number = MSG_CARGO_SHIELD2;
                       else if (strstr(tok,"*cargo_bombs")) number = MSG_CARGO_BOMBS;
                       else if (strstr(tok,"*cargo_bigbombs")) number = MSG_CARGO_BIGBOMBS;
                       else if (strstr(tok,"*cargo_bullets")) number = MSG_CARGO_BULLETS;
                       else if (strstr(tok,"*cargo_missiles")) number = MSG_CARGO_MISSILES;
                       else if (strstr(tok,"*cargo_cargo1")) number = MSG_CARGO_CARGO1;
                       else if (strstr(tok,"*cargo_cargo2")) number = MSG_CARGO_CARGO2;
                       else if (strstr(tok,"*cargo_received")) number = MSG_CARGO_RECEIVED;
                       else if (strstr(tok,"*cargo_left")) number = MSG_CARGO_LEFT;
                       else if (strstr(tok,"*cargo_mines1")) number = MSG_CARGO_MINES1;
                       else if (strstr(tok,"*cargo_mines2")) number = MSG_CARGO_MINES2;
                       else if (strstr(tok,"*cargo_mines3")) number = MSG_CARGO_MINES3;
                       else if (strstr(tok,"*cargo_slot6na")) number = MSG_CARGO_SLOT6NA;
                       break;
             case 3  :
                       if (strstr(tok,"*mission_gameover")) number = MSG_MISSION_GAMEOVER;
                       else if (strstr(tok,"*mission_complete")) number = MSG_MISSION_COMPLETE;
                       else if (strstr(tok,"*mission_failed")) number = MSG_MISSION_FAILED;
                       else if (strstr(tok,"*mission_race_complete")) number = MSG_MISSION_RACE_COMPLETE;
                       break;
             case 4  :
                       if (strstr(tok,"*other_nofuel")) number = MSG_OTHER_NOFUEL;
                       else if (strstr(tok,"*other_bombingrun")) number = MSG_OTHER_BOMBINGRUN;
                       else if (strstr(tok,"*other_remtime")) number = MSG_OTHER_REMTIME;
                       else if (strstr(tok,"*other_time")) number = MSG_OTHER_TIME;
                       else if (strstr(tok,"*other_lap")) number = MSG_OTHER_LAP;
                       else if (strstr(tok,"*other_mega_autofire")) number = MSG_OTHER_MEGA_AUTOFIRE;
                       else if (strstr(tok,"*other_mega_shield")) number = MSG_OTHER_MEGA_SHIELD;
                       else if (strstr(tok,"*other_minutes")) number = MSG_OTHER_MINUTES;
                       else if (strstr(tok,"*other_seconds")) number = MSG_OTHER_SECONDS;
                       break;
             case 5  :
                       if (strstr(tok,"*menu_yes")) number = MSG_MENU_YES;
                       else if (strstr(tok,"*menu_singleplayer")) number = MSG_MENU_SINGLEPLAYER;
                       else if (strstr(tok,"*menu_multiplayer")) number = MSG_MENU_MULTIPLAYER;
                       else if (strstr(tok,"*menu_campaign")) number = MSG_MENU_CAMPAIGN;
                       else if (strstr(tok,"*menu_qdogfight")) number = MSG_MENU_QDOGFIGHT;
                       else if (strstr(tok,"*menu_mprace")) number = MSG_MENU_MPRACE;
                       else if (strstr(tok,"*menu_dogfight")) number = MSG_MENU_DOGFIGHT;
                       else if (strstr(tok,"*menu_race")) number = MSG_MENU_RACE;
                       else if (strstr(tok,"*menu_training")) number = MSG_MENU_TRAINING;
                       else if (strstr(tok,"*menu_options")) number = MSG_MENU_OPTIONS;
                       else if (strstr(tok,"*menu_playdemo")) number = MSG_MENU_PLAYDEMO;
                       else if (strstr(tok,"*menu_quit")) number = MSG_MENU_QUIT;
                       else if (strstr(tok,"*menu_level")) number = MSG_MENU_LEVEL;
                       else if (strstr(tok,"*menu_playlevel")) number = MSG_MENU_PLAYLEVEL;
                       else if (strstr(tok,"*menu_unlocklevel")) number = MSG_MENU_UNLOCKLEVEL;
                       else if (strstr(tok,"*menu_play")) number = MSG_MENU_PLAY;
                       else if (strstr(tok,"*menu_password")) number = MSG_MENU_PASSWORD;
                       else if (strstr(tok,"*menu_back")) number = MSG_MENU_BACK;
                       else if (strstr(tok,"*menu_recdemo")) number = MSG_MENU_RECDEMO;
                       else if (strstr(tok,"*menu_score")) number = MSG_MENU_SCORE;
                       else if (strstr(tok,"*menu_name")) number = MSG_MENU_NAME;
                       else if (strstr(tok,"*menu_filename")) number = MSG_MENU_FILENAME;
                       else if (strstr(tok,"*menu_version")) number = MSG_MENU_VERSION;
                       else if (strstr(tok,"*menu_opt_pixanz_desc")) number = MSG_MENU_OPT_PIXANZ_DESC;
                       else if (strstr(tok,"*menu_opt_pixanz")) number = MSG_MENU_OPT_PIXANZ;
                       else if (strstr(tok,"*menu_opt_soundvol_desc")) number = MSG_MENU_OPT_SOUNDVOL_DESC;
                       else if (strstr(tok,"*menu_opt_soundvol")) number = MSG_MENU_OPT_SOUNDVOL;
                       else if (strstr(tok,"*menu_opt_resolution")) number = MSG_MENU_OPT_RESOLUTION;
                       else if (strstr(tok,"*menu_opt_language_desc")) number = MSG_MENU_OPT_LANGUAGE_DESC;
                       else if (strstr(tok,"*menu_opt_language")) number = MSG_MENU_OPT_LANGUAGE;
                       else if (strstr(tok,"*menu_opt_german")) number = MSG_MENU_OPT_GERMAN;
                       else if (strstr(tok,"*menu_opt_english")) number = MSG_MENU_OPT_ENGLISH;
                       else if (strstr(tok,"*menu_opt_mmusic_desc")) number = MSG_MENU_OPT_MMUSIC_DESC;
                       else if (strstr(tok,"*menu_opt_mmusic")) number = MSG_MENU_OPT_MMUSIC;
                       else if (strstr(tok,"*menu_opt_hateit")) number = MSG_MENU_OPT_HATEIT;
                       else if (strstr(tok,"*menu_player")) number = MSG_MENU_PLAYER;
                       else if (strstr(tok,"*menu_ok")) number = MSG_MENU_OK;
                       else if (strstr(tok,"*menu_select")) number = MSG_MENU_SELECT;
                       else if (strstr(tok,"*menu_enter_password")) number = MSG_MENU_ENTER_PASSWORD;
                       else if (strstr(tok,"*menu_opt_zoom_desc")) number = MSG_MENU_OPT_ZOOM_DESC;
                       else if (strstr(tok,"*menu_opt_zoom")) number = MSG_MENU_OPT_ZOOM;
                       else if (strstr(tok,"*menu_opt_debris_desc")) number = MSG_MENU_OPT_DEBRIS_DESC;
                       else if (strstr(tok,"*menu_opt_debris")) number = MSG_MENU_OPT_DEBRIS;
                       else if (strstr(tok,"*menu_opt_pgrav_desc")) number = MSG_MENU_OPT_PGRAV_DESC;
                       else if (strstr(tok,"*menu_opt_pgrav")) number = MSG_MENU_OPT_PGRAV;
                       else if (strstr(tok,"*menu_opt_zoom")) number = MSG_MENU_OPT_ZOOM;
                       else if (strstr(tok,"*menu_opt_on")) number = MSG_MENU_OPT_ON;
                       else if (strstr(tok,"*menu_opt_off")) number = MSG_MENU_OPT_OFF;
                       else if (strstr(tok,"*menu_frag_limit")) number = MSG_MENU_FRAG_LIMIT;
                       else if (strstr(tok,"*menu_time_limit")) number = MSG_MENU_TIME_LIMIT;
                       else if (strstr(tok,"*menu_gravity")) number = MSG_MENU_GRAVITY;
                       else if (strstr(tok,"*menu_p1weapons")) number = MSG_MENU_P1WEAPONS;
                       else if (strstr(tok,"*menu_p2weapons")) number = MSG_MENU_P2WEAPONS;
                       else if (strstr(tok,"*menu_extras")) number = MSG_MENU_EXTRAS;
                       else if (strstr(tok,"*menu_rare")) number = MSG_MENU_RARE;
                       else if (strstr(tok,"*menu_medium")) number = MSG_MENU_MEDIUM;
                       else if (strstr(tok,"*menu_much")) number = MSG_MENU_MUCH;
                       else if (strstr(tok,"*menu_none")) number = MSG_MENU_NONE;
                       else if (strstr(tok,"*menu_no")) number = MSG_MENU_NO;

                       else if (strstr(tok,"*menu_key_controls")) number = MSG_KEY_CONTROLS;
                       else if (strstr(tok,"*menu_key_2player")) number = MSG_KEY_2PLAYER;
                       else if (strstr(tok,"*menu_key_thrust")) number = MSG_KEY_THRUST;
                       else if (strstr(tok,"*menu_key_shoot")) number = MSG_KEY_SHOOT;
                       else if (strstr(tok,"*menu_key_left")) number = MSG_KEY_LEFT;
                       else if (strstr(tok,"*menu_key_right")) number = MSG_KEY_RIGHT;
                       else if (strstr(tok,"*menu_key_special")) number = MSG_KEY_SPECIAL;
                       else if (strstr(tok,"*menu_key_nweapon")) number = MSG_KEY_NWEAPON;
                       else if (strstr(tok,"*menu_key_pweapon")) number = MSG_KEY_PWEAPON;
                       else if (strstr(tok,"*menu_key_w1")) number = MSG_KEY_W1;
                       else if (strstr(tok,"*menu_key_w2")) number = MSG_KEY_W2;
                       else if (strstr(tok,"*menu_key_w3")) number = MSG_KEY_W3;
                       else if (strstr(tok,"*menu_key_w4")) number = MSG_KEY_W4;
                       else if (strstr(tok,"*menu_key_w5")) number = MSG_KEY_W5;
                       else if (strstr(tok,"*menu_key_w6")) number = MSG_KEY_W6;

                       else if (strstr(tok,"*menu_key_ESC")) number = MSG_KEY_ESC;
                       else if (strstr(tok,"*menu_key_F10")) number = MSG_KEY_F10;
                       else if (strstr(tok,"*menu_key_F11")) number = MSG_KEY_F11;
                       else if (strstr(tok,"*menu_key_F12")) number = MSG_KEY_F12;
                       else if (strstr(tok,"*menu_key_F1")) number = MSG_KEY_F1;
                       else if (strstr(tok,"*menu_key_F2")) number = MSG_KEY_F2;
                       else if (strstr(tok,"*menu_key_F3")) number = MSG_KEY_F3;
                       else if (strstr(tok,"*menu_key_F4")) number = MSG_KEY_F4;
                       else if (strstr(tok,"*menu_key_F5")) number = MSG_KEY_F5;
                       else if (strstr(tok,"*menu_key_F6")) number = MSG_KEY_F6;
                       else if (strstr(tok,"*menu_key_F7")) number = MSG_KEY_F7;
                       else if (strstr(tok,"*menu_key_F8")) number = MSG_KEY_F8;
                       else if (strstr(tok,"*menu_key_F9")) number = MSG_KEY_F9;
                       else if (strstr(tok,"*menu_key_TAB")) number = MSG_KEY_TAB;
                       else if (strstr(tok,"*menu_key_LSHIFT")) number = MSG_KEY_LSHIFT;
                       else if (strstr(tok,"*menu_key_RSHIFT")) number = MSG_KEY_RSHIFT;
                       else if (strstr(tok,"*menu_key_LCTRL")) number = MSG_KEY_LCTRL;
                       else if (strstr(tok,"*menu_key_RCTRL")) number = MSG_KEY_RCTRL;
                       else if (strstr(tok,"*menu_key_LALT")) number = MSG_KEY_LALT;
                       else if (strstr(tok,"*menu_key_RALT")) number = MSG_KEY_RALT;
                       else if (strstr(tok,"*menu_key_SPACE")) number = MSG_KEY_SPACE;
                       else if (strstr(tok,"*menu_key_BACKSPACE")) number = MSG_KEY_BACKSPACE;
                       else if (strstr(tok,"*menu_key_ARUP")) number = MSG_KEY_ARUP;
                       else if (strstr(tok,"*menu_key_ARDOWN")) number = MSG_KEY_ARDOWN;
                       else if (strstr(tok,"*menu_key_ARLEFT")) number = MSG_KEY_ARLEFT;
                       else if (strstr(tok,"*menu_key_ARRIGHT")) number = MSG_KEY_ARRIGHT;
                       else if (strstr(tok,"*menu_key_INSERT")) number = MSG_KEY_INSERT;
                       else if (strstr(tok,"*menu_key_DELETE")) number = MSG_KEY_DELETE;
                       else if (strstr(tok,"*menu_key_HOME")) number = MSG_KEY_HOME;
                       else if (strstr(tok,"*menu_key_END")) number = MSG_KEY_END;
                       else if (strstr(tok,"*menu_key_PGUP")) number = MSG_KEY_PGUP;
                       else if (strstr(tok,"*menu_key_PGDOWN")) number = MSG_KEY_PGDOWN;
                       else if (strstr(tok,"*menu_key_PAUSE")) number = MSG_KEY_PAUSE;
                       else if (strstr(tok,"*menu_key_MINUS_PAD")) number = MSG_KEY_MINUS_PAD;
                       else if (strstr(tok,"*menu_key_PLUS_PAD")) number = MSG_KEY_PLUS_PAD;
                       else if (strstr(tok,"*menu_key_DEL_PAD")) number = MSG_KEY_DEL_PAD;
                       else if (strstr(tok,"*menu_key_ENTER_PAD")) number = MSG_KEY_ENTER_PAD;
                       else if (strstr(tok,"*menu_key_ENTER")) number = MSG_KEY_ENTER;
                       else if (strstr(tok,"*menu_key_TILDE")) number = MSG_KEY_TILDE;
                       else if (strstr(tok,"*menu_key_MINUS")) number = MSG_KEY_MINUS;
                       else if (strstr(tok,"*menu_key_EQUALS")) number = MSG_KEY_EQUALS;
                       else if (strstr(tok,"*menu_key_OPENBRACE")) number = MSG_KEY_OPENBRACE;
                       else if (strstr(tok,"*menu_key_CLOSEBRACE")) number = MSG_KEY_CLOSEBRACE;
                       else if (strstr(tok,"*menu_key_COLON")) number = MSG_KEY_COLON;
                       else if (strstr(tok,"*menu_key_QUOTE")) number = MSG_KEY_QUOTE;
                       else if (strstr(tok,"*menu_key_BACKSLASH2")) number = MSG_KEY_BACKSLASH2;
                       else if (strstr(tok,"*menu_key_BACKSLASH")) number = MSG_KEY_BACKSLASH;
                       else if (strstr(tok,"*menu_key_COMMA")) number = MSG_KEY_COMMA;
                       else if (strstr(tok,"*menu_key_STOP")) number = MSG_KEY_STOP;
                       else if (strstr(tok,"*menu_key_SLASH_PAD")) number = MSG_KEY_SLASH_PAD;
                       else if (strstr(tok,"*menu_key_SLASH")) number = MSG_KEY_SLASH;
                       else if (strstr(tok,"*menu_key_ASTERISK")) number = MSG_KEY_ASTERISK;

                       else if (strstr(tok,"*menu_joy0_up")) number = MSG_JOY0_UP;
                       else if (strstr(tok,"*menu_joy0_down")) number = MSG_JOY0_DOWN;
                       else if (strstr(tok,"*menu_joy0_left")) number = MSG_JOY0_LEFT;
                       else if (strstr(tok,"*menu_joy0_right")) number = MSG_JOY0_RIGHT;
                       else if (strstr(tok,"*menu_joy0_b1")) number = MSG_JOY0_B1;
                       else if (strstr(tok,"*menu_joy0_b2")) number = MSG_JOY0_B2;
                       else if (strstr(tok,"*menu_joy0_b3")) number = MSG_JOY0_B3;
                       else if (strstr(tok,"*menu_joy0_b4")) number = MSG_JOY0_B4;
                       else if (strstr(tok,"*menu_joy1_up")) number = MSG_JOY1_UP;
                       else if (strstr(tok,"*menu_joy1_down")) number = MSG_JOY1_DOWN;
                       else if (strstr(tok,"*menu_joy1_left")) number = MSG_JOY1_LEFT;
                       else if (strstr(tok,"*menu_joy1_right")) number = MSG_JOY1_RIGHT;
                       else if (strstr(tok,"*menu_joy1_b1")) number = MSG_JOY1_B1;
                       else if (strstr(tok,"*menu_joy1_b2")) number = MSG_JOY1_B2;
                       else if (strstr(tok,"*menu_joy1_b3")) number = MSG_JOY1_B3;
                       else if (strstr(tok,"*menu_joy1_b4")) number = MSG_JOY1_B4;
                       break;
             case 6  :
                       if (strstr(tok,"*stat_statistics")) number = MSG_STAT_STATISTICS;
                       else if (strstr(tok,"*stat_status")) number = MSG_STAT_STATUS;
                       else if (strstr(tok,"*stat_lifes")) number = MSG_STAT_LIFES;
                       else if (strstr(tok,"*stat_fuel")) number = MSG_STAT_FUEL;
                       else if (strstr(tok,"*stat_killed_quality")) number = MSG_STAT_KILLED_QUALITY;
                       else if (strstr(tok,"*stat_killed")) number = MSG_STAT_KILLED;
                       else if (strstr(tok,"*stat_cargosaved")) number = MSG_STAT_CARGOSAVED;
                       else if (strstr(tok,"*stat_hitpercent")) number = MSG_STAT_HITPERCENT;
                       else if (strstr(tok,"*stat_lap")) number = MSG_STAT_LAP;
                       else if (strstr(tok,"*stat_timeused")) number = MSG_STAT_TIMEUSED;
                       else if (strstr(tok,"*stat_overall_time")) number = MSG_STAT_OVERALL_TIME;
                       else if (strstr(tok,"*stat_score")) number = MSG_STAT_SCORE;
                       else if (strstr(tok,"*stat_hiscore!")) number = MSG_STAT_INHISCORE;
                       else if (strstr(tok,"*stat_hiscorecode")) number = MSG_STAT_HISCORECODE;
                       else if (strstr(tok,"*stat_enter_name")) number = MSG_STAT_ENTER_NAME;
                       else if (strstr(tok,"*stat_press_key")) number = MSG_STAT_PRESS_KEY;
                       else if (strstr(tok,"*stat_fragsmin")) number = MSG_STAT_FRAGSMIN;
                       else if (strstr(tok,"*stat_frags")) number = MSG_STAT_FRAGS;
                       else if (strstr(tok,"*stat_suicides")) number = MSG_STAT_SUICIDES;
                       else if (strstr(tok,"*stat_extrastaken")) number = MSG_STAT_EXTRASTAKEN;
                       else if (strstr(tok,"*stat_timeplayed")) number = MSG_STAT_TIMEPLAYED;
                       else if (strstr(tok,"*stat_time")) number = MSG_STAT_TIME;
                       else if (strstr(tok,"*stat_winner")) number = MSG_STAT_WINNER;
                       else if (strstr(tok,"*stat_player")) number = MSG_STAT_PLAYER;
                       else if (strstr(tok,"*stat_draw")) number = MSG_STAT_DRAW;
                       else if (strstr(tok,"*stat_place")) number = MSG_STAT_PLACE;
                       break;
           } // switch mode
         } // pos==0
         else if (pos > 0 && number >= 0)
         {
           if (strlen(tok) > 0)
           switch (mode)
           {
             case 1  :
                        switch (pos)
                        {
                          case 1  :  //error_message[number].text1 = (char *)malloc(strlen(tok));
                                     strcpy(error_message[number].text1,tok);
                                     break;
                          case 2  :
                                     //error_message[number].text2 = (char *)malloc(strlen(tok));
                                     strcpy(error_message[number].text2,tok);
                                     break;
                          case 3  :
                                     //error_message[number].text3 = (char *)malloc(strlen(tok));
                                     strcpy(error_message[number].text3,tok);
                                     break;
                        } // switch pos
                        break;
             case 2  :
                        switch (pos)
                        {
                          case 1  :  //cargo_message[number].text1 = (char *)malloc(strlen(tok));
                                     strcpy(cargo_message[number].text1,tok);
                                     break;
                          case 2  :  //cargo_message[number].text2 = (char *)malloc(strlen(tok));
                                     strcpy(cargo_message[number].text2,tok);
                                     break;
                          case 3  :  //cargo_message[number].text3 = (char *)malloc(strlen(tok));
                                     strcpy(cargo_message[number].text3,tok);
                                     break;
                        } // switch pos
                        break;
             case 3  :
                        switch (pos)
                        {
                          case 1  :  //mission_message[number].text1 = (char *)malloc(strlen(tok));
                                     strcpy(mission_message[number].text1,tok);
                                     break;
                          case 2  :  //mission_message[number].text2 = (char *)malloc(strlen(tok));
                                     strcpy(mission_message[number].text2,tok);
                                     break;
                          case 3  :  //mission_message[number].text3 = (char *)malloc(strlen(tok));
                                     strcpy(mission_message[number].text3,tok);
                                     break;
                        } // switch pos
                        break;
             case 4  :
                        switch (pos)
                        {
                          case 1  :  //other_message[number].text1 = (char *)malloc(strlen(tok));
                                     strcpy(other_message[number].text1,tok);
                                     break;
                          case 2  :  //other_message[number].text2 = (char *)malloc(strlen(tok));
                                     strcpy(other_message[number].text2,tok);
                                     break;
                          case 3  :  //other_message[number].text3 = (char *)malloc(strlen(tok));
                                     strcpy(other_message[number].text3,tok);
                                     break;
                        } // switch pos
                        break;
             case 5  :
                        switch (pos)
                        {
                          case 1  :  //menu_message[number].text1 = (char *)malloc(strlen(tok));
                                     strcpy(menu_message[number].text1,tok);
                                     break;
                          case 2  :  //menu_message[number].text2 = (char *)malloc(strlen(tok));
                                     strcpy(menu_message[number].text2,tok);
                                     break;
                          case 3  :  //menu_message[number].text3 = (char *)malloc(strlen(tok));
                                     strcpy(menu_message[number].text3,tok);
                                     break;
                        } // switch pos
                        break;
             case 6  :
                        switch (pos)
                        {
                          case 1  :  //stat_message[number].text1 = (char *)malloc(strlen(tok));
                                     strcpy(stat_message[number].text1,tok);
                                     break;
                          case 2  :  //stat_message[number].text2 = (char *)malloc(strlen(tok));
                                     strcpy(stat_message[number].text2,tok);
                                     break;
                          case 3  :  //stat_message[number].text3 = (char *)malloc(strlen(tok));
                                     strcpy(stat_message[number].text3,tok);
                                     break;
                        } // switch pos
                        break;
           } // switch mode
         } // else if pos > 0

         pos++;
       } // for tok

       line++;
    } // if message

  } // while !feof

  pack_fclose(f);
}

void read_level_message_file(int language_setting)
{
  char tmp[250];
  PACKFILE *f;
  char *tok;
  int line=0,pos=0;
  int mode=0,number=0,ok=FALSE;

  strcpy(tmp_dir,lng_dir);
//  f = fopen((char*)strcat(tmp_dir,lng_name),"rt");

  packfile_password("tanja");
  sprintf(tmp_dir,"%s%s#%s",dat_dir,"gflng.dat",lng_name);
  f = pack_fopen(tmp_dir,"r");

  if (!f) { error_exit(47,error_message[MSG_ERROR_LNGFILE].text1,lng_name,47,0); }

  while (!pack_feof(f))
  {
    // read line
    pack_fgets(tmp,250,f);

    // kill last character (newline)
//    if (strlen(tmp) > 1)
//      tmp[strlen(tmp)-1] = 0;

    // correct language section?
    switch (language_setting)
    {
      case 0  : if (strstr(tmp,"%%english")) { if (!ok) ok = TRUE; }
                else if (strstr(tmp,"%%")) { if (ok) ok = FALSE; }
                break;
      case 1  : if (strstr(tmp,"%%german")) { if (!ok) ok = TRUE; }
                else if (strstr(tmp,"%%")) { if (ok) ok = FALSE; }
                break;
    }

    if (strstr(tmp,"%actions")) { line = 0; mode = 1; }
    else if (strstr(tmp,"%misc")) { line = 0; mode = 2; }

    // Zeile auswerten
    if ( (tmp[0]=='*') && ok && mode)
    {
       pos = 0;
       for (tok = (char *)strtok(tmp,";"); tok; tok = (char *)strtok(0,";"))
       {
         if (pos==0)
         {
           switch (mode)
           {
             case 1  :
                       if (strstr(tok,"*action1")) number = MSG_LEVEL_ACTION1;
                       else if (strstr(tok,"*action2")) number = MSG_LEVEL_ACTION2;
                       else if (strstr(tok,"*action3")) number = MSG_LEVEL_ACTION3;
                       else if (strstr(tok,"*action4")) number = MSG_LEVEL_ACTION4;
                       else if (strstr(tok,"*action5")) number = MSG_LEVEL_ACTION5;
                       else if (strstr(tok,"*action6")) number = MSG_LEVEL_ACTION6;
                       else if (strstr(tok,"*action7")) number = MSG_LEVEL_ACTION7;
                       else if (strstr(tok,"*action8")) number = MSG_LEVEL_ACTION8;
                       else if (strstr(tok,"*action9")) number = MSG_LEVEL_ACTION9;
                       else if (strstr(tok,"*action10")) number = MSG_LEVEL_ACTION10;
                       break;
             case 2  :
                       if (strstr(tok,"*misc1")) number = MSG_LEVEL_MISC1;
                       else if (strstr(tok,"*misc2")) number = MSG_LEVEL_MISC2;
                       else if (strstr(tok,"*misc3")) number = MSG_LEVEL_MISC3;
                       else if (strstr(tok,"*misc4")) number = MSG_LEVEL_MISC4;
                       else if (strstr(tok,"*misc5")) number = MSG_LEVEL_MISC5;
           } // switch mode
         } // pos==0
         else if (pos > 0)
         {
           if (strlen(tok) > 0)
           switch (mode)
           {
             case 1  :
                        switch (pos)
                        {
                          case 1  :  //l_action_message[number].text1 = (char *)malloc(strlen(tok));
                                     strcpy(l_action_message[number].text1,tok);
                                     break;
                          case 2  :
                                     //l_action_message[number].text2 = (char *)malloc(strlen(tok));
                                     strcpy(l_action_message[number].text2,tok);
                                     break;
                          case 3  :
                                     //l_action_message[number].text3 = (char *)malloc(strlen(tok));
                                     strcpy(l_action_message[number].text3,tok);
                                     break;
                        } // switch pos
                        break;
             case 2  :
                        switch (pos)
                        {
                          case 1  :  //l_misc_message[number].text1 = (char *)malloc(strlen(tok));
                                     strcpy(l_misc_message[number].text1,tok);
                                     break;
                          case 2  :  //l_misc_message[number].text2 = (char *)malloc(strlen(tok));
                                     strcpy(l_misc_message[number].text2,tok);
                                     break;
                          case 3  :  //l_misc_message[number].text3 = (char *)malloc(strlen(tok));
                                     strcpy(l_misc_message[number].text3,tok);
                                     break;
                        } // switch pos
                        break;
           } // switch mode
         } // else if pos > 0

         pos++;
       } // for tok

       line++;
    } // if message

  } // while !feof

  pack_fclose(f);
}


void clear_level_info()
{
  int nr;

  for (nr=0; nr < 10; nr++)
  {
    level_info[nr].mode = 0;
    level_info[nr].time = 0;
    level_info[nr].length = 0;
    level_info[nr].clear = 0;
    level_info[nr].border = 0;
    level_info[nr].ypos = 0;
    level_info[nr].speed = 0;
    level_info[nr].color1 = 0;
    level_info[nr].color2 = 0;
    level_info[nr].user1 = 0;
    level_info[nr].user2 = 0;
    level_info[nr].font1 = NULL;
    level_info[nr].font2 = NULL;
    strcpy(level_info[nr].text1,"");
    strcpy(level_info[nr].text2,"");
  }
}

void read_level_text(int language_setting)
{
  char tmp[250];
  PACKFILE *f;
  char *tok;
  int line=0,pos=0;
  int mode=0,ok=FALSE;

//  strcpy(tmp_dir,lng_dir);
//  f = fopen((char*)strcat(tmp_dir,lng_name),"rt");

  packfile_password("tanja");
  sprintf(tmp_dir,"%s%s#%s",dat_dir,"gflng.dat",lng_name);
  f = pack_fopen(tmp_dir,"r");

  if (!f) { error_exit(48,error_message[MSG_ERROR_LNGFILE].text1,lng_name,48,0); }

  if (f)
  while (!pack_feof(f))
  {
    // read line
    pack_fgets(tmp,250,f);

    // kill last character (newline)
//    if (strlen(tmp) > 1)
//      tmp[strlen(tmp)-1] = 0;

    // correct language section?
    switch (language_setting)
    {
      case 0  : if (strstr(tmp,"%%english"))
                {
                  if (!ok) ok = TRUE;
                }
                else if (strstr(tmp,"%%") || strstr(tmp,"%action"))
                {
                  if (ok) ok = FALSE;
                }
                break;
      case 1  : if (strstr(tmp,"%%german"))
                {
                  if (!ok) ok = TRUE;
                }
                else if (strstr(tmp,"%%") || strstr(tmp,"%action"))
                {
                  if (ok) ok = FALSE;
                }
                break;
    }

    // Zeile auswerten
    if ( (tmp[0]=='*') && ok)
    {
       pos = 0;
       for (tok = (char *)strtok(tmp,";"); tok; tok = (char *)strtok(0,";"))
       {
         if (pos==0)
         {
           if (strstr(tok,"*0")) { mode = 0; level_info[line].mode = 0; }
           else if (strstr(tok,"*1")) { mode = 1; level_info[line].mode = 1; }
         }
         else if (pos > 0)
         {
           if (mode==0)
           {
             switch (pos)
             {
               case 1  :  level_info[line].time = atoi(tok);
                          break;
               case 2  :  level_info[line].length = atoi(tok);
                          break;
               case 3  :  level_info[line].clear = atoi(tok);
                          break;
               case 4  :  level_info[line].border = atoi(tok);
                          break;
               case 5  :  if (strstr(tok,"yellow")) level_info[line].color1 = col_yellow;
                          else if (strstr(tok,"orange")) level_info[line].color1 = col_orange;
                          else if (strstr(tok,"green")) level_info[line].color1 = col_green;
                          else if (strstr(tok,"white")) level_info[line].color1 = col_white;
                          else level_info[line].color1 = col_yellow;
                          break;
               case 6  :  if (strstr(tok,"yellow")) level_info[line].color2 = col_yellow;
                          else if (strstr(tok,"orange")) level_info[line].color2 = col_orange;
                          else if (strstr(tok,"green")) level_info[line].color2 = col_green;
                          else if (strstr(tok,"white")) level_info[line].color2 = col_white;
                          else level_info[line].color2 = col_yellow;
                          break;
               case 7  :  if (strstr(tok,"i10")) level_info[line].font1 = impact10h;
                          else if (strstr(tok,"i14")) level_info[line].font1 = impact14h;
                          else if (strstr(tok,"a14")) level_info[line].font1 = arial14h;
                          else level_info[line].font1 = impact10h;
                          break;
               case 8  :  if (strstr(tok,"i10")) level_info[line].font2 = impact10h;
                          else if (strstr(tok,"i14")) level_info[line].font2 = impact14h;
                          else if (strstr(tok,"a14")) level_info[line].font2 = arial14h;
                          else level_info[line].font2 = impact10h;
                          break;
               case 9  :  level_info[line].user1 = atoi(tok);
                          break;
               case 10 :  level_info[line].user2 = atoi(tok);
                          break;
               case 11 :  //level_info[line].text1 = malloc(strlen(tok));
                          strcpy(level_info[line].text1,tok);
                          break;
               case 12 :  //level_info[line].text2 = malloc(strlen(tok));
                          strcpy(level_info[line].text2,tok);
                          break;
             } // switch pos
           } // mode 0
           else if (mode==1)
           {
             switch (pos)
             {
               case 1  :  level_info[line].time = atoi(tok);
                          break;
               case 2  :  level_info[line].length = atoi(tok);
                          break;
               case 3  :  level_info[line].clear = atoi(tok);
                          break;
               case 4  :  level_info[line].border = atoi(tok);
                          break;
               case 5  :  level_info[line].ypos = atoi(tok);
                          break;
               case 6  :  level_info[line].speed = atoi(tok);
                          break;
               case 7  :  if (strstr(tok,"yellow")) level_info[line].color1 = col_yellow;
                          else if (strstr(tok,"orange")) level_info[line].color1 = col_orange;
                          else if (strstr(tok,"green")) level_info[line].color1 = col_green;
                          else if (strstr(tok,"white")) level_info[line].color1 = col_white;
                          else level_info[line].color1 = col_yellow;
                          break;
               case 8  :  if (strstr(tok,"i10")) level_info[line].font1 = impact10h;
                          else if (strstr(tok,"i14")) level_info[line].font1 = impact14h;
                          else if (strstr(tok,"a14")) level_info[line].font1 = arial14h;
                          else level_info[line].font1 = impact10h;
                          break;
               case 9  :  level_info[line].user1 = atoi(tok);
                          break;
               case 10 :  level_info[line].user2 = atoi(tok);
                          break;
               case 11 :  //level_info[line].text1 = malloc(strlen(tok));
                          strcpy(level_info[line].text1,tok);
                          break;
             } // switch pos
           } // mode 1

         } // else if pos > 0

         pos++;
       } // for tok

       line++;
    } // if message

  } // while !feof

  level_info_maxlines = line;
  cur_info_line = 0;

  pack_fclose(f);


  // Zusaetzliche Messages laden
  read_level_message_file(language_setting);

}

void show_level_info_message()
{

  if ( (!abort_start_message) && (cur_info_line <= level_info_maxlines) )
  {
   if (counter==level_info[cur_info_line].time)
   {
     if (cur_info_line == 0) play_sound_sample(snd_osd_on,1000,0,playship[0].xpos,playship[0].ypos,100);

     if (level_info[cur_info_line].clear) clear_osd(level_info[cur_info_line].border);

     if (level_info[cur_info_line].mode == 0)
     {
       show_normal_osd_message(level_info[cur_info_line].font1,level_info[cur_info_line].font2,level_info[cur_info_line].text1,
                               level_info[cur_info_line].text2,level_info[cur_info_line].length,
                               level_info[cur_info_line].color1,level_info[cur_info_line].color2,level_info[cur_info_line].border);
     } // mode 0 (normal)
     else if (level_info[cur_info_line].mode == 1)
     {
       show_typewriter_osd_message(level_info[cur_info_line].font1,level_info[cur_info_line].text1,
                                   level_info[cur_info_line].speed,level_info[cur_info_line].length,
                                   level_info[cur_info_line].color1,level_info[cur_info_line].ypos);
     } // mode 1 (typewriter)

     if (cur_info_line < level_info_maxlines) cur_info_line++;
   } // if counter
  } // if !abort...

}

