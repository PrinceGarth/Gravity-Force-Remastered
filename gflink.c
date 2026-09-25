/*
 *    GRAVITY FORCE, gflink.c
 *
 *    -- handling of linked list stuff (bullets, pixels, craters, explosions)
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gflink.h"
#include "gfnetw.h"
#include "gfsound.h"
#include "gfdraw.h"
#include "gfpal.h"
#include "gfsprt.h"
#include "gfstat.h"
#include "gfinit.h"
#include "gfspibul.h"
#include "gf.h"

#define  P_BULLET_INFO    12

int crater_draw;
CNODE *crater_draw_c;
int crater_added;

void add_2_list(RLIST *l, int x, int y, int w, int h)
{
  if (l->count+1 < MAX_RECTANGLES)
  {
    l->rect[l->count].x = x;
    l->rect[l->count].y = y;
    l->rect[l->count].w = w;
    l->rect[l->count].h = h;
    l->count++;
  }
  else
  {
    error_exit(70,"Error in add_2_list!","",70,0);
  }
}

int add_bullet(int pnr, int typ, int itype, float px, float py, float xi, float yi, int mix, int miy, int max, int may)
{
  int added = FALSE;
  BNODE *TempBull;
//  netdata_type NetData;

  TempBull = (BNODE *)malloc(sizeof(BNODE));

  if (TempBull!=NULL)
  {
    if (first_bullet==NULL)
    {
      last_bullet = first_bullet = TempBull;
      first_bullet->next = first_bullet->prev = NULL;
    }
    else
    {
      last_bullet->next = TempBull;
      TempBull->prev = last_bullet;
      last_bullet = TempBull;
      TempBull->next = NULL;
    }

    TempBull->dat.nr = pnr;
    TempBull->dat.active = TRUE;
    TempBull->dat.xpos = px;
    TempBull->dat.ypos = py;
    TempBull->dat.xspd = xi; TempBull->dat.yspd = yi;
    TempBull->dat.dxspd = 0; TempBull->dat.dyspd = 0;
    TempBull->dat.xacc = 0; TempBull->dat.yacc = 0;
    TempBull->dat.frame = 0;
    TempBull->dat.startframe = 0;
    TempBull->dat.maxframe = 0;
    TempBull->dat.framet = 0;
    TempBull->dat.type2 = itype;
    TempBull->dat.ttl = 0;
    TempBull->dat.drot = 0;
    TempBull->dat.user1 = -1;
    TempBull->dat.user2 = 0;
    TempBull->dat.active = 0;

    TempBull->dat.ttl = 60*3;

    switch (itype)
    {
      case W_DEFAULT   : switch (typ)
                         {
                           case B_OWN   : TempBull->dat.image = bullet_sprite[W_DEFAULT];
                                          break;
                           case B_ENEMY : TempBull->dat.image = bullet_sprite[W_DEFAULT2];
                                          break;
                           case B_NEUTRAL:TempBull->dat.image = bullet_sprite[W_DEFAULT3];
                                          break;
                         }
                         TempBull->dat.ttl = 60*3;
                         break;
      case W_BOMB      : switch (typ)
                         {
                           case B_OWN   : TempBull->dat.image = bullet_sprite[W_BOMB];
                                          break;
                           case B_ENEMY : TempBull->dat.image = bullet_sprite[W_BOMB2];
                                          break;
                           case B_NEUTRAL:TempBull->dat.image = bullet_sprite[W_BOMB3];
                                          break;
                         }
                         TempBull->dat.ttl = 60*7;
                         break;
      case W_BIGBOMB   : switch (typ)
                         {
                           case B_OWN   : TempBull->dat.image = bullet_sprite[W_BIGBOMB];
                                          break;
                           case B_ENEMY : TempBull->dat.image = bullet_sprite[W_BIGBOMB2];
                                          break;
                           case B_NEUTRAL:TempBull->dat.image = bullet_sprite[W_BIGBOMB3];
                                          break;
                         }
                         TempBull->dat.ttl = 60*7;
                         break;
      case W_ROCKET    : switch (typ)
                         {
                           case B_OWN   : TempBull->dat.image = bullet_sprite[W_ROCKET];
                                          TempBull->dat.user1 = -1;//SearchTarget(TempBull->dat.xpos,TempBull->dat.ypos);
                                          TempBull->dat.user2 = 1; // -> Zeit nach Abschuss
                                          break;
                           case B_ENEMY : TempBull->dat.image = bullet_sprite[W_ROCKET2];
                                          TempBull->dat.user1 = SearchTarget(TempBull->dat.xpos,TempBull->dat.ypos,pnr);
                                          TempBull->dat.user2 = 1; // -> Zeit nach Abschuss
                                          break;
                           case B_NEUTRAL:TempBull->dat.image = bullet_sprite[W_ROCKET3];
                                          TempBull->dat.user1 = SearchTarget(TempBull->dat.xpos,TempBull->dat.ypos,pnr);
                                          TempBull->dat.user2 = 1; // -> Zeit nach Abschuss
                                          break;
                         }
                         TempBull->dat.ttl = 60*5;
                         break;
      case W_MINE1 :     switch (typ)
                         {
                           case B_OWN   : TempBull->dat.image.data = data[weapon_mine1_1].dat;
                                          TempBull->dat.image.width = ((BITMAP *)data[weapon_mine1_1].dat)->w;
                                          TempBull->dat.image.height = ((BITMAP *)data[weapon_mine1_1].dat)->h;
                                          TempBull->dat.image.boundxa = 0;
                                          TempBull->dat.image.boundxe = 5;
                                          TempBull->dat.image.boundya = 0;
                                          TempBull->dat.image.boundye = 5;
                                          break;
                           case B_ENEMY : TempBull->dat.image.data = data[weapon_mine1e_1].dat;
                                          TempBull->dat.image.width = ((BITMAP *)data[weapon_mine1e_1].dat)->w;
                                          TempBull->dat.image.height = ((BITMAP *)data[weapon_mine1e_1].dat)->h;
                                          TempBull->dat.image.boundxa = 0;
                                          TempBull->dat.image.boundxe = 5;
                                          TempBull->dat.image.boundya = 0;
                                          TempBull->dat.image.boundye = 5;
                                          break;
                         }
                         TempBull->dat.maxframe = 1;
                         TempBull->dat.maxframet = 30;
                         TempBull->dat.dxspd = 0.01;
                         TempBull->dat.dyspd = 0.01;
                         TempBull->dat.user1 = 50; // Radius (user1/2 = activation radius)
                         TempBull->dat.active = 15; // Strength
                         TempBull->dat.ttl = 60*20; // Lebenszeit in Frames
                         break;
      case W_MINE2 :     switch (typ)
                         {
                           case B_OWN   : TempBull->dat.image.data = data[weapon_mine2_1].dat;
                                          TempBull->dat.image.width = ((BITMAP *)data[weapon_mine2_1].dat)->w;
                                          TempBull->dat.image.height = ((BITMAP *)data[weapon_mine2_1].dat)->h;
                                          TempBull->dat.image.boundxa = 0;
                                          TempBull->dat.image.boundxe = 5;
                                          TempBull->dat.image.boundya = 0;
                                          TempBull->dat.image.boundye = 5;
                                          break;
                           case B_ENEMY : TempBull->dat.image.data = data[weapon_mine2e_1].dat;
                                          TempBull->dat.image.width = ((BITMAP *)data[weapon_mine2e_1].dat)->w;
                                          TempBull->dat.image.height = ((BITMAP *)data[weapon_mine2e_1].dat)->h;
                                          TempBull->dat.image.boundxa = 0;
                                          TempBull->dat.image.boundxe = 5;
                                          TempBull->dat.image.boundya = 0;
                                          TempBull->dat.image.boundye = 5;
                                          break;
                         }
                         TempBull->dat.maxframe = 1;
                         TempBull->dat.maxframet = 30;
                         TempBull->dat.dxspd = 0.1;
                         TempBull->dat.dyspd = 0.1;
                         TempBull->dat.user1 = 80;
                         TempBull->dat.active = 30;
                         TempBull->dat.ttl = 60*30; // Lebenszeit in Frames
                         break;
      case W_MINE3 :
                         switch (typ)
                         {
                           case B_OWN   : TempBull->dat.image.data = data[weapon_mine3_1].dat;
                                          TempBull->dat.image.width = ((BITMAP *)data[weapon_mine3_1].dat)->w;
                                          TempBull->dat.image.height = ((BITMAP *)data[weapon_mine3_1].dat)->h;
                                          TempBull->dat.image.boundxa = 0;
                                          TempBull->dat.image.boundxe = 7;
                                          TempBull->dat.image.boundya = 0;
                                          TempBull->dat.image.boundye = 7;
                                          break;
                           case B_ENEMY : TempBull->dat.image.data = data[weapon_mine3e_1].dat;
                                          TempBull->dat.image.width = ((BITMAP *)data[weapon_mine3e_1].dat)->w;
                                          TempBull->dat.image.height = ((BITMAP *)data[weapon_mine3e_1].dat)->h;
                                          TempBull->dat.image.boundxa = 0;
                                          TempBull->dat.image.boundxe = 7;
                                          TempBull->dat.image.boundya = 0;
                                          TempBull->dat.image.boundye = 7;
                                          break;
                         }
                         TempBull->dat.maxframe = 1;
                         TempBull->dat.maxframet = 30;
                         TempBull->dat.dxspd = 0.02;
                         TempBull->dat.dyspd = 0.02;
                         TempBull->dat.user1 = 80;
                         TempBull->dat.active = 50;
                         TempBull->dat.ttl = 60*40; // Lebenszeit in Frames
                         break;
      default          : TempBull->dat.image = bullet_sprite[W_DEFAULT];
                         break;
    }

    TempBull->dat.min_x = mix; TempBull->dat.max_x = max;
    TempBull->dat.min_y = miy; TempBull->dat.max_y = may;
    TempBull->dat.type = typ;

    switch (TempBull->dat.type)
    {
      default : ;
    } // switch type

    added = TRUE;
    bullet_anz++;
  } // if !null

  if (!added)
    error_exit(40,"Error:","Couldn't add bullet!",40,0);

  return added;
}

int erase_bullet(BNODE *temp)
{
  int erased = FALSE;
  BNODE *next_bull, *prev_bull;

  if (temp!=NULL)
  {
    prev_bull = temp->prev;
    next_bull = temp->next;

    if (prev_bull==NULL) first_bullet = next_bull;
    else prev_bull->next = next_bull;

    if (next_bull==NULL) last_bullet = prev_bull;
    else next_bull->prev = prev_bull;

    free(temp);

    temp = NULL;
    erased = TRUE;

    bullet_anz--;
  } // if !null

  if (!erased)
    error_exit(41,"Error","Couldn't erase bullet!",41,0);
  return erased;
}

void add_bomb_bullets(BNODE *temp)
{
  add_bullet(temp->dat.nr,temp->dat.type,W_DEFDOUBLE,temp->dat.xpos,temp->dat.ypos,0,-3,
             temp->dat.xpos-200,temp->dat.ypos-200,
             temp->dat.xpos+200,temp->dat.ypos+200 );
  add_bullet(temp->dat.nr,temp->dat.type,W_DEFDOUBLE,temp->dat.xpos,temp->dat.ypos,3,0,
             temp->dat.xpos-200,temp->dat.ypos-200,
             temp->dat.xpos+200,temp->dat.ypos+200 );
  add_bullet(temp->dat.nr,temp->dat.type,W_DEFDOUBLE,temp->dat.xpos,temp->dat.ypos,0,3,
             temp->dat.xpos-200,temp->dat.ypos-200,
             temp->dat.xpos+200,temp->dat.ypos+200 );
  add_bullet(temp->dat.nr,temp->dat.type,W_DEFDOUBLE,temp->dat.xpos,temp->dat.ypos,-3,0,
             temp->dat.xpos-200,temp->dat.ypos-200,
             temp->dat.xpos+200,temp->dat.ypos+200 );
  add_bullet(temp->dat.nr,temp->dat.type,W_DEFDOUBLE,temp->dat.xpos,temp->dat.ypos,1.5,-1.5,
             temp->dat.xpos-200,temp->dat.ypos-200,
             temp->dat.xpos+200,temp->dat.ypos+200 );
  add_bullet(temp->dat.nr,temp->dat.type,W_DEFDOUBLE,temp->dat.xpos,temp->dat.ypos,1.5,1.5,
             temp->dat.xpos-200,temp->dat.ypos-200,
             temp->dat.xpos+200,temp->dat.ypos+200 );
  add_bullet(temp->dat.nr,temp->dat.type,W_DEFDOUBLE,temp->dat.xpos,temp->dat.ypos,-1.5,1.5,
             temp->dat.xpos-200,temp->dat.ypos-200,
             temp->dat.xpos+200,temp->dat.ypos+200 );
  add_bullet(temp->dat.nr,temp->dat.type,W_DEFDOUBLE,temp->dat.xpos,temp->dat.ypos,-1.5,-1.5,
             temp->dat.xpos-200,temp->dat.ypos-200,
             temp->dat.xpos+200,temp->dat.ypos+200 );

  player_stat[temp->dat.nr].anz_shot+=8;
}

int add_pixels(int anz, int x, int y, float xab, float yab, int spd, int tbl[], int maxtbl, int dspd, int max_xab, int max_yab, int img)
{
  int nr,a;
  int added = FALSE;
  float r;
  PNODE *TempPixel;

  anz *= (float)PIXEL_DIVISOR/100.0;

  for (nr=0; nr < anz; nr++)
  {
    added = FALSE;

    TempPixel = (PNODE *)malloc(sizeof(PNODE));

    if (TempPixel!=NULL)
    {
      if (first_pixel==NULL)
      {
        last_pixel = first_pixel = TempPixel;
        first_pixel->next = first_pixel->prev = NULL;
//        TempPixel->next = NULL; TempPixel->prev = NULL;
      }
      else
      {
        last_pixel->next = TempPixel;
        TempPixel->prev = last_pixel;
        last_pixel = TempPixel;
        TempPixel->next = NULL;
      }

      TempPixel->dat.t = 0;
      TempPixel->dat.mt = 2000;
      TempPixel->dat.x = x;
      TempPixel->dat.y = y;
      TempPixel->dat.startx = x;
      TempPixel->dat.starty = y;
      TempPixel->dat.max_xab = max_xab;
      TempPixel->dat.max_yab = max_yab;

      a = (maxtbl/2) + (rand() % (maxtbl/2));
      TempPixel->dat.c = tbl[a];
      TempPixel->dat.tc = a;
      TempPixel->dat.tbl = tbl;
      TempPixel->dat.dspd = dspd;
      TempPixel->dat.user = 0;

      if (img)
        TempPixel->dat.image = data[img+(rand() % dspd)].dat;
      else
        TempPixel->dat.image = 0;

      if (spd==0)
      {
        TempPixel->dat.xspd = 0;
        TempPixel->dat.yspd = 0;
        TempPixel->dat.xspd2 = xab;
        TempPixel->dat.yspd2 = yab;
      }
      else
      {
        r = (1.0-(rand() % 2) * 3.0) / ((rand() % 9)+1);
//        if ( ((xab > 0) && (r < 0)) || ((xab < 0) && (r > 0)) ) r = -r;
        TempPixel->dat.xspd = 0.01+(xab+(6.0/((rand() % spd)+1))*r);
//        if (TempPixel->dat.xi == 0) TempPixel->dat.xi = 0.1/r;
        r = (1.0-(rand() % 2) * 3.0) / ((rand() % 9)+1);
//        if ( ((yab > 0) && (r < 0)) || ((yab < 0) && (r > 0)) ) r = -r;
        TempPixel->dat.yspd = 0.01+(yab+(6.0/((rand() % spd)+1))*r);
//        TempPixel->dat.active = TRUE;
        TempPixel->dat.xspd2 = 0;
        TempPixel->dat.yspd2 = 0;
      }

      pixel_anz++;
      added = TRUE;
    } // if !null

    if (!added)
      error_exit(49,"Error","Couldn't add pixel!",49,0);
  } // for nr

  return added;
}

int remove_pixel(PNODE *temp)
{
  int erased = FALSE;
  PNODE *next_pixel, *prev_pixel;

  if (temp!=NULL)
  {
    prev_pixel = temp->prev;
    next_pixel = temp->next;

    if (prev_pixel==NULL) first_pixel = next_pixel;
    else prev_pixel->next = next_pixel;

    if (next_pixel==NULL) last_pixel = prev_pixel;
    else next_pixel->prev = prev_pixel;

    free(temp);

    temp = NULL;
    erased = TRUE;

    pixel_anz--;
  } // if !null

  if (!erased)
    error_exit(38,"Error","Couldn't erase pixel!",38,0);
  return erased;
}

int add_explosion(int pnr, int type, int maxframe, int maxframet, int starttime, int x, int y)
{
  int added = FALSE;
  ENODE *TempExplosion;
  BITMAP *bit;

 if (on_draw_field(x,y,((BITMAP*)data[type].dat)->w,((BITMAP*)data[type].dat)->h))
 {
  TempExplosion = (ENODE *)malloc(sizeof(ENODE));

  if (TempExplosion!=NULL)
  {
    if (first_explosion==NULL)
    {
      last_explosion = first_explosion = TempExplosion;
      first_explosion->next = first_explosion->prev = NULL;
    }
    else
    {
      last_explosion->next = TempExplosion;
      TempExplosion->prev = last_explosion;
      last_explosion = TempExplosion;
      TempExplosion->next = NULL;
    }

    TempExplosion->dat.nr = pnr;
    TempExplosion->dat.x = x;
    TempExplosion->dat.y = y;
    TempExplosion->dat.type = type;
    TempExplosion->dat.curframe = 1;
    TempExplosion->dat.curframet = 0;
    TempExplosion->dat.maxframe = maxframe;
    TempExplosion->dat.maxframet = maxframet;
    TempExplosion->dat.starttime = starttime;

    bit = data[TempExplosion->dat.type].dat;
    TempExplosion->dat.tempbit = create_bitmap(bit->w,bit->h);
    blit(vscreen,TempExplosion->dat.tempbit,x-map_x[1],y-map_y[1],0,0,bit->w,bit->h);

    added = TRUE;
    explosion_anz++;
  } // for nr

  switch (type)
  {
    case expl_big2_01 :
                   fade_out_active = TRUE; fade_pos = 0; fade_opos = 0;
                   fade_type = 3; fade_count = 0; fade_count_to = 30; fade_speed = 1;
                   break;
    case expl_big_1   :
                   fade_out_active = TRUE; fade_pos = 0; fade_opos = 0;
                   fade_type = 3; fade_count = 0; fade_count_to = 10; fade_speed = 2;
                   break;
    case expl_sm1_01   :
                   fade_out_active = TRUE; fade_pos = 0; fade_opos = 0;
                   fade_type = 3; fade_count = 0; fade_count_to = 6; fade_speed = 1;
                   break;
  } // switch

  if (!added)
    error_exit(51,"Error","Couldn't add explosion!",51,0);;
 } return added;
}

int remove_explosion(ENODE *temp)
{
  int erased = FALSE;
  ENODE *next_explosion, *prev_explosion;

  if (temp!=NULL)
  {
    prev_explosion = temp->prev;
    next_explosion = temp->next;

    if (prev_explosion==NULL) first_explosion = next_explosion;
    else prev_explosion->next = next_explosion;

    if (next_explosion==NULL) last_explosion = prev_explosion;
    else next_explosion->prev = prev_explosion;

//    destroy_bitmap(temp->dat.tempbit);

    free(temp);

    temp = NULL;
    erased = TRUE;

    explosion_anz--;
  } // if !null

  if (!erased)
    error_exit(52,"Error","Couldn't erase explosion!",52,0);
  return erased;
}

void do_explosions()
{
  int c;
  struct ENODE *temp, *next_explosion;
  RLE_SPRITE *bit;

  temp = first_explosion;

  while (temp!=NULL)
  {
    next_explosion = temp->next;

    if (temp->dat.starttime <= 0)
    {
      // Wenn Explosion vorbei, diese entfernen
      if (temp->dat.curframe >= temp->dat.maxframe)
      {
        remove_explosion(temp);
        crater_draw = TRUE;
        crater_added = FALSE;
      }
      // ansonsten Daten updaten
      else
      {
        temp->dat.curframet++;
        if (temp->dat.curframet >= temp->dat.maxframet)
        {
          temp->dat.curframet = 0;
          temp->dat.curframe++;
        }

        // Sounds spielen
        if (temp->dat.curframe==1)
        {
          switch (temp->dat.type)
          {
            case effect_shield_1  : play_sound_sample(snd_shield,1000,0,temp->dat.x,temp->dat.y,100);
                                    break;
            case expl_sm3_01 :      //play_sound_sample(snd_exp_mid_4,1000,0,temp->dat.x,temp->dat.y,20);
                                    break;
            default               : c = rand() % 4;
                                    switch (c)
                                    {
                                      case 0 :
                                        play_sound_sample(snd_exp_mid_1,1000,0,temp->dat.x,temp->dat.y,100);
                                        break;
                                      case 1 :
                                        play_sound_sample(snd_exp_mid_2,1000,0,temp->dat.x,temp->dat.y,100);
                                        break;
                                      case 2 :
                                        play_sound_sample(snd_exp_mid_3,1000,0,temp->dat.x,temp->dat.y,100);
                                        break;
                                      case 3 :
                                        play_sound_sample(snd_exp_mid_4,1000,0,temp->dat.x,temp->dat.y,100);
                                        break;
                                      default :
                                        play_sound_sample(snd_exp_sm_4,1000,0,temp->dat.x,temp->dat.y,100);
                                    }
          } // switch

        } // if frame=1

        // Expl./Effekt-spezifische Sachen
        switch (temp->dat.type)
        {
          case expl_big2_01     :
                                   if (!fade_out_active)
                                   {
                                     fade_out_active = TRUE; fade_pos = 0; fade_opos = 0;
                                     fade_type = 2; fade_count = 0; fade_count_to = 100; fade_speed = 1;
                                   }
                                   break;
          case expl_big_1       :
                                   if (!fade_out_active)
                                   {
                                     fade_out_active = TRUE; fade_pos = 0; fade_opos = 0;
                                     fade_type = 2; fade_count = 0; fade_count_to = 50; fade_speed = 1;
                                   }
                                   break;
          case expl_sm1_01       :
                                   if (!fade_out_active)
                                   {
                                     fade_out_active = TRUE; fade_pos = 0; fade_opos = 0;
                                     fade_type = 2; fade_count = 0; fade_count_to = 30; fade_speed = 1;
                                   }
                                   break;
          case effect_shield_1  :
                                   temp->dat.x = playship[temp->dat.nr].xpos-6;
                                   temp->dat.y = playship[temp->dat.nr].ypos-5;
//                                   blit(vscreen,temp->dat.tempbit,temp->dat.x-map_x[1],temp->dat.y-map_y[1],0,0,
//                                        ((BITMAP*)temp->dat.tempbit)->w,
//                                        ((BITMAP*)temp->dat.tempbit)->h);
                                   break;
        }

        // Explosion zeichnen
        bit = data[temp->dat.type + temp->dat.curframe - 1].dat;

        draw_rle_sprite(vscreen,bit,temp->dat.x-map_x[1],temp->dat.y-map_y[1]);
      }
    } // if starttime == 0
    else
    {
      temp->dat.starttime--;
    }

    temp = next_explosion;
  } // while

}

int add_crater(int x, int y, int size)
{
  int added = FALSE;
  CNODE *temp_crater;

  if (crater_anz+1 > MAX_CRATERS) remove_small_crater();

  temp_crater = (CNODE *)malloc(sizeof(CNODE));

  if (temp_crater!=NULL)
  {
    if (first_crater==NULL)
    {
      last_crater = first_crater = temp_crater;
      first_crater->next = first_crater->prev = NULL;
    }
    else
    {
      last_crater->next = temp_crater;
      temp_crater->prev = last_crater;
      last_crater = temp_crater;
      temp_crater->next = NULL;
    }

    temp_crater->dat.x = x;
    temp_crater->dat.y = y;
    temp_crater->dat.size = size;

    crater_added = TRUE;
    added = TRUE;
    crater_anz++;
  } // if !null

  if (!added)
    error_exit(45,"Error","Couldn't create crater!",45,0);

  draw_crater(vscreen, x, y, size);

  crater_draw_c = temp_crater;

  return added;
}

int remove_crater(CNODE *temp)
{
  int erased = FALSE;
  CNODE *next_crater, *prev_crater;

  if (temp!=NULL)
  {
    prev_crater = temp->prev;
    next_crater = temp->next;

    if (prev_crater==NULL) first_crater = next_crater;
    else prev_crater->next = next_crater;

    if (next_crater==NULL) last_crater = prev_crater;
    else next_crater->prev = prev_crater;

    free(temp);

    temp = NULL;
    erased = TRUE;

    crater_anz--;
  } // if !null

  if (!erased)
    error_exit(41,"Error","Couldn't erase crater!",41,0);
  return erased;
}

int remove_small_crater()
{
  int remove_size = 10;
  int removed = FALSE;
  struct CNODE *temp, *next_crater;

  temp = first_crater;

  while (temp!=NULL && !removed)
  {
    next_crater = temp->next;

    if (temp->dat.size <= remove_size)
    {
      remove_crater(temp);
      removed = TRUE;
    }

    temp = next_crater;
  } // while

  if (!removed) remove_crater(first_crater);

  return removed;
}

