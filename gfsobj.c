/*
 *    GRAVITY FORCE, gfsobj.c
 *
 *    -- object and special object handling
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfdraw.h"
#include "gfsobj.h"
#include "gf.h"
#include "gflink.h"
#include "gfsprt.h"
#include "gfsound.h"
#include "gfinit.h"
#include "gfmes.h"
#include "gfosd.h"
#include "gfpanel.h"
#include "gfstat.h"
#include "gfspibul.h"

int last_tele,last_in,last_out;

void do_objects()
{
  int nr,nr2;
  int w,h;

  for (nr=0; nr < object_anz; nr++)
  {
    w = object[nr].image.width/2;
    h = object[nr].image.height/2;

    if (object[nr].active)
    {
      if (object[nr].maxframe > 0)
      {
        object[nr].framet++;
        if (object[nr].framet >= object[nr].maxframet) { object[nr].frame++; object[nr].framet = 0; }
        if (object[nr].frame > object[nr].maxframe) object[nr].frame=object[nr].startframe;
      }

      if (object[nr].onehit)
        switch (object[nr].type)
        {
          case SWITCHL : case SWITCHR : case SWITCHU : case SWITCHD :
            if (object[nr].frame == 0) object[nr].frame = 1;
              else if (object[nr].frame == 1) object[nr].frame = 0;
            object[nr].onehit = FALSE;
            play_sound_sample(snd_switch,1000,0,object[nr].xpos+w,object[nr].ypos+h,100);
            break;
          default : object[nr].onehit = FALSE;
        }

      switch (object[nr].type)
      {
        case PILE     :  switch (object[nr].frame)
                         {
                           case 0 : object[nr].image.data = pile1;
                                    break;
                           case 1 : object[nr].image.data = pile2;
                                    break;
                           default : object[nr].image.data = pile1;
                         }
                         break;
        case SWITCHL  :  switch (object[nr].frame)
                         {
                           case 0 : object[nr].image.data = obj_switchl_1;
                                    break;
                           case 1 : object[nr].image.data = obj_switchl_2;
                                    break;
                         }
                         break;
        case SWITCHR  :  switch (object[nr].frame)
                         {
                           case 0 : object[nr].image.data = obj_switchr_1;
                                    break;
                           case 1 : object[nr].image.data = obj_switchr_2;
                                    break;
                         }
                         break;
        case SWITCHU  :  switch (object[nr].frame)
                         {
                           case 0 : object[nr].image.data = obj_switchu_1;
                                    break;
                           case 1 : object[nr].image.data = obj_switchu_2;
                                    break;
                         }
                         break;
        case SWITCHD  :  switch (object[nr].frame)
                         {
                           case 0 : object[nr].image.data = obj_switchd_1;
                                    break;
                           case 1 : object[nr].image.data = obj_switchd_2;
                                    break;
                         }
                         break;
        case BUILDING1 : object[nr].image.data = obj_building1_1;
                         break;
        case BUILDING2 : object[nr].image.data = obj_building2_1;
                         break;
        case BUILDING3 : object[nr].image.data = obj_building3_1;
                         break;
        case BUILDING4 : object[nr].image.data = obj_building4_1;
                         break;
        case BUILDING4_2 : object[nr].image.data = obj_building4_2;
                         break;
        case BUILDING_GRAV : object[nr].image.data = obj_b_grav_1+object[nr].frame;
                         add_2_list(&rl,object[nr].xpos+8,object[nr].ypos+4,object[nr].image.width-16,object[nr].image.height-10);
                         break;
        case TUNNEL1   : object[nr].image.data = obj_tunnel1_1+object[nr].frame;
                         break;
        case MAN1      : object[nr].image.data = obj_man1_1+object[nr].frame;
                         break;
        case DOORH     : object[nr].image.data = obj_doorh_1+object[nr].frame;
                         break;
        case DOORV     : object[nr].image.data = obj_doorv_1+object[nr].frame;
                         break;
        case DOORH2    : object[nr].image.data = obj_doorh2_1+object[nr].frame;
                         break;
        case DOORV2    : object[nr].image.data = obj_doorv2_1+object[nr].frame;
                         break;
        case POOL1    :  switch (object[nr].frame)
                         {
                           case 0 : object[nr].image.data = obj_pool1_1;
                                    break;
                           case 1 : object[nr].image.data = obj_pool1_2;
                                    break;
                           case 2 : object[nr].image.data = obj_pool1_3;
                                    break;
                           case 3 : object[nr].image.data = obj_pool1_2;
                                    break;
                         }
                         break;
        case POOL2    :  switch (object[nr].frame)
                         {
                           case 0 : object[nr].image.data = obj_pool2_1;
                                    break;
                           case 1 : object[nr].image.data = obj_pool2_2;
                                    break;
                           case 2 : object[nr].image.data = obj_pool2_3;
                                    break;
                           case 3 : object[nr].image.data = obj_pool2_2;
                                    break;
                         }
                         break;
        case RPOLEL    : object[nr].image.data = obj_rpolel_1+object[nr].frame;
                         break;
        case RPOLER    : object[nr].image.data = obj_rpoler_1+object[nr].frame;
                         break;
        case RPOLEU    : object[nr].image.data = obj_rpoleu_1+object[nr].frame;
                         break;
        case RPOLED    : object[nr].image.data = obj_rpoled_1+object[nr].frame;
                         break;
        case TREE      : object[nr].image.data = obj_tree_2;
                         break;

      } // switch frame

      if (object[nr].hit >= object[nr].maxhit) object[nr].dead = 1;

      if (object[nr].dead == 1)
      {
        add_2_list(&rl,object[nr].xpos,object[nr].ypos,object[nr].image.width,object[nr].image.height);
        object[nr].dead = 0;
        object[nr].active = FALSE;

        clear_screen(0);

        switch (object[nr].type)
        {
           case BUILDING1 : case BUILDING2 :
           case BUILDING3 : case BUILDING4 : case BUILDING4_2 :
           case TUNNEL1 : case POOL1 : case POOL2 :
                 play_sound_sample(snd_exp_big_2,1000,0,object[nr].xpos+w,object[nr].ypos+h,100);
                 add_crater(object[nr].xpos+w,object[nr].ypos+h*2,22);
                 for (nr2=0; nr2 < 5; nr2++)
                 {
                   add_explosion(-1,expl_big_1,5,4,(nr2*5),object[nr].xpos+rand() % (w*2),object[nr].ypos+rand() % (h*2));
                   add_explosion(-1,expl_sm1_01,10,2,(nr2*3),object[nr].xpos+rand() % (w*2),object[nr].ypos+rand() % (h*2));
                   add_pixels(80,object[nr].xpos+w,object[nr].ypos+h,3-(rand() % 4),(rand() % 2),50,tbl_white,10,50,0,0,0);
                   if (SHOW_DEBRIS) add_pixels(15,object[nr].xpos+w,object[nr].ypos+h,1-(rand() % 2),(rand() % 2)+1,50,tbl_white,10,10,0,0,debris_4_01);
                 }
                 break;
           case BUILDING_GRAV :
                 play_sound_sample(snd_exp_big_2,1000,0,object[nr].xpos+w,object[nr].ypos+h,100);
                 add_crater(object[nr].xpos+w/2,object[nr].ypos+h*2,25);
                 add_crater(object[nr].xpos+w*2-w/2,object[nr].ypos+h*2,25);
                 for (nr2=0; nr2 < 10; nr2++)
                 {
                   add_explosion(-1,expl_big_1,5,4,(nr2*10),object[nr].xpos+rand() % (w*2),object[nr].ypos+rand() % (h*2));
                   add_explosion(-1,expl_sm1_01,10,2,(nr2*6),object[nr].xpos+rand() % (w*2),object[nr].ypos+rand() % (h*2));
                   add_pixels(100,object[nr].xpos+w,object[nr].ypos+h,3-(rand() % 4),(rand() % 2),50,tbl_white,10,50,0,0,0);
                   if (SHOW_DEBRIS) add_pixels(15,object[nr].xpos+w,object[nr].ypos+h,1-(rand() % 2),(rand() % 2)+1,50,tbl_white,10,10,0,0,debris_4_01);
                 }
                 break;
           case MAN1 :
                 play_sound_sample(snd_scream,1000,0,object[nr].xpos+w,object[nr].ypos+h,100);
                 break;
           default :
                 play_sound_sample(snd_exp_mid_2,1000,0,object[nr].xpos+w,object[nr].ypos+h,100);
                 add_crater(object[nr].xpos+w,object[nr].ypos+h*2,10);
                 add_explosion(-1,expl_sm1_01,10,2,1,object[nr].xpos+w,object[nr].ypos+h);
                 add_pixels(80,object[nr].xpos+w,object[nr].ypos+h,2-(rand() % 3),(rand() % 2),50,tbl_white,10,50,0,0,0);
        }

      } // if dead

    } // if active
  } // for nr
}

void apply_power(int x, int y, float *xs, float *ys, int nr, int type)
{
  float angle,scale_x,scale_y,xrot,yrot;
  int gx,gy,s;

  gx = spobject[nr].xpos+spobject[nr].image.width/2;
  gy = spobject[nr].ypos+spobject[nr].image.height/2;

  angle = V_GetAngle(gx,gy,x,y);
  xrot = sin(angle);
  yrot = cos(angle);

  if (x >= gx)
    scale_x = -xrot;
  else
    scale_x = xrot;
  scale_y = -yrot;

  if (type==0) s = 50.0;
  else s = 20.0;

  *xs += scale_x/s;

  if (type==0)
    *ys -= scale_y/s;
  else
    *ys += scale_y/s;

}

void apply_lin_power(int x, int y, float *xs, float *ys, float factor, int nr)
{
  int tx1,ty1,tx2,ty2,tw,th,xab,yab,xabmax,yabmax;
  float p;

  tx1 = x;
  ty1 = y;
  tx2 = spobject[nr].xpos;
  ty2 = spobject[nr].ypos;
  tw  = spobject[nr].image.width;
  th  = spobject[nr].image.height;

  switch (spobject[nr].type)
  {
    case GWALL_L :
                   xab = tx2-tx1;
                   xabmax = spobject[nr].powr*10;

                   if ( (ty1 > ty2) && (ty1 < ty2+th) &&
                        (xab < xabmax) && (xab > 0) )
                   {
                     p = 1.0-((float)xab/(float)xabmax);
                     *xs += ((float)spobject[nr].powr * p) * factor;
                   }
                   break;
    case GWALL_R :
                   xab = tx1-tx2;
                   xabmax = spobject[nr].powr*10;

                   if ( (ty1 > ty2) && (ty1 < ty2+th) &&
                        (xab < xabmax) && (xab > 0) )
                   {
                     p = 1.0-((float)xab/(float)xabmax);
                     *xs -= ((float)spobject[nr].powr * p) * factor;
                   }
                   break;
    case GWALL_U :
                   yab = ty2-ty1;
                   yabmax = spobject[nr].powr*10;

                   if ( (tx1 > tx2) && (tx1 < tx2+tw) &&
                        (yab < yabmax) && (yab > 0) )
                   {
                     p = 1.0-((float)yab/(float)yabmax);
                     *ys -= ((float)spobject[nr].powr * p) * factor;
                   }
                   break;
    case GWALL_D :
                   yab = ty1-ty2;
                   yabmax = spobject[nr].powr*10;

                   if ( (tx1 > tx2) && (tx1 < tx2+tw) &&
                        (yab < yabmax) && (yab > 0) )
                   {
                     p = 1.0-((float)yab/(float)yabmax);
                     *ys += ((float)spobject[nr].powr * p) * factor;
                   }
                   break;
  } // switch type

}


void do_spobjects()
{
  int nr,nr2;
  struct BNODE *temp, *next_bull;
  struct PNODE *ptemp, *next_pixel;
  extern int GRAV_4_PIXELS;

  for (nr=0; nr < spobject_anz; nr++)
  {
    if (spobject[nr].active)
    {
      if (spobject[nr].maxframe > 0)
      {
        spobject[nr].framet++;
        if (spobject[nr].framet >= spobject[nr].maxframet) { spobject[nr].frame++; spobject[nr].framet = 0; }
        if (spobject[nr].frame > spobject[nr].maxframe) spobject[nr].frame=spobject[nr].startframe;
      }

      switch (spobject[nr].type)
      {
        case TELEPORT  :
                         spobject[nr].image.data = effect_tele_01+spobject[nr].frame;

                         // playship beamen
                         for (nr2=0; nr2 < player_anz; nr2++)
                         if (collision(playship[nr2].xpos,playship[nr2].ypos,PLAYER_WIDTH,PLAYER_HEIGHT,
                                       spobject[nr].xpos+spobject[nr].image.width/2,spobject[nr].ypos+spobject[nr].image.height/2,1,1))
                         {
                           if (spobject[nr].min_x && spobject[nr].min_y)
                           {
                             play_sound_sample(snd_beamship,1000,0,spobject[nr].xpos+spobject[nr].image.width/2,spobject[nr].ypos+spobject[nr].image.height/2,100);
                             add_explosion(-1,effect_beam1_1,5,4,0,playship[nr2].xpos,playship[nr2].ypos);
                             playship[nr2].xpos = spobject[nr].min_x;
                             playship[nr2].ypos = spobject[nr].min_y;
                             add_explosion(0,effect_beam1_1,5,4,0,playship[nr2].xpos,playship[nr2].ypos);
                           }
                           else if ( (spobject[spobject[nr].connect].active) && (playship[nr2].tele == 0) )
                           {
                             play_sound_sample(snd_beamship,1000,0,spobject[nr].xpos+spobject[nr].image.width/2,spobject[nr].ypos+spobject[nr].image.height/2,100);
                             add_explosion(-1,effect_beam1_1,5,2,0,playship[nr2].xpos,playship[nr2].ypos);
                             playship[nr2].xpos = spobject[spobject[nr].connect].xpos;//+spobject[nr].image.width+1;
                             playship[nr2].ypos = spobject[spobject[nr].connect].ypos;//+spobject[nr].image.height/2;
                             playship[nr2].tele = 1; // nach Bemaen Beamcounter aktivieren und 1 sek. nicht mehr beamen
                             playship[nr2].telet = 1;
                             redraw = TRUE;
//                             add_explosion(-1,effect_beam1_1,5,2,0,playship[0].xpos,playship[0].ypos);
                           }
                         }

                         // weapons beamen
                         temp = first_bullet;
                         while (temp!=NULL)
                         {
                           next_bull = temp->next;
                           switch (temp->dat.type2)
                           {
//                             case W_BOMB : case W_BIGBOMB :
                               default:
                                 if (collision(temp->dat.xpos,temp->dat.ypos,temp->dat.image.width,temp->dat.image.height,
                                     spobject[nr].xpos+spobject[nr].image.width/2-8,spobject[nr].ypos+spobject[nr].image.height/2-8,14,14))
                                 {
                                   if (spobject[nr].min_x && spobject[nr].min_y)
                                   {
                                     play_sound_sample(snd_beambull,1000,0,spobject[nr].xpos+spobject[nr].image.width/2,spobject[nr].ypos+spobject[nr].image.height/2,100);
                                     add_explosion(-1,effect_beam2_01,10,1,0,temp->dat.xpos,temp->dat.ypos);
                                     temp->dat.xpos = spobject[nr].min_x;
                                     temp->dat.ypos = spobject[nr].min_y;
                                     add_explosion(-1,effect_beam2_01,10,1,0,temp->dat.xpos,temp->dat.ypos);
                                   }
                                   else if ( (spobject[spobject[nr].connect].active) && (temp->dat.user1 != nr) )
                                   {
                                     play_sound_sample(snd_beambull,1000,0,spobject[nr].xpos+spobject[nr].image.width/2,spobject[nr].ypos+spobject[nr].image.height/2,100);
                                     add_explosion(-1,effect_beam2_01,10,1,0,temp->dat.xpos,temp->dat.ypos);
                                     temp->dat.xpos = spobject[spobject[nr].connect].xpos+6;
                                     temp->dat.ypos = spobject[spobject[nr].connect].ypos+6;
                                     temp->dat.user1 = spobject[nr].connect;
                                     add_explosion(-1,effect_beam2_01,10,1,0,temp->dat.xpos,temp->dat.ypos);
                                   }
                                 } // if collision
                           }
                           temp = next_bull;
                         }
                         break;
        case TELEPORT2 : // beamt nur weapons
                         spobject[nr].image.data = effect_tele2_01+spobject[nr].frame;

                         // weapons beamen
                         temp = first_bullet;
                         while (temp!=NULL)
                         {
                           next_bull = temp->next;
                           switch (temp->dat.type2)
                           {
//                             case W_BOMB : case W_BIGBOMB :
                               default:
                                 if (collision(temp->dat.xpos,temp->dat.ypos,temp->dat.image.width,temp->dat.image.height,
                                     spobject[nr].xpos+spobject[nr].image.width/2-8,spobject[nr].ypos+spobject[nr].image.height/2-8,14,14))
                                 {
                                   if (spobject[nr].min_x && spobject[nr].min_y)
                                   {
                                     play_sound_sample(snd_beambull,1000,0,spobject[nr].xpos+spobject[nr].image.width/2,spobject[nr].ypos+spobject[nr].image.height/2,100);
                                     add_explosion(-1,effect_beam2_01,10,1,0,temp->dat.xpos,temp->dat.ypos);
                                     temp->dat.xpos = spobject[nr].min_x;
                                     temp->dat.ypos = spobject[nr].min_y;
                                     add_explosion(-1,effect_beam2_01,10,1,0,temp->dat.xpos,temp->dat.ypos);
                                   }
                                   else if ( (spobject[spobject[nr].connect].active) && (temp->dat.user1 != nr) )
                                   {
                                     play_sound_sample(snd_beambull,1000,0,spobject[nr].xpos+spobject[nr].image.width/2,spobject[nr].ypos+spobject[nr].image.height/2,100);
                                     add_explosion(-1,effect_beam2_01,10,1,0,temp->dat.xpos,temp->dat.ypos);
                                     temp->dat.xpos = spobject[spobject[nr].connect].xpos+6;
                                     temp->dat.ypos = spobject[spobject[nr].connect].ypos+6;
                                     temp->dat.user1 = spobject[nr].connect;
                                     add_explosion(-1,effect_beam2_01,10,1,0,temp->dat.xpos,temp->dat.ypos);
                                   }
                                 } // if collision
                           }
                           temp = next_bull;
                         }
                         break;
        case GRAVTRAP  :
                         // playship
                         for (nr2=0; nr2 < player_anz; nr2++)
                         {
                           apply_power(playship[nr2].xpos+5,playship[nr2].ypos+5,
                                       &playship[nr2].xspd,&playship[nr2].yspd,nr,0);
                         }


                         // weapons
                         temp = first_bullet;
                         while (temp!=NULL)
                         {
                           next_bull = temp->next;
                           switch (temp->dat.type2)
                           {
//                             case W_BOMB : case W_BIGBOMB :
                               default:
                               apply_power(temp->dat.xpos,temp->dat.ypos,
                                           &temp->dat.xspd,&temp->dat.yspd,nr,1);
                           }
                           temp = next_bull;
                         }
                         break;
        case GWALL_L   :
                         spobject[nr].image.data = obj_gwalll_1+spobject[nr].frame;

                         if (spobject[nr].maxframe != 0)
                         {
                           // playship
                           for (nr2=0; nr2 < player_anz; nr2++)
                           apply_lin_power(playship[nr2].xpos+5,playship[nr2].ypos+5,
                                       &playship[nr2].xspd,&playship[nr2].yspd,0.001,nr);
  
                           // weapons
                           temp = first_bullet;
                           while (temp!=NULL)
                           {
                             next_bull = temp->next;
                             switch (temp->dat.type2)
                             {
  //                             case W_BOMB : case W_BIGBOMB :
                                 case W_MINE1 : case W_MINE2 : case W_MINE3 :
                                   break;
                                 default:
                                 apply_lin_power(temp->dat.xpos,temp->dat.ypos,
                                             &temp->dat.xspd,&temp->dat.yspd,0.003,nr);
                             }
                             temp = next_bull;
                           }

                           // pixels
                           if (GRAV_4_PIXELS)
                           {
                             ptemp = first_pixel;
                             while (ptemp!=NULL)
                             {
                               next_pixel = ptemp->next;
                               apply_lin_power(ptemp->dat.x,ptemp->dat.y,
                                               &ptemp->dat.xspd,&ptemp->dat.yspd,0.001,nr);
                               ptemp = next_pixel;
                             }
                           }

                         }
                         break;
        case GWALL_R   :
                         spobject[nr].image.data = obj_gwallr_1+spobject[nr].frame;

                         if (spobject[nr].maxframe != 0)
                         {
                           // playship
                           for (nr2=0; nr2 < player_anz; nr2++)
                           apply_lin_power(playship[nr2].xpos+5,playship[nr2].ypos+5,
                                       &playship[nr2].xspd,&playship[nr2].yspd,0.001,nr);
  
                           // weapons
                           temp = first_bullet;
                           while (temp!=NULL)
                           {
                             next_bull = temp->next;
                             switch (temp->dat.type2)
                             {
                                 case W_MINE1 : case W_MINE2 : case W_MINE3 :
                                   break;
                                 default:
                                 apply_lin_power(temp->dat.xpos,temp->dat.ypos,
                                             &temp->dat.xspd,&temp->dat.yspd,0.003,nr);
                             }
                             temp = next_bull;
                           }

                           // pixels
                           if (GRAV_4_PIXELS)
                           {
                             ptemp = first_pixel;
                             while (ptemp!=NULL)
                             {
                               next_pixel = ptemp->next;
                               apply_lin_power(ptemp->dat.x,ptemp->dat.y,
                                               &ptemp->dat.xspd,&ptemp->dat.yspd,0.001,nr);
                               ptemp = next_pixel;
                             }
                           }

                         }
                         break;
        case GWALL_U   :
                         spobject[nr].image.data = obj_gwallu_1+spobject[nr].frame;

                         if (spobject[nr].maxframe != 0)
                         {
                           for (nr2=0; nr2 < player_anz; nr2++)
                           apply_lin_power(playship[nr2].xpos+5,playship[nr2].ypos+5,
                                       &playship[nr2].xspd,&playship[nr2].yspd,0.001,nr);

                           // weapons
                           temp = first_bullet;
                           while (temp!=NULL)
                           {
                             next_bull = temp->next;
                             switch (temp->dat.type2)
                             {
                                 case W_MINE1 : case W_MINE2 : case W_MINE3 :
                                   break;
                                 default:
                                 apply_lin_power(temp->dat.xpos,temp->dat.ypos,
                                             &temp->dat.xspd,&temp->dat.yspd,-0.003,nr);
                             }
                             temp = next_bull;
                           }

                           // pixels
                           if (GRAV_4_PIXELS)
                           {
                             ptemp = first_pixel;
                             while (ptemp!=NULL)
                             {
                               next_pixel = ptemp->next;
                               apply_lin_power(ptemp->dat.x,ptemp->dat.y,
                                               &ptemp->dat.xspd,&ptemp->dat.yspd,0.001,nr);
                               ptemp = next_pixel;
                             }
                           }

                         }
                         break;
        case GWALL_D   :
                         spobject[nr].image.data = obj_gwalld_1+spobject[nr].frame;

                         if (spobject[nr].maxframe != 0)
                         {
                           // playship
                           for (nr2=0; nr2 < player_anz; nr2++)
                           apply_lin_power(playship[nr2].xpos+5,playship[nr2].ypos+5,
                                       &playship[nr2].xspd,&playship[nr2].yspd,0.001,nr);
  
                           // weapons
                           temp = first_bullet;
                           while (temp!=NULL)
                           {
                             next_bull = temp->next;
                             switch (temp->dat.type2)
                             {
                                 case W_MINE1 : case W_MINE2 : case W_MINE3 :
                                   break;
                                 default:
                                 apply_lin_power(temp->dat.xpos,temp->dat.ypos,
                                             &temp->dat.xspd,&temp->dat.yspd,-0.003,nr);
                             }
                             temp = next_bull;
                           }

                           // pixels
                           if (GRAV_4_PIXELS)
                           {
                             ptemp = first_pixel;
                             while (ptemp!=NULL)
                             {
                               next_pixel = ptemp->next;
                               apply_lin_power(ptemp->dat.x,ptemp->dat.y,
                                               &ptemp->dat.xspd,&ptemp->dat.yspd,0.001,nr);
                               ptemp = next_pixel;
                             }
                           }

                         }
                         break;

        case BUMPU     : spobject[nr].image.data = obj_bumpu_1;

                         // player collision with bumper?
                         for (nr2=0; nr2 < player_anz; nr2++)
                         {
                           if (playship[nr2].yspd < 0 &&
                               collision(
                               playship[nr2].xpos, playship[nr2].ypos,
                               PLAYER_WIDTH, PLAYER_HEIGHT-2,
                               spobject[nr].xpos, spobject[nr].ypos,
                               spobject[nr].image.width, spobject[nr].image.height))
                           {
                             spobject[nr].frame = 1;
                             playship[nr2].yspd = -playship[nr2].yspd;
                             if (playship[nr2].yspd < 0.1) playship[nr2].yspd = LEVEL_GRAVITY*5;
                           }
                         }

                         // weapons
                         temp = first_bullet;
                         while (temp!=NULL)
                         {
                           next_bull = temp->next;
                           switch (temp->dat.type2)
                           {
                                 case W_MINE1 : case W_MINE2 : case W_MINE3 : case W_ROCKET :
                                   break;
                                 default :
                                 if (temp->dat.yspd > 0 &&
                                     collision(
                                     temp->dat.xpos, temp->dat.ypos,
                                     temp->dat.image.width, temp->dat.image.height,
                                     spobject[nr].xpos-2, spobject[nr].ypos,
                                     spobject[nr].image.width+4, spobject[nr].image.height+2))
                                 {
                                   spobject[nr].frame = 1;
                                   temp->dat.yspd = -temp->dat.yspd;
                                   temp->dat.user2++;
                                 }
                                 break;
                           }
                           temp = next_bull;
                         }


                         if (spobject[nr].frame > 0)
                         {
                           switch (spobject[nr].frame)
                           {
                             case 1 : spobject[nr].image.data = obj_bumpu_2;
                                      spobject[nr].frame++;
                                      play_sound_sample(snd_switch,1500,0,spobject[nr].xpos,spobject[nr].ypos,30);
                                      break;
                             case 2 : spobject[nr].image.data = obj_bumpu_2;
                                      spobject[nr].frame++;
                                      break;
                             case 3 : spobject[nr].image.data = obj_bumpu_3;
                                      spobject[nr].frame++;
                                      break;
                             case 4 : spobject[nr].image.data = obj_bumpu_3;
                                      spobject[nr].frame = 0;
                                      break;
                           }
                         }
                         break;

        case BUMPD     : spobject[nr].image.data = obj_bumpd_1;

                         // player collision with bumper?
                         for (nr2=0; nr2 < player_anz; nr2++)
                         {
                           if (playship[nr2].yspd > 0 &&
                               collision(
                               playship[nr2].xpos, playship[nr2].ypos,
                               PLAYER_WIDTH, PLAYER_HEIGHT,
                               spobject[nr].xpos-2, spobject[nr].ypos,
                               spobject[nr].image.width+4, spobject[nr].image.height))
                           {
                             spobject[nr].frame = 1;
                             playship[nr2].yspd = -playship[nr2].yspd;
                           }
                         }

                         // weapons
                         temp = first_bullet;
                         while (temp!=NULL)
                         {
                           next_bull = temp->next;
                           switch (temp->dat.type2)
                           {
                               case W_MINE1 : case W_MINE2 : case W_MINE3 : case W_ROCKET :
                                 break;
                               default :
                                 if (temp->dat.yspd < 0 &&
                                     collision(
                                     temp->dat.xpos, temp->dat.ypos,
                                     temp->dat.image.width, temp->dat.image.height+2,
                                     spobject[nr].xpos-2, spobject[nr].ypos,
                                     spobject[nr].image.width+4, spobject[nr].image.height))
                                 {
                                   spobject[nr].frame = 1;
                                   temp->dat.yspd = -temp->dat.yspd;
                                   temp->dat.user2++;
                                 }
                                 break;
                           }
                           temp = next_bull;
                         }


                         if (spobject[nr].frame > 0)
                         {
                           switch (spobject[nr].frame)
                           {
                             case 1 : spobject[nr].image.data = obj_bumpd_2;
                                      spobject[nr].frame++;
                                      play_sound_sample(snd_switch,1500,0,spobject[nr].xpos,spobject[nr].ypos,30);
                                      break;
                             case 2 : spobject[nr].image.data = obj_bumpd_2;
                                      spobject[nr].frame++;
                                      break;
                             case 3 : spobject[nr].image.data = obj_bumpd_3;
                                      spobject[nr].frame++;
                                      break;
                             case 4 : spobject[nr].image.data = obj_bumpd_3;
                                      spobject[nr].frame = 0;
                                      break;
                           }
                         }
                         break;

        case BUMPL     : spobject[nr].image.data = obj_bumpl_1;

                         // player collision with bumper?
                         for (nr2=0; nr2 < player_anz; nr2++)
                         {
                           if (playship[nr2].xspd < 0 &&
                               collision(
                               playship[nr2].xpos-2, playship[nr2].ypos,
                               PLAYER_WIDTH, PLAYER_HEIGHT,
                               spobject[nr].xpos, spobject[nr].ypos,
                               spobject[nr].image.width, spobject[nr].image.height))
                           {
                             spobject[nr].frame = 1;
                             playship[nr2].xspd = -playship[nr2].xspd;
                           }
                         }

                         // weapons
                         temp = first_bullet;
                         while (temp!=NULL)
                         {
                           next_bull = temp->next;
                           switch (temp->dat.type2)
                           {
                               case W_MINE1 : case W_MINE2 : case W_MINE3 : case W_ROCKET :
                                 break;
                               default :
                                 if (temp->dat.xspd < 0 &&
                                     collision(
                                     temp->dat.xpos-2, temp->dat.ypos,
                                     temp->dat.image.width, temp->dat.image.height,
                                     spobject[nr].xpos, spobject[nr].ypos-2,
                                     spobject[nr].image.width, spobject[nr].image.height+4))
                                 {
                                   spobject[nr].frame = 1;
                                   temp->dat.xspd = -temp->dat.xspd;
                                   temp->dat.user2++;
                                 }
                                 break;
                           }
                           temp = next_bull;
                         }


                         if (spobject[nr].frame > 0)
                         {
                           switch (spobject[nr].frame)
                           {
                             case 1 : spobject[nr].image.data = obj_bumpl_2;
                                      spobject[nr].frame++;
                                      play_sound_sample(snd_switch,1500,0,spobject[nr].xpos,spobject[nr].ypos,30);
                                      break;
                             case 2 : spobject[nr].image.data = obj_bumpl_2;
                                      spobject[nr].frame++;
                                      break;
                             case 3 : spobject[nr].image.data = obj_bumpl_3;
                                      spobject[nr].frame++;
                                      break;
                             case 4 : spobject[nr].image.data = obj_bumpl_3;
                                      spobject[nr].frame = 0;
                                      break;
                           }
                         }
                         break;

        case BUMPR     : spobject[nr].image.data = obj_bumpr_1;

                         // player collision with bumper?
                         for (nr2=0; nr2 < player_anz; nr2++)
                         {
                           if (playship[nr2].xspd > 0 &&
                               collision(
                               playship[nr2].xpos, playship[nr2].ypos,
                               PLAYER_WIDTH+2, PLAYER_HEIGHT,
                               spobject[nr].xpos, spobject[nr].ypos-2,
                               spobject[nr].image.width, spobject[nr].image.height+4))
                           {
                             spobject[nr].frame = 1;
                             playship[nr2].xspd = -playship[nr2].xspd;
                           }
                         }

                         // weapons
                         temp = first_bullet;
                         while (temp!=NULL)
                         {
                           next_bull = temp->next;
                           switch (temp->dat.type2)
                           {
                               case W_MINE1 : case W_MINE2 : case W_MINE3 : case W_ROCKET :
                                 break;
                               default :
                                 if (temp->dat.xspd > 0 &&
                                     collision(
                                     temp->dat.xpos, temp->dat.ypos,
                                     temp->dat.image.width+2, temp->dat.image.height,
                                     spobject[nr].xpos, spobject[nr].ypos-2,
                                     spobject[nr].image.width, spobject[nr].image.height+4))
                                 {
                                   spobject[nr].frame = 1;
                                   temp->dat.xspd = -temp->dat.xspd;
                                   temp->dat.user2++;
                                 }
                                 break;
                           }
                           temp = next_bull;
                         }


                         if (spobject[nr].frame > 0)
                         {
                           switch (spobject[nr].frame)
                           {
                             case 1 : spobject[nr].image.data = obj_bumpr_2;
                                      spobject[nr].frame++;
                                      play_sound_sample(snd_switch,1500,0,spobject[nr].xpos,spobject[nr].ypos,30);
                                      break;
                             case 2 : spobject[nr].image.data = obj_bumpr_2;
                                      spobject[nr].frame++;
                                      break;
                             case 3 : spobject[nr].image.data = obj_bumpr_3;
                                      spobject[nr].frame++;
                                      break;
                             case 4 : spobject[nr].image.data = obj_bumpr_3;
                                      spobject[nr].frame = 0;
                                      break;
                           }
                         }
                         break;

        case EXTRA1    : spobject[nr].image.data = obj_extra1_1+spobject[nr].frame;

                         spobject[nr].xspd += spobject[nr].xacc;
                         if (spobject[nr].xspd >= 0.2 || spobject[nr].xspd <= -0.2) spobject[nr].xacc = -spobject[nr].xacc;
                         spobject[nr].yspd += spobject[nr].yacc;
                         if (spobject[nr].yspd >= 0.2 || spobject[nr].yspd <= -0.2) spobject[nr].yacc = -spobject[nr].yacc;

                         spobject[nr].xpos += spobject[nr].xspd;
                         spobject[nr].ypos += spobject[nr].yspd;

                         // player hit extra?
                         for (nr2=0; nr2 < player_anz; nr2++)
                         {
                           if (!playship[nr2].dead &&
                               collision(
                               playship[nr2].xpos, playship[nr2].ypos,
                               PLAYER_WIDTH+2, PLAYER_HEIGHT,
                               spobject[nr].xpos, spobject[nr].ypos,
                               spobject[nr].image.width, spobject[nr].image.height))
                           {
                             collect_extra(nr, nr2);
                           }
                         }
                         break;

      } // switch frame

    } // if active
  } // for nr

}

void collect_extra(int objnum, int player)
{
  int r1,r2,col1,col2;
  char s1[50];

  spobject[objnum].active = FALSE;

  player_stat[player].extras++;

  // random number between 0 and 6
  r1=(rand() % 7);

  if (player==0) { col1 = col_white; col2 = col_yellow; }
  else { col1 = col_yellow; col2 = col_white; }

  switch (r1)
  {
    case 0 : // double blaster
             r2 = (rand() % 5)+5;
             r2 *= 10;
             playship[player].weapon.bullets[W_MG]+=r2;
             sprintf(s1, "%i %s", r2, cargo_message[MSG_CARGO_BULLETS].text2);
             play_sound_sample(snd_osd_on,3000,0,playship[player].xpos,playship[player].ypos,60);
             show_normal_osd_message(impact14h,impact10h,cargo_message[MSG_CARGO_BULLETS].text1,s1,2,col1,col2,0);
             update_panel_weapons();
             break;

    case 1 : // bomb
             r2 = (rand() % 5)+1;
             r2 *= 2;
             playship[player].weapon.bullets[W_BOMB]+=r2;
             sprintf(s1, "%i %s", r2, cargo_message[MSG_CARGO_BOMBS].text1);
             play_sound_sample(snd_osd_on,3000,0,playship[player].xpos,playship[player].ypos,60);
             show_normal_osd_message(impact14h,impact10h,s1,"",2,col1,col2,0);
             update_panel_weapons();
             break;

    case 2 : // bigbomb
             r2 = (rand() % 5)+1;
             playship[player].weapon.bullets[W_BIGBOMB]+=r2;
             sprintf(s1, "%i %s", r2, cargo_message[MSG_CARGO_BIGBOMBS].text1);
             play_sound_sample(snd_osd_on,3000,0,playship[player].xpos,playship[player].ypos,60);
             show_normal_osd_message(impact14h,impact10h,s1,"",2,col1,col2,0);
             update_panel_weapons();
             break;

    case 3 : // rockets
             r2 = (rand() % 5)+1;
             r2 *= 2;
             playship[player].weapon.bullets[W_ROCKET]+=r2;
             sprintf(s1, "%i %s", r2, cargo_message[MSG_CARGO_MISSILES].text1);
             play_sound_sample(snd_osd_on,3000,0,playship[player].xpos,playship[player].ypos,60);
             show_normal_osd_message(impact14h,impact10h,s1,"",2,col1,col2,0);
             update_panel_weapons();
             break;

    case 4 : // mines
             r2 = (rand() % 10)+1;
             r2 *= 3;
             playship[player].weapon.bullets[W_MINES]+=r2;
             playship[player].weapon.type2 = W_MINE1;
             sprintf(s1, "%i %s", r2, cargo_message[MSG_CARGO_MINES1].text1);
             play_sound_sample(snd_osd_on,3000,0,playship[player].xpos,playship[player].ypos,60);
             show_normal_osd_message(impact14h,impact10h,s1,"",2,col1,col2,0);
             update_panel_weapons();
             break;

    case 5 : // super-autofire
             playship[player].AUTOFIRE_RATE = 7;
             sprintf(s1, "%s", other_message[MSG_OTHER_MEGA_AUTOFIRE].text1);
             play_sound_sample(snd_osd_on,3000,0,playship[player].xpos,playship[player].ypos,60);
             show_normal_osd_message(impact14h,impact10h,s1,"",2,col1,col2,0);
             update_panel_weapons();
             break;

    case 6 : // super shield
             playship[player].SHIELD_RECHARGE = 0;
             sprintf(s1, "%s", other_message[MSG_OTHER_MEGA_SHIELD].text1);
             play_sound_sample(snd_osd_on,3000,0,playship[player].xpos,playship[player].ypos,60);
             show_normal_osd_message(impact14h,impact10h,s1,"",2,col1,col2,0);
             update_panel_weapons();
             break;

  }
}
