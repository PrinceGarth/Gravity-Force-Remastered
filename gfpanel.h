/*
 *    GRAVITY FORCE, gfpanel.h
 *
 *
 *
 *    see source.txt for further information
 */

int show_debug_info;

char upanel_text1[80];
char upanel_text2[100];
char upanel_text3[80];
char upanel_text4[80];

int  upanel_bar_maxhit, upanel_bar_hit;

// Header aus gfpanel.c

void init_upanel();
void draw_bsprite(BITMAP *b, BITMAP *s, int x, int y);
void draw_panel();
void draw_upanel();
void update_panel_fuel();
void update_panel_lifes();
void update_panel_cargo();
void update_panel_shield();
void update_panel_weapons_ammo();
void update_panel_weapons();
void update_panel_infobox();
void draw_panel_text();
void update_upanel_time();
void update_upanel_bar();
void update_upanel_infobox();

