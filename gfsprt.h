/*
 *    GRAVITY FORCE, gfsprt.h
 *
 *
 *
 *    see source.txt for further information
 */

// Header aus gfsprt.c

int enemy_on_screen(SPRITE_TYPE en);
int on_draw_field(int x, int y, int w, int h);
int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
float strecke(int x1, int y1, int x2, int y2);
