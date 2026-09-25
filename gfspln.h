/*
 *    GRAVITY FORCE, gfspln.h
 *
 *
 *
 *    see source.txt for further information
 */

// Header aus gfspln.c
fixed node_dist(NODE n1, NODE n2);
void draw_node(int n);
void get_control_points(NODE n1, NODE n2, int points[8], fixed curviness);
void count_line_points(BITMAP *b, int x, int y, int d);
void get_line_points(BITMAP *b, int x, int y, int d);
void ncalc_spline(int points[8], int npts, int *x, int *y);
void draw_spline(NODE n1, NODE n2, fixed curviness);
void draw_nodes();
void draw_splines();

