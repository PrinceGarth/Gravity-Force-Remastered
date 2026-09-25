/*
 *    GRAVITY FORCE, gflevel.c
 *
 *    -- level specific stuff: initialization, runtime checks, end checks
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gf.h"
#include "gflevel.h"
#include "gfinit.h"
#include "gfosd.h"
#include "gflink.h"
#include "gfpal.h"
#include "gfmes.h"
#include "gfstat.h"
#include "gfsprt.h"
#include "gfrace.h"
#include "gfg2pdog.h"
#include "gflog.h"

volatile int game_time,fps,fpst,shield_time;
volatile int overall_gametime;
int action1,action2,action3;
int level_end;
int temp_counter;
int flag[20];
int iu1=0,iu2=0,iu3=0;
float fu1=0,fu2=0,fu3=0,fu4=0;

// setup weapons for dogfight mode (0=no extra weapons, 10=mega weapons)
void init_playship_weapons()
{
  int nr,amount;

  for (nr=0; nr < 2; nr++)
  {
    if (nr==0) amount = config_file.config.p1weapons;
    else if (nr==1) amount = config_file.config.p2weapons;

    switch (amount)
    {
      case 0  :
                 playship[nr].weapon.bullets[W_MG] = 0;
                 playship[nr].weapon.bullets[W_BOMB] = 0;
                 playship[nr].weapon.bullets[W_BIGBOMB] = 0;
                 playship[nr].weapon.bullets[W_ROCKET] = 0;
                 playship[nr].weapon.bullets[W_MINES] = 0;
                 playship[nr].weapon.type2 = 0;
                 break;

      case 1  :
                 playship[nr].weapon.bullets[W_MG] = 100;
                 playship[nr].weapon.bullets[W_BOMB] = 0;
                 playship[nr].weapon.bullets[W_BIGBOMB] = 0;
                 playship[nr].weapon.bullets[W_ROCKET] = 0;
                 playship[nr].weapon.bullets[W_MINES] = 0;
                 playship[nr].weapon.type2 = 0;
                 break;

      case 2  :
                 playship[nr].weapon.bullets[W_MG] = 150;
                 playship[nr].weapon.bullets[W_BOMB] = 10;
                 playship[nr].weapon.bullets[W_BIGBOMB] = 0;
                 playship[nr].weapon.bullets[W_ROCKET] = 0;
                 playship[nr].weapon.bullets[W_MINES] = 0;
                 playship[nr].weapon.type2 = 0;
                 break;

      case 3  :
                 playship[nr].weapon.bullets[W_MG] = 170;
                 playship[nr].weapon.bullets[W_BOMB] = 15;
                 playship[nr].weapon.bullets[W_BIGBOMB] = 5;
                 playship[nr].weapon.bullets[W_ROCKET] = 0;
                 playship[nr].weapon.bullets[W_MINES] = 0;
                 playship[nr].weapon.type2 = 0;
                 break;

      case 4  :
                 playship[nr].weapon.bullets[W_MG] = 200;
                 playship[nr].weapon.bullets[W_BOMB] = 25;
                 playship[nr].weapon.bullets[W_BIGBOMB] = 7;
                 playship[nr].weapon.bullets[W_ROCKET] = 5;
                 playship[nr].weapon.bullets[W_MINES] = 0;
                 playship[nr].weapon.type2 = 0;
                 break;

      case 5  :
                 playship[nr].weapon.bullets[W_MG] = 250;
                 playship[nr].weapon.bullets[W_BOMB] = 30;
                 playship[nr].weapon.bullets[W_BIGBOMB] = 8;
                 playship[nr].weapon.bullets[W_ROCKET] = 10;
                 playship[nr].weapon.bullets[W_MINES] = 5;
                 playship[nr].weapon.type2 = W_MINE1;
                 break;

      case 6  :
                 playship[nr].weapon.bullets[W_MG] = 280;
                 playship[nr].weapon.bullets[W_BOMB] = 35;
                 playship[nr].weapon.bullets[W_BIGBOMB] = 12;
                 playship[nr].weapon.bullets[W_ROCKET] = 15;
                 playship[nr].weapon.bullets[W_MINES] = 10;
                 playship[nr].weapon.type2 = W_MINE1;
                 break;

      case 7  :
                 playship[nr].weapon.bullets[W_MG] = 300;
                 playship[nr].weapon.bullets[W_BOMB] = 40;
                 playship[nr].weapon.bullets[W_BIGBOMB] = 15;
                 playship[nr].weapon.bullets[W_ROCKET] = 20;
                 playship[nr].weapon.bullets[W_MINES] = 20;
                 playship[nr].weapon.type2 = W_MINE1;
                 break;

      case 8  :
                 playship[nr].weapon.bullets[W_MG] = 340;
                 playship[nr].weapon.bullets[W_BOMB] = 45;
                 playship[nr].weapon.bullets[W_BIGBOMB] = 20;
                 playship[nr].weapon.bullets[W_ROCKET] = 25;
                 playship[nr].weapon.bullets[W_MINES] = 20;
                 playship[nr].weapon.type2 = W_MINE2;
                 break;

      case 9  :
                 playship[nr].weapon.bullets[W_MG] = 370;
                 playship[nr].weapon.bullets[W_BOMB] = 50;
                 playship[nr].weapon.bullets[W_BIGBOMB] = 25;
                 playship[nr].weapon.bullets[W_ROCKET] = 30;
                 playship[nr].weapon.bullets[W_MINES] = 25;
                 playship[nr].weapon.type2 = W_MINE2;
                 break;

      case 10 :
                 playship[nr].weapon.bullets[W_MG] = 400;
                 playship[nr].weapon.bullets[W_BOMB] = 60;
                 playship[nr].weapon.bullets[W_BIGBOMB] = 30;
                 playship[nr].weapon.bullets[W_ROCKET] = 40;
                 playship[nr].weapon.bullets[W_MINES] = 30;
                 playship[nr].weapon.type2 = W_MINE3;
                 break;
    }
  }

  // Extras?
  switch (config_file.config.extras)
  {
    case 0  :  use_extras = 0;
               break;
    case 1  :  use_extras = 4;
               break;
    case 2  :  use_extras = 2;
               break;
    case 3  :  use_extras = 1;
               break;
  }

}

void level_inits()
{
  int nr;

  show_infobox = FALSE;
  playship_noweap = FALSE;
  use_extras = FALSE;

  for (nr=0; nr < 20; nr++) flag[nr] = FALSE;

  // default-Werte
  DEF_SHIELD_RECHARGE = 60;
  DEF_AUTOFIRE_RATE = 15;

  playship[0].SHIELD_RECHARGE = DEF_SHIELD_RECHARGE;
  playship[0].AUTOFIRE_RATE = DEF_AUTOFIRE_RATE;
  playship[1].SHIELD_RECHARGE = DEF_SHIELD_RECHARGE;
  playship[1].AUTOFIRE_RATE = DEF_AUTOFIRE_RATE;
  playship[2].SHIELD_RECHARGE = DEF_SHIELD_RECHARGE;
  playship[2].AUTOFIRE_RATE = DEF_AUTOFIRE_RATE;
  playship[3].SHIELD_RECHARGE = DEF_SHIELD_RECHARGE;
  playship[3].AUTOFIRE_RATE = DEF_AUTOFIRE_RATE;
  playship[4].SHIELD_RECHARGE = DEF_SHIELD_RECHARGE;
  playship[4].AUTOFIRE_RATE = DEF_AUTOFIRE_RATE;

  if (game_mode == SP_CAMPAIGN)
  switch (current_level)
  {
    case  1  :
               strcpy(gfx_name,"gfw1.fmp");
               strcpy(cfg_name,"gf01.cfg");
               strcpy(map_name,"gf01.mar");
               strcpy(lng_name,"gf01.lng");

               playship[0].lifes = 5; playship[0].shield = 0; playship[0].shield_active = FALSE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 0;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = 0;

               SHOW_RADAR = FALSE;
               temp_counter = 0;
               playship[0].START_BASE = 0;

               level_time_factor = 0.03;
               killable_enemies = 0;
               break;

    case  2  :
               strcpy(gfx_name,"gfw1.fmp");
               strcpy(cfg_name,"gf02.cfg");
               strcpy(map_name,"gf02.mar");
               strcpy(lng_name,"gf02.lng");

               playship[0].lifes = 5; playship[0].shield = 0; playship[0].shield_active = FALSE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 0;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = 0;

               SHOW_RADAR = FALSE;
               playship[0].START_BASE = 0;
               temp_counter = 0;
               level_time_factor = 0.04;
               killable_enemies = 10;
               break;
    case  3  :
               strcpy(gfx_name,"gfw2.fmp");
               strcpy(cfg_name,"gf03.cfg");
               strcpy(map_name,"gf03.mar");
               strcpy(lng_name,"gf03.lng");

               playship[0].lifes = 3; playship[0].shield = 2; playship[0].shield_active = TRUE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 300;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = 0;

               SHOW_RADAR = FALSE;
               playship[0].START_BASE = 0;

               temp_counter = 0;
               level_time_factor = 0.02;
               killable_enemies = 45;
               break;
    case  4  :
               strcpy(gfx_name,"gfw1.fmp");
               strcpy(cfg_name,"gf04.cfg");
               strcpy(map_name,"gf04.mar");
               strcpy(lng_name,"gf04.lng");

               playship[0].lifes = 4; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 200;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = 0;

               SHOW_RADAR = FALSE;
               playship[0].START_BASE = 0;

               temp_counter = 0;
               level_time_factor = 0.01;
               killable_enemies = 0;
               break;
    case  5  :
               strcpy(gfx_name,"gfw2.fmp");
               strcpy(cfg_name,"gf05.cfg");
               strcpy(map_name,"gf05.mar");
               strcpy(lng_name,"gf05.lng");

               playship[0].lifes = 5; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 0;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = 0;

               SHOW_RADAR = FALSE;

               playship[0].START_BASE = 0;

               temp_counter = 0;
               level_time_factor = 0.01;
               killable_enemies = 33;

               flag[0] = flag[1] = 1;
               break;
    case  6  :
               strcpy(gfx_name,"gfw2.fmp");
               strcpy(cfg_name,"gf06.cfg");
               strcpy(map_name,"gf06.mar");
               strcpy(lng_name,"gf06.lng");

               playship[0].lifes = 5; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 0;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = 0;

               SHOW_RADAR = FALSE;
               playship[0].START_BASE = 0;

               temp_counter = 0;
               level_time_factor = 0.005;
               killable_enemies = 43;
               break;
    case  7  :
               strcpy(gfx_name,"gfw2.fmp");
               strcpy(cfg_name,"gf07.cfg");
               strcpy(map_name,"gf07.mar");
               strcpy(lng_name,"gf07.lng");

               playship[0].lifes = 6; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 0;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = 0;

               SHOW_RADAR = FALSE;
               playship[0].START_BASE = 0;

               playship[0].SHIELD_RECHARGE = 20;

               temp_counter = 0;
               level_time_factor = 0.01;
               killable_enemies = 29;
               break;

    case  8  :
               strcpy(gfx_name,"gfw1.fmp");
               strcpy(cfg_name,"gf08.cfg");
               strcpy(map_name,"gf08.mar");
               strcpy(lng_name,"gf08.lng");

               playship[0].lifes = 5; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 0;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = 0;

               SHOW_RADAR = TRUE;
               playship[0].START_BASE = 0;

               playship[0].SHIELD_RECHARGE = 4;
               REFUEL_TIME = 20;

               temp_counter = 0;
               level_time_factor = 0.005;
               killable_enemies = 28;
               break;
    case  9  :
               strcpy(gfx_name,"gfw2.fmp");
               strcpy(cfg_name,"gf09.cfg");
               strcpy(map_name,"gf09.mar");
               strcpy(lng_name,"gf09.lng");

               playship[0].lifes = 5; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 20;
               playship[0].weapon.bullets[W_MG] = 400;
               playship[0].weapon.bullets[W_BIGBOMB] = 13;
               playship[0].weapon.bullets[W_ROCKET] = 10;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = 0;

               SHOW_RADAR = FALSE;
               playship[0].START_BASE = 0;
               REFUEL_TIME = 40;

               temp_counter = 0;
               level_time_factor = 0.01;
               killable_enemies = 54;
               break;
    case  10 :
               strcpy(gfx_name,"gfw1.fmp");
               strcpy(cfg_name,"gf10.cfg");
               strcpy(map_name,"gf10.mar");
               strcpy(lng_name,"gf10.lng");

               playship[0].lifes = 5; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 0;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = 0;

               playship_noweap = TRUE;

               SHOW_RADAR = FALSE;
               playship[0].START_BASE = 0;
               REFUEL_TIME = 30;

               playship[0].SHIELD_RECHARGE = 20;
               temp_counter = 0;
               level_time_factor = 0.003;
               killable_enemies = 44;
               break;

    default:
               strcpy(gfx_name,"gfw1.fmp");

               playship[0].lifes = 99; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].weapon.bullets[W_BOMB] = 999;
               playship[0].weapon.bullets[W_MG] = 9999;
               playship[0].weapon.bullets[W_BIGBOMB] = 99;
               playship[0].weapon.bullets[W_ROCKET] = 999;
//               playship[0].weapon.bullets[W_BOUNCE] = 200;
               SHOW_RADAR = TRUE;

               level_time_factor = 0;
               temp_counter = 0;
               break;
  } // switch current_level (campaign)

  else if (game_mode == SP_TRAINING)
  switch (current_training_level)
  {
    case  1  :
               strcpy(gfx_name,"gfw1.fmp");
               strcpy(cfg_name,"gft01.cfg");
               strcpy(map_name,"gft01.mar");
               strcpy(lng_name,"gft01.lng");

               playship[0].lifes = 99; playship[0].shield = 0; playship[0].shield_active = FALSE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 0;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = W_MINE1;
               SHOW_RADAR = FALSE;
               playship[0].START_BASE = 0;

               playship_noweap = TRUE;

               temp_counter = 0;
               level_time_factor = 0;
               killable_enemies = 0;
               SHOW_RADAR = FALSE;
               break;

    case  2  :
               strcpy(gfx_name,"gfw1.fmp");
               strcpy(cfg_name,"gft02.cfg");
               strcpy(map_name,"gft02.mar");
               strcpy(lng_name,"gft02.lng");

               playship[0].lifes = 99; playship[0].shield = 0; playship[0].shield_active = FALSE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 0;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = W_MINE1;
               SHOW_RADAR = FALSE;
               playship[0].START_BASE = 0;

               playship_noweap = TRUE;

               temp_counter = 0;
               level_time_factor = 0;
               killable_enemies = 0;
               SHOW_RADAR = FALSE;
               break;

    case  3  :
               strcpy(gfx_name,"gfw1.fmp");
               strcpy(cfg_name,"gft03.cfg");
               strcpy(map_name,"gft03.mar");
               strcpy(lng_name,"gft03.lng");

               playship[0].lifes = 99; playship[0].shield = 0; playship[0].shield_active = FALSE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 0;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = W_MINE1;
               SHOW_RADAR = TRUE;
               playship[0].START_BASE = 0;

               temp_counter = 0;
               level_time_factor = 0;
               killable_enemies = 0;
               break;

    case  4  :
               strcpy(gfx_name,"gfw2.fmp");
               strcpy(cfg_name,"gft04.cfg");
               strcpy(map_name,"gft04.mar");
               strcpy(lng_name,"gft04.lng");

               playship[0].lifes = 99; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 0;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = W_MINE1;
               SHOW_RADAR = TRUE;
               playship[0].START_BASE = 0;

               playship_noweap = FALSE;

               temp_counter = 0;
               level_time_factor = 0;
               killable_enemies = 0;
               break;


    case  5  :
               strcpy(gfx_name,"gfw1.fmp");
               strcpy(cfg_name,"gft05.cfg");
               strcpy(map_name,"gft05.mar");
               strcpy(lng_name,"gft05.lng");

               playship[0].lifes = 99; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;
               playship[0].weapon.bullets[W_BOMB] = 50;
               playship[0].weapon.bullets[W_MG] = 200;
               playship[0].weapon.bullets[W_BIGBOMB] = 10;
               playship[0].weapon.bullets[W_ROCKET] = 100;
               playship[0].weapon.bullets[W_MINES] = 50;
               playship[0].weapon.type2 = W_MINE1;
               SHOW_RADAR = TRUE;
               playship[0].START_BASE = 0;

               temp_counter = 0;
               level_time_factor = 0;
               killable_enemies = 0;
               break;
  }

  else if (game_mode == SP_RACE)
  switch (current_sprace_level)
  {
    case  1  :
               strcpy(gfx_name,"gfw1.fmp");
               strcpy(cfg_name,"gfr01.cfg");
               strcpy(map_name,"gfr01.mar");
               strcpy(lng_name,"gfr01.lng");

               playship[0].lifes = 99; playship[0].shield = 0; playship[0].shield_active = FALSE;
               playship[0].pshield = 3; playship[0].pshield_active = TRUE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 0;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = 0;

               SHOW_RADAR = FALSE;
               temp_counter = 0;
               playship[0].START_BASE = 0;

               level_time_factor = 0.03;
               killable_enemies = 0;

               max_lap = 5;
               break;

    case  2  :
               strcpy(gfx_name,"gfw1.fmp");
               strcpy(cfg_name,"gfr02.cfg");
               strcpy(map_name,"gfr02.mar");
               strcpy(lng_name,"gfr02.lng");

               playship[0].lifes = 99; playship[0].shield = 0; playship[0].shield_active = FALSE;
               playship[0].pshield = 3; playship[0].pshield_active = TRUE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 0;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = 0;

               SHOW_RADAR = FALSE;
               temp_counter = 0;
               playship[0].START_BASE = 0;

               level_time_factor = 0.03;
               killable_enemies = 0;

               max_lap = 3;
               break;

    case  3  :
               strcpy(gfx_name,"gfw2.fmp");
               strcpy(cfg_name,"gfr03.cfg");
               strcpy(map_name,"gfr03.mar");
               strcpy(lng_name,"gfr03.lng");

               playship[0].lifes = 99; playship[0].shield = 0; playship[0].shield_active = FALSE;
               playship[0].pshield = 3; playship[0].pshield_active = TRUE;
               playship[0].weapon.bullets[W_BOMB] = 0;
               playship[0].weapon.bullets[W_MG] = 0;
               playship[0].weapon.bullets[W_BIGBOMB] = 0;
               playship[0].weapon.bullets[W_ROCKET] = 0;
               playship[0].weapon.bullets[W_MINES] = 0;
               playship[0].weapon.type2 = 0;

               SHOW_RADAR = FALSE;
               temp_counter = 0;
               playship[0].START_BASE = 0;

               level_time_factor = 0.03;
               killable_enemies = 0;

               max_lap = 3;
               break;

  } // switch level (SP_RACE)

  else if (game_mode == MP_2PDOGFIGHT)
  switch (current_qdogfight_level)
  {
    case  1  :
               strcpy(gfx_name,"gfw1.fmp");
               strcpy(cfg_name,"gfqd01.cfg");
               strcpy(map_name,"gfqd01.mar");
               strcpy(lng_name,"gfqd01.lng");

               playship[0].lifes = 999; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].pshield = 3; playship[0].pshield_active = TRUE;

               playship[1].lifes = 999; playship[1].shield = 5; playship[1].shield_active = TRUE;
               playship[1].pshield = 3; playship[1].pshield_active = TRUE;

               init_playship_weapons();

               SHOW_RADAR = FALSE;
               temp_counter = 0;
               playship[0].START_BASE = 0;
               playship[1].START_BASE = 1;

               killable_enemies = 0;

               DEF_SHIELD_RECHARGE = 10;

               cur_l = 0;
               l_scroll_x[cur_l] = 16;
               l_scroll_y[cur_l] = 36;
               break;

    case  2  : // Bump Arena
               strcpy(gfx_name,"gfw1.fmp");
               strcpy(cfg_name,"gfqd02.cfg");
               strcpy(map_name,"gfqd02.mar");
               strcpy(lng_name,"gfqd02.lng");

               playship[0].lifes = 999; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;

               playship[1].lifes = 999; playship[1].shield = 5; playship[1].shield_active = TRUE;
               playship[1].pshield = 0; playship[1].pshield_active = FALSE;

               init_playship_weapons();

               SHOW_RADAR = FALSE;
               temp_counter = 0;
               playship[0].START_BASE = -1;
               playship[1].START_BASE = -1;
               playship[0].oxpos = 1746;
               playship[0].oypos = 1848;
               playship[1].oxpos = 2032;
               playship[1].oypos = 1848;

               killable_enemies = 0;

               DEF_SHIELD_RECHARGE = 5;

               cur_l = 0;
               l_scroll_x[cur_l] = 1578;
               l_scroll_y[cur_l] = 1643;
               break;

    case  3  : // Moving Stones
               strcpy(gfx_name,"gfw2.fmp");
               strcpy(cfg_name,"gfqd03.cfg");
               strcpy(map_name,"gfqd03.mar");
               strcpy(lng_name,"gfqd03.lng");

               playship[0].lifes = 999; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;

               playship[1].lifes = 999; playship[1].shield = 5; playship[1].shield_active = TRUE;
               playship[1].pshield = 0; playship[1].pshield_active = FALSE;

               init_playship_weapons();

               SHOW_RADAR = FALSE;
               temp_counter = 0;
               playship[0].START_BASE = 0;
               playship[1].START_BASE = 1;

               killable_enemies = 0;

               DEF_SHIELD_RECHARGE = 10;

               cur_l = 0;
               l_scroll_x[cur_l] = 16;
               l_scroll_y[cur_l] = 36;
               break;

    case  4  : // Boundless
               strcpy(gfx_name,"gfw2.fmp");
               strcpy(cfg_name,"gfqd04.cfg");
               strcpy(map_name,"gfqd04.mar");
               strcpy(lng_name,"gfqd04.lng");

               playship[0].lifes = 999; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;

               playship[1].lifes = 999; playship[1].shield = 5; playship[1].shield_active = TRUE;
               playship[1].pshield = 0; playship[1].pshield_active = FALSE;

               init_playship_weapons();

               SHOW_RADAR = FALSE;
               temp_counter = 0;
               playship[0].START_BASE = -1;
               playship[1].START_BASE = -1;
               playship[0].oxpos = 260;
               playship[0].oypos = 300;
               playship[1].oxpos = 620;
               playship[1].oypos = 300;

               killable_enemies = 0;

               DEF_SHIELD_RECHARGE = 7;

               cur_l = 0;
               l_scroll_x[cur_l] = 120;
               l_scroll_y[cur_l] = 120;
               break;

    case  5  : // Black Hole
               strcpy(gfx_name,"gfw2.fmp");
               strcpy(cfg_name,"gfqd05.cfg");
               strcpy(map_name,"gfqd05.mar");
               strcpy(lng_name,"gfqd05.lng");

               playship[0].lifes = 999; playship[0].shield = 5; playship[0].shield_active = TRUE;
               playship[0].pshield = 0; playship[0].pshield_active = FALSE;

               playship[1].lifes = 999; playship[1].shield = 5; playship[1].shield_active = TRUE;
               playship[1].pshield = 0; playship[1].pshield_active = FALSE;

               init_playship_weapons();

               SHOW_RADAR = FALSE;
               temp_counter = 0;
               playship[0].START_BASE = -1;
               playship[1].START_BASE = -1;
               playship[0].oxpos = 260;
               playship[0].oypos = 300;
               playship[1].oxpos = 620;
               playship[1].oypos = 300;

               killable_enemies = 0;

               DEF_SHIELD_RECHARGE = 7;

               cur_l = 0;
               l_scroll_x[cur_l] = 120;
               l_scroll_y[cur_l] = 120;
               break;

  } // switch level (MP_2PDOGFIGHT)

}

void level_runtime_checks()
{
  char tt[100];
  int nr;
  static float tspd = 0;
  extern int time_counter;
  int h;
  BNODE *temp, *next_bull;
  temp = first_bullet;

  if (game_mode == SP_CAMPAIGN)
  switch (current_level)
  {
    case  1  : // Gravity Force
               show_level_info_message();
               break;

    case  2  : // Easy, But Tricky
               show_level_info_message();
               break;

    case  3  : // Bombing Run
               show_level_info_message();

               // Tunnel deaktivieren, wenn Geb. 7 hin
               if ((!object[7].active) && (object[7].dead != 2))
               {
                   for (nr=15; nr <= 17; nr++)
                   {
                     object[nr].maxframe = 0;
                     object[nr].frame = rand() % 4;
                   }
                   object[7].dead = 2;
               }

               // Countdown aktivieren, wenn alle Ziele hin
               if ( ( (!object[7].active && !object[8].active && !object[9].active && !use_countdown && !flag[0]) ) )
               {
                 use_countdown = TRUE;
                 countdown = 90;
                 flag[0] = TRUE;

                 // Lichtschranke ausschalten
                 object[10].frame = 0; object[10].maxframe = 0; object[10].startframe = 0;
               }

               if ((countdown==90) && !flag[1])
               {
                 show_normal_osd_message(impact10h,impact14h,other_message[MSG_OTHER_BOMBINGRUN].text1,other_message[MSG_OTHER_BOMBINGRUN].text2,15,col_yellow,col_white,1);
                 flag[1] = TRUE;
               }

               if ((countdown==89) && !flag[2])
               {
                 strcpy(tt,"ø90 ");
                 show_typewriter_osd_message(impact10h,(char *)strcat(tt,other_message[MSG_OTHER_BOMBINGRUN].text3),90,3,col_orange,45);
                 flag[2] = TRUE;
               }

               // Explosionen waehrend Countdown
               if ( (countdown>4) && !flag[3])
                 if (rand() % countdown == countdown-1)
                 {
                   add_explosion(0,expl_big_1,5,4,rand() % 20,
                    (playship[0].xpos-200) + rand() % 400,
                    (playship[0].ypos-200) + rand() % 300);
                   add_explosion(0,expl_sm1_01,10,2,rand() % 25,
                    (playship[0].xpos-300) + rand() % 600,
                    (playship[0].ypos-200) + rand() % 400);
                 }

               // Wenn Countdown abgelaufen...
               if ((use_countdown && (countdown == 5)) && !flag[3])
               {
                 fade_out_active = TRUE; fade_pos = 0; fade_opos = 0;
                 fade_type = 4; fade_count = 0; fade_count_to = 500;
                 fade_speed = 0.1;
               }
               else if (use_countdown && (countdown == 0))
               {
                 use_countdown = FALSE;
                 playship[0].dead = TRUE; playship[0].lifes = 0;
                 level_end = END_BAD;
               }

               // Wenn Schiff Ziel erreicht, flag3 setzen, Countdown abschalten
               if ( (playship[0].xpos > 4500) && (playship[0].ypos > 4000) && !flag[3])
               {
                 flag[3] = TRUE;
                 use_countdown = FALSE;
                 countdown = 0;
               }

               // Dann auf Landung warten und beenden
               if (flag[3] && flag[0])
               {
                 if (playship[0].land) level_end = END_GOOD;
                 if (playship[0].dead) level_end = END_BAD;
               }
               break;

    case  4  : // Heavy Load
               show_level_info_message();

               // Erstes Paket aufgenommen -> Tuer o2 aufmachen
               if (base[2].cargo == 0 && !flag[0])
               {
                 flag[0] = TRUE;
                 object[2].frame = 0; object[2].maxframe = 0; object[2].startframe = 0;
                 // "Room 1 unlocked"
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION1].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION1].text2,90,3,col_orange,45);
               }

               // Zweites Paket aufgenommen -> Tuer o3/o4 aufmachen
               if (base[1].cargo == 0 && !flag[1])
               {
                 flag[1] = TRUE;
                 object[3].frame = 0; object[3].maxframe = 0; object[3].startframe = 0;
                 object[4].frame = 0; object[4].maxframe = 0; object[4].startframe = 0;
                 // "Room 2 unlocked"
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION2].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION2].text2,90,3,col_orange,45);
               }

               // Drittes Paket aufgenommen -> Tuer o5 aufmachen
               if (base[3].cargo == 0 && !flag[2])
               {
                 flag[2] = TRUE;
                 object[5].frame = 0; object[5].maxframe = 0; object[5].startframe = 0;
                 // "Room 3 unlocked"
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION3].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION3].text2,90,3,col_orange,45);
               }

               // Letztes Paket aufgenommen -> Tuer o5 aufmachen
               if (base[4].cargo == 0 && !flag[3])
               {
                 flag[3] = TRUE;
                 object[6].frame = 0; object[6].maxframe = 0; object[6].startframe = 0;
                 // "Exit unlocked"
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION4].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION4].text2,90,3,col_orange,45);
               }
               break;

    case  5  : // Big Brother I
               show_level_info_message();

               // Switches ueberpruefen
               // Sw. 18
               if (object[18].frame == 1 && !flag[0])
               {
                 object[8].frame = 1; object[8].maxframe = 3; object[8].startframe = 1;
                 flag[0] = 1;
               }
               else if (object[18].frame == 0 && flag[0])
               {
                 object[8].frame = 0; object[8].maxframe = 0; object[8].startframe = 0;
                 flag[0] = 0;
               }
               // Sw. 17
               if (object[17].frame == 1 && !flag[1])
               {
                 object[8].frame = 0; object[8].maxframe = 0; object[8].startframe = 0;
                 object[9].frame = 1; object[9].maxframe = 3; object[9].startframe = 1;
                 flag[1] = 1;
               }
               else if (object[17].frame == 0 && flag[1])
               {
                 object[8].frame = 1; object[8].maxframe = 3; object[8].startframe = 1;
                 object[9].frame = 0; object[9].maxframe = 0; object[9].startframe = 0;
                 flag[1] = 0;
               }

               // Mr. Relash retten
               if (base[15].cargo == 0 && !flag[2])
               {
                 flag[2] = TRUE;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION1].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION1].text2,90,3,col_orange,45);
                 playship[0].hide = TRUE;
                 abort_start_message = FALSE;
               }

               if (playship[0].base == 0 && playship[0].ldur == 10 && flag[2])
               {
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION2].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION2].text2,90,3,col_orange,45);
               }
               break;
    case  6  : // Big Brother II
               show_level_info_message();

               // Auf Kollisionen mit "Bomber" checken
               for (nr=0; nr < enemy_anz; nr++)
                 if (enemy[nr].active && !(enemy[nr].user3 >= 5) && !enemy[111].dead)
                   if (collision(enemy[111].xpos+enemy[111].image.boundxa,
                                 enemy[111].ypos+enemy[111].image.boundya,
                                 enemy[111].image.boundxe,
                                 enemy[111].image.boundye,
                                 enemy[nr].xpos+enemy[nr].image.boundxa,
                                 enemy[nr].ypos+enemy[nr].image.boundya,
                                 enemy[nr].image.boundxe,
                                 enemy[nr].image.boundye) )
                   {
                     enemy[111].dead = 1;
                     enemy[nr].dead = 1;
                   }

               // Switch 12 - Pulse Shooter on/off
               if (object[12].frame == 1)
               {
                 enemy[37].react.xw1 = -4047; enemy[37].react.xw2 = 4407;
                 enemy[37].react.yw1 = -4000; enemy[37].react.yw2 = 3000;
               }
               else if (object[12].frame == 0)
               {
                 enemy[37].react.xw1 = 0; enemy[37].react.xw2 = 0;
                 enemy[37].react.yw1 = 0; enemy[37].react.yw2 = 0;
               }

               // Mission -- start
               if (time_counter == 1 && !flag[0])
               {
                 flag[0] = TRUE;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION1].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION1].text2,90,3,col_orange,45);
               }

               // Starting Bombship
               if (time_counter == 12 && !flag[1])
               {
                 flag[1] = TRUE;
                 enemy[111].max_waypnt = 14;
               }

               // Trouble?
               if (enemy[111].hit > enemy[111].maxhit/2 && !flag[2])
               {
                 flag[2] = TRUE;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION2].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION2].text2,90,3,col_orange,45);
               }

               // Dead?
               if (enemy[111].dead && !flag[3])
               {
                 flag[3] = time_counter;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION3].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION3].text2,90,3,col_orange,45);
               }

               // Mission failed
               if (enemy[111].dead && time_counter > flag[3]+3)
               {
                 level_end = END_BAD;
               }

               // Start Bomb Attack
               if (enemy[111].cur_waypnt == 14 && !flag[4])
               {
                 flag[4] = TRUE;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION4].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION4].text2,90,3,col_orange,45);
                 enemy[111].maxshot = 1;
                 enemy[111].start_waypnt = 13;
               }

               // Erfolg checken, dann Bombardierung aussetzen & Tuer oeffnen
               if (!object[19].active && !object[20].active &&
                   !object[21].active && !object[22].active &&
                   !object[23].active && !object[24].active &&
                   !object[25].active && !flag[5])
               {
                 flag[5] = TRUE;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION5].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION5].text2,90,3,col_orange,45);
                 enemy[111].maxshot = 0;
                 object[4].frame = 0; object[4].maxframe = 0; object[4].startframe = 0;
               }

               // Letztes KW zerstoert?
               if (!object[26].active && !flag[6])
               {
                 flag[6] = TRUE;
                 object[4].frame = 1; object[4].maxframe = 3; object[4].startframe = 1;
                 playship[0].START_BASE = 1;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION6].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION6].text2,90,3,col_orange,45);
               }

               // Gelandet?
               if (playship[0].land && playship[0].base == 1 && !flag[7] && flag[6])
               {
                 flag[7] = TRUE;
                 level_end = END_GOOD;
               }
               break;
    case  7  : // Big Brother III
               show_level_info_message();

               // Startbase aendern, wenn gelandet
               if (playship[0].land && playship[0].base == 2 && !flag[0])
               {
                 flag[0] = TRUE;
                 playship[0].START_BASE = 2;
                 show_normal_osd_message(impact10h,impact14h,"",l_misc_message[MSG_LEVEL_MISC1].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_misc_message[MSG_LEVEL_MISC1].text2,90,3,col_orange,45);
                 abort_start_message = FALSE;
               }

               // Anfeuern
               if (enemy[110].hit > 50 && enemy[110].hit < 150 && !flag[1])
               {
                 flag[1] = TRUE;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION1].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION1].text2,90,3,col_orange,45);
               }
               if (enemy[110].hit > 1000 && enemy[110].hit < 2000 && !flag[2])
               {
                 flag[2] = TRUE;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION2].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION2].text2,90,3,col_orange,45);
               }
               if (enemy[110].hit > 2000 && enemy[110].hit < 2700 && !flag[3])
               {
                 flag[3] = TRUE;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION3].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION3].text2,90,3,col_orange,45);
               }
               if (enemy[110].hit > 2750 && !flag[4])
               {
                 flag[4] = TRUE;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION4].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION4].text2,90,3,col_orange,45);
               }

               // Bigboss dead
               if (enemy[110].dead && !flag[5])
               {
                 flag[5] = TRUE;
                 object[7].frame = 0; object[7].maxframe = 0; object[7].startframe = 0;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION5].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION5].text2,90,3,col_orange,45);
               }

               // Wenn gelandet, Level beenden
               if (playship[0].land && playship[0].base == 3 && !flag[6])
               {
                 flag[6] = TRUE;
                 level_end = END_GOOD;
               }
               break;
     case  8 : // Deathmatch
               show_level_info_message();

               // Begin
               if (time_counter == 2 && !flag[0])
               {
                 flag[0] = TRUE;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION5].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION5].text2,90,3,col_orange,45);
                 use_countdown = TRUE;
                 countdown = 30;
               }

               // Release no 1
               if ((time_counter > 28 && countdown <= 0 && !flag[1]))
               {
                 flag[1] = TRUE;
                 use_countdown = FALSE;
                 object[8].frame = 0; object[8].maxframe = 0; object[8].startframe = 0;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION1].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION1].text2,90,3,col_orange,45);
                 enemy[0].chase = enemy[1].chase = TRUE;
               }

               // countdown for no 2
               if (time_counter == 100 && !flag[2])
               {
                 flag[2] = TRUE;
                 use_countdown = TRUE;
                 countdown = 20;
               }

               // Release no 2
               if (time_counter > 118 && countdown <= 0 && !flag[3])
               {
                 flag[3] = TRUE;
                 use_countdown = FALSE;
                 object[9].frame = 0; object[9].maxframe = 0; object[9].startframe = 0;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION2].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION2].text2,90,3,col_orange,45);
                 enemy[2].chase = TRUE;
               }

               // countdown for no 3
               if (time_counter == 180 && !flag[4])
               {
                 flag[4] = TRUE;
                 use_countdown = TRUE;
                 countdown = 20;
               }

               // Release no 3
               if (time_counter > 198 && countdown <= 0 && !flag[5])
               {
                 flag[5] = TRUE;
                 use_countdown = FALSE;
                 object[10].frame = 0; object[10].maxframe = 0; object[10].startframe = 0;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION3].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION3].text2,90,3,col_orange,45);
                 enemy[3].chase = enemy[5].chase = TRUE;
               }

               // countdown for no 4
               if (time_counter == 280 && !flag[6])
               {
                 flag[6] = TRUE;
                 use_countdown = TRUE;
                 countdown = 20;
               }

               // Release no 4
               if ((time_counter > 298 && countdown <= 0 && !flag[7]))
               {
                 flag[7] = TRUE;
                 use_countdown = FALSE;
                 object[11].frame = 0; object[11].maxframe = 0; object[11].startframe = 0;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION4].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION4].text2,90,3,col_orange,45);
                 enemy[4].chase = enemy[6].chase = enemy[7].chase = TRUE;
               }

               // If all dead: Mission part 2
               if (!enemy[0].active && !enemy[1].active && !enemy[2].active && !enemy[3].active &&
                   !enemy[4].active && !enemy[5].active && !enemy[6].active && !enemy[7].active &&
                   !flag[8])
               {
                 flag[8] = TRUE;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION6].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION6].text2,90,3,col_orange,45);
                 object[16].frame = object[17].frame = 0;
               }

               // Switches?
               if (flag[8] && object[16].frame == 1 && object[17].frame == 1 && !flag[9])
               {
                 flag[9] = TRUE;
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION7].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION7].text2,90,3,col_orange,45);
                 use_countdown = TRUE;
                 countdown = 90;

                 object[12].frame = 0; object[12].maxframe = 0; object[12].startframe = 0;
                 object[13].frame = 0; object[13].maxframe = 0; object[13].startframe = 0;
                 object[14].frame = 0; object[14].maxframe = 0; object[14].startframe = 0;
                 object[15].frame = 0; object[15].maxframe = 0; object[15].startframe = 0;

                 for (nr=9; nr <= 27; nr++)
                 {
                   enemy[nr].max_waypnt = 12;
                   enemy[nr].start_waypnt = 4;
                 }
               }

               // Time over?
               if (countdown <= 0 && flag[9] && !flag[10])
               {
                 flag[10] = TRUE;
                 level_end = END_GOOD;
               }
               break;

     case  9  : // Gravity Games
               show_level_info_message();

               // initial values
               if (!flag[0])
               {
                 fu1 = PARTICLE_GRAVITY;
                 fu2 = LEVEL_GRAVITY;
                 fu3 = BOMB_GRAVITY;
                 fu4 = -100;
                 PARTICLE_GRAVITY = fu1 * (fu4/100);
                 LEVEL_GRAVITY = fu2 * (fu4/100);
                 BOMB_GRAVITY = fu3 * (fu4/100);

                 show_infobox = TRUE;
                 strcpy(show_infobox_message,l_misc_message[MSG_LEVEL_MISC1].text1);
                 sprintf(show_infobox_message2,"%.0f%%",fu4);

                 flag[0] = TRUE;
               }

               // Generator 1 destroyed
               if (!object[0].active && !flag[1])
               {
                 if (flag[6])
                 {
                   if (fu4 < 100) fu4 += 0.1;
                   else flag[2] = TRUE;
                 }
                 else
                 {
                   if (fu4 < 3) fu4 += 0.1;
                   else flag[1] = TRUE;
                 }
                 sprintf(show_infobox_message2,"%.0f%%",fu4);
                 PARTICLE_GRAVITY = fu1 * (fu4/100);
                 LEVEL_GRAVITY = fu2 * (fu4/100);
                 BOMB_GRAVITY = fu3 * (fu4/100);
                 if (!flag[5])
                 {
                   show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION1].text1,15,col_yellow,col_white,0);
                   show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION1].text2,90,3,col_orange,45);
                   flag[5] = TRUE;
                 }
               }

               // Generator 2 destroyed
               if (!object[1].active && !flag[2])
               {
                 if (flag[5])
                 {
                   if (fu4 < 100) fu4 += 0.1;
                   else flag[2] = TRUE;
                 }
                 else
                 {
                   if (fu4 < 3) fu4 += 0.1;
                   else flag[2] = TRUE;
                 }
                 sprintf(show_infobox_message2,"%.0f%%",fu4);
                 PARTICLE_GRAVITY = fu1 * (fu4/100);
                 LEVEL_GRAVITY = fu2 * (fu4/100);
                 BOMB_GRAVITY = fu3 * (fu4/100);
                 if (!flag[6])
                 {
                   show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION2].text1,15,col_yellow,col_white,0);
                   show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION2].text2,90,3,col_orange,45);
                   flag[6] = TRUE;
                 }
               }

               // Grav stabilized
               if (flag[1] && flag[2] && !flag[3])
               {
                 if ( (global_saved == 8) )
                 {
                   show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION3].text1,15,col_yellow,col_white,0);
                   show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION3].text2,90,3,col_orange,45);
                   abort_start_message = FALSE;
                 }
                 else
                 {
                   show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION4].text1,15,col_yellow,col_white,0);
                   show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION4].text2,90,3,col_orange,45);
                   abort_start_message = FALSE;
                 }
                 flag[3] = TRUE;
               }
               break;

     case 10  : // Pacifism
               show_level_info_message();

               // Pulse Shooter on/off
               if (spobject[4].frame == 3 && enemy[49].react.xw1 == 0)
               {
                 enemy[49].react.xw1 = -800; enemy[49].react.xw2 = 800;
                 enemy[49].react.yw1 = -800; enemy[49].react.yw2 = 500;
               }
               else if (spobject[4].frame == 3 && enemy[49].react.xw1 != 0)
               {
                 enemy[49].react.xw1 = 0; enemy[49].react.xw2 = 0;
                 enemy[49].react.yw1 = 0; enemy[49].react.yw2 = 0;
               }

               // Big Stone movement
               if (spobject[5].frame == 3 && enemy[15].max_waypnt == 1)
                 enemy[15].max_waypnt = 2;
               else if (spobject[5].frame == 3 && enemy[15].max_waypnt == 2)
                 enemy[15].max_waypnt = 1;

               // Laser Doors
               if (time_counter % 2 == 0 && time_counter != flag[0])
               {
                 if (object[13].startframe == 0)
                 {
                   object[13].frame = 1; object[13].maxframe = 3; object[13].startframe = 1;
                   object[14].frame = 0; object[14].maxframe = 0; object[14].startframe = 0;
                   object[15].frame = 0; object[15].maxframe = 0; object[15].startframe = 0;
                   object[16].frame = 1; object[16].maxframe = 3; object[16].startframe = 1;
                   flag[0] = time_counter;
                 }
                 else if (object[15].startframe == 0)
                 {
                   object[13].frame = 0; object[13].maxframe = 0; object[13].startframe = 0;
                   object[14].frame = 1; object[14].maxframe = 3; object[14].startframe = 1;
                   object[15].frame = 1; object[15].maxframe = 3; object[15].startframe = 1;
                   object[16].frame = 0; object[16].maxframe = 0; object[16].startframe = 0;
                   flag[0] = time_counter;
                 }
               }

               // All packets safed?
               if (global_saved == 5 && !flag[1])
               {
                 show_normal_osd_message(impact10h,impact14h,"",l_action_message[MSG_LEVEL_ACTION1].text1,15,col_yellow,col_white,0);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION1].text2,90,3,col_orange,45);

                 object[11].frame = 0; object[11].maxframe = 0; object[11].startframe = 0;
                 object[12].frame = 0; object[12].maxframe = 0; object[12].startframe = 0;

                 flag[1] = TRUE;
               }

               break;

  } // switch current_level

  else if (game_mode == SP_TRAINING)
  switch (current_training_level)
  {
    case  1  : // Flying, Landing
               show_level_info_message();

               if (playship[0].land && playship[0].base == 1 && !flag[0])
               {
                 flag[0] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION1].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION1].text2,90,5,col_yellow,30);
                 abort_start_message = FALSE;
               }

               if (flag[0] && !flag[1] && playship[0].land && playship[0].base == 2)
               {
                 flag[1] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION2].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION2].text2,90,5,col_yellow,30);
                 abort_start_message = FALSE;
               }

               if (flag[1] && !flag[2] && playship[0].land && playship[0].base == 3)
               {
                 flag[2] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION3].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION3].text2,90,5,col_yellow,30);
                 abort_start_message = FALSE;
               }

               break;

    case  2  : // Cargo
               show_level_info_message();

               if (base[1].cargo == 0 && !flag[0])
               {
                 flag[0] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION1].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION1].text2,90,2,col_yellow,30);
               }

               if (base[3].cargo == 0 && !flag[1])
               {
                 flag[1] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION2].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION2].text2,90,2,col_yellow,30);
               }

               if (playship[0].land && playship[0].base == 0 && player_stat[0].cargo_lost && !flag[2])
               {
                 flag[2] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION4].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION4].text2,90,2,col_yellow,30);
               }
               if (global_saved == 11 && playship[0].land && playship[0].base == 0 && !flag[3])
               {
                 flag[3] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION3].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION3].text2,90,2,col_yellow,30);
               }

               break;

    case  3  : // Fighting
               show_level_info_message();

               if (playship[0].land && playship[0].base == 1 && player_stat[0].enemies_killed == 11 && !flag[0])
               {
                 flag[0] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION1].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION1].text2,90,2,col_yellow,30);
               }
               break;

    case  4  : // Weapons
               show_level_info_message();

               if (player_stat[0].enemies_killed == 2 && !flag[0])
               {
                 flag[0] = TRUE;
                 object[11].frame = 0; object[11].maxframe = 0; object[11].startframe = 0;
                 playship[0].START_BASE = 1;
               }

               if (player_stat[0].enemies_killed == 9 && !flag[1])
               {
                 flag[1] = TRUE;
                 object[12].frame = 0; object[12].maxframe = 0; object[12].startframe = 0;
                 object[11].frame = 1; object[11].maxframe = 3; object[11].startframe = 1;
                 playship[0].START_BASE = 2;
               }

               if (player_stat[0].enemies_killed == 16 && !flag[2])
               {
                 flag[2] = TRUE;
                 object[13].frame = 0; object[13].maxframe = 0; object[13].startframe = 0;
                 object[12].frame = 1; object[12].maxframe = 3; object[12].startframe = 1;
                 playship[0].START_BASE = 3;
               }

               if (player_stat[0].enemies_killed == 17 && !object[7].active &&
                   !object[8].active && !object[9].active && !object[10].active
                   && !flag[3])
               {
                 flag[3] = TRUE;
                 object[14].frame = 0; object[14].maxframe = 0; object[14].startframe = 0;
                 object[13].frame = 1; object[13].maxframe = 3; object[13].startframe = 1;
                 playship[0].START_BASE = 4;
               }

               if (player_stat[0].enemies_killed == 24 && !flag[4])
               {
                 flag[4] = TRUE;
                 object[15].frame = 0; object[15].maxframe = 0; object[15].startframe = 0;
                 object[14].frame = 1; object[14].maxframe = 3; object[14].startframe = 1;
                 playship[0].START_BASE = 5;
               }

               if (player_stat[0].enemies_killed == 31 && !flag[5])
               {
                 flag[5] = TRUE;
                 object[16].frame = 0; object[16].maxframe = 0; object[16].startframe = 0;
                 object[15].frame = 1; object[15].maxframe = 3; object[15].startframe = 1;
                 playship[0].START_BASE = 6;
               }

               // Level beendet
               if (player_stat[0].enemies_killed == 54 && !flag[6])
               {
                 flag[6] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION7].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION7].text2,90,2,col_yellow,30);
               }

               // Aufnehmen von Ladung
               if (base[1].cargo == 0 && !flag[7])
               {
                 flag[7] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION1].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION1].text2,90,2,col_yellow,30);
               }
               if (base[2].cargo == 0 && !flag[8])
               {
                 flag[8] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION2].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION2].text2,90,2,col_yellow,30);
               }
               if (base[3].cargo == 0 && !flag[9])
               {
                 flag[9] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION3].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION3].text2,90,2,col_yellow,30);
               }
               if (base[4].cargo == 0 && !flag[10])
               {
                 flag[10] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION4].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION4].text2,90,2,col_yellow,30);
               }
               if (base[5].cargo == 0 && !flag[11])
               {
                 flag[11] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION5].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION5].text2,90,2,col_yellow,30);
               }

               // Letzter Raum
               if (playship[0].ypos < object[16].ypos && !flag[12])
               {
                 flag[12] = TRUE;
                 show_normal_osd_message(impact14h,impact10h,l_action_message[MSG_LEVEL_ACTION6].text1,"",15,col_white,col_white,1);
                 show_typewriter_osd_message(impact10h,l_action_message[MSG_LEVEL_ACTION6].text2,90,2,col_yellow,30);
               }

               break;

    case  5  : // Testlevel
               show_level_info_message();

//               if (game_time < 3) if ((map_x[1]+scroll_x[1] <= 650) && (map_y[1]+scroll_y[1] <= 500)) add_pixels(1,150,150,0.6,0.9,400,tbl_blue,10,30,NULL);
//               if (game_time < 3) add_pixels(1,1050,197,0.6,0.5,400,tbl_blue,10,0,NULL);
               if (overall_gametime % 2 == 0) add_pixels(2,660,197,-0.4,0.8,400,tbl_blue,10,0,300,300,0);
               if (overall_gametime % 2 == 0) add_pixels(2,665,187,tspd,1.4,400,tbl_red,10,0,700,300,0);
               if (key[KEY_O]) { tspd+=0.01; clear_keybuf(); }
               if (key[KEY_I]) { tspd-=0.01; clear_keybuf(); }
               if (object[6].frame == 1)
               {
//                 add_pixels(1,250,460,0.6,0.9,400,tbl_red,10,100,NULL);
                 if (object[14].maxframe == 0)
                 {
                   object[14].frame = 1; object[14].maxframe = 3; object[14].startframe = 1;
                   object[12].frame = 0; object[12].maxframe = 0; object[12].startframe = 0;
                   object[13].frame = 0; object[13].maxframe = 0; object[13].startframe = 0;
                 }
               enemy[0].react.xw1 = -4047; enemy[0].react.xw2 = 4407;
               enemy[0].react.yw1 = -4000; enemy[0].react.yw2 = 3000;
               }
               else if (object[6].frame == 0)
               {
                 if (object[14].maxframe == 3)
                 {
                   object[14].frame = 0; object[14].maxframe = 0; object[14].startframe = 0;
                   object[12].frame = 1; object[12].maxframe = 3; object[12].startframe = 1;
                   object[13].frame = 1; object[13].maxframe = 3; object[13].startframe = 1;
                 }
               enemy[0].react.xw1 = 0; enemy[0].react.xw2 = 0;
               enemy[0].react.yw1 = 0; enemy[0].react.yw2 = 0;
               }

               if (playship[0].land && (playship[0].base == 0) && (playship[0].ldur == 1))
               {
//               show_normal_osd_message(impact10h,impact14h,"WELCOME TO","GRAVITY FORCE!",3,col_white,col_orange,0);
//               show_typewriter_osd_message(impact10h,"THIS IS YOUR HOMEBASE.\nPRESS SPACE TO LIFT OFF.\nFIRE WITH ARROW UP & HAVE FUN!",60,5,col_yellow,50);
               }
               break;
  }

  else if (game_mode == SP_RACE)
  switch (current_sprace_level)
  {
    case  1  : // Race 1
               show_level_info_message();
               break;

    case  2  : // Race 2
               show_level_info_message();
               break;

    case  3  : // Race 3
               show_level_info_message();

               // Fountaine
               if (overall_gametime % 2 == 0)
                 add_pixels(2,object[8].xpos,object[8].ypos+7,
                            -0.9,0.4,400,tbl_blue,10,0,300,300,0);

               break;

  }

  else if (game_mode == MP_2PDOGFIGHT)
  switch (current_qdogfight_level)
  {
    case  1  : // Arena 1
               show_level_info_message();

               // adjust gravity
               if (!flag[0])
               {
                 fu1 = PARTICLE_GRAVITY;
                 fu2 = LEVEL_GRAVITY;
                 fu3 = BOMB_GRAVITY;
                 fu4 = config_file.config.def_gravity;

                 PARTICLE_GRAVITY = fu1 * (fu4/100);
                 LEVEL_GRAVITY = fu2 * (fu4/100);
                 BOMB_GRAVITY = fu3 * (fu4/100);

                 flag[0] = TRUE;
               }
               break;

    case  2  : // Arena 2
               show_level_info_message();

               // adjust gravity
               if (!flag[0])
               {
                 fu1 = PARTICLE_GRAVITY;
                 fu2 = LEVEL_GRAVITY;
                 fu3 = BOMB_GRAVITY;
                 fu4 = config_file.config.def_gravity;

                 PARTICLE_GRAVITY = fu1 * (fu4/100);
                 LEVEL_GRAVITY = fu2 * (fu4/100);
                 BOMB_GRAVITY = fu3 * (fu4/100);

                 flag[0] = TRUE;
               }

               break;

    case  3  : // Arena 3
               show_level_info_message();

               // adjust gravity
               if (!flag[0])
               {
                 fu1 = PARTICLE_GRAVITY;
                 fu2 = LEVEL_GRAVITY;
                 fu3 = BOMB_GRAVITY;
                 fu4 = config_file.config.def_gravity;

                 PARTICLE_GRAVITY = fu1 * (fu4/100);
                 LEVEL_GRAVITY = fu2 * (fu4/100);
                 BOMB_GRAVITY = fu3 * (fu4/100);

                 flag[0] = TRUE;
               }

               break;

    case  4  : // Arena 4
               show_level_info_message();

               h = (show_panel) ? PLAYSCREEN_HEIGHT : PLAYSCREEN_HEIGHT+SCORE_HEIGHT;

               // adjust gravity
               if (!flag[0])
               {
                 fu1 = PARTICLE_GRAVITY;
                 fu2 = LEVEL_GRAVITY;
                 fu3 = BOMB_GRAVITY;
                 fu4 = config_file.config.def_gravity;

                 PARTICLE_GRAVITY = fu1 * (fu4/100);
                 LEVEL_GRAVITY = fu2 * (fu4/100);
                 BOMB_GRAVITY = fu3 * (fu4/100);

                 flag[0] = TRUE;
               }


               // Player outside screen? Beam to other side
               for (nr=0; nr < player_anz; nr++)
                 if (playship[nr].xpos < l_scroll_x[0]-10) playship[nr].xpos = l_scroll_x[0]+SCREEN_W;
                 else if (playship[nr].xpos > l_scroll_x[0]+SCREEN_W) playship[nr].xpos = l_scroll_x[0]-10;
                 else if (playship[nr].ypos < l_scroll_y[0]-15) playship[nr].ypos = l_scroll_y[0]+h+15;
                 else if (playship[nr].ypos > l_scroll_y[0]+h+15) playship[nr].ypos = l_scroll_y[0]-15;

               temp = first_bullet;

               // same to weapons
               while (temp!=NULL)
               {
                 next_bull = temp->next;

                 if (temp->dat.xpos < l_scroll_x[0]-10) temp->dat.xpos = l_scroll_x[0]+SCREEN_W;
                 else if (temp->dat.xpos > l_scroll_x[0]+SCREEN_W) temp->dat.xpos = l_scroll_x[0]-10;
                 else if (temp->dat.ypos < l_scroll_y[0]-15) temp->dat.ypos = l_scroll_y[0]+h+15;
                 else if (temp->dat.ypos > l_scroll_y[0]+h+15) temp->dat.ypos = l_scroll_y[0]-15;

                 temp = next_bull;
               } // while

               break;

    case  5  : // Arena 5
               show_level_info_message();
			   			

               h = (show_panel) ? PLAYSCREEN_HEIGHT : PLAYSCREEN_HEIGHT+SCORE_HEIGHT;

               // adjust gravity
               if (!flag[0])
               {
                 fu1 = PARTICLE_GRAVITY;
                 fu2 = LEVEL_GRAVITY;
                 fu3 = BOMB_GRAVITY;
                 fu4 = config_file.config.def_gravity;

                 PARTICLE_GRAVITY = fu1 * (fu4/100);
                 LEVEL_GRAVITY = fu2 * (fu4/100);
                 BOMB_GRAVITY = fu3 * (fu4/100);

                 flag[0] = TRUE;
               }

               // Player outside screen? Beam to other side
               for (nr=0; nr < player_anz; nr++)
                 if (playship[nr].xpos < l_scroll_x[0]-10) playship[nr].xpos = l_scroll_x[0]+SCREEN_W;
                 else if (playship[nr].xpos > l_scroll_x[0]+SCREEN_W) playship[nr].xpos = l_scroll_x[0]-10;
                 else if (playship[nr].ypos < l_scroll_y[0]-15) playship[nr].ypos = l_scroll_y[0]+h+15;
                 else if (playship[nr].ypos > l_scroll_y[0]+h+15) playship[nr].ypos = l_scroll_y[0]-15;

               temp = first_bullet;

               // same to weapons
               while (temp!=NULL)
               {
                 next_bull = temp->next;

                 if (temp->dat.xpos < l_scroll_x[0]-10) temp->dat.xpos = l_scroll_x[0]+SCREEN_W;
                 else if (temp->dat.xpos > l_scroll_x[0]+SCREEN_W) temp->dat.xpos = l_scroll_x[0]-10;
                 else if (temp->dat.ypos < l_scroll_y[0]-15) temp->dat.ypos = l_scroll_y[0]+h+15;
                 else if (temp->dat.ypos > l_scroll_y[0]+h+15) temp->dat.ypos = l_scroll_y[0]-15;

                 temp = next_bull;
               } // while	   

               break;

  }

}

void level_end_check()
{

  if (game_mode == SP_CAMPAIGN)
  switch (current_level)
  {
    case  1  : // Gravity Force
               if (global_saved == 3) level_end = END_GOOD;
               else if (player_stat[0].cargo_lost) level_end = END_BAD;
               if ((playship[0].lifes <= 0) && (temp_counter == 0) && (player_dead_counter >= NEW_SHIP_TIME))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_GAMEOVER].text1,100,4,col_yellow,36);
                 temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               if ((level_end == END_GOOD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_COMPLETE].text1,100,4,col_yellow,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               else if ((level_end == END_BAD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_FAILED].text1,100,4,col_orange,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               break;
    case  2  : // Easy, but tricky
               if (global_saved == 5) level_end = END_GOOD;
               if (player_stat[0].cargo_lost) level_end = END_BAD;
               if ((playship[0].lifes <= 0) && (temp_counter == 0) && (player_dead_counter >= NEW_SHIP_TIME))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_GAMEOVER].text1,100,4,col_yellow,36);
                 temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               if ((level_end == END_GOOD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_COMPLETE].text1,100,4,col_yellow,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               else if ((level_end == END_BAD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_FAILED].text1,100,4,col_orange,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               break;
    case  3  : // Bombing Run
               if ((playship[0].lifes <= 0) && (temp_counter == 0) && (player_dead_counter >= NEW_SHIP_TIME))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_GAMEOVER].text1,100,4,col_yellow,36);
                 temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               if ((level_end == END_GOOD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_COMPLETE].text1,100,4,col_yellow,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               else if ((level_end == END_BAD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_FAILED].text1,100,4,col_orange,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               break;
    case  4  : // Heavy Load
               if (global_saved == 5) level_end = END_GOOD;
               if (player_stat[0].cargo_lost) level_end = END_BAD;
               if ((playship[0].lifes <= 0) && (temp_counter == 0) && (player_dead_counter >= NEW_SHIP_TIME))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_GAMEOVER].text1,100,4,col_yellow,36);
                 temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               if ((level_end == END_GOOD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_COMPLETE].text1,100,4,col_yellow,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               else if ((level_end == END_BAD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_FAILED].text1,100,4,col_orange,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               break;
    case  5  : // Big Brother I
               if (global_saved == 1 && playship[0].ldur >= 320) level_end = END_GOOD;
               else if (player_stat[0].cargo_lost) level_end = END_BAD;
               if ((playship[0].lifes <= 0) && (temp_counter == 0) && (player_dead_counter >= NEW_SHIP_TIME))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_GAMEOVER].text1,100,4,col_yellow,36);
                 temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               if ((level_end == END_GOOD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_COMPLETE].text1,100,4,col_yellow,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               else if ((level_end == END_BAD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_FAILED].text1,100,4,col_orange,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 300; p_fade_speed = 0.2;
               }
               break;
    case  6  : // Big Brother II
               if ((playship[0].lifes <= 0) && (temp_counter == 0) && (player_dead_counter >= NEW_SHIP_TIME))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_GAMEOVER].text1,100,4,col_yellow,36);
                 temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               if ((level_end == END_GOOD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_COMPLETE].text1,100,4,col_yellow,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               else if ((level_end == END_BAD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_FAILED].text1,100,4,col_orange,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 300; p_fade_speed = 0.2;
               }
               break;
    case  7  : // Big Brother III
               if ((playship[0].lifes <= 0) && (temp_counter == 0) && (player_dead_counter >= NEW_SHIP_TIME))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_GAMEOVER].text1,100,4,col_yellow,36);
                 temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               if ((level_end == END_GOOD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_COMPLETE].text1,100,4,col_yellow,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               else if ((level_end == END_BAD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_FAILED].text1,100,4,col_orange,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 300; p_fade_speed = 0.2;
               }
               break;
    case  8  : // Deathmatch
               if ((playship[0].lifes <= 0) && (temp_counter == 0) && (player_dead_counter >= NEW_SHIP_TIME))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_GAMEOVER].text1,100,4,col_yellow,36);
                 temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               if ((level_end == END_GOOD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_COMPLETE].text1,100,4,col_yellow,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               else if ((level_end == END_BAD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_FAILED].text1,100,4,col_orange,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 300; p_fade_speed = 0.2;
               }
               break;
    case  9  : // Gravity Games
               if (global_saved == 8 && flag[1] && flag[2] && playship[0].land && playship[0].base == 0) level_end = END_GOOD;
               else if (player_stat[0].cargo_lost) level_end = END_BAD;
               if ((playship[0].lifes <= 0) && (temp_counter == 0) && (player_dead_counter >= NEW_SHIP_TIME))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_GAMEOVER].text1,100,4,col_yellow,36);
                 temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               if ((level_end == END_GOOD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_COMPLETE].text1,100,4,col_yellow,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               else if ((level_end == END_BAD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_FAILED].text1,100,4,col_orange,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               break;

    case 10  : // Pacifism
               if (global_saved >= 5 && flag[1] && playship[0].land && playship[0].base == 7) level_end = END_GOOD;
               else if (player_stat[0].cargo_lost > CARGO_WEIGHT) level_end = END_BAD;
               if ((playship[0].lifes <= 0) && (temp_counter == 0) && (player_dead_counter >= NEW_SHIP_TIME))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_GAMEOVER].text1,100,4,col_yellow,36);
                 temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               if ((level_end == END_GOOD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_COMPLETE].text1,100,4,col_yellow,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               else if ((level_end == END_BAD) && (temp_counter == 0))
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_FAILED].text1,100,4,col_orange,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               break;

  } // switch current_level

  if (game_mode == SP_TRAINING)
  switch (current_training_level)
  {
    case  1  : // Flying, Landing
               if (flag[2] && playship[0].ldur > 700) level_end = END_GOOD;
               if ((level_end == END_GOOD) && (temp_counter == 0))
               {
                 temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               break;
    case  2  : // Cargo
               if ((flag[2] || flag[3]) && playship[0].ldur > 700) level_end = END_GOOD;
               if ((level_end == END_GOOD) && (temp_counter == 0))
               {
                 temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               break;
    case  3  : // Fighting
               if (flag[0] && playship[0].ldur > 700) level_end = END_GOOD;
               if ((level_end == END_GOOD) && (temp_counter == 0))
               {
                 temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               break;
    case  4  : // Weapons
               if (flag[6]) level_end = END_GOOD;
               if (flag[6] && (temp_counter == 0) && playship[0].land)
               {
                 temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               break;
    case  5  : // Test level
               break;
  }

  else if (game_mode == SP_RACE)
  switch (current_sprace_level)
  {
    case  1  : // Race 1
               if (race_done && (playship[0].dead || playship[0].land) && temp_counter == 0)
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_RACE_COMPLETE].text1,100,4,col_yellow,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               break;

    case  2  : // Race 2
               if (race_done && (playship[0].dead || playship[0].land) && temp_counter == 0)
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_RACE_COMPLETE].text1,100,4,col_yellow,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               break;

    case  3  : // Race 3
               if (race_done && (playship[0].dead || playship[0].land) && temp_counter == 0)
               {
                 clear_osd(1); show_typewriter_osd_message(impact14h,mission_message[MSG_MISSION_RACE_COMPLETE].text1,100,4,col_yellow,36); temp_counter = counter;
                 p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0; p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 200; p_fade_speed = 0.3;
               }
               break;

  }

  else if (game_mode == MP_2PDOGFIGHT)
  switch (current_qdogfight_level)
  {
    case  1  : // Arena 1
               break;
  }

}

