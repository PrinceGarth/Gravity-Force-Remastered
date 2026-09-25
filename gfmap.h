/*
 *    GRAVITY FORCE, gfmap.h
 *
 *
 *
 *    see source.txt for further information
 */

int scroll_cur_x, scroll_cur_y;
int scroll_to_x, scroll_to_y;
int scroll_speed;

void update_screen_map();
void make_levelmap();
void map_stuff();
void scroll_to(int x, int y, int speed);
int do_scroll();
