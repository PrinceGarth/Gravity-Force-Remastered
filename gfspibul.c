/*
 *    GRAVITY FORCE, gfspibul.c
 *
 *    -- Bullet movement / collision checking, pixel movement & checking...
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfspibul.h"
#include "gfstat.h"
#include "gfsprt.h"
#include "gflink.h"
#include "gfsound.h"
#include "gf.h"
#include "gfpanel.h"
#include "gfsobj.h"
#include "gfmisc.h"
#include "gfinit.h"
#include "gfdraw.h"
#include "gflog.h"
#include "gfosd.h"
#include "gfg2pdog.h"
#include "mappyal.h"

int block_in_path_ok;
int for_anz;
BLKSTR *block;

void check_line_for_block(BITMAP *v, int x, int y, int d)
{
  for_anz++;

  if (for_anz % 20 == 0)
  {
    block = MapGetBlock(x/TILE_W,y/TILE_H);
    if (!block->trigger)
    {
      block_in_path_ok = TRUE;
    }
  }
}

int block_in_path(int x, int y, int en)
{
  block_in_path_ok = FALSE;
  for_anz = 0;
  do_line(vscreen,x,y,enemy[en].xpos,enemy[en].ypos,0,check_line_for_block);

  return block_in_path_ok;
}

int SearchTarget(int px, int py, int pnr)
{
  int a,b,r,bx,by,nr;
  extern int MAX_ROCKET_LOCK;

  a = MAX_ROCKET_LOCK;
  r = -1;
  for (nr=0; nr < enemy_anz; nr++)
  {
    if (enemy[nr].active)
    {
      switch (enemy[nr].type)
      {
        case GSTONE_B1 : case GSTONE_S1 : case GSTONE_S2 :
          break;
        default :
          bx = enemy[nr].xpos; by = enemy[nr].ypos;
          b = strecke(px,py,bx,by);
          if (b <= a && !block_in_path(px,py,nr)) { a = b; r = nr; }

      }
    }
  } // for nr

  if (game_mode == MP_2PDOGFIGHT)
  for (nr=0; nr < player_anz; nr++)
  {
    if (nr != pnr)
    {
      bx = playship[nr].xpos; by = playship[nr].ypos;
      b = strecke(px,py,bx,by);
      if (b <= a) { a = b; r = nr+1000; }
    }
  } // for nr

  return r;
}

void do_pixels(int c)
{
  int done;
  int w,h;
  struct PNODE *temp, *next_pixel;

  temp = first_pixel;

  while (temp!=NULL)
  {
    next_pixel = temp->next;

    done = FALSE;

    temp->dat.yspd -= PARTICLE_GRAVITY+temp->dat.yspd2;
    temp->dat.x += temp->dat.xspd+temp->dat.xspd2;
    temp->dat.y -= temp->dat.yspd;
    temp->dat.t++;

    // Checken, ob zulaessiges Gebiet verlassen
    if (temp->dat.max_xab && temp->dat.max_yab)
      if ((temp->dat.x > temp->dat.startx+temp->dat.max_xab/2) ||
          (temp->dat.x < temp->dat.startx-temp->dat.max_xab/2) ||
          (temp->dat.y > temp->dat.starty+temp->dat.max_yab/2) ||
          (temp->dat.y < temp->dat.starty-temp->dat.max_yab/2))
      {
        remove_pixel(temp);
        done = TRUE;
      }

    if (!done)
    {
      if (!temp->dat.image)
      {
        // Pixelfarbe dekrementieren
        if (temp->dat.dspd > 0 && !done)
          if (temp->dat.t % temp->dat.dspd == 0)
            if (temp->dat.tc > 0)
            {
              temp->dat.tc--;
              temp->dat.c = *(temp->dat.tbl+temp->dat.tc);
            }
  
        if ( ((getpixel(vscreen,temp->dat.x-map_x[1],temp->dat.y-map_y[1]) > 10) ||
             (temp->dat.y > 5000) || (temp->dat.y < 1) || (temp->dat.x < 1) ||
             (temp->dat.t > temp->dat.mt) || (temp->dat.yspd < -8) || (temp->dat.tc == 0)))
        {
          if (temp->dat.user < 2)
          {
            temp->dat.yspd = -temp->dat.yspd/3;
            temp->dat.user++;
          }
          else
            remove_pixel(temp);
        } // if getpixel
      }
      else
      {
        w = ((BITMAP *)temp->dat.image)->w;
        h = ((BITMAP *)temp->dat.image)->h;

        if ( ((getpixel(vscreen,temp->dat.x-map_x[1]+w/2,temp->dat.y-map_y[1]+h/2-h+temp->dat.yspd) > 10) ||
             (getpixel(vscreen,temp->dat.x-map_x[1]+w/2,temp->dat.y-map_y[1]+h/2+h-temp->dat.yspd) > 10) ||
             (getpixel(vscreen,temp->dat.x-map_x[1]+w/2-w-temp->dat.xspd,temp->dat.y-map_y[1]+h/2) > 10) ||
             (getpixel(vscreen,temp->dat.x-map_x[1]+w/2+w+temp->dat.xspd,temp->dat.y-map_y[1]+h/2) > 10) ||
             (temp->dat.t > temp->dat.mt) || (temp->dat.yspd < -8) || (temp->dat.tc == 0)))
        {
          if (temp->dat.user < 2)
          {
            temp->dat.yspd = -temp->dat.yspd/3;
            temp->dat.user++;
          }
          else
          {
            add_explosion(-1,expl_sm3_01,5,6,0,temp->dat.x-2,temp->dat.y-2);
            remove_pixel(temp);
          }
        } // if getpixel
      } // else
    } // if !done
    temp = next_pixel;
  } // while
}


double V_ScalarProduct(vector v1, vector v2)
{
  return ((v1.x*v2.x)+(v1.y*v2.y));
}

vector V_Normalize(vector *v)
{
  double len=sqrt(pow(v->x,2)+pow(v->y,2));
  if (len)
  {
    v->x/=len;
    v->y/=len;
  }
  return *v;
}

vector V_DifferenceVector(vector v1, vector v2)
{
  vector temp;
  temp.x=v2.x-v1.x;
  temp.y=v2.y-v1.y;
  return temp;
}

double V_GetAngle(int x1, int y1, int x2, int y2)// vector v1, vector v2)
{
  double product;
  vector temp,diff,v1,v2;
  v1.x = x1; v1.y = y1;
  v2.x = x2; v2.y = y2;
  temp.x=0;
  temp.y=1;
  diff=V_DifferenceVector(v1,v2);

  V_Normalize(&temp);
  V_Normalize(&diff);
  product = V_ScalarProduct(temp,diff);

  return acos(product);
}

void do_bullets()
{
  int nr2,pnr;
  float tx,ty,pspd,enx=0,eny=0,mx,my,rot;
  int done = FALSE,r;
  int hashit,erased = FALSE;
  float xrot,yrot,scale_x,scale_y;
  int hilfsy;
  int *tablepntr = NULL;
  extern float ALIEN_ROCKET_TURN;
  int special;

  struct BNODE *temp, *next_bull, *temp2, *next_bull2;

  temp = first_bullet;

  while (temp!=NULL)
  {
      next_bull = temp->next;

      if (temp->dat.maxframe > 0)
      {
        temp->dat.framet++;
        if (temp->dat.framet >= temp->dat.maxframet) { temp->dat.frame++; temp->dat.framet = 0; }
        if (temp->dat.frame > temp->dat.maxframe) temp->dat.frame=temp->dat.startframe;
      }

      switch (temp->dat.type2)
      {
        case W_BOMB : case W_BIGBOMB :
                         temp->dat.yacc  = BOMB_GRAVITY;
                         if (temp->dat.yspd+temp->dat.yacc < 6)
                           temp->dat.yspd += temp->dat.yacc;
                         temp->dat.xpos += temp->dat.xspd;
                         temp->dat.ypos += temp->dat.yspd;
                         switch (temp->dat.type)
                         {
                           case B_OWN     : add_pixels(2,temp->dat.xpos-2+random()%4,temp->dat.ypos-2+random()%4,0,-PARTICLE_GRAVITY,0,tbl_white,9,2,0,0,0);
                                            break;
                           case B_ENEMY   : add_pixels(2,temp->dat.xpos-2+random()%4,temp->dat.ypos-2+random()%4,0,-PARTICLE_GRAVITY,0,tbl_yellow,9,2,0,0,0);
                                            break;
                           case B_NEUTRAL : add_pixels(2,temp->dat.xpos-2+random()%4,temp->dat.ypos-2+random()%4,0,-PARTICLE_GRAVITY,0,tbl_red,9,2,0,0,0);
                                            break;
                         }

                         temp->dat.ttl--;
                         if (temp->dat.ttl == 0)
                         {
                           if (temp->dat.type2 == W_BOMB)
                           {
                             add_bomb_bullets(temp);
                             play_sound_sample(snd_exp_sm_1,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                             add_explosion(-1,expl_sm1_01,10,1,0,temp->dat.xpos,temp->dat.ypos);
                           }
                           else if (temp->dat.type2 == W_BIGBOMB)
                           {
                             play_sound_sample(snd_exp_mid_3,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                             add_explosion(-1,expl_big2_01,13,3,0,temp->dat.xpos-35,temp->dat.ypos-50);
                           }
                           erase_bullet(temp);
                           done = TRUE;
                         }
                         break;
        case W_ROCKET  :
                         temp->dat.user2++;
                     
            		 if ((temp->dat.user1 >= 0 && temp->dat.user2 > 10) && ((temp->dat.user1 >= 1000 && !playship[temp->dat.user1-1000].dead) || (temp->dat.user1 < 1000 && enemy[temp->dat.user1].active)))
                         {
                           if (temp->dat.type == B_OWN)
                           {
                             // Enemy (< 1000) or Player (> 1000)?
                             if (temp->dat.user1 < 1000)
                             {
                               enx = enemy[temp->dat.user1].xpos+enemy[temp->dat.user1].image.width/2;
                               eny = enemy[temp->dat.user1].ypos+enemy[temp->dat.user1].image.height/2;
                             }
                             else
                             {
                               enx = playship[temp->dat.user1-1000].xpos+PLAYER_WIDTH/2;
                               eny = playship[temp->dat.user1-1000].ypos+PLAYER_HEIGHT/2;
                             }
                           }
                           else if (temp->dat.type == B_ENEMY)
                           {
                             enx = playship[0].xpos+PLAYER_WIDTH/2;
                             eny = playship[0].ypos+PLAYER_HEIGHT/2;
                           }
                           mx  = temp->dat.xpos;
                           my  = temp->dat.ypos;

                           rot = V_GetAngle(enx,eny,mx,my);
                           xrot = sin(rot);
                           yrot = cos(rot);

                           if (mx >= enx)
                             scale_x = -xrot;
                           else
                             scale_x = xrot;
                           scale_y = -yrot;

                           temp->dat.dxspd = scale_x * (temp->dat.user2 / 10);
                           temp->dat.dyspd = scale_y * (temp->dat.user2 / 10);
                         }
                         else if ( (temp->dat.user1 < 0 && temp->dat.user2 > 10) || (!enemy[temp->dat.user1].active) && (temp->dat.user2 > 10))
                         {
                           // neues Ziel suchen
                           if (temp->dat.type == B_OWN)
                             temp->dat.user1 = SearchTarget(temp->dat.xpos,temp->dat.ypos,temp->dat.nr);
                         }
                         else
                         {
                           temp->dat.dxspd = temp->dat.xspd;
                           temp->dat.dyspd = temp->dat.yspd;
                         }

                         if (temp->dat.type == B_OWN)
                         {
                           if (temp->dat.xspd > temp->dat.dxspd) temp->dat.xspd -= ROCKET_TURN;
                           else if (temp->dat.xspd < temp->dat.dxspd) temp->dat.xspd += ROCKET_TURN;
                           if (temp->dat.yspd > temp->dat.dyspd) temp->dat.yspd -= ROCKET_TURN;
                           else if (temp->dat.yspd < temp->dat.dyspd) temp->dat.yspd += ROCKET_TURN;
                         }
                         else if (temp->dat.type == B_ENEMY || temp->dat.type == B_NEUTRAL)
                         {
                           if (temp->dat.xspd > temp->dat.dxspd) temp->dat.xspd -= ALIEN_ROCKET_TURN;
                           else if (temp->dat.xspd < temp->dat.dxspd) temp->dat.xspd += ALIEN_ROCKET_TURN;
                           if (temp->dat.yspd > temp->dat.dyspd) temp->dat.yspd -= ALIEN_ROCKET_TURN;
                           else if (temp->dat.yspd < temp->dat.dyspd) temp->dat.yspd += ALIEN_ROCKET_TURN;
                         }

                         temp->dat.xpos += temp->dat.xspd;
                         temp->dat.ypos += temp->dat.yspd;
                         switch (temp->dat.type)
                         {
                           case B_OWN     : add_pixels(4,temp->dat.xpos+random()%3,temp->dat.ypos+random()%5,0,-PARTICLE_GRAVITY-0.001,0,tbl_white,9,10,0,0,0);
                                            break;
                           case B_ENEMY   : add_pixels(4,temp->dat.xpos+random()%3,temp->dat.ypos+random()%5,0,-PARTICLE_GRAVITY-0.001,0,tbl_yellow,9,10,0,0,0);
                                            break;
                           case B_NEUTRAL : add_pixels(4,temp->dat.xpos+random()%3,temp->dat.ypos+random()%5,0,-PARTICLE_GRAVITY-0.001,0,tbl_red,9,10,0,0,0);
                                            break;
                         }

                         temp->dat.ttl--;
                         if (temp->dat.ttl == 0)
                         {
                           play_sound_sample(snd_exp_sm_1,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                           add_explosion(-1,expl_sm1_01,10,1,0,temp->dat.xpos,temp->dat.ypos);
                           erase_bullet(temp);
                           done = TRUE;
                         }
                         break;

        case W_MINE1 :
                         temp->dat.xspd += temp->dat.dxspd;
                         if (temp->dat.xspd >= 0.1 || temp->dat.xspd <= -0.1) temp->dat.dxspd = -temp->dat.dxspd;
                         temp->dat.yspd += temp->dat.dyspd;
                         if (temp->dat.yspd >= 0.1 || temp->dat.yspd <= -0.1) temp->dat.dyspd = -temp->dat.dyspd;

                         temp->dat.xpos += temp->dat.xspd;
                         temp->dat.ypos += temp->dat.yspd;

                         if (temp->dat.ttl > 1) temp->dat.ttl--;
                         else if (temp->dat.ttl == 1) { temp->dat.user2 = TRUE; temp->dat.ttl = 0; }

                         switch (temp->dat.type)
                         {
                           case B_OWN     :
                                           switch (temp->dat.frame)
                                           {
                                             case 0 : temp->dat.image.data = data[weapon_mine1_1].dat;
                                                      break;
                                             case 1 : temp->dat.image.data = data[weapon_mine1_2].dat;
                                                      break;
                                           }
                                           break;
                           case B_ENEMY   :
                                           switch (temp->dat.frame)
                                           {
                                             case 0 : temp->dat.image.data = data[weapon_mine1e_1].dat;
                                                      break;
                                             case 1 : temp->dat.image.data = data[weapon_mine1e_2].dat;
                                                      break;
                                           }
                                           break;
                         }
                         if (temp->dat.user2 > 1) temp->dat.user2--;
                         else if (temp->dat.user2)
                         {
                           play_sound_sample(snd_exp_mid_1,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                           add_explosion(-1,expl_med1_01,7,4,0,temp->dat.xpos-2,temp->dat.ypos-2);
                           add_pixels(30,temp->dat.xpos+2,temp->dat.ypos+2,0,0,10,tbl_white,10,50,0,0,0);
                           if (SHOW_DEBRIS) add_pixels(5,temp->dat.xpos+2,temp->dat.ypos+2,0,0.1,10,tbl_blue,10,10,0,0,debris_5_01);
                           check_mine_radius(temp);
                           erase_bullet(temp);
                           done = TRUE;
                         }
                         break;
        case W_MINE2 :
                         temp->dat.xspd += temp->dat.dxspd;
                         if (temp->dat.xspd >= 0.4 || temp->dat.xspd <= -0.3) temp->dat.dxspd = -temp->dat.dxspd;
                         temp->dat.yspd += temp->dat.dyspd;
                         if (temp->dat.yspd >= 0.6 || temp->dat.yspd <= -0.5) temp->dat.dyspd = -temp->dat.dyspd;

                         temp->dat.xpos += temp->dat.xspd;
                         temp->dat.ypos += temp->dat.yspd;

                         if (temp->dat.ttl > 1) temp->dat.ttl--;
                         else if (temp->dat.ttl == 1) { temp->dat.user2 = TRUE; temp->dat.ttl = 0; }

                         switch (temp->dat.type)
                         {
                           case B_OWN     :
                                            switch (temp->dat.frame)
                                            {
                                              case 0 : temp->dat.image.data = data[weapon_mine2_1].dat;
                                                       break;
                                              case 1 : temp->dat.image.data = data[weapon_mine2_2].dat;
                                                       break;
                                            }
                                            break;
                           case B_ENEMY   :
                                            switch (temp->dat.frame)
                                            {
                                              case 0 : temp->dat.image.data = data[weapon_mine2e_1].dat;
                                                       break;
                                              case 1 : temp->dat.image.data = data[weapon_mine2e_2].dat;
                                                       break;
                                            }
                                            break;
                         }
                         if (temp->dat.user2 > 1) temp->dat.user2--;
                         else if (temp->dat.user2)
                         {
                           play_sound_sample(snd_exp_mid_1,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                           add_explosion(-1,expl_med1_01,7,4,0,temp->dat.xpos-2,temp->dat.ypos-2);
                           add_pixels(60,temp->dat.xpos+3,temp->dat.ypos+3,0,0,10,tbl_white,10,50,0,0,0);
                           if (SHOW_DEBRIS) add_pixels(8,temp->dat.xpos+2,temp->dat.ypos+2,0,0.1,10,tbl_blue,10,10,0,0,debris_5_01);
                           check_mine_radius(temp);
                           erase_bullet(temp);
                           done = TRUE;
                         }
                         break;
        case W_MINE3 :
                         temp->dat.xspd += temp->dat.dxspd;
                         if (temp->dat.xspd >= 0.5 || temp->dat.xspd <= -0.5) temp->dat.dxspd = -temp->dat.dxspd;
                         temp->dat.yspd += temp->dat.dyspd;
                         if (temp->dat.yspd >= 0.2 || temp->dat.yspd <= -0.2) temp->dat.dyspd = -temp->dat.dyspd;

                         temp->dat.xpos += temp->dat.xspd;
                         temp->dat.ypos += temp->dat.yspd;

                         if (temp->dat.ttl > 1) temp->dat.ttl--;
                         else if (temp->dat.ttl == 1) { temp->dat.user2 = TRUE; temp->dat.ttl = 0; }

                         switch (temp->dat.type)
                         {
                           case B_OWN     :
                                            switch (temp->dat.frame)
                                            {
                                              case 0 : temp->dat.image.data = data[weapon_mine3_1].dat;
                                                       break;
                                              case 1 : temp->dat.image.data = data[weapon_mine3_2].dat;
                                                       break;
                                            }
                                            break;
                           case B_ENEMY   :
                                            switch (temp->dat.frame)
                                            {
                                              case 0 : temp->dat.image.data = data[weapon_mine3e_1].dat;
                                                       break;
                                              case 1 : temp->dat.image.data = data[weapon_mine3e_2].dat;
                                                       break;
                                            }
                                            break;
                         }
                         if (temp->dat.user2 > 1) temp->dat.user2--;
                         else if (temp->dat.user2)
                         {
                           play_sound_sample(snd_exp_mid_1,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                           add_explosion(-1,expl_med1_01,7,4,0,temp->dat.xpos-2,temp->dat.ypos-2);
                           add_pixels(100,temp->dat.xpos+3,temp->dat.ypos+3,0,0,10,tbl_white,10,50,0,0,0);
                           if (SHOW_DEBRIS) add_pixels(12,temp->dat.xpos+3,temp->dat.ypos+3,0,0.1,10,tbl_blue,10,10,0,0,debris_5_01);
                           if (SHOW_DEBRIS) add_pixels(4,temp->dat.xpos+3,temp->dat.ypos+3,0,0.1,10,tbl_blue,10,10,0,0,debris_3_01);
                           check_mine_radius(temp);
                           erase_bullet(temp);
                           done = TRUE;
                         }
                         break;
        default        :
                         temp->dat.xpos += temp->dat.xspd;
                         temp->dat.ypos += temp->dat.yspd;
                         switch (temp->dat.type)
                         {
                           case B_OWN     : add_pixels(1,temp->dat.xpos+-1+random()%2,temp->dat.ypos-1+random()%2,0,-PARTICLE_GRAVITY,0,tbl_white,9,2,0,0,0);
                                            break;
                           case B_ENEMY   : add_pixels(1,temp->dat.xpos+-1+random()%2,temp->dat.ypos-1+random()%2,0,-PARTICLE_GRAVITY,0,tbl_yellow,9,2,0,0,0);
                                            break;
                           case B_NEUTRAL : add_pixels(1,temp->dat.xpos+-1+random()%2,temp->dat.ypos-1+random()%2,0,-PARTICLE_GRAVITY,0,tbl_red,9,2,0,0,0);
                                            break;
                         }

                         temp->dat.ttl--;
                         if (temp->dat.ttl == 0)
                         {
                           erase_bullet(temp);
                           done = TRUE;
                         }
                         break;
      }

      // Screenbounds checken
      if (!done)
      {
        if ( (temp->dat.xpos < temp->dat.min_x) || (temp->dat.xpos > temp->dat.max_x) ||
             (temp->dat.ypos < temp->dat.min_y) || (temp->dat.ypos > temp->dat.max_y) )
        {
          if (!erased)
            erased = erase_bullet(temp);
          done = TRUE;
        }

      } // if !done (1)

      if (!done)
      {
        // Checken, ob andere Objekte getroffen
        if ( (temp->dat.type == B_OWN) || (temp->dat.type == B_NEUTRAL) )
        {
                          // Andere Spieler getroffen?
                          if (game_mode == MP_2PDOGFIGHT)
                          for (nr2=0; nr2 < player_anz; nr2++)
                          {
                            if (temp->dat.nr != nr2 &&
                                (temp->dat.type2 == W_MINE1 ||
                                temp->dat.type2 == W_MINE2 ||
                                temp->dat.type2 == W_MINE3))
                            {
                              if (strecke(temp->dat.xpos+temp->dat.image.width/2,
                                          temp->dat.ypos+temp->dat.image.height/2,
                                          playship[nr2].xpos+PLAYER_WIDTH/2,
                                          playship[nr2].ypos+PLAYER_HEIGHT/2) < (temp->dat.user1/2))
                              {
                                temp->dat.user2 = TRUE;
                                done = TRUE;
                              }
                            }

                            if (!done)
                            if ( temp->dat.nr != nr2 && !playship[nr2].dead &&
                                 collision(temp->dat.xpos,temp->dat.ypos,temp->dat.image.width,temp->dat.image.height,
                                          playship[nr2].xpos+3,playship[nr2].ypos+3,
                                          PLAYER_WIDTH-3,PLAYER_HEIGHT-3) )
                            {
                              switch (temp->dat.type2)
                              {
                                case W_BOMB :
                                      playship[nr2].shield-=2;
                                      playship[nr2].onehit = TRUE;
                                      if (playship[nr2].shield > 0) add_explosion(nr2,effect_shield_1,3,6,0,playship[nr2].xpos-6,playship[nr2].ypos-5);
                                      update_panel_shield();
//                                      player_stat[temp->dat.nr].anz_hit++;
                                      break;
                                case W_BIGBOMB :
                                      playship[nr2].shield-=10;
                                      player_stat[temp->dat.nr].anz_hit++;
                                      break;
                                case W_ROCKET :
                                      playship[nr2].shield-=3;
                                      playship[nr2].onehit = TRUE;
                                      if (playship[nr2].shield > 0) add_explosion(nr2,effect_shield_1,3,6,0,playship[nr2].xpos-6,playship[nr2].ypos-5);
                                      update_panel_shield();
                                      player_stat[temp->dat.nr].anz_hit++;
                                      break;
                                default :
                                      playship[nr2].shield--;
                                      playship[nr2].onehit = TRUE;
                                      if (playship[nr2].shield > 0) add_explosion(nr2,effect_shield_1,3,6,0,playship[nr2].xpos-6,playship[nr2].ypos-5);
                                      update_panel_shield();
                                      player_stat[temp->dat.nr].anz_hit++;
                              }

                              if (playship[nr2].shield <= 0)
                              {
                                add_explosion(nr2,expl_big_1,5,2,0,playship[nr2].xpos-2,playship[nr2].ypos-2);
                                playship[nr2].dead = 1;
                                player_stat[temp->dat.nr].frags++;
                              }

                              if (!erased)
                                erased = erase_bullet(temp);
                              done = TRUE;
                            } // if getroffen

                          } // for nr2 (players)

                          for (nr2=0; nr2 < enemy_anz; nr2++)
                          {
                            if ((enemy[nr2].active) && (!enemy[nr2].dead) &&
                                temp->dat.ypos+40 >= enemy[nr2].ypos &&
                                temp->dat.ypos-40 <= enemy[nr2].ypos+enemy[nr2].image.height)
                            {
                              hashit = FALSE;

                              // special-flag for stones
                              if (enemy[nr2].type == GSTONE_B1 ||
                                  enemy[nr2].type == GSTONE_S1 ||
                                  enemy[nr2].type == GSTONE_S2) special = TRUE;
                              else special = FALSE;

                              switch (temp->dat.type2)
                              {
                                case W_BOMB  :
                                               if (collision(temp->dat.xpos-10,temp->dat.ypos-10,20,20,
                                                             enemy[nr2].xpos+enemy[nr2].image.boundxa,
                                                             enemy[nr2].ypos+enemy[nr2].image.boundya,
                                                             enemy[nr2].image.boundxe,
                                                             enemy[nr2].image.boundye) )
                                               {
                                                 hashit = TRUE;
                                               }
                                               break;
                                case W_BIGBOMB  :
                                               if (collision(temp->dat.xpos-10,temp->dat.ypos-10,20,20,
                                                             enemy[nr2].xpos+enemy[nr2].image.boundxa,
                                                             enemy[nr2].ypos+enemy[nr2].image.boundya,
                                                             enemy[nr2].image.boundxe,
                                                             enemy[nr2].image.boundye) )
                                               {
                                                 hashit = TRUE;
                                               }
                                               break;
                                case W_ROCKET :
                                               if (collision(temp->dat.xpos-10,temp->dat.ypos-10,20,20,
                                                             enemy[nr2].xpos+enemy[nr2].image.boundxa,
                                                             enemy[nr2].ypos+enemy[nr2].image.boundya,
                                                             enemy[nr2].image.boundxe,
                                                             enemy[nr2].image.boundye) )
                                               {
                                                 hashit = TRUE;
                                               }
                                               break;
                                case W_MINE1 :
                                               if (strecke(temp->dat.xpos+temp->dat.image.width/2,
                                                           temp->dat.ypos+temp->dat.image.height/2,
                                                           enemy[nr2].xpos+enemy[nr2].image.width/2,
                                                           enemy[nr2].ypos+enemy[nr2].image.height/2) < (temp->dat.user1/2))
                                               {
                                                 temp->dat.user2 = TRUE;
                                                 hashit = FALSE;
                                               }
                                               break;
                                case W_MINE2 :
                                               if (strecke(temp->dat.xpos+temp->dat.image.width/2,
                                                           temp->dat.ypos+temp->dat.image.height/2,
                                                           enemy[nr2].xpos+enemy[nr2].image.width/2,
                                                           enemy[nr2].ypos+enemy[nr2].image.height/2) < (temp->dat.user1/2))
                                               {
                                                 temp->dat.user2 = TRUE;
                                                 hashit = FALSE;
                                               }
                                               break;
                                case W_MINE3 :
                                               if (strecke(temp->dat.xpos+temp->dat.image.width/2,
                                                           temp->dat.ypos+temp->dat.image.height/2,
                                                           enemy[nr2].xpos+enemy[nr2].image.width/2,
                                                           enemy[nr2].ypos+enemy[nr2].image.height/2) < (temp->dat.user1/2))
                                               {
                                                 temp->dat.user2 = TRUE;
                                                 hashit = FALSE;
                                               }
                                               break;
                                default      : if (collision(temp->dat.xpos,temp->dat.ypos,1,1,
                                                             enemy[nr2].xpos+enemy[nr2].image.boundxa,
                                                             enemy[nr2].ypos+enemy[nr2].image.boundya,
                                                             enemy[nr2].image.boundxe,
                                                             enemy[nr2].image.boundye) )
                                               {
                                                 hashit = TRUE;
                                               }
                               }

                               if (hashit)
                               {
                                 switch (temp->dat.type2)
                                 {
                                   case W_BOMB :
                                                 play_sound_sample(snd_exp_sm_1,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                                                 add_bomb_bullets(temp);
//                                                 player_stat.anz_hit++;
                                                 break;
                                   case W_BIGBOMB :
                                                 play_sound_sample(snd_exp_mid_3,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                                                 add_explosion(-1,expl_big2_01,13,3,0,temp->dat.xpos-35,temp->dat.ypos-40);
                                                 if (!special)
                                                 {
                                                   enemy[nr2].hit += 1000;
                                                   player_stat[temp->dat.nr].anz_hit++;
                                                 }
                                                 break;

                                   default     :
                                                 play_sound_sample(snd_bonk,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                                                 if (!special) player_stat[temp->dat.nr].anz_hit++;
                                                 switch (enemy[nr2].type)
                                                 {
                                                   case SHOOTER_UP : case SHOOTER_DOWN :
                                                   case SHOOTER_LEFT : case SHOOTER_RIGHT :
                                                   case THREE_SHOOTER : case PULSE_SHOOTER :
                                                     add_pixels(20,temp->dat.xpos,temp->dat.ypos,2-(rand() % 3),2-(rand() % 3),60,tbl_white,10,50,0,0,0);
                                                     add_explosion(-1,expl_sm1_01+5,5,1,0,temp->dat.xpos,temp->dat.ypos);
                                                     break;
                                                   case ENEMY_SHIP_1 :
                                                     add_pixels(50*enemy[nr2].hit/enemy[nr2].maxhit,temp->dat.xpos,temp->dat.ypos,2-(rand() % 3),2-(rand() % 3),60,tbl_blue,10,50,0,0,0);
                                                     add_explosion(-1,expl_sm1_01+5,5,1,0,temp->dat.xpos,temp->dat.ypos);
                                                     break;
                                                   case ENEMY_SHIP_2 :
                                                     add_pixels(15*enemy[nr2].hit/enemy[nr2].maxhit,temp->dat.xpos,temp->dat.ypos,2-(rand() % 3),2-(rand() % 3),60,tbl_red,10,50,0,0,0);
                                                     add_explosion(-1,expl_sm1_01+5,5,1,0,temp->dat.xpos,temp->dat.ypos);
                                                     break;
                                                   case ENEMY_SHIP_3 :
                                                     add_pixels(5*enemy[nr2].hit/enemy[nr2].maxhit,temp->dat.xpos,temp->dat.ypos,2-(rand() % 3),2-(rand() % 3),60,tbl_white,10,50,0,0,0);
                                                     add_explosion(-1,expl_sm1_01+5,5,1,0,temp->dat.xpos,temp->dat.ypos);
                                                     break;
                                                   case ENEMY_SHIP_4 :
                                                     add_pixels(15*enemy[nr2].hit/enemy[nr2].maxhit,temp->dat.xpos,temp->dat.ypos,2-(rand() % 3),2-(rand() % 3),60,tbl_red,10,50,0,0,0);
                                                     add_explosion(-1,expl_sm1_01+5,5,1,0,temp->dat.xpos,temp->dat.ypos);
                                                     break;
                                                   case ENEMY_SHIP_5 :
                                                     add_pixels(100*enemy[nr2].hit/enemy[nr2].maxhit,temp->dat.xpos,temp->dat.ypos,2-(rand() % 3),2-(rand() % 3),60,tbl_blue,10,50,0,0,0);
                                                     add_explosion(-1,expl_sm1_01+5,5,1,0,temp->dat.xpos,temp->dat.ypos);
                                                     break;
                                                   case ENEMY_NSHIP_1 :
                                                     add_pixels(50*enemy[nr2].hit/enemy[nr2].maxhit,temp->dat.xpos,temp->dat.ypos,2-(rand() % 3),2-(rand() % 3),60,tbl_blue,10,50,0,0,0);
                                                     add_explosion(-1,expl_sm1_01+5,5,1,0,temp->dat.xpos,temp->dat.ypos);
                                                     break;
                                                   case ENEMY_BOSS_1 :
                                                     add_pixels(60*enemy[nr2].hit/enemy[nr2].maxhit,temp->dat.xpos,temp->dat.ypos,2-(rand() % 3),2-(rand() % 3),60,tbl_white,10,50,0,0,0);
                                                     add_explosion(-1,expl_sm1_01+5,5,1,0,temp->dat.xpos,temp->dat.ypos);
                                                     break;
                                                  case GSTONE_B1 : case GSTONE_S1 : case GSTONE_S2 :
                                                     add_pixels(5,temp->dat.xpos,temp->dat.ypos,2-(rand() % 3),2-(rand() % 3),60,tbl_white,10,50,0,0,0);
                                                     break;

                                                 } // switch type
                                                 switch (temp->dat.type2)
                                                 {
                                                   case W_ROCKET :
                                                     play_sound_sample(snd_exp_sm_2,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                                                     add_explosion(-1,expl_sm1_01,10,1,0,temp->dat.xpos,temp->dat.ypos);
                                                     if (!special) enemy[nr2].hit += 8;
                                                     break;
                                                 } // switch type2
                                 } // switch type2

                                 if (!special) enemy[nr2].onehit = TRUE;

                                 if (!special)
                                 switch (temp->dat.type2)
                                 {
                                   case W_DEFDOUBLE : enemy[nr2].hit+=DEFDOUBLE_POWER;
                                                      break;
                                   default          : enemy[nr2].hit++;
                                 }

                                 if (enemy[nr2].hit <= enemy[nr2].maxhit)
                                 {
                                   upanel_bar_hit = enemy[nr2].hit;
                                   upanel_bar_maxhit = enemy[nr2].maxhit;
                                   update_upanel_bar();
                                 }

//                                 if (enemy[nr2].hit >= enemy[nr2].maxhit) enemy[nr2].dead = TRUE;
                                 if (!erased)
                                   erased = erase_bullet(temp);
                                 done = TRUE;
                               } // if getroffen


                            } // if active
                          } // for nr2 (enemies)

                          if (!done)
                          for (nr2=0; nr2 < object_anz; nr2++)
                          {
                            if ((object[nr2].active) && (!object[nr2].dead) &&
                                temp->dat.ypos > object[nr2].ypos-40 &&
                                temp->dat.ypos <= object[nr2].ypos+object[nr2].image.height+40)
                            {
                              hashit = FALSE;

                              switch (temp->dat.type2)
                              {
                                case W_MINE1 : case W_MINE2 : case W_MINE3 :
                                               break;
                                default      : if (temp->dat.type2 == W_BIGBOMB)
                                                 (BOMB_GRAVITY < 0) ? (hilfsy = -5) : (hilfsy = 5);
                                               else hilfsy = 0;
                                               if (collision(temp->dat.xpos,temp->dat.ypos+hilfsy,temp->dat.image.width,temp->dat.image.height,
                                                             object[nr2].xpos,
                                                             object[nr2].ypos-hilfsy,
                                                             object[nr2].image.width,
                                                             object[nr2].image.height) )

                                               {
                                                 done = TRUE;
                                                 switch (object[nr2].type)
                                                 {
                                                   case MAN1 : case PILE :
                                                   case SWITCHR : case SWITCHL :
                                                   case SWITCHU : case SWITCHD :
                                                   case DOORH : case DOORV :
                                                   case DOORH2 : case DOORV2 :
                                                             hashit = TRUE;
                                                             break;
                                                   default : if (getpixel(vscreen,temp->dat.xpos-map_x[1],temp->dat.ypos-map_y[1]) > 0)
                                                               hashit = TRUE;
                                                 } // switch

                                               } // if collision
                              }

                               // Bei geoffneten Tueren Bullet nicht entfernen
                               if (((object[nr2].type == DOORH) || (object[nr2].type == DOORV) ||
                                    (object[nr2].type == DOORH2) || (object[nr2].type == DOORV2) ) &&
                                    (object[nr2].frame == 0))
                                 hashit = FALSE;

                               if (hashit)
                               {
                                 switch (temp->dat.type2)
                                 {
                                   case W_BOMB :
                                                 play_sound_sample(snd_exp_sm_1,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                                                 add_bomb_bullets(temp);
                                                 break;
                                   case W_BIGBOMB :
                                                 object[nr2].hit += 1000;
                                                 do_objects();
                                                 play_sound_sample(snd_exp_mid_3,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                                                 add_explosion(-1,expl_big2_01,13,3,0,temp->dat.xpos-35,temp->dat.ypos-40);
                                                 break;
                                   case W_ROCKET :
                                                 play_sound_sample(snd_exp_sm_5,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                                                 add_explosion(-1,expl_sm1_01,10,1,0,temp->dat.xpos,temp->dat.ypos);
                                                 object[nr2].hit += 8;
                                                 break;
//                                   case W_MINE1 :
//                                                 break;
                                   default     :
                                                 switch (object[nr2].type)
                                                 {
                                                   case BUILDING1 : case BUILDING2 :
                                                   case BUILDING3 : case BUILDING4 :
                                                   case BUILDING4_2 : case BUILDING_GRAV :
                                                   case TUNNEL1 : case PILE :
                                                     add_pixels(2,temp->dat.xpos,temp->dat.ypos,0,0.4,400,tbl_white,10,50,0,0,0);
                                                     break;
                                                   case MAN1 :
                                                     add_pixels(100,temp->dat.xpos,temp->dat.ypos,0,0.6,800,tbl_red,10,40,0,0,0);
                                                     object[nr2].dead = 1;
                                                   default :
                                                     break;
                                                 } // switch type
                                 } // switch type2

                                 object[nr2].onehit = TRUE;

                                 switch (temp->dat.type2)
                                 {
                                   case W_DEFDOUBLE : object[nr2].hit+=DEFDOUBLE_POWER;
                                                      break;
                                   default          : object[nr2].hit++;
                                 }

//                                 if (object[nr2].hit >= object[nr2].maxhit) object[nr2].dead = TRUE;
                                 if (!erased)
                                   erased = erase_bullet(temp);
                                 done = TRUE;
                               } // if getroffen

                            } // if active
                          } // for nr2 (objects)

                          if (!done)
                          for (nr2=0; nr2 < base_anz; nr2++)
                          {
                              hashit = FALSE;

                              switch (temp->dat.type2)
                              {
//                                case W_MINE1 :
//                                             break;
                                case W_BOMB :
                                case W_BIGBOMB :
                                             if (temp->dat.type2 == W_BIGBOMB) hilfsy = 5; else hilfsy = 0;
                                             if (collision(temp->dat.xpos,temp->dat.ypos+hilfsy,temp->dat.image.width,temp->dat.image.height,
                                                           base[nr2].x1,
                                                           base[nr2].y-hilfsy,
                                                           base[nr2].x2-base[nr2].x1,
                                                           5) )
                                             {
                                               done = TRUE;
                                               switch (temp->dat.type2)
                                               {
                                                 case W_BOMB :
                                                               play_sound_sample(snd_exp_sm_1,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                                                               add_bomb_bullets(temp);
                                                               break;
                                                 case W_BIGBOMB :
                                                               play_sound_sample(snd_exp_mid_3,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                                                               add_explosion(-1,expl_big2_01,13,3,0,temp->dat.xpos-35,temp->dat.ypos-40);
                                                               break;
                                               } // switch
                                               if (!erased)
                                                 erased = erase_bullet(temp);
                                               done = TRUE;
                                             } // if collision
                               } // if getroffen
                          } // for nr2 (bases)

        } // if b_own or b_neutral

        if ( (temp->dat.type == B_ENEMY) || (temp->dat.type == B_NEUTRAL) )
        {
                          for (nr2=0; nr2 < player_anz; nr2++)
                          {
                          if (playship[nr2].dead == 0 && temp->dat.ypos+40 >= playship[nr2].ypos && temp->dat.ypos-40 <= playship[nr2].ypos+PLAYER_HEIGHT )
                          {
                            if (temp->dat.type2 == W_MINE1 ||
                                temp->dat.type2 == W_MINE2 ||
                                temp->dat.type2 == W_MINE3)
                            {
                              if (strecke(temp->dat.xpos+temp->dat.image.width/2,
                                          temp->dat.ypos+temp->dat.image.height/2,
                                          playship[nr2].xpos+PLAYER_WIDTH/2,
                                          playship[nr2].ypos+PLAYER_HEIGHT/2) < (temp->dat.user1/2))
                              {
                                temp->dat.user2 = TRUE;
                                done = TRUE;
                              }
                            }

                            if (!done)
                            if (collision(temp->dat.xpos,temp->dat.ypos,temp->dat.image.width,temp->dat.image.height,
                                          playship[nr2].xpos+3,playship[nr2].ypos+3,
                                          PLAYER_WIDTH-3,PLAYER_HEIGHT-3) )
                            {
                              switch (temp->dat.type2)
                              {
                                case W_BOMB :
                                      playship[nr2].shield-=2;
                                      playship[nr2].onehit = TRUE;
                                      if (playship[nr2].shield > 0) add_explosion(nr2,effect_shield_1,3,6,0,playship[nr2].xpos-6,playship[nr2].ypos-5);
                                      update_panel_shield();
                                      break;
                                case W_BIGBOMB :
                                      playship[nr2].shield-=10;
                                      break;
                                case W_ROCKET :
                                      playship[nr2].shield-=3;
                                      playship[nr2].onehit = TRUE;
                                      if (playship[nr2].shield > 0) add_explosion(nr2,effect_shield_1,3,6,0,playship[nr2].xpos-6,playship[nr2].ypos-5);
                                      update_panel_shield();
                                      break;
                                default :
                                      playship[nr2].shield--;
                                      playship[nr2].onehit = TRUE;
                                      if (playship[nr2].shield > 0) add_explosion(nr2,effect_shield_1,3,6,0,playship[nr2].xpos-6,playship[nr2].ypos-5);
                                      update_panel_shield();
                              }

                              if (playship[nr2].shield <= 0)
                              {
                                add_explosion(nr2,expl_big_1,5,2,0,playship[nr2].xpos-2,playship[nr2].ypos-2);
                                playship[nr2].dead = 1;
                              }

                              if (!erased)
                                erased = erase_bullet(temp);
                              done = TRUE;
                            } // if getroffen
                          } // if !dead
                          }

                          if (!done)
                          for (nr2=0; nr2 <= enemy_anz; nr2++)
                          {
                              if (enemy[nr2].active && temp->dat.nr != nr2 &&
                                  temp->dat.ypos > enemy[nr2].ypos-40 &&
                                  temp->dat.ypos <= enemy[nr2].ypos+enemy[nr2].image.height+40)
                              {
                                switch (temp->dat.type2)
                                {
                                  case W_MINE1 : case W_MINE2 : case W_MINE3 :
//                                        done = TRUE;
                                        break;
                                  default :
//                                        if (enemy[nr2].user3 >= 5)
                                        if (collision(temp->dat.xpos,temp->dat.ypos,temp->dat.image.width,temp->dat.image.height,
                                                      enemy[nr2].xpos+enemy[nr2].image.boundxa,
                                                      enemy[nr2].ypos+enemy[nr2].image.boundya,
                                                      enemy[nr2].image.boundxe,
                                                      enemy[nr2].image.boundye) )
                                        {
                                          // friendly ship
                                          if (enemy[nr2].user3 >= 5)
                                          {
                                            enemy[nr2].hit++;
                                            play_sound_sample(snd_bonk,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                                            switch (enemy[nr2].type)
                                            {
                                              case ENEMY_NSHIP_1 :
                                                add_pixels(50*enemy[nr2].hit/enemy[nr2].maxhit,temp->dat.xpos,temp->dat.ypos,2-(rand() % 3),2-(rand() % 3),60,tbl_blue,10,50,0,0,0);
                                                add_explosion(-1,expl_sm1_01+5,5,1,0,temp->dat.xpos,temp->dat.ypos);
                                                break;
                                            } // switch type
                                          }
                                          if (!erased)
                                            erased = erase_bullet(temp);
                                          done = TRUE;
                                        }
                                }
                              }
                          }

                          if (!done)
                          for (nr2=0; nr2 < object_anz; nr2++)
                          {
                            if ((object[nr2].active) && (!object[nr2].dead) &&
                                temp->dat.ypos > object[nr2].ypos-40 &&
                                temp->dat.ypos <= object[nr2].ypos+object[nr2].image.height+40)
                            {
                              hashit = FALSE;

                              switch (temp->dat.type2)
                              {
                                case W_MINE1 : case W_MINE2 : case W_MINE3 :
                                               hashit = FALSE;
                                               break;
                                default      : if (temp->dat.type2 == W_BIGBOMB)
                                                 (BOMB_GRAVITY < 0) ? (hilfsy = -5) : (hilfsy = 5);
                                               else hilfsy = 0;
                                               if (collision(temp->dat.xpos,temp->dat.ypos+hilfsy,temp->dat.image.width,temp->dat.image.height,
                                                             object[nr2].xpos,
                                                             object[nr2].ypos-hilfsy,
                                                             object[nr2].image.width,
                                                             object[nr2].image.height) )
                                               {
                                                 hashit = TRUE;
                                               }
                               }

                               // Bei geoffneten Tueren Bullet nicht entfernen
                               if (((object[nr2].type == DOORH) || (object[nr2].type == DOORV) ||
                                    (object[nr2].type == DOORH2) || (object[nr2].type == DOORV2) ) &&
                                    (object[nr2].frame == 0))
                                 hashit = FALSE;

                               if (hashit)
                               {
                                 switch (temp->dat.type2)
                                 {
                                   case W_BOMB :
                                                 play_sound_sample(snd_exp_sm_1,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                                                 add_bomb_bullets(temp);
                                                 break;
                                   case W_BIGBOMB :
                                                 object[nr2].hit += 1000;
                                                 do_objects();
                                                 play_sound_sample(snd_exp_mid_3,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                                                 add_explosion(-1,expl_big2_01,13,3,0,temp->dat.xpos-35,temp->dat.ypos-40);
                                                 break;
                                   case W_ROCKET :
                                                 play_sound_sample(snd_exp_sm_4,1000,0,temp->dat.xpos,temp->dat.ypos,100);
                                                 add_explosion(-1,expl_sm1_01,10,1,0,temp->dat.xpos,temp->dat.ypos);
                                                 object[nr2].hit += 8;
                                                 break;
//                                   case W_MINE1 :
//                                                 break;
                                   default     :
                                                 switch (object[nr2].type)
                                                 {
                                                   case BUILDING1 : case BUILDING2 :
                                                   case BUILDING3 : case BUILDING4 :
                                                   case BUILDING4_2 : case BUILDING_GRAV :
                                                   case TUNNEL1 : case PILE :
                                                     add_pixels(2,temp->dat.xpos,temp->dat.ypos,0,0.4,400,tbl_white,10,50,0,0,0);
                                                     break;
                                                   case MAN1 :
                                                     add_pixels(100,temp->dat.xpos,temp->dat.ypos,0,0.6,800,tbl_red,10,40,0,0,0);
                                                     object[nr2].dead = 1;
                                                   default :
                                                     break;
                                                 } // switch type
                                 } // switch type2

                                 object[nr2].onehit = TRUE;

                                 switch (temp->dat.type2)
                                 {
                                   case W_DEFDOUBLE : object[nr2].hit+=DEFDOUBLE_POWER;
                                                      break;
                                   default          : object[nr2].hit++;
                                 }

                                 if (!erased)
                                   erased = erase_bullet(temp);
                                 done = TRUE;
                               } // if getroffen

                            } // if active
                          } // for nr2 (objects)
        } // if b_enemy or b_neutral
      } // if not done (1)

      if (!done)
      {
        if ( (temp->dat.type2 == W_BOMB) || (temp->dat.type2 == W_BIGBOMB) )
        {
          // hit wall?
          // or "bump count" (user2) too high?
          if ( (getpixel(vscreen,temp->dat.xpos+temp->dat.image.width/2-map_x[1]-10,temp->dat.ypos+temp->dat.image.height/2-map_y[1]) > 0) ||
               (getpixel(vscreen,temp->dat.xpos+temp->dat.image.width/2-map_x[1]+10,temp->dat.ypos+temp->dat.image.height/2-map_y[1]) > 0) ||
               (getpixel(vscreen,temp->dat.xpos+temp->dat.image.width/2-map_x[1],temp->dat.ypos+temp->dat.image.height/2-map_y[1]-10) > 0) ||
               (getpixel(vscreen,temp->dat.xpos+temp->dat.image.width/2-map_x[1],temp->dat.ypos+temp->dat.image.height/2-map_y[1]+10) > 0) ||
               temp->dat.user2 > 9)
          {
            //temp->dat.dead = 1;

            if (temp->dat.type2 == W_BOMB)
            {
              add_bomb_bullets(temp);
              play_sound_sample(snd_exp_sm_1,1000,0,temp->dat.xpos,temp->dat.ypos,100);
              if (temp->dat.user2 < 5)
                add_crater(temp->dat.xpos+1,temp->dat.ypos+1,8);
              add_explosion(-1,expl_sm1_01,10,1,0,temp->dat.xpos,temp->dat.ypos);
            }
            else if (temp->dat.type2 == W_BIGBOMB)
            {
              play_sound_sample(snd_exp_mid_3,1000,0,temp->dat.xpos,temp->dat.ypos,100);
              if (temp->dat.user2 < 5)
                add_crater(temp->dat.xpos+1,temp->dat.ypos+1,20);
              add_explosion(-1,expl_big2_01,13,3,0,temp->dat.xpos-35,temp->dat.ypos-50);
            }

            done = TRUE;
            if (!erased)
              erased = erase_bullet(temp);

          } // addbullets
        } // Bombe unmittelbar vor Auftreffen aktivieren
        else if ( ((temp->dat.type2 == W_ROCKET)) && (!done))
        {
          pnr = 0;
          pnr = getpixel(vscreen,temp->dat.xpos-map_x[1],temp->dat.ypos-map_y[1]);
          if (pnr == 0) { pnr = getpixel(vscreen,temp->dat.xpos+temp->dat.image.width/2-map_x[1]-4,temp->dat.ypos+temp->dat.image.height/2-map_y[1]); r = 1; }
          if (pnr == 0) { pnr = getpixel(vscreen,temp->dat.xpos+temp->dat.image.width/2-map_x[1]+4,temp->dat.ypos+temp->dat.image.height/2-map_y[1]); r = 2; }
          if (pnr == 0) { pnr = getpixel(vscreen,temp->dat.xpos+temp->dat.image.width/2-map_x[1],temp->dat.ypos+temp->dat.image.height/2-map_y[1]-5); r = 3; }
          if (pnr == 0) { pnr = getpixel(vscreen,temp->dat.xpos+temp->dat.image.width/2-map_x[1],temp->dat.ypos+temp->dat.image.height/2-map_y[1]+5); r = 4; }

          if ( pnr )
          {

            if (temp->dat.type2 == W_ROCKET)
            {
              play_sound_sample(snd_exp_sm_4,1000,0,temp->dat.xpos,temp->dat.ypos,100);
              add_crater(temp->dat.xpos+1,temp->dat.ypos+1,6);
              add_explosion(-1,expl_sm1_01,10,1,0,temp->dat.xpos-2,temp->dat.ypos-2);
            }

            if (mainpal[pnr].b < 30)
            {
              if (temp->dat.yspd < 0) ty = -0.4;
              else ty = 0.9;

              tx = ((rand() % 4) - 2) / 5;

              pspd = abs(sqrt(pow(temp->dat.yspd,2) + pow(temp->dat.xspd,2)));
              if (strstr("gfw1.fmp",gfx_name)) tablepntr = tbl_green;
              else tablepntr = tbl_white;
              add_pixels((rand() % ((int)(pspd*2)+1)) + 50,temp->dat.xpos,temp->dat.ypos,tx,ty,400,tablepntr,10,50,0,0,0);
            }

            done = TRUE;
            if (!erased)
              erased = erase_bullet(temp);

          } // if pnr
        } // else rockets
        else if (temp->dat.type2 == W_MINE1 || temp->dat.type2 == W_MINE2 || temp->dat.type2 == W_MINE3)
        {
        }
        else
        {
          // Umgebungskontakt checken
          pnr = 0;
          pnr = getpixel(vscreen,temp->dat.xpos-map_x[1],temp->dat.ypos-map_y[1]);
          if (pnr == 0) pnr = getpixel(vscreen,temp->dat.xpos+temp->dat.image.width/2-map_x[1]-1,temp->dat.ypos+temp->dat.image.height/2-map_y[1]);
          if (pnr == 0) pnr = getpixel(vscreen,temp->dat.xpos+temp->dat.image.width/2-map_x[1]+1,temp->dat.ypos+temp->dat.image.height/2-map_y[1]);
          if (pnr == 0) pnr = getpixel(vscreen,temp->dat.xpos+temp->dat.image.width/2-map_x[1],temp->dat.ypos+temp->dat.image.height/2-map_y[1]-1);
          if (pnr == 0) pnr = getpixel(vscreen,temp->dat.xpos+temp->dat.image.width/2-map_x[1],temp->dat.ypos+temp->dat.image.height/2-map_y[1]+1);
          if (temp->dat.user2 > 2) pnr = 255;
          if ( pnr )
          {
            if (pnr < 255 && mainpal[pnr].b < 30)
            {
              if (temp->dat.yspd < 0) ty = -0.4;
              else ty = 0.9;

              tx = ((rand() % 4) - 2) / 5;

              pspd = abs(sqrt(pow(temp->dat.yspd,2) + pow(temp->dat.xspd,2)));
              if (strstr("gfw1.fmp",gfx_name)) tablepntr = tbl_green;
              else tablepntr = tbl_white;

              add_pixels((rand() % ((int)(pspd*2)+1)) + 1,temp->dat.xpos,temp->dat.ypos,tx,ty,400,tablepntr,10,50,0,0,0);
            }

            done = TRUE;

            if (!erased)
              erased = erase_bullet(temp);
          } // fuer normale Bullets und fehlgeleitete Bombs
        } // else
      } // if not done (2)

      if (!done)
      {
        temp2 = first_bullet;

        switch (temp->dat.type)
        {
          case B_OWN :
                        if (temp->dat.type2 == W_MINE1 || temp->dat.type2 == W_MINE2 || temp->dat.type2 == W_MINE3)
                          while (temp2)
                          {
                            next_bull2 = temp2->next;

                            if (temp->dat.ypos > temp2->dat.ypos-8 &&
                                temp->dat.ypos <= temp2->dat.ypos+temp2->dat.image.height+8 &&
                                (temp2->dat.type == B_ENEMY || temp2->dat.type == B_NEUTRAL))
                              if (collision(temp->dat.xpos-4,temp->dat.ypos-4,temp->dat.image.width+8,temp->dat.image.height+8,
                                            temp2->dat.xpos,temp2->dat.ypos,temp2->dat.image.width,temp2->dat.image.height))
                              {
                                temp->dat.user2 = TRUE;
                              }
                            temp2 = next_bull2;
                          }
                        break;
          case B_ENEMY :
                        if (temp->dat.type2 == W_MINE1 || temp->dat.type2 == W_MINE2 || temp->dat.type2 == W_MINE3)
                          while (temp2)
                          {
                            next_bull2 = temp2->next;

                            if (temp->dat.ypos > temp2->dat.ypos-8 &&
                                temp->dat.ypos <= temp2->dat.ypos+temp2->dat.image.height+8 &&
                                (temp2->dat.type == B_OWN || temp2->dat.type == B_NEUTRAL))
                              if (collision(temp->dat.xpos-4,temp->dat.ypos-4,temp->dat.image.width+8,temp->dat.image.height+8,
                                            temp2->dat.xpos,temp2->dat.ypos,temp2->dat.image.width,temp2->dat.image.height))
                              {
                                temp->dat.user2 = TRUE;
                              }
                            temp2 = next_bull2;
                          }
                        break;
          case B_NEUTRAL :
                        if (temp->dat.type2 == W_MINE1 || temp->dat.type2 == W_MINE2 || temp->dat.type2 == W_MINE3)
                          while (temp2)
                          {
                            next_bull2 = temp2->next;

                            if (temp->dat.ypos > temp2->dat.ypos-8 &&
                                temp->dat.ypos <= temp2->dat.ypos+temp2->dat.image.height+8 &&
                                (temp2->dat.type == B_OWN || temp2->dat.type == B_ENEMY))
                              if (collision(temp->dat.xpos-4,temp->dat.ypos-4,temp->dat.image.width+8,temp->dat.image.height+8,
                                            temp2->dat.xpos,temp2->dat.ypos,temp2->dat.image.width,temp2->dat.image.height))
                              {
                                temp->dat.user2 = TRUE;
                              }
                            temp2 = next_bull2;
                          }
                        break;
        } // switch type
      }

      temp = next_bull;
  } // while !null

//  check_mines();

}

// Checken, ob Enemy oder andere Mine in Detonationsradius
void check_mine_radius(BNODE *tb)
{
  int nr,c;
  struct BNODE *temp, *next_bull;
  float s;
  float a;

  temp = first_bullet;

  // Andere Minen?
  while (temp!=NULL)
  {
    next_bull = temp->next;

    switch (temp->dat.type2)
    {
      case W_MINE1 : case W_MINE2 : case W_MINE3 :
         if (strecke(temp->dat.xpos,temp->dat.ypos,tb->dat.xpos,tb->dat.ypos) < tb->dat.user1/1.5)
         {
           temp->dat.user2 = 5;
         }
         break;
    }
    temp = next_bull;
  } // while temp!=null

  switch (tb->dat.type)
  {
    case B_OWN :
           // Enemy hit?
           for (nr=0; nr < enemy_anz; nr++)
           {
             if (enemy[nr].active)
             if (tb->dat.ypos-100 < enemy[nr].ypos &&
                 tb->dat.ypos+tb->dat.image.height+100 > enemy[nr].ypos+enemy[nr].image.height)
             {
               s = strecke(tb->dat.xpos+tb->dat.image.width/2,
                           tb->dat.ypos+tb->dat.image.height/2,
                           enemy[nr].xpos+enemy[nr].image.width/2,
                           enemy[nr].ypos+enemy[nr].image.width/2);
           
               if (s < tb->dat.user1)
               {
                 a = (1-(s/tb->dat.user1)) * tb->dat.active;
                 enemy[nr].hit += a;
                 tb->dat.user2 = TRUE;

                 if (enemy[nr].hit <= enemy[nr].maxhit)
                 {
                   upanel_bar_hit = enemy[nr].hit;
                   upanel_bar_maxhit = enemy[nr].maxhit;
                   update_upanel_bar();
                 }

                 switch (enemy[nr].type)
                 {
                   case ENEMY_SHIP_1 :
                    add_pixels(enemy[nr].hit/enemy[nr].maxhit,
                                enemy[nr].xpos+enemy[nr].image.width/2,
                                enemy[nr].ypos+enemy[nr].image.height/2,
                                1-(rand() % 1),1-(rand() % 1),
                                60,tbl_blue,10,50,0,0,0);
                     break;
                 }
               }
             }
           } // for enemynr

           // Player <-> Player
           if (game_mode == MP_2PDOGFIGHT)
           for (c=0; c < player_anz; c++)
           if (c != tb->dat.nr)
           {
             s = strecke(tb->dat.xpos+tb->dat.image.width/2,
                         tb->dat.ypos+tb->dat.image.height/2,
                         playship[c].xpos+PLAYER_WIDTH/2,
                         playship[c].ypos+PLAYER_HEIGHT/2);
             if (s < tb->dat.user1 && !playship[c].dead)
             {
               a = (float)(1.0-((float)s/(float)tb->dat.user1)) * (float)tb->dat.active;
               playship[c].shield -= a/20.0;
               playship[c].onehit = TRUE;
               if (playship[c].shield > 0) add_explosion(c,effect_shield_1,3,6,0,playship[c].xpos-6,playship[c].ypos-5);
               if (playship[c].shield <= 0) { playship[c].dead = 1; player_stat[tb->dat.nr].frags++; }
               update_panel_shield();
               tb->dat.user2 = TRUE;
             }
           }
           break;

    case B_ENEMY :
           for (c=0; c < player_anz; c++)
           {
             s = strecke(tb->dat.xpos+tb->dat.image.width/2,
                         tb->dat.ypos+tb->dat.image.height/2,
                         playship[c].xpos+PLAYER_WIDTH/2,
                         playship[c].ypos+PLAYER_HEIGHT/2);
             if (s < tb->dat.user1 && !playship[c].dead)
             {
               a = (float)(1.0-((float)s/(float)tb->dat.user1)) * (float)tb->dat.active;
               playship[c].shield -= a/20.0;
               playship[c].onehit = TRUE;
               if (playship[c].shield > 0) add_explosion(c,effect_shield_1,3,6,0,playship[c].xpos-6,playship[c].ypos-5);
               if (playship[c].shield <= 0) playship[c].dead = 1;
               update_panel_shield();
               tb->dat.user2 = TRUE;
             }
           }
           break;
  } // switch type

}

void detonate_mines()
{
  struct BNODE *temp, *next_bull;

  temp = first_bullet;

  while (temp!=NULL)
  {
    next_bull = temp->next;

    if (temp->dat.type == B_OWN)
      switch (temp->dat.type2)
      {
        case W_MINE1 : case W_MINE2 : case W_MINE3 :
           temp->dat.user2 = rand() % 10 + 1;
           break;
      }
    temp = next_bull;
  } // while temp!=null
}
