/*
 *    GRAVITY FORCE, gfstat.h
 *
 *
 *
 *    see source.txt for further information
 */

// Stat-Header

typedef struct {
  int anz_shot;
  int anz_hit;
  int lifes;
  int fuel;
  long int score;
  int time;
  int enemies_killed;
  int enemies_killed_quality;
  int mission_status;
  float cargo_active;
  float cargo_saved;
  float cargo_lost;
  int frags;
  int suicides;
  int extras;
} PL_STAT;

PL_STAT player_stat[2];


typedef struct {
  char name[30];
  int  score;
} HISCORE_ENTRY;

typedef struct {
  HISCORE_ENTRY nr[10];
} HISCORE_TYPE;

typedef struct {
  char name[30];
  int  m,s,hs;
} RACE_HISCORE_ENTRY;

typedef struct {
  RACE_HISCORE_ENTRY nr[10];
} RACE_HISCORE_TYPE;


typedef struct {
  char last_hiscore_name[30];
  char level_available[50];
  int language;
  int pixanz;
  int svolume;
  int current_level;
  int zoom_effect;
  int pixel_grav;
  int show_debris;
  int current_training_level;
  int current_sprace_level;
  int current_mprace_level;
  int current_dogfight_level;
  int current_qdogfight_level;
  int play_menu_music;
  int def_frag_limit;
  int def_time_limit;
  int def_gravity;
  int p1weapons,p2weapons;
  int extras;
  int no_mouse,user6,user7,user8;   // no_mouse: Linux port (was user5)
  char tuser1[10];
  char tuser2[50];
  char tuser3[30];
  char tuser4[30];
  char tuser5[10];
  char tuser6[10];
} CONFIG_TYPE;

typedef struct {
  HISCORE_TYPE hiscore[50]; // maxlevels
  RACE_HISCORE_TYPE race_hiscore[20];
} HISCORE_FILE_TYPE;

typedef struct {
  CONFIG_TYPE config;
} CONFIG_FILE_TYPE;

CONFIG_FILE_TYPE config_file;
HISCORE_FILE_TYPE hiscore_file;
FILE *cfg_file;
PACKFILE *score_file;


// Header aus gfstat.c

int strtrimr(char *pszStr);
int strtriml(char *pszStr);
int strtrim(char *pszStr);
void sort(HISCORE_ENTRY v[], int n);
int add_2_hiscore(char *name, int score, int mode);
void input_name(char *dtext, char *name);
char *CalcScoreString(char *tmp, char *f, long int score);
char *CalcRaceScoreString(char *tmp, char *f, int m, int s, int hs);
void generate_statistics();
void generate_race_statistics();
void generate_2pdog_statistics();

