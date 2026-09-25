/*
 *    GRAVITY FORCE, gfosd.h
 *
 *
 *
 *    see source.txt for further information
 */

volatile int osd_update_counter;

typedef struct {
  char text[300];
  char curline[150];
  int pos,gpos;
  int l1,l2;
  int active;
  FONT *f;
  int col,x,y;
} buftype;

buftype osd_buf;

/*
BITMAP *small_osd, *small_osdb;
int small_osd_show;
int small_osd_x, small_osd_y;
int small_osd_w, small_osd_h;
*/

// Header aus gfosd.c

void update_timer(void);
void clear_osd(int r);
void init_osd();
void show_normal_osd_message(FONT *f1, FONT *f2, char *m1, char *m2, int le, int col1, int col2, int r);
void show_typewriter_osd_message(FONT *f, char *m, int l1, int l2, int col, int y);
void update_typewriter_osd_message();
/*
void clear_small_osd();
void init_small_osd(int x, int y, int w, int h);
void kill_small_osd();
void put_text_on_small_osd(char *t, FONT *f, int col, int line);
*/
