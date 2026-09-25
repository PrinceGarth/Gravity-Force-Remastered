/*
 *    GRAVITY FORCE, gfg2pdog.c
 *
 *    -- Specific main game loop for 2 Player Dogfight
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gf.h"
#include "gfdraw.h"
#include "gfinit.h"
#include "gfpanel.h"
#include "gfcontrl.h"
#include "gfpal.h"
#include "gflevel.h"
#include "gfosd.h"
#include "gfsenm.h"
#include "gfsobj.h"
#include "gfspibul.h"
#include "gfspln.h"
#include "gflink.h"
#include "gfmisc.h"
#include "gfcnfg.h"
#include "gfnetw.h"
#include "gfstat.h"
#include "gfmes.h"
#include "gfmap.h"
#include "string.h"
#include "gflog.h"
#include "mappyal.h"
#include "gfg2pdog.h"

int update_graphics;


static void fps_timer(void)
{
  fpst = fps;
  fps = 0;
}
static END_OF_FUNCTION(fps_timer);

static void game_timer(void)
{
  if (!gpause)
  {
    game_time++;
    overall_gametime++;
  }
}
static END_OF_FUNCTION(game_timer);

static void second_timer(void)
{
  if (!gpause)
  {
    second_time++;
    osd_counter--;
  }
}
static END_OF_FUNCTION(second_timer);

int play_2pdog_level()
{
  int endbed=0,endthis;
  int osd_on = FALSE;
  int osd_x,osd_y;
  int cur_second,cur60_second1,cur60_second2;
  int actual_time;
  extern int level_end;
  extern int radar_zoom;
  extern int crater_draw;
  extern CNODE *crater_draw_c;
  int esc_pressed = FALSE;
  int warn5m=FALSE,warn1m=FALSE,warn20s=FALSE;
  char warn1[50],warn2[50];

  srand(time(0));
  seed = rand() % 4700;

  fps = fpst = 0;

  LOCK_VARIABLE(game_time);
  LOCK_VARIABLE(fps);
  LOCK_VARIABLE(fpst);
  LOCK_VARIABLE(second_time);
  LOCK_FUNCTION(fps_timer);
  LOCK_FUNCTION(game_timer);
  LOCK_FUNCTION(second_timer);

  install_int(fps_timer,1000);
  install_int_ex(game_timer,BPS_TO_TIMER(60));
  install_int_ex(second_timer,SECS_TO_TIMER(1));

  player_anz = 2;

  init_ship(0);
  init_ship(1);

  map_x[1] = 0;
  map_y[1] = 0;

  map_stuff(0);

  scroll_x[1] = l_scroll_x[cur_l];
  scroll_y[1] = l_scroll_y[cur_l];

  scroll_to(l_scroll_x[cur_l],l_scroll_y[cur_l],1000);
  do_scroll();

  redraw = TRUE;
  MapDraw8BG(vscreen,map_x[1],map_y[1],0,0,PLAYSCREEN_WIDTH+PLUS_X,PLAYSCREEN_HEIGHT+PLUS_Y);

  osd_x = PLAYSCREEN_WIDTH/2 - osd->w/2;
  osd_y = 100;

  radar_zoom = 5;

  if (show_panel) draw_panel();

  counter = 0;

  endthis = FALSE;
  level_end = 0;

  fade_count = fade_count_to = 0;

  fade_out_active = TRUE; fade_pos = 0; fade_opos = 0; fade_type = 2; fade_count = 0; fade_count_to = 65; fade_speed = 1;

  abort_start_message = FALSE;

  memset(&player_stat,0,sizeof(player_stat));

  update_graphics = FALSE;

  open_demo_file("level2.fil",0);

  time_counter = 0;
  overall_counter = 0;
  overall_gametime = 0;
  game_time = 0;
  actual_time = 0;
  second_time = 0;
  cur_second = 0;
  cur60_second1 = cur60_second2 = 0;
  countdown = 0; use_countdown = FALSE;
  crater_draw = FALSE;
  fade_from_nr = 0; fade_to_nr = 255;

  start_lifes = playship[0].lifes;

  time_counter = 0;

  sprintf(upanel_text1,"%s %02d:%02d",other_message[MSG_OTHER_TIME].text1,time_counter / 60,time_counter % 60);
  update_upanel_time();

  while ( (!endthis) )
  {
    while (game_time > actual_time)
    {
      actual_time++;
      overall_counter++;

      if (key[KEY_ESC] && !esc_pressed)
      {
        esc_pressed = TRUE;
        p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0;
        p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 40;
        p_fade_speed = 1.5;
        clear_keybuf();
      }

      CheckFade();

      if ((playship[0].lifes == 0) || (level_end)) if (p_fade_out_active && (p_fade_type == 1)) if (p_fade_count >= p_fade_count_to) endthis = TRUE;
      if (esc_pressed) if (p_fade_out_active && (p_fade_type == 1)) if (p_fade_count >= p_fade_count_to) endthis = TRUE;

      draw_objects(vscreen,0);

      read_keys(0);
      read_keys(1);

      if (use_joystick) poll_joystick();

      update_typewriter_osd_message();

      do_enemies();
      do_objects();
      do_spobjects();
      do_bullets(0);
      do_pixels(0);

      if (key[KEY_N]) { scroll_to(l_scroll_x[cur_l],200,6); clear_keybuf(); }
      if (key[KEY_M]) { scroll_to(l_scroll_x[cur_l],l_scroll_y[cur_l],3); clear_keybuf(); }

      do_scroll();
     
      update_graphics = TRUE;
    }  // while game_time > actual_time

    if (update_graphics)
    {
      if (redraw)
      {
        update_screen_map();
        redraw = FALSE;
      } // if redraw
  
      // Playerarea sichern...
      if (!playship[0].dead) blit(vscreen,playship[0].player_temp,playship[0].xpos-5-map_x[1],playship[0].ypos-2-map_y[1],0,0,19,18);
      if (!playship[1].dead) blit(vscreen,playship[1].player_temp,playship[1].xpos-5-map_x[1],playship[1].ypos-2-map_y[1],0,0,19,18);

      draw_bases(vscreen,0);

      draw_enemies(vscreen,0);
      if (!NO_CLIP) check_collisions(vscreen,0);
      if (!NO_CLIP) check_collisions(vscreen,1);
      level_end_check();

      level_runtime_checks();
  
      draw_bullets(vscreen,0);
  
      draw_spobjects(vscreen,0);
      do_explosions(vscreen,0);

      if (crater_draw)
      {
        if (crater_draw_c)
          draw_crater(vscreen,crater_draw_c->dat.x,crater_draw_c->dat.y,crater_draw_c->dat.size);
        crater_draw = FALSE;
        crater_draw_c = NULL;
      }

      draw_pixels(vscreen,0);
  
      draw_player(vscreen,0);
      draw_player(vscreen,1);

      draw_cargo(vscreen,0);

//      update_panel_infobox();
	  
      // Sekundenzeugs
      if (second_time != cur_second)
      {
        cur_second = second_time;
        if (cur60_second1 < playship[0].SHIELD_RECHARGE) cur60_second1++;
        else
        {
          cur60_second1 = 0;
          if ((playship[0].shield_active) && (playship[0].shield < MAX_SHIELD))
          {
            playship[0].shield++;
            update_panel_shield();
          }
        }

        if (cur60_second2 < playship[1].SHIELD_RECHARGE) cur60_second2++;
        else
        {
          cur60_second2 = 0;
          if ((playship[1].shield_active) && (playship[1].shield < MAX_SHIELD))
          {
            playship[1].shield++;
            update_panel_shield();
          }
        }

        // Warn at 5 and 1 minutes
        if (time_limit*60-time_counter == 60*5 && !warn5m)
        {
          warn5m = TRUE;
          strcpy(warn1,other_message[MSG_OTHER_REMTIME].text1);
          sprintf(warn2,"5 %s", strupr(other_message[MSG_OTHER_MINUTES].text1));
          show_normal_osd_message(impact10h,impact14h,warn1,warn2,2,col_yellow,col_white,0);
        }
        if (time_limit*60-time_counter == 60 && !warn1m)
        {
          warn1m = TRUE;
          strcpy(warn1,other_message[MSG_OTHER_REMTIME].text1);
          sprintf(warn2,"1 %s", strupr(other_message[MSG_OTHER_MINUTES].text1));
          warn2[strlen(warn2)-2] = 0;
          show_normal_osd_message(impact10h,impact14h,warn1,warn2,2,col_yellow,col_white,0);
        }
        if (time_limit*60-time_counter == 20 && !warn20s)
        {
          warn20s = TRUE;
          strcpy(warn1,other_message[MSG_OTHER_REMTIME].text1);
          sprintf(warn2,"20 %s", strupr(other_message[MSG_OTHER_SECONDS].text1));
          show_normal_osd_message(impact10h,impact14h,warn1,warn2,2,col_yellow,col_white,0);
        }

        update_panel_lifes();
        update_panel_weapons();

        if (playship[0].tele)
        {
          if (playship[0].telet > 0) playship[0].telet--;
          if (playship[0].telet == 0) playship[0].tele = 0;
        }
        if (playship[1].tele)
        {
          if (playship[1].telet > 0) playship[1].telet--;
          if (playship[1].telet == 0) playship[1].tele = 0;
        }
  
        // game counter (every second)
        counter++;
        if (abort_start_message) time_counter++;

        // Time/Frag limit exceeded?
        if (time_counter >= time_limit*60 || player_stat[0].frags >= frag_limit || player_stat[1].frags >= frag_limit)
        {
          p_fade_out_active = TRUE; p_fade_pos = 0; p_fade_opos = 0;
          p_fade_type = 1; p_fade_count = 0; p_fade_count_to = 40;
          p_fade_speed = 1.5;
          level_end = TRUE;
        }
        
      } // if second_time...

      // OSD anzeigen
      if (message && osd_counter)
      {
        blit(vscreen,osdb,osd_x+scroll_x[1],osd_y+scroll_y[1],0,0,osd->w,osd->h);
        masked_blit(osd,vscreen,0,0,osd_x+scroll_x[1],osd_y+scroll_y[1],osd->w,osd->h);
        osd_on = TRUE;
      }

      acquire_screen();

      if (show_panel)
      {
        blit(vscreen,screen,scroll_x[1],scroll_y[1],PLAYSCREEN_XSTART,PLAYSCREEN_YSTART,PLAYSCREEN_WIDTH,PLAYSCREEN_HEIGHT+USCORE_HEIGHT);
        blit(score_table,screen,0,0,0,SCREEN_H-SCORE_HEIGHT,SCORE_WIDTH,SCORE_HEIGHT);
//        blit(uscore_table,screen,0,0,0,0,SCORE_WIDTH,USCORE_HEIGHT);
      }
      else
        blit(vscreen,screen,scroll_x[1],scroll_y[1],PLAYSCREEN_XSTART,PLAYSCREEN_YSTART,PLAYSCREEN_WIDTH,PLAYSCREEN_HEIGHT+SCORE_HEIGHT+USCORE_HEIGHT);

      release_screen();

      // OSD Background restore
      if ((message && osd_counter) || (osd_on))
      {
        blit(osdb,vscreen,0,0,osd_x+scroll_x[1],osd_y+scroll_y[1],osd->w,osd->h);
        osd_on = FALSE;
      }

      if (!osd_counter) { message = FALSE; osd_on = FALSE; }

      clear_screen(1);
      clear_explosions();

      update_graphics = FALSE;
      fps++;
    } // update_graphics

    if (save_screen)
    {
      make_screenshot();
      save_screen = FALSE;
    }

    // pause
    if (key[KEY_PAUSE]) { gpause = TRUE; clear_keybuf(); while (!keypressed()); gpause = FALSE; }

  } // while !keyESC


  clear(vscreen); clear(screen);

  close_demo_file();

  remove_int(fps_timer);
  remove_int(game_timer);
  remove_int(second_timer);

  if (key[KEY_ESC]) endbed = 0;
  else if (playship[0].lifes <= 0) endbed = 1;
  else if (level_end) endbed = level_end;

  // Stats schreiben
  player_stat[0].time = time_counter;
  player_stat[1].time = time_counter;

  clear_keybuf();

  generate_2pdog_statistics();

  return endbed;
}
