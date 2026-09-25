/*
 *    GRAVITY FORCE, gf.h
 *
 *    -- some globals needed for all game modes    
 *
 *
 *
 *    see source.txt for further information
 */

char tmpstr[100];
volatile int game_time,fps,fpst,second_time,hundred_time,tele,telet;
long int overall_counter;
int net_send_count;
int seed;
volatile int overall_gametime;
int gpause;
int start_lifes;
BITMAP *mapbit;
int time_counter;
int USE_CHEATS;

enum GAME_MODE {
  SP_CAMPAIGN,
  SP_RACE,
  SP_TRAINING,
  MP_DOGFIGHT,
  MP_RACE,
  MP_2PDOGFIGHT
};

enum GAME_MODE game_mode;


int current_level,current_sprace_level,current_mprace_level,current_dogfight_level,current_training_level,current_qdogfight_level;
int cur_max_levels,cur_max_race_levels,cur_max_dogfight_levels,cur_max_qdogfight_levels,cur_max_training_levels;


