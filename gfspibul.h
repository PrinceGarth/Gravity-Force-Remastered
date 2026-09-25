/*
 *    GRAVITY FORCE, gfspibul.h
 *
 *
 *
 *    see source.txt for further information
 */

typedef struct
{
  double x;
  double y;
} vector;

// Header aus gfspibul.c

int SearchTarget(int px, int py, int pnr);
void do_pixels(int c);
void do_bullets();
void detonate_mines();
void check_mine_radius(BNODE *tb);

double V_ScalarProduct(vector v1, vector v2);
vector V_Normalize(vector *v);
vector V_DifferenceVector(vector v1, vector v2);
double V_GetAngle(int x1, int y1, int x2, int y2);

