/*
 *    GRAVITY FORCE, gf.c
 *
 *    -- main function, handles the initialization/beginning of the
 *       different game types
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gflink.h"
#include "gfinit.h"
#include "gfcnfg.h"
#include "gflog.h"
#include "gfosd.h"
#include "gfstat.h"
#include "gf.h"
#include "gfgsngle.h"
#include "gfgsrace.h"
#include "gfg2pdog.h"

int main(int argc, char *argv[])
{
  int tst=FALSE;
  int weiter=TRUE;
  extern int actual_menu;
  extern int level_end;

  gf_platform_init();

  gpause = FALSE;

  first_bullet = NULL; last_bullet = NULL;
  first_pixel = NULL; last_pixel = NULL;
  first_explosion = NULL; last_explosion = NULL;
  first_crater = NULL; last_crater = NULL;
  first_rect = NULL; last_rect = NULL;
  rl.count = orl.count = 0;

  play_sound = TRUE;

  player_anz = 1;
  show_panel = TRUE;

  cur_max_levels = 10;
  cur_max_race_levels = 3;
  cur_max_dogfight_levels = 0;
  cur_max_qdogfight_levels = 5;
  cur_max_training_levels = 5;
  current_level = 1;
  current_sprace_level = 1;
  current_training_level = 1;
  current_dogfight_level = 1;
  current_qdogfight_level = 1;
  current_mprace_level = 1;

  actual_menu = 0;

  // Levelfiles schreiben?
  write_files = FALSE;

  setup_global_stuff_once();
  setup_global_stuff();
  read_config_file();

  USE_JOYSTICK = FALSE;
  USE_CHEATS = FALSE;

  if (argc==2)
  {
    if (strstr(argv[1],"cheat")) USE_CHEATS = TRUE;
  }
/*
  config_file.config.level_available[5] = FALSE;
  config_file.config.level_available[6] = FALSE;
  config_file.config.level_available[7] = FALSE;
  config_file.config.level_available[8] = FALSE;
  config_file.config.level_available[9] = FALSE;
  config_file.config.level_available[10] = FALSE;
*/
//  show_intro();

  init_first();

  do
  {
    init(tst);

//    init_network();
//    network_game = FALSE;

    switch (game_mode)
    {
       case SP_CAMPAIGN :
                          while (play_sp_level() == END_GOOD)
                          {
                            // Nexter Level
                            write_files = FALSE;
                            if (current_level < cur_max_levels) current_level++;
                              else current_level = 1;
                            osd_buf.active = FALSE;
                            clear_osd(0);
                            init_spec(tst);
                            level_end = 0;
                            set_palette_range(black_palette,0,255,TRUE);
                            clear_keybuf();
                            config_file.config.level_available[current_level] = TRUE;
                          }
                          break;

      case SP_RACE :
                          play_sp_race_level();
                          break;

      case SP_TRAINING :
                          while (play_sp_level() == END_GOOD)
                          {
                            // Nexter Level
                            write_files = FALSE;
                            if (current_training_level < cur_max_training_levels) current_training_level++;
                              else current_training_level = 1;
                            osd_buf.active = FALSE;
                            clear_osd(0);
                            init_spec(tst);
                            level_end = 0;
                            set_palette_range(black_palette,0,255,TRUE);
                            clear_keybuf();
                          }
                          break;

      case MP_2PDOGFIGHT :
                          play_2pdog_level();
                          break;

      case MP_RACE :
                          break;

      case MP_DOGFIGHT :
                          break;

    } // switch game_mode

    free_all();
//    allegro_exit();

    if (tst) exit(0);

  } while (weiter);

  write_config_file();
  close_log();

//  if (network_game) network_close();
  clear_keybuf();
  exit(0);
  return 0;
}

END_OF_MAIN()

