/*
 *    GRAVITY FORCE, gfsenm.c
 *
 *    -- enemy movement in biiiiiig functions...
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfsenm.h"
#include "gfstat.h"
#include "gflink.h"
#include "gfsound.h"
#include "gfspln.h"
#include "gfsprt.h"
#include "gfdraw.h"
#include "gfmisc.h"
#include "gfinit.h"
#include "gf.h"

int player_in_path_ok;

void check_line_for_player(BITMAP *v, int x, int y, int d)
{
  if (playship[0].xpos+PLAYER_WIDTH/2  - d/2  <  x  &&
      playship[0].xpos+PLAYER_WIDTH/2  + d/2  >  x  &&
      playship[0].ypos+PLAYER_HEIGHT/2 - d/2  <  y  &&
      playship[0].ypos+PLAYER_HEIGHT/2 + d/2  >  y)
  {
    player_in_path_ok = TRUE;
  }
}

int player_in_path(int dest, int en)
{
  int cx = path_node[enemy[en].real_waypnt].x;
  int cy = path_node[enemy[en].real_waypnt].y;
  int dx = path_node[dest].x;
  int dy = path_node[dest].y;
  int r = 200; // radius to search

  if (enemy[en].type == ENEMY_SHIP_3) r = 100;

  player_in_path_ok = FALSE;
  do_line(vscreen,cx,cy,dx,dy,r,check_line_for_player);

  return player_in_path_ok;
}

void do_waypoints(int nr)
{
  int w,w1=0,w2=0,cnt,tmp;
  int tmin1,tmin2,dnode,yp,xp,x1,y1,x2,y2,ok,c=0;
  int points[8];

  if (enemy[nr].bezier)
  {

   for (cur_player=0; cur_player < player_anz; cur_player++)
   {

    // Schiff an Node angekommen...
    if ( (enemy[nr].cur_point >= enemy[nr].anz_points-1) )
    {
      if (enemy[nr].chase)
      {

        if ((!enemy[nr].chase_now) && (enemy[nr].dest_point != 0))
          if (enemy[nr].real_waypnt == enemy[nr].nwaypnt[1])
          {
            enemy[nr].dest_point = 0;
            enemy[nr].cur_waypnt = 0;
          }

        if ((enemy[nr].dest_point == 0) || (!enemy[nr].chase_now))
        {
          if ( (playship[0].xpos > enemy[nr].xpos+enemy[nr].react.xw1/2) &&
               (playship[0].xpos < enemy[nr].xpos+enemy[nr].image.width+enemy[nr].react.xw2/2) &&
               (playship[0].ypos > enemy[nr].ypos+enemy[nr].react.yw1/2) &&
               (playship[0].ypos < enemy[nr].ypos+enemy[nr].image.height+enemy[nr].react.yw2/2) )
          {

            tmin1 = tmin2 = 0;
            tmin1 = strecke(path_node[1].x,path_node[1].y,playship[0].xpos,playship[0].ypos);
            dnode = 1;
            for (cnt=2; cnt < node_anz; cnt++)
            {
              tmin2 = strecke(path_node[cnt].x,path_node[cnt].y,playship[0].xpos,playship[0].ypos);
              if ( (tmin2 <= tmin1) ) { tmin1 = tmin2; dnode = cnt; }
            } // for cnt

            enemy[nr].dest_point = dnode;
            enemy[nr].chase_now = TRUE;
            switch (enemy[nr].type)
            {
              case ENEMY_SHIP_3 : enemy[nr].startframe = 1;
                                  break;
              case ENEMY_SHIP_4 : enemy[nr].maxframet = 4;
                                  break;
            }
          } // if player in der naehe -> first chase

        } // if dest_point == 0
        else if ((enemy[nr].dest_point > 0) && (enemy[nr].chase_now))
        {
          tmin1 = tmin2 = 0;
          tmin1 = strecke(path_node[1].x,path_node[1].y,playship[0].xpos,playship[0].ypos);
          dnode = 1;
          for (cnt=2; cnt < node_anz; cnt++)
          {
            tmin2 = strecke(path_node[cnt].x,path_node[cnt].y,playship[0].xpos,playship[0].ypos);
            if (tmin2 <= tmin1) { tmin1 = tmin2; dnode = cnt; }
          } // for cnt

          enemy[nr].dest_point = dnode;

        } // if dest_point > 0 (wenn sich dest_point aendert)

        // Route finden
        if ( (enemy[nr].dest_point > 0) )
        {
          tmin1 = tmin2 = 0;
          xp = yp = ok = 0;

          c = random() % 3;

          tmp = path_node[enemy[nr].real_waypnt].connect[0];

          if (c == 1)
            tmin1 = abs(path_node[tmp].x - path_node[enemy[nr].dest_point].x);
          else if (c == 2)
            tmin1 = abs(path_node[tmp].y - path_node[enemy[nr].dest_point].y);
          else
            tmin1 = strecke(path_node[tmp].x,path_node[tmp].y,path_node[enemy[nr].dest_point].x,path_node[enemy[nr].dest_point].y);

          dnode = tmp;

          for (cnt=1; cnt < MAX_NODE_CONNECTIONS; cnt++)
          {
            tmp = path_node[enemy[nr].real_waypnt].connect[cnt];
            if (tmp > 0)
            {
              x1 = path_node[tmp].x; y1 = path_node[tmp].y;
              x2 = path_node[enemy[nr].dest_point].x; y2 = path_node[enemy[nr].dest_point].y;

              if (c == 1)
                tmin2 = abs(x1 - x2);
              else if (c == 2)
                tmin2 = abs(y1 - y2);
              else
                tmin2 = strecke(x1,y1,x2,y2);

              if ( (tmin2 <= tmin1) && (tmp != enemy[nr].real_waypnt) )
              {
                if (!player_in_path(tmp, nr))
                {
                  tmin1 = tmin2; dnode = tmp;
                }
              } // if tmin2<tmin1...

            } // if tmp>0
          } // for cnt

          w1 = enemy[nr].real_waypnt;
          w2 = dnode;
          enemy[nr].real_waypnt = w2;
          enemy[nr].user2 = w2;
        }
      } // if chase

      // Wenn Spieler zu weit weg, Verfolgung abbrechen (user1 = max_chase_dist)
      if ((enemy[nr].chase_now) && (strecke(playship[0].xpos,playship[0].ypos,enemy[nr].xpos,enemy[nr].ypos) > enemy[nr].user1))
      {
        enemy[nr].dest_point = enemy[nr].nwaypnt[1];
        enemy[nr].chase_now = FALSE;
        switch (enemy[nr].type)
        {
          case ENEMY_SHIP_3 : enemy[nr].startframe = 5;
                              break;
          case ENEMY_SHIP_4 : enemy[nr].maxframet = 8;
                              break;
        }
      } // if strecke zu gross

      // Normale Route aufnehmen
      if (enemy[nr].dest_point == 0)
      {
        // normales Schiff
        (enemy[nr].cur_waypnt < enemy[nr].max_waypnt) ? (enemy[nr].cur_waypnt++) : (enemy[nr].cur_waypnt = 1);
        w1 = enemy[nr].nwaypnt[enemy[nr].cur_waypnt];
        w2 = (enemy[nr].cur_waypnt+1 > enemy[nr].max_waypnt) ? enemy[nr].nwaypnt[1] : enemy[nr].nwaypnt[enemy[nr].cur_waypnt+1];
        enemy[nr].real_waypnt = w2;
      }

 if (!show_panel)
 {
  textout(score_table,font,ftoa(tmpstr,enemy[20].dest_point),10,30,col_white);
  textout(score_table,font,ftoa(tmpstr,enemy[20].real_waypnt),10,40,col_white);
  textout(score_table,font,ftoa(tmpstr,w1),100,30,col_white);
  textout(score_table,font,ftoa(tmpstr,w2),100,40,col_white);
  textout(score_table,font,ftoa(tmpstr,c),200,30,col_white);
//  textout(score_table,font,ftoa(enemy[20].chase_now),200,40,col_white);
 }

      // Strecke berechnen
      enemy[nr].anz_points = (fixtoi(node_dist(path_node[w1], path_node[w2]))+3) / enemy[nr].xspd;
      if (enemy[nr].anz_points < 1)
         enemy[nr].anz_points = 1;
      else if (enemy[nr].anz_points > MAX_NODE_POINTS)
         enemy[nr].anz_points = MAX_NODE_POINTS;
      get_control_points(path_node[w1], path_node[w2], points, enemy[nr].curviness);
      calc_spline(points, enemy[nr].anz_points, enemy[nr].path_x, enemy[nr].path_y);
      enemy[nr].cur_point = 0;
    }

    enemy[nr].cur_point++;
    enemy[nr].xpos = enemy[nr].path_x[enemy[nr].cur_point];
    enemy[nr].ypos = enemy[nr].path_y[enemy[nr].cur_point];

   } // for cur_player...
  }
  else
  {

  w = enemy[nr].cur_waypnt;

  if ( (enemy[nr].waypnt[w].x < enemy[nr].xpos+5) &&
       (enemy[nr].waypnt[w].x > enemy[nr].xpos-5) &&
       (enemy[nr].waypnt[w].y < enemy[nr].ypos+5) &&
       (enemy[nr].waypnt[w].y > enemy[nr].ypos-5) )
     {
       (enemy[nr].cur_waypnt < enemy[nr].max_waypnt) ? (enemy[nr].cur_waypnt++) : (enemy[nr].cur_waypnt = enemy[nr].start_waypnt);
       w = enemy[nr].cur_waypnt;
       enemy[nr].xspd = (enemy[nr].waypnt[w].x - enemy[nr].xpos) / (enemy[nr].waypnt[w].s);
       enemy[nr].yspd = (enemy[nr].waypnt[w].y - enemy[nr].ypos) / (enemy[nr].waypnt[w].s);
     }


  } // if bezier

}

void do_enemies()
{
  int nr,nr2,cnt,a=0;
  int ok,ok2;
  int xs[10],ys[10];
  int w,h;
  float xt=0,yt=0,scl=0;

  for (nr=0; nr < enemy_anz; nr++)
  {
    if (enemy[nr].active)
    {
       // Bewegen
      if (!enemy[nr].bezier)
      {
        enemy[nr].xpos += enemy[nr].xspd;
        enemy[nr].ypos += enemy[nr].yspd;
      }

      w = enemy[nr].image.width/2; h = enemy[nr].image.height/2;

      // Pruefen, ob playship[0] in der Naehe
      ok = FALSE;
      ok = (playship[0].xpos > enemy[nr].xpos+w+enemy[nr].react.xw1) &&
           (playship[0].xpos < enemy[nr].xpos+w+enemy[nr].react.xw2) &&
           (playship[0].ypos > enemy[nr].ypos+h+enemy[nr].react.yw1) &&
           (playship[0].ypos < enemy[nr].ypos+h+enemy[nr].react.yw2);

      // Schiessen
      if (ok) enemy[nr].shoott++; else enemy[nr].shoott = 0;

      ok2 = FALSE;

      if (enemy[nr].shootr) ok2 = enemy[nr].shoott >= ((rand() % enemy[nr].shootf));
      else ok2 = enemy[nr].shoott >= enemy[nr].shootf;

      if (ok2)
      {
        enemy[nr].shoott = 0;

        for (nr2=0; nr2 < enemy[nr].maxshot; nr2++)
        {
          if ( (!enemy[nr].dead) && (ok) )
          {
            if (enemy[nr].shootr)
            {
              xt = playship[0].xpos+PLAYER_WIDTH/2 - enemy[nr].xpos+w;
              yt = playship[0].ypos+PLAYER_HEIGHT/2 - enemy[nr].ypos+h;
              scl = strecke(enemy[nr].xpos+w, enemy[nr].ypos+h, playship[0].xpos+PLAYER_WIDTH/2, playship[0].ypos+PLAYER_HEIGHT/2);
            }
            switch (enemy[nr].type)
            {
              case ENEMY_SHIP_1 : enemy[nr].shot[nr2].xoff = 20; enemy[nr].shot[nr2].yoff = 10;
                                  if (enemy[nr].shootr)
                                  {
                                    enemy[nr].shot[nr2].xspd = ( (xt/scl*2.4) + ( ((random() % 10)-5.0) / 10.0) )* 2.0;
                                    enemy[nr].shot[nr2].yspd = ( (yt/scl*2.4) + ( ((random() % 10)-5.0) / 10.0) )* 2.0;
                                  }
                                  else
                                  {
                                    enemy[nr].shot[nr2].xspd = (random() % 6) - 3; enemy[nr].shot[nr2].yspd = (random() % 6) - 3;
                                  }
                                  if (enemy[nr].shot[nr2].xspd == 0) enemy[nr].shot[nr2].xspd = 1;
                                  if (enemy[nr].shot[nr2].yspd == 0) enemy[nr].shot[nr2].yspd = 1;
                                  add_bullet(nr,B_ENEMY,W_DEFAULT,enemy[nr].xpos+enemy[nr].shot[nr2].xoff,enemy[nr].ypos+enemy[nr].shot[nr2].yoff,enemy[nr].shot[nr2].xspd,enemy[nr].shot[nr2].yspd,
                                             enemy[nr].shot[nr2].min_x,enemy[nr].shot[nr2].min_y,
                                             enemy[nr].shot[nr2].max_x,enemy[nr].shot[nr2].max_y);
                                  play_sound_sample(snd_shot_esingle,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                  break;
              case ENEMY_SHIP_2 : enemy[nr].shot[nr2].xoff = 20; enemy[nr].shot[nr2].yoff = 10;
                                  if (enemy[nr].shootr)
                                  {
                                    enemy[nr].shot[nr2].xspd = ( (xt/scl*2.4) + ( ((random() % 10)-5.0) / 10.0) )* 2.0;
                                    enemy[nr].shot[nr2].yspd = ( (yt/scl*2.4) + ( ((random() % 10)-5.0) / 10.0) )* 2.0;
                                  }
                                  else
                                  {
                                    enemy[nr].shot[nr2].xspd = (random() % 8) - 4; enemy[nr].shot[nr2].yspd = (random() % 8) - 4;
                                  }
                                  if (enemy[nr].shot[nr2].xspd == 0) enemy[nr].shot[nr2].xspd = 1;
                                  if (enemy[nr].shot[nr2].yspd == 0) enemy[nr].shot[nr2].yspd = 1;
                                  add_bullet(nr,B_ENEMY,W_DEFAULT,enemy[nr].xpos+enemy[nr].shot[nr2].xoff,enemy[nr].ypos+enemy[nr].shot[nr2].yoff,enemy[nr].shot[nr2].xspd,enemy[nr].shot[nr2].yspd,
                                             enemy[nr].shot[nr2].min_x,enemy[nr].shot[nr2].min_y,
                                             enemy[nr].shot[nr2].max_x,enemy[nr].shot[nr2].max_y);
                                  play_sound_sample(snd_shot_esingle,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                  break;
              case ENEMY_SHIP_3 : if (enemy[nr].chase_now)
                                  {
                                    enemy[nr].shot[nr2].xoff = 10; enemy[nr].shot[nr2].yoff = 9;
                                    enemy[nr].shot[nr2].xspd = (xt/scl*2.4) + ( ((random() % 10)-5.0) / 10.0);
                                    enemy[nr].shot[nr2].yspd = (yt/scl*2.4) + ( ((random() % 10)-5.0) / 10.0);
                                    if (enemy[nr].shot[nr2].xspd == 0) enemy[nr].shot[nr2].xspd = 1;
                                    if (enemy[nr].shot[nr2].yspd == 0) enemy[nr].shot[nr2].yspd = 1;
                                    add_bullet(nr,B_ENEMY,W_DEFAULT,enemy[nr].xpos+enemy[nr].shot[nr2].xoff,enemy[nr].ypos+enemy[nr].shot[nr2].yoff,enemy[nr].shot[nr2].xspd,enemy[nr].shot[nr2].yspd,
                                               enemy[nr].shot[nr2].min_x,enemy[nr].shot[nr2].min_y,
                                               enemy[nr].shot[nr2].max_x,enemy[nr].shot[nr2].max_y);
                                    play_sound_sample(snd_shot_esingle,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                  }
                                  break;
              case ENEMY_SHIP_4 : if (enemy[nr].chase_now)
                                  {
                                    enemy[nr].shot[nr2].xoff = 10; enemy[nr].shot[nr2].yoff = 9;
                                    enemy[nr].shot[nr2].xspd = ( (xt/scl*2.4) + ( ((random() % 10)-5.0) / 10.0) )* 2.0;
                                    enemy[nr].shot[nr2].yspd = ( (yt/scl*2.4) + ( ((random() % 10)-5.0) / 10.0) )* 2.0;
                                    if (enemy[nr].shot[nr2].xspd == 0) enemy[nr].shot[nr2].xspd = 1;
                                    if (enemy[nr].shot[nr2].yspd == 0) enemy[nr].shot[nr2].yspd = 1;
                                    add_bullet(nr,B_ENEMY,W_DEFAULT,enemy[nr].xpos+enemy[nr].shot[nr2].xoff,enemy[nr].ypos+enemy[nr].shot[nr2].yoff,enemy[nr].shot[nr2].xspd,enemy[nr].shot[nr2].yspd,
                                               enemy[nr].shot[nr2].min_x,enemy[nr].shot[nr2].min_y,
                                               enemy[nr].shot[nr2].max_x,enemy[nr].shot[nr2].max_y);
//                                    add_bullet(nr,B_ENEMY,W_MINE2,enemy[nr].xpos+7,enemy[nr].ypos+7,
//                                               0,0,
//                                               0,0,5000,5000);
                                    play_sound_sample(snd_shot_esingle,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                  }
                                  break;
              case ENEMY_NSHIP_1 : enemy[nr].shot[nr2].xoff = enemy[nr].image.width/2; enemy[nr].shot[nr2].yoff = enemy[nr].image.height;
//                                  if (rand() % enemy[nr].shootf == 0)
//                                  {
                                    add_bullet(nr,B_ENEMY,W_BIGBOMB,enemy[nr].xpos+enemy[nr].shot[nr2].xoff,enemy[nr].ypos+enemy[nr].shot[nr2].yoff,enemy[nr].shot[nr2].xspd,enemy[nr].shot[nr2].yspd,
                                               enemy[nr].shot[nr2].min_x,enemy[nr].shot[nr2].min_y,
                                               enemy[nr].shot[nr2].max_x,enemy[nr].shot[nr2].max_y);
                                    play_sound_sample(snd_shot_esingle,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
//                                  }
                                  break;
              case ENEMY_BOSS_1 : if (playship[0].xpos < enemy[nr].xpos) a = 3;
                                  else if (playship[0].xpos > enemy[nr].ypos+enemy[nr].image.width) a = 2;
                                  if (playship[0].ypos < enemy[nr].ypos) a = 0;
                                  else if (playship[0].ypos > enemy[nr].ypos+enemy[nr].image.height) a = 1;
                                  switch(a)
                                  {
                                    case 0 :
                                        enemy[nr].shot[nr2].xoff = enemy[nr].image.width/2; enemy[nr].shot[nr2].yoff = 0;
                                        enemy[nr].shot[nr2].xspd = 0; enemy[nr].shot[nr2].yspd = -1;
                                        break;
                                    case 1 :
                                        enemy[nr].shot[nr2].xoff = enemy[nr].image.width/2; enemy[nr].shot[nr2].yoff = enemy[nr].image.height;
                                        enemy[nr].shot[nr2].xspd = 0; enemy[nr].shot[nr2].yspd = 1;
                                        break;
                                    case 2 :
                                        enemy[nr].shot[nr2].xoff = enemy[nr].image.width; enemy[nr].shot[nr2].yoff = enemy[nr].image.height/2;
                                        enemy[nr].shot[nr2].xspd = 1; enemy[nr].shot[nr2].yspd = 0;
                                        break;
                                    case 3 :
                                        enemy[nr].shot[nr2].xoff = 0; enemy[nr].shot[nr2].yoff = enemy[nr].image.height/2;
                                        enemy[nr].shot[nr2].xspd = -1; enemy[nr].shot[nr2].yspd = 0;
                                        break;
                                  }
                                  add_bullet(nr,B_ENEMY,W_ROCKET,enemy[nr].xpos+enemy[nr].shot[nr2].xoff,enemy[nr].ypos+enemy[nr].shot[nr2].yoff,enemy[nr].shot[nr2].xspd,enemy[nr].shot[nr2].yspd,
                                             enemy[nr].shot[nr2].min_x,enemy[nr].shot[nr2].min_y,
                                             enemy[nr].shot[nr2].max_x,enemy[nr].shot[nr2].max_y);
                                  play_sound_sample(snd_shot_rocket,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                  break;
              case PULSE_SHOOTER: add_bullet(nr,B_NEUTRAL,W_DEFAULT,enemy[nr].xpos+enemy[nr].shot[nr2].xoff,enemy[nr].ypos+enemy[nr].shot[nr2].yoff,(random() % (int)enemy[nr].shot[nr2].xspd) - (enemy[nr].shot[nr2].xspd/1.5),enemy[nr].shot[nr2].yspd,
                                             enemy[nr].shot[nr2].min_x,enemy[nr].shot[nr2].min_y,
                                             enemy[nr].shot[nr2].max_x,enemy[nr].shot[nr2].max_y);
                                  play_sound_sample(snd_shot_shooter,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                  break;
              default           : add_bullet(nr,B_ENEMY,W_DEFAULT,enemy[nr].xpos+enemy[nr].shot[nr2].xoff,enemy[nr].ypos+enemy[nr].shot[nr2].yoff,enemy[nr].shot[nr2].xspd,enemy[nr].shot[nr2].yspd,
                                             enemy[nr].shot[nr2].min_x,enemy[nr].shot[nr2].min_y,
                                             enemy[nr].shot[nr2].max_x,enemy[nr].shot[nr2].max_y);
                                  play_sound_sample(snd_shot_esingle,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                  break;
            } // switch type
          } // if !dead
        } // for nr2
      } // if ok2 (shoot)

//    } // for cur_player...

     if ( (ok) || (enemy[nr].chase) )
     {
      enemy[nr].framet++;
      if (enemy[nr].framet >= enemy[nr].maxframet) { enemy[nr].frame++; enemy[nr].framet = 0; }
      if (enemy[nr].frame > enemy[nr].maxframe) enemy[nr].frame=enemy[nr].startframe;
     }
     else enemy[nr].frame = 0;

      switch (enemy[nr].type)
      {
        case SHOOTER_UP   :  switch (enemy[nr].frame)
                             {
                               case 15 : enemy[nr].image.data = shooter_up1;
                                         break;
                               case 16 : enemy[nr].image.data = shooter_up2;
                                         break;
                               case 17 : enemy[nr].image.data = shooter_up2;
                                         break;
                               case 18 : enemy[nr].image.data = shooter_up3;
                                         break;
                               case 19 : enemy[nr].image.data = shooter_up3;
                                         break;
                               default : enemy[nr].image.data = shooter_up1;
                                         break;
                             } // switch frame
                             break;
        case SHOOTER_DOWN :  switch (enemy[nr].frame)
                             {
                               case 15 : enemy[nr].image.data = shooter_down1;
                                         break;
                               case 16 : enemy[nr].image.data = shooter_down2;
                                         break;
                               case 17 : enemy[nr].image.data = shooter_down2;
                                         break;
                               case 18 : enemy[nr].image.data = shooter_down3;
                                         break;
                               case 19 : enemy[nr].image.data = shooter_down3;
                                         break;
                               default : enemy[nr].image.data = shooter_down1;
                                         break;
                             } // switch frame
                             break;
        case SHOOTER_RIGHT:  switch (enemy[nr].frame)
                             {
                               case 15 : enemy[nr].image.data = shooter_right1;
                                         break;
                               case 16 : enemy[nr].image.data = shooter_right2;
                                         break;
                               case 17 : enemy[nr].image.data = shooter_right2;
                                         break;
                               case 18 : enemy[nr].image.data = shooter_right3;
                                         break;
                               case 19 : enemy[nr].image.data = shooter_right3;
                                         break;
                               default : enemy[nr].image.data = shooter_right1;
                                         break;
                             } // switch frame
                             break;
        case SHOOTER_LEFT :  switch (enemy[nr].frame)
                             {
                               case 15 : enemy[nr].image.data = shooter_left1;
                                         break;
                               case 16 : enemy[nr].image.data = shooter_left2;
                                         break;
                               case 17 : enemy[nr].image.data = shooter_left2;
                                         break;
                               case 18 : enemy[nr].image.data = shooter_left3;
                                         break;
                               case 19 : enemy[nr].image.data = shooter_left3;
                                         break;
                               default : enemy[nr].image.data = shooter_left1;
                                         break;
                             } // switch frame
                             break;
        case THREE_SHOOTER:  switch (enemy[nr].frame)
                             {
                               default : enemy[nr].image.data = shooter1_1;
                                         break;
                             } // switch frame
                             break;
        case PULSE_SHOOTER:  switch (enemy[nr].frame)
                             {
                               case 15 : enemy[nr].image.data = shooter_pulse_1;
                                         break;
                               case 16 : enemy[nr].image.data = shooter_pulse_2;
                                         break;
                               case 17 : enemy[nr].image.data = shooter_pulse_2;
                                         break;
                               case 18 : enemy[nr].image.data = shooter_pulse_3;
                                         break;
                               case 19 : enemy[nr].image.data = shooter_pulse_3;
                                         break;
                               default : enemy[nr].image.data = shooter_pulse_1;
                                         break;
                             } // switch frame
                             break;
        case ENEMY_SHIP_1 :  switch (enemy[nr].frame)
                             {
                               case 1  : case 10
                                       : enemy[nr].image.data = enemy_s1_01;
                                         break;
                               case 2  : case 9
                                       : enemy[nr].image.data = enemy_s1_02;
                                         break;
                               case 3  : case 8
                                       : enemy[nr].image.data = enemy_s1_03;
                                         break;
                               case 4  : case 7
                                       : enemy[nr].image.data = enemy_s1_04;
                                         break;
                               case 5  : case 6
                                       : enemy[nr].image.data = enemy_s1_05;
                                         break;
                               default : enemy[nr].image.data = enemy_s1_01;
                             } // switch frame
                             do_waypoints(nr);
                             break;
        case ENEMY_SHIP_2 :  switch (enemy[nr].frame)
                             {
                               case 1  : case 6
                                       : enemy[nr].image.data = enemy_s2_01;
                                         break;
                               case 2  : case 5
                                       : enemy[nr].image.data = enemy_s2_02;
                                         break;
                               case 3  : case 4
                                       : enemy[nr].image.data = enemy_s2_03;
                                         break;
                               default : enemy[nr].image.data = enemy_s2_03;
                             } // switch frame
                             do_waypoints(nr);
                             break;
        case ENEMY_SHIP_3 :  enemy[nr].image.data = enemy_s3_01+enemy[nr].frame-1;
                             do_waypoints(nr);
                             break;
        case ENEMY_SHIP_4 :
                             switch (enemy[nr].frame)
                             {
                               case 1  : case 17
                                       : enemy[nr].image.data = enemy_s4_01;
                                         break;
                               case 2  : case 16
                                       : enemy[nr].image.data = enemy_s4_02;
                                         break;
                               case 3  : case 15
                                       : enemy[nr].image.data = enemy_s4_03;
                                         break;
                               case 4  : case 14
                                       : enemy[nr].image.data = enemy_s4_04;
                                         break;
                               case 5  : case 13
                                       : enemy[nr].image.data = enemy_s4_05;
                                         break;
                               case 6  : case 12
                                       : enemy[nr].image.data = enemy_s4_06;
                                         break;
                               case 7  : case 11
                                       : enemy[nr].image.data = enemy_s4_07;
                                         break;
                               case 8  : case 10
                                       : enemy[nr].image.data = enemy_s4_08;
                                         break;
                               case 9  :
                                         enemy[nr].image.data = enemy_s4_09;
                                         break;
                             }
                             do_waypoints(nr);
                             break;
        case ENEMY_SHIP_5 :  switch (enemy[nr].frame)
                             {
                               case 1  :
                                         enemy[nr].image.data = enemy_s5_01;
                                         break;
                               case 2  : case 11
                                       : enemy[nr].image.data = enemy_s5_02;
                                         break;
                               case 3  : case 10
                                       : enemy[nr].image.data = enemy_s5_03;
                                         break;
                               case 4  : case 9
                                       : enemy[nr].image.data = enemy_s5_04;
                                         break;
                               case 5  :
                                         enemy[nr].image.data = enemy_s5_05;
                                         break;
                               case 6  :
                                         enemy[nr].image.data = enemy_s5_06;
                                         break;
                               case 7  :
                                         enemy[nr].image.data = enemy_s5_07;
                                         break;
                               case 8  :
                                         enemy[nr].image.data = enemy_s5_08;
                                         break;
                             } // switch frame
                             do_waypoints(nr);
                             break;
        case ENEMY_NSHIP_1 : switch (enemy[nr].frame)
                             {
                               case 0  :
                                         enemy[nr].image.data = enemy_n1_1;
                                         break;
                               case 1  :
                                         enemy[nr].image.data = enemy_n1_2;
                                         break;
                               case 2  :
                                         enemy[nr].image.data = enemy_n1_3;
                                         break;
                               case 3  :
                                         enemy[nr].image.data = enemy_n1_2;
                                         break;
                               case 4  :
                                         enemy[nr].image.data = enemy_n1_1;
                                         break;
                               case 5  :
                                         enemy[nr].image.data = enemy_n1_4;
                                         break;
                               case 6  :
                                         enemy[nr].image.data = enemy_n1_5;
                                         break;
                               case 7  :
                                         enemy[nr].image.data = enemy_n1_4;
                                         break;
                               default : enemy[nr].image.data = enemy_n1_1;
                             } // switch frame
                             do_waypoints(nr);
                             break;
        case ENEMY_BOSS_1 :  switch (enemy[nr].frame)
                             {
                               case 1  : case 10
                                       : enemy[nr].image.data = enemy_bigboss1_1;
                                         break;
                               case 2  : case 9
                                       : enemy[nr].image.data = enemy_bigboss1_2;
                                         break;
                               case 3  : case 8
                                       : enemy[nr].image.data = enemy_bigboss1_3;
                                         break;
                               case 4  : case 7
                                       : enemy[nr].image.data = enemy_bigboss1_4;
                                         break;
                               case 5  : case 6
                                       : enemy[nr].image.data = enemy_bigboss1_5;
                                         break;
                               default : enemy[nr].image.data = enemy_bigboss1_1;
                             } // switch frame
                             do_waypoints(nr);
                             break;

        case GSTONE_B1  :    do_waypoints(nr);
                             break;
        case GSTONE_S1  :    do_waypoints(nr);
                             break;
        case GSTONE_S2  :    do_waypoints(nr);
                             break;

      } // switch type

      if ((enemy[nr].hit >= enemy[nr].maxhit) && (enemy[nr].dead == 0)) enemy[nr].dead = TRUE;

      if (enemy[nr].dead)
      {
        if (enemy[nr].dead == 1)
        {
          xs[1] = -3; ys[1] =  0;
          xs[2] = -2; ys[2] = -1;
          xs[3] =  0; ys[3] = -2;
          xs[4] =  2; ys[4] = -1;
          xs[5] =  3; ys[5] =  0;
          xs[6] =  2; ys[6] =  1;
          xs[7] =  0; ys[7] =  2;
          xs[8] = -2; ys[8] =  1;
          w = enemy[nr].image.width/2;
          h = enemy[nr].image.height/2;
          player_stat[0].enemies_killed++;

          switch (enemy[nr].type)
          {
            case ENEMY_SHIP_1 : for (cnt=1; cnt < 9; cnt++)
                                {
                                  add_bullet(nr,B_ENEMY,W_DEFAULT,enemy[nr].xpos+w,enemy[nr].ypos+h,xs[cnt],ys[cnt],
                                             enemy[nr].xpos+w-200,enemy[nr].ypos+h-200,
                                             enemy[nr].xpos+w+200,enemy[nr].ypos+h+200);
                                } // for cnt
                                play_sound_sample(snd_exp_big_2,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                add_pixels(120,enemy[nr].xpos+w,enemy[nr].ypos+h,2-(rand() % 3),2-(rand() % 3),10,tbl_blue,10,50,0,0,0);
                                if (SHOW_DEBRIS) add_pixels(40,enemy[nr].xpos+(rand()%(w*2)),enemy[nr].ypos+(rand()%(h*2)),2-(rand() % 3),0.1,10,tbl_blue,10,10,0,0,debris_1_01);
                                for (nr2=0; nr2 < 7; nr2++)
                                {
                                  add_explosion(-1,expl_big_1,5,2,rand() % 50,enemy[nr].xpos+w+(rand() % 30)-15,enemy[nr].ypos+h+(rand() % 20)-10);
                                  add_explosion(-1,expl_med1_01,7,2,rand() % 50,enemy[nr].xpos+w+(rand() % 30)-15,enemy[nr].ypos+h+(rand() % 20)-10);
                                }
                                player_stat[0].enemies_killed_quality+=10;
                                break;
            case ENEMY_SHIP_2 : for (cnt=1; cnt < 9; cnt++)
                                {
                                  add_bullet(nr,B_ENEMY,W_DEFAULT,enemy[nr].xpos+w,enemy[nr].ypos+h,xs[cnt],ys[cnt],
                                             enemy[nr].xpos+w-200,enemy[nr].ypos+h-200,
                                             enemy[nr].xpos+w+200,enemy[nr].ypos+h+200);
                                } // for cnt
                                play_sound_sample(snd_exp_big_1,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                add_pixels(220,enemy[nr].xpos+w,enemy[nr].ypos+h,2-(rand() % 3),2-(rand() % 3),10,tbl_red,10,50,0,0,0);
                                if (SHOW_DEBRIS) add_pixels(60,enemy[nr].xpos+(rand()%(w*2)),enemy[nr].ypos+(rand()%(h*2)),2-(rand() % 3),0.1,10,tbl_blue,10,10,0,0,debris_2_01);
                                for (nr2=0; nr2 < 15; nr2++)
                                {
                                  add_explosion(-1,expl_big_1,5,2,rand() % 60,enemy[nr].xpos+w+(rand() % 44)-22,enemy[nr].ypos+h+(rand() % 30)-15);
                                  add_explosion(-1,expl_med1_01,7,2,rand() % 50,enemy[nr].xpos+w+(rand() % 30)-15,enemy[nr].ypos+h+(rand() % 20)-10);
                                }
                                player_stat[0].enemies_killed_quality+=30;
                                break;
            case ENEMY_SHIP_3 : for (cnt=1; cnt < 9; cnt++)
                                {
                                  add_bullet(nr,B_ENEMY,W_DEFAULT,enemy[nr].xpos+w,enemy[nr].ypos+h,xs[cnt],ys[cnt],
                                             enemy[nr].xpos+w-200,enemy[nr].ypos+h-200,
                                             enemy[nr].xpos+w+200,enemy[nr].ypos+h+200);
                                } // for cnt
                                play_sound_sample(snd_exp_mid_2,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                add_pixels(120,enemy[nr].xpos+w,enemy[nr].ypos+h,2-(rand() % 3),2-(rand() % 3),10,tbl_white,10,50,0,0,0);
                                if (SHOW_DEBRIS) add_pixels(20,enemy[nr].xpos+(rand()%(w*2)),enemy[nr].ypos+(rand()%(h*2)),2-(rand() % 3),0.1,10,tbl_blue,10,10,0,0,debris_3_01);
                                for (nr2=0; nr2 < 3; nr2++)
                                {
                                  add_explosion(-1,expl_big_1,5,2,rand() % 30,enemy[nr].xpos+w+(rand() % 30)-15,enemy[nr].ypos+h+(rand() % 20)-10);
                                  add_explosion(-1,expl_med1_01,7,2,rand() % 50,enemy[nr].xpos+w+(rand() % 30)-15,enemy[nr].ypos+h+(rand() % 20)-10);
                                }
                                player_stat[0].enemies_killed_quality+=40;
                                break;
            case ENEMY_SHIP_4 : for (cnt=1; cnt < 9; cnt++)
                                {
                                  add_bullet(nr,B_ENEMY,W_DEFAULT,enemy[nr].xpos+w,enemy[nr].ypos+h,xs[cnt],ys[cnt],
                                             enemy[nr].xpos+w-200,enemy[nr].ypos+h-200,
                                             enemy[nr].xpos+w+200,enemy[nr].ypos+h+200);
                                } // for cnt
                                play_sound_sample(snd_exp_mid_4,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                add_pixels(220,enemy[nr].xpos+w,enemy[nr].ypos+h,2-(rand() % 3),2-(rand() % 3),10,tbl_red,10,50,0,0,0);
                                if (SHOW_DEBRIS) add_pixels(20,enemy[nr].xpos+(rand()%(w*2)),enemy[nr].ypos+(rand()%(h*2)),2-(rand() % 3),0.1,10,tbl_blue,10,10,0,0,debris_2_01);
                                for (nr2=0; nr2 < 7; nr2++)
                                {
                                  add_explosion(-1,expl_big_1,5,2,rand() % 30,enemy[nr].xpos+w+(rand() % 30)-15,enemy[nr].ypos+h+(rand() % 20)-10);
                                  add_explosion(-1,expl_med1_01,7,2,rand() % 50,enemy[nr].xpos+w+(rand() % 30)-15,enemy[nr].ypos+h+(rand() % 20)-10);
                                }
                                player_stat[0].enemies_killed_quality+=80;
                                break;
            case ENEMY_SHIP_5 : for (cnt=1; cnt < 9; cnt++)
                                {
                                  add_bullet(nr,B_ENEMY,W_DEFAULT,enemy[nr].xpos+w,enemy[nr].ypos+h,xs[cnt],ys[cnt],
                                             enemy[nr].xpos+w-200,enemy[nr].ypos+h-200,
                                             enemy[nr].xpos+w+200,enemy[nr].ypos+h+200);
                                } // for cnt
                                play_sound_sample(snd_exp_big_2,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                add_pixels(220,enemy[nr].xpos+w,enemy[nr].ypos+h,2-(rand() % 3),2-(rand() % 3),10,tbl_blue,10,50,0,0,0);
                                if (SHOW_DEBRIS) add_pixels(80,enemy[nr].xpos+(rand()%(w*2)),enemy[nr].ypos+(rand()%(h*2)),2-(rand() % 3),0.1,10,tbl_blue,10,10,0,0,debris_1_01);
                                for (nr2=0; nr2 < 7; nr2++)
                                {
                                  add_explosion(-1,expl_big_1,5,2,rand() % 50,enemy[nr].xpos+w+(rand() % 30)-15,enemy[nr].ypos+h+(rand() % 20)-10);
                                  add_explosion(-1,expl_med1_01,7,2,rand() % 50,enemy[nr].xpos+w+(rand() % 30)-15,enemy[nr].ypos+h+(rand() % 20)-10);
                                }
                                player_stat[0].enemies_killed_quality+=25;
                                break;
            case ENEMY_NSHIP_1 : for (cnt=1; cnt < 9; cnt++)
                                {
                                  add_bullet(nr,B_ENEMY,W_DEFAULT,enemy[nr].xpos+w,enemy[nr].ypos+h,xs[cnt],ys[cnt],
                                             enemy[nr].xpos+w-200,enemy[nr].ypos+h-200,
                                             enemy[nr].xpos+w+200,enemy[nr].ypos+h+200);
                                } // for cnt
                                play_sound_sample(snd_exp_big_1,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                add_pixels(220,enemy[nr].xpos+w,enemy[nr].ypos+h,2-(rand() % 3),2-(rand() % 3),10,tbl_blue,10,50,0,0,0);
                                if (SHOW_DEBRIS) add_pixels(70,enemy[nr].xpos+(rand()%(w*2)),enemy[nr].ypos+(rand()%(h*2)),2-(rand() % 3),0.1,10,tbl_blue,10,10,0,0,debris_1_01);
                                for (nr2=0; nr2 < 15; nr2++)
                                {
                                  add_explosion(-1,expl_big_1,5,2,rand() % 60,enemy[nr].xpos+w+(rand() % 44)-22,enemy[nr].ypos+h+(rand() % 30)-15);
                                  add_explosion(-1,expl_med1_01,7,2,rand() % 50,enemy[nr].xpos+w+(rand() % 30)-15,enemy[nr].ypos+h+(rand() % 20)-10);
                                }
                                player_stat[0].enemies_killed_quality+=10;
                                break;
            case ENEMY_BOSS_1 : for (cnt=1; cnt < 9; cnt++)
                                {
                                  add_bullet(nr,B_ENEMY,W_DEFDOUBLE,enemy[nr].xpos+w,enemy[nr].ypos+h,xs[cnt],ys[cnt],
                                             enemy[nr].xpos+w-200,enemy[nr].ypos+h-200,
                                             enemy[nr].xpos+w+200,enemy[nr].ypos+h+200);
                                } // for cnt
                                play_sound_sample(snd_exp_big_1,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                play_sound_sample(snd_exp_big_2,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                for (nr2=0; nr2 < 30; nr2++)
                                {
                                  add_explosion(-1,expl_big_1,5,2,rand() % 60,enemy[nr].xpos+(rand() % enemy[nr].image.width),enemy[nr].ypos+(rand() % enemy[nr].image.height));
                                  add_explosion(-1,expl_med1_01,5,2,rand() % 60,enemy[nr].xpos+(rand() % enemy[nr].image.width),enemy[nr].ypos+(rand() % enemy[nr].image.height));
                                  add_pixels(50,enemy[nr].xpos+(rand() % enemy[nr].image.width),enemy[nr].ypos+(rand() % enemy[nr].image.height),2-(rand() % 3),2-(rand() % 3),10,tbl_white,10,50,0,0,0);
                                  if (SHOW_DEBRIS) add_pixels(10,enemy[nr].xpos+(rand()%(w*2)),enemy[nr].ypos+(rand()%(h*2)),2-(rand() % 3),0.1,10,tbl_blue,10,10,0,0,debris_3_01);
                                }
                                player_stat[0].enemies_killed_quality+=200;
                                break;
            default           : play_sound_sample(snd_exp_sm_3,1000,0,enemy[nr].xpos,enemy[nr].ypos,100);
                                add_pixels(70,enemy[nr].xpos+w,enemy[nr].ypos+h,2-(rand() % 3),2-(rand() % 3),10,tbl_all,256,0,0,0,0);
                                add_crater(enemy[nr].xpos+w,enemy[nr].ypos+h,10);
                                add_explosion(-1,expl_sm1_01,10,1,0,enemy[nr].xpos,enemy[nr].ypos);
                                player_stat[0].enemies_killed_quality+=5;
          } // switch type;
        } // if dead==1
        if (enemy[nr].dead < enemy[nr].maxdead) enemy[nr].dead++;
        else
        {
          enemy[nr].active = FALSE;
//          if (enemy_on_screen(enemy[nr]))
//            MapDraw8BGT(vscreen,enemy[nr].xpos-20,enemy[nr].ypos-20,enemy[nr].xpos-map_x[1]-20,enemy[nr].ypos-map_y[1]-20,48,48);
        }
      } // if dead

    } // if active
  } // for cur_player
}