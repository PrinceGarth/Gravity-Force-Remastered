/*
 *    GRAVITY FORCE, gfspln.c
 *
 *    -- Functions needed for paths of enemies
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gflink.h"
#include "gfspln.h"

int nnpts,ncnt,nstep,ncur,nmax;
int nx[MAX_NODE_POINTS], ny[MAX_NODE_POINTS];

/* calculates the distance between two nodes */
fixed node_dist(NODE n1, NODE n2)
{
   #define SCALE  64

   fixed dx = itofix(n1.x - n2.x) / SCALE;
   fixed dy = itofix(n1.y - n2.y) / SCALE;

   return fsqrt(fmul(dx, dx) + fmul(dy, dy)) * SCALE;
}

/* draws one of the path nodes */
void draw_node(int n)
{
   char b[8];

   circlefill(vscreen, path_node[n].x-map_x[1], path_node[n].y-map_y[1], 2, col_yellow);

   sprintf(b, "%d", n);
   textout(vscreen, font, b, path_node[n].x-7-map_x[1], path_node[n].y-7-map_y[1], col_red);
}


/* calculates the control points for a spline segment */
void get_control_points(NODE n1, NODE n2, int points[8], fixed curviness)
{
   fixed dist = fmul(node_dist(n1, n2), curviness);

   points[0] = n1.x;
   points[1] = n1.y;

   points[2] = n1.x + fixtoi(fmul(fcos(n1.tangent), dist));
   points[3] = n1.y + fixtoi(fmul(fsin(n1.tangent), dist));

   points[4] = n2.x - fixtoi(fmul(fcos(n2.tangent), dist));
   points[5] = n2.y - fixtoi(fmul(fsin(n2.tangent), dist));

   points[6] = n2.x;
   points[7] = n2.y;
}

void count_line_points(BITMAP *b, int x, int y, int d)
{
  ncnt++;
}

void get_line_points(BITMAP *b, int x, int y, int d)
{
  ncur++;
  if (ncur >= nstep)
  {
    ncur = 0;

    nx[nmax] = x;
    ny[nmax] = y;

    nmax++;
  }
}

void ncalc_spline(int points[8], int npts, int *x, int *y)
{
  int x1 = points[0];
  int y1 = points[1];
  int x2 = points[6];
  int y2 = points[7];
  int nr;

  nnpts = npts;

  ncnt = 0;
  do_line(vscreen,x1,y1,x2,y2,0,count_line_points);

  nstep = ncnt / npts;
  if (nstep < 1) nstep = 1;
  ncur = nmax = 0;

  do_line(vscreen,x1,y1,x2,y2,0,get_line_points);

  for (nr=0; nr < MAX_NODE_POINTS; nr++)
  {
    x[nr] = nx[nr];
    y[nr] = ny[nr];
  }

//  x = nx;
//  y = ny;
}

/* draws a spline curve connecting two nodes */
void draw_spline(NODE n1, NODE n2, fixed curviness)
{
   int points[8];

   get_control_points(n1, n2, points, curviness);
   points[0] -= map_x[1];
   points[1] -= map_y[1];
   points[2] -= map_x[1];
   points[3] -= map_y[1];
   points[4] -= map_x[1];
   points[5] -= map_y[1];
   points[6] -= map_x[1];
   points[7] -= map_y[1];

   spline(vscreen, points, col_white);
}

void draw_nodes()
{
  int nr;

  text_mode(-1);
 
  for (nr=1; nr <= node_anz; nr++)
  {
    draw_node(nr);
  }

  text_mode(0);
}

void draw_splines()
{
  int nr,nr2;

  for (nr=1; nr <= node_anz-1; nr++)
  {
//    draw_spline(path_node[nr],path_node[nr+1],enemy[20].curviness);
    for (nr2=0; nr2 < 5; nr2++)
      if (path_node[nr].connect[nr2] > 0)
        draw_spline(path_node[nr],path_node[path_node[nr].connect[nr2]],enemy[20].curviness);
  }

}

