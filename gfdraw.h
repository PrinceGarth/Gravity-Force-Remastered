/*
 *    GRAVITY FORCE, gfdraw.h
 *
 *
 *
 *    see source.txt for further information
 */

int test_global;

// Header aus gfdraw.c

void make_base_room(BITMAP *scr, int c);
void draw_bases(BITMAP *scr, int c);
void draw_cargo(BITMAP *scr, int c);
void draw_objects(BITMAP *scr, int c);
void draw_spobjects(BITMAP *scr, int c);
void draw_enemies(BITMAP *scr, int c);
void draw_player(BITMAP *scr, int c);
void draw_bullets(BITMAP *scr, int c);
void draw_pixels(BITMAP *scr, int c);
void masked_kraxel(BITMAP *bmp, int x, int y, int d);
void draw_crater(BITMAP *bmp, int x, int y, int size);
void draw_craters(BITMAP *scr, int c);
void clear_screen(int c);
void clear_explosions();

