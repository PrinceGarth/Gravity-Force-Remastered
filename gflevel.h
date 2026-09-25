/*
 *    GRAVITY FORCE, gflevel.h
 *
 *
 *
 *    see source.txt for further information
 */

int DEF_AUTOFIRE_RATE;
int DEF_SHIELD_RECHARGE;

int show_infobox;
char show_infobox_message[30];
char show_infobox_message2[20];
float level_time_factor;
int killable_enemies;
int playship_noweap;
int use_extras;

// Header aus gflevel.c

void level_inits();
void level_runtime_checks();
void level_end_check();
