/*
 *    GRAVITY FORCE, gfrace.h
 *
 *
 *
 *    see source.txt for further information
 */

typedef struct R_WAYPOINTS {
  int x,y,w,h;
  int o1,o2;
} R_WAYPOINTS;

typedef struct R_LAPS {
  int m,s,hs;
} R_LAPS;

R_WAYPOINTS waypoint[20];
R_LAPS lap_stat[15];
int active_waypoint, max_waypoint;

int current_lap, max_lap;
int time_m, time_s, time_hs, time_start;
int race_done;


void tausche(int *x1, int *x2);
void get_waypoints();
void check_waypoints();
