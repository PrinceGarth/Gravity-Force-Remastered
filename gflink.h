/*
 *    GRAVITY FORCE, gflink.h
 *
 *
 *
 *    see source.txt for further information
 */

typedef struct {
  int x,y;
  int size;
} CRATER_TYPE;

typedef struct CNODE {
  CRATER_TYPE dat;
  struct CNODE *prev, *next;
} CNODE;

CNODE *first_crater, *last_crater;
int crater_anz;
int MAX_CRATERS;

// Header aus gflink.c

void add_2_list(RLIST *l, int x, int y, int w, int h);
int add_bullet(int pnr, int typ, int itype, float px, float py, float xi, float yi, int mix, int miy, int max, int may);
int erase_bullet(BNODE *temp);
void add_bomb_bullets(BNODE *temp);
int add_pixels(int anz, int x, int y, float xab, float yab, int spd, int tbl[], int maxtbl, int dspd, int max_xab, int max_yab, int img);
int remove_pixel(PNODE *temp);
int add_explosion(int pnr, int type, int maxframe, int maxframet, int starttime, int x, int y);
int remove_explosion(ENODE *temp);
void do_explosions();
int add_crater(int x, int y, int size);
int remove_crater(CNODE *temp);
int remove_small_crater();

