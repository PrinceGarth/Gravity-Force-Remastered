/*
 *    GRAVITY FORCE, gfpanel.c
 *
 *    -- Drawing/handling of the info panel
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfmisc.h"
#include "gfpanel.h"
#include "gflevel.h"
#include "gfmes.h"
#include "gf.h"
#include "gfrace.h"
#include "gfg2pdog.h"
#include "gfmap.h"
#include "gfstat.h"

#define  UBOX1_X1     4
#define  UBOX1_X2   106
#define  UBOX2_X1   111
#define  UBOX2_X2   192
#define  UBOX3_X1   197
#define  UBOX3_X2   635
#define  UBOX_Y1      3
#define  UBOX_Y2     14

extern int radar_zoom;

extern BITMAP *mapbit;

void draw_bsprite(BITMAP *b, BITMAP *s, int x, int y)
{
  blit(s,b,0,0,x,y,s->w,s->h);
}

void draw_upanel()
{
  blit(paneldat[panel_cpl_lpanel].dat,uscore_table,0,0,0,0,SCORE_WIDTH,USCORE_HEIGHT);
  update_upanel_time();
  update_upanel_bar();
  update_upanel_infobox();
}

void draw_panel()
{
  if (!show_debug_info)
  {
    if (game_mode == MP_2PDOGFIGHT)
    {
      blit(paneldat[panel_cpl_2panel].dat,score_table,0,0,0,0,SCORE_WIDTH,SCORE_HEIGHT);
      update_panel_lifes();
      update_panel_shield();
      update_panel_weapons();
    }
    else
    {
      blit(paneldat[panel_cpl_panel].dat,score_table,0,0,0,0,SCORE_WIDTH,SCORE_HEIGHT);
      update_panel_fuel();
      update_panel_lifes();
      update_panel_cargo();
      update_panel_shield();
      update_panel_weapons();
      update_panel_infobox();
      draw_upanel();
    }

  }
  else
  {
    draw_panel_text();
    draw_upanel();
  }
}

void update_panel_fuel()
{
  char f[4];

  mitoa(f,playship[0].fuel);

  // leading 0
  if (f[1] == '\0') { f[1] = f[0]; f[0] = '0'; f[2] = '\0'; }

  if (show_panel && !show_debug_info && game_mode != MP_2PDOGFIGHT)
  {
    rectfill(score_table,520,23,547,47,col_black);

    if (playship[0].fuel > 60)
      jhtextout(score_table,impact14h,f,524,27,col_green);
    else if (playship[0].fuel > 30)
      jhtextout(score_table,impact14h,f,524,27,col_yellow);
    else
      jhtextout(score_table,impact14h,f,524,27,col_orange);
  }
  else if (show_panel && show_debug_info)
  {
    draw_panel_text();
  }

}

void update_panel_lifes()
{
  char f[4];
  int nr,sx[2];

  sx[0] = 273;
  sx[1] = 593;

  if (show_panel && !show_debug_info)
  {
    if (game_mode == MP_2PDOGFIGHT)
    {
      for (nr=0; nr < player_anz; nr++)
      {
        sprintf(f,"%02d",player_stat[nr].frags);
        rectfill(score_table,sx[nr]+5,23,sx[nr]+25,47,col_black);
    
        jhtextout(score_table,impact14h,f,sx[nr]+5,27,col_yellow);
      }
    }
    else
    {
      sprintf(f,"%02d",playship[0].lifes);
      rectfill(score_table,590,23,615,47,col_black);
  
      if (playship[0].lifes > 2)
        jhtextout(score_table,impact14h,f,593,27,col_yellow);
      else
        jhtextout(score_table,impact14h,f,593,27,col_orange);
    }
  }
  else if (show_panel && show_debug_info)
  {
    draw_panel_text();
  }

}

void update_panel_cargo()
{
  char f[12];
  int dh,dw,h,nr,x,y;
  int sx = 320,sy = 25,ex = 354,ey = 52;

  if (show_panel && !show_debug_info && game_mode != MP_2PDOGFIGHT)
  {
    // display height
    dh = ey-sy;
    // display width
    dw = ex-sx;

    // current cargo height
    h  = dh / ((MAX_SHIP_WEIGHT*1000) / (playship[0].wght*1000));

    // graphical display
    rectfill(score_table,ex,ey,sx,sy,col_black);
    rectfill(score_table,sx,sy+dh-h,ex,ey,col_blue);

    // text display
    sprintf(f,"%.0f",(float)playship[0].wght*(float)1000.0);
    // leading 0
    if (f[1] == '\0') { f[1] = f[0]; f[0] = '0'; f[2] = '\0'; }
    h = (dw/2) - (text_length(impact14h,f) / 2) + sx;
    text_mode(-1);
    jhtextout(score_table,impact14h,f,h,27,col_yellow);
    // text display 2
    rectfill(score_table,358,25,400,45,_getpixel(score_table,358,25));

    for (x=0; x < 5; x++)
      for (y=0; y < 3; y++)
      {
        rect(score_table, 360+x*10, 25+y*10, 360+x*10+7, 25+y*10+7, col_white);
        rectfill(score_table, 360+x*10+1, 25+y*10, 360+x*10+7, 25+y*10+6, col_black);
      }

    for (nr=1; nr < base_anz && nr <= 15; nr++)
    {
      y = (nr-1) / 5;
      x = (nr-1) % 5;
      switch (base[nr].cargo)
      {
        case 0 :
                   break;
        case 1 :
                   rectfill(score_table, 360+x*10+2, 25+y*10+6, 360+x*10+6, 25+y*10+4, col_yellow);
                   break;
        case 2 :
                   rectfill(score_table, 360+x*10+2, 25+y*10+6, 360+x*10+6, 25+y*10+3, col_yellow);
                   break;
        case 3 :
                   rectfill(score_table, 360+x*10+2, 25+y*10+6, 360+x*10+6, 25+y*10+2, col_yellow);
                   break;
        case 4 :
                   rectfill(score_table, 360+x*10+2, 25+y*10+6, 360+x*10+6, 25+y*10+1, col_yellow);
                   break;
        case FUEL : case LIFE : case CSHIELD :
                   rectfill(score_table, 360+x*10+2, 25+y*10+6, 360+x*10+6, 25+y*10+2, col_lightblue);
                   break;
        case CBOMBS10 : case CBOMBS20 :
        case CBIGBOMB1 : case CBIGBOMB3 : case CMGBULLTS :
                   rectfill(score_table, 360+x*10+2, 25+y*10+6, 360+x*10+6, 25+y*10+2, col_green);
                   break;
        case CMAN1 :
                   rectfill(score_table, 360+x*10+2, 25+y*10+6, 360+x*10+6, 25+y*10+2, col_orange);
                   break;
        default :
                   rectfill(score_table, 360+x*10+2, 25+y*10+6, 360+x*10+6, 25+y*10+2, col_blue);
                   break;
      }
    } // for basenr
/*
    c = 0;
    for (nr=1; nr < base_anz; nr++)
    {
      if ((base[nr].cargo > 0) && (base[nr].cargo < 10))
      {
        scale = 2;
        scale += base[nr].cargo*1.5;
        h = ((BITMAP *)data[cargo1].dat)->h;
        stretch_sprite(score_table,data[cargo1].dat,360+c*8,35-h-(scale-h),scale,scale);
        c++;
      }
    }
*/
/*    aatextout(score_table,impact10h,"LEFT:",360,33,col_orange);
    strcpy(f,mitoa(global_active));
    aatextout(score_table,impact10h,f,392,33,col_green);
*/
  }
  else if (show_panel && show_debug_info)
  {
    draw_panel_text();
  }

}

void update_panel_shield()
{
  int nr;
  int sx[2],sy[2],ax,ay,xm,w,h;

  if (game_mode == MP_2PDOGFIGHT)
  {
    sx[0] = 193; sy[0] = 20;
    sx[1] = 513; sy[1] = 20;
    xm = 0;
    ax = 7; ay = 0;
    w = ((BITMAP*)paneldat[panel_shield_on].dat)->w - 11;
    h = ((BITMAP*)paneldat[panel_shield_on].dat)->h;
  }
  else
  {
    sx[0] = 427; sy[0] = 20;
    xm = ax = ay = 0; ax=7;
    w = ((BITMAP*)paneldat[panel_shield_on].dat)->w;
    h = ((BITMAP*)paneldat[panel_shield_on].dat)->h;
  }

  if (show_panel && !show_debug_info)
  {
   for (nr=0; nr < player_anz; nr++)
   {
    if (playship[nr].shield_active)
      switch (playship[nr].shield)
      {
        case 0  : blit(paneldat[panel_shield_on].dat,score_table,ax,ay,sx[nr]+xm,sy[nr],w,h);
                  break;
        case 1  : blit(paneldat[panel_shield_on1].dat,score_table,ax,ay,sx[nr]+xm,sy[nr],w,h);
                  break;
        case 2  : blit(paneldat[panel_shield_on2].dat,score_table,ax,ay,sx[nr]+xm,sy[nr],w,h);
                  break;
        case 3  : blit(paneldat[panel_shield_on3].dat,score_table,ax,ay,sx[nr]+xm,sy[nr],w,h);
                  break;
        case 4  : blit(paneldat[panel_shield_on4].dat,score_table,ax,ay,sx[nr]+xm,sy[nr],w,h);
                  break;
        case 5  : blit(paneldat[panel_shield_on5].dat,score_table,ax,ay,sx[nr]+xm,sy[nr],w,h);
                  break;
        default : blit(paneldat[panel_shield_on].dat,score_table,ax,ay,sx[nr]+xm,sy[nr],w,h);
                  break;
      } // switch
    else
      blit(paneldat[panel_shield_off].dat,score_table,ax,ay,sx[nr]+xm,sy[nr],w,h);

    if (playship[nr].pshield_active)
      switch (playship[nr].pshield)
      {
        case 1  : draw_sprite(score_table,paneldat[panel_pshield_1].dat,sx[nr]-7,sy[nr]+3);
                  break;
        case 2  : draw_sprite(score_table,paneldat[panel_pshield_2].dat,sx[nr]-7,sy[nr]+3);
                  break;
        case 3  : draw_sprite(score_table,paneldat[panel_pshield_3].dat,sx[nr]-7,sy[nr]+3);
                  break;
      }
   }
  } // if show_panel
  else if (show_panel && show_debug_info)
  {
    draw_panel_text();
  }

}

void update_panel_weapons_ammo()
{
  char f[10];
  int sx[2],sy[2],ex[2],ey[2];
  int x,nr;

  if (game_mode == MP_2PDOGFIGHT)
  {
    sx[0] = 142; sy[0] = 22; ex[0] = 182; ey[0] = 41;
    sx[1] = 462; sy[1] = 22; ex[1] = 502; ey[1] = 41;
  }
  else
  {
    sx[0] = 246; sy[0] = 22; ex[0] = 288; ey[0] = 41;
  }

  for (nr=0; nr < player_anz; nr++)
  if (show_panel && !show_debug_info)
  {
    rectfill(score_table,sx[nr],sy[nr],ex[nr],ey[nr],_getpixel(score_table,sx[0]-1,sy[0]-1));

    if (playship[nr].weapon.type != W_DEFAULT)
      mitoa(f,playship[nr].weapon.bullets[playship[nr].weapon.type]);
    else
      strcpy(f,"");

    x = ex[nr] - text_length(impact14h,f);

    jhtextout(score_table,impact14h,f,x,sy[nr],col_yellow);
  }
  else if (show_panel && show_debug_info)
  {
    draw_panel_text();
  }
}

void update_panel_weapons()
{
  int nr;
  int sx[2],sy[2],ax[2],ay[2];
  int d = ((BITMAP*)paneldat[panel_weapon1_1].dat)->w;

  if (game_mode == MP_2PDOGFIGHT)
  {
    sx[0] =   8; sy[0] = 47;
    ax[0] =   8; ay[0] = 23;
    sx[1] = 328; sy[1] = 47;
    ax[1] = 328; ay[1] = 23;
  }
  else
  {
    sx[0] = 114; sy[0] = 47;
    ax[0] = 114; ay[0] = 23;
  }

 // non/active / selected

 for (nr=0; nr < player_anz; nr++)
 if (show_panel && !show_debug_info)
 {
  if (playship[nr].weapon.type == W_DEFAULT)
  {
    draw_bsprite(score_table,paneldat[panel_weapon1_3].dat,sx[nr],sy[nr]);
    draw_bsprite(score_table,paneldat[panel_weapon1_pic].dat,ax[nr],ay[nr]);
  }
  else
    draw_bsprite(score_table,paneldat[panel_weapon1_2].dat,sx[nr],sy[nr]);

  if (playship[nr].weapon.type == W_MG)
  {
    draw_bsprite(score_table,paneldat[panel_weapon2_3].dat,sx[nr]+d+2,sy[nr]);
    draw_bsprite(score_table,paneldat[panel_weapon2_pic].dat,ax[nr],ay[nr]);
  }
  else if (playship[nr].weapon.bullets[W_MG])
    draw_bsprite(score_table,paneldat[panel_weapon2_2].dat,sx[nr]+d+2,sy[nr]);
  else
    draw_bsprite(score_table,paneldat[panel_weapon2_1].dat,sx[nr]+d+2,sy[nr]);

  if (playship[nr].weapon.type == W_BOMB)
  {
    draw_bsprite(score_table,paneldat[panel_weapon3_3].dat,sx[nr]+(d*2)+3,sy[nr]);
    draw_bsprite(score_table,paneldat[panel_weapon3_pic].dat,ax[nr],ay[nr]);
  }
  else if (playship[nr].weapon.bullets[W_BOMB])
    draw_bsprite(score_table,paneldat[panel_weapon3_2].dat,sx[nr]+(d*2)+3,sy[nr]);
  else
    draw_bsprite(score_table,paneldat[panel_weapon3_1].dat,sx[nr]+(d*2)+3,sy[nr]);

  if (playship[nr].weapon.type == W_BIGBOMB)
  {
    draw_bsprite(score_table,paneldat[panel_weapon4_3].dat,sx[nr]+(d*3)+4,sy[nr]);
    draw_bsprite(score_table,paneldat[panel_weapon4_pic].dat,ax[nr],ay[nr]);
  }
  else if (playship[nr].weapon.bullets[W_BIGBOMB])
    draw_bsprite(score_table,paneldat[panel_weapon4_2].dat,sx[nr]+(d*3)+4,sy[nr]);
  else
    draw_bsprite(score_table,paneldat[panel_weapon4_1].dat,sx[nr]+(d*3)+4,sy[nr]);

  if (playship[nr].weapon.type == W_ROCKET)
  {
    draw_bsprite(score_table,paneldat[panel_weapon5_3].dat,sx[nr]+(d*4)+5,sy[nr]);
    draw_bsprite(score_table,paneldat[panel_weapon5_pic].dat,ax[nr],ay[nr]);
  }
  else if (playship[nr].weapon.bullets[W_ROCKET])
    draw_bsprite(score_table,paneldat[panel_weapon5_2].dat,sx[nr]+(d*4)+5,sy[nr]);
  else
    draw_bsprite(score_table,paneldat[panel_weapon5_1].dat,sx[nr]+(d*4)+5,sy[nr]);

  if (playship[nr].weapon.type == W_MINES)
  {
    draw_bsprite(score_table,paneldat[panel_weapon6_3].dat,sx[nr]+(d*5)+6,sy[nr]);
    switch (playship[nr].weapon.type2)
    {
      case W_MINE1 :
                     draw_bsprite(score_table,paneldat[panel_weapon6_pic].dat,ax[nr],ay[nr]);
                     break;
      case W_MINE2 :
                     draw_bsprite(score_table,paneldat[panel_weapon7_pic].dat,ax[nr],ay[nr]);
                     break;
      case W_MINE3 :
                     draw_bsprite(score_table,paneldat[panel_weapon8_pic].dat,ax[nr],ay[nr]);
                     break;
    }
  }
  else if (playship[nr].weapon.bullets[W_MINES])
    draw_bsprite(score_table,paneldat[panel_weapon6_2].dat,sx[nr]+(d*5)+6,sy[nr]);
  else
    draw_bsprite(score_table,paneldat[panel_weapon6_1].dat,sx[nr]+(d*5)+6,sy[nr]);

 }
 else if (show_panel && show_debug_info)
 {
   draw_panel_text();
 }

 update_panel_weapons_ammo();
}

void update_panel_infobox()
{
  char f[20];
  int sx = 6,sy = 4,ex = 102,ey = 55,w=ex-sx,h=ey-sy,tw;
  int nr,plx,ply,enx,eny,pixelh,pixelw,divx,divy,abx,aby,abx2,aby2;
  int x1,y1,x2,y2;

  if (show_panel && !show_debug_info)
  {
    rectfill(score_table,sx,sy,ex,ey,0);

    if (game_mode == SP_CAMPAIGN || game_mode == SP_TRAINING || game_mode == SP_RACE)
    {
      /* Countdown hat hoechste Prioritaet. Dann Info, Radar, Versionsinfo.
      if (use_countdown)
      {
        strcpy(f,other_message[MSG_OTHER_REMTIME].text1);
        tw = text_length(impact10h,f);
        jhtextout(score_table,impact10h,f,middle(w,tw)+sx+2,sy+3,col_red);
  
        mitoa(f,countdown);
        if (countdown < 10) { strcpy(f,"0"); strcat(f,mitoa(tmp,countdown)); }
        jhtextout(score_table,impact14h,f,sx+37,sy+23,col_orange);
      }
      */
      if (show_infobox)
      {
        strcpy(f,show_infobox_message);
        tw = text_length(impact10h,show_infobox_message);
        jhtextout(score_table,impact10h,f,middle(w,tw)+sx+3,sy+3,col_red);
  
        strcpy(f,show_infobox_message2);
        tw = text_length(impact14h,show_infobox_message2);
        jhtextout(score_table,impact14h,f,middle(w,tw)+sx+3,sy+23,col_orange);
      }
      else if (SHOW_RADAR)
      {
        plx = playship[0].xpos; ply = playship[0].ypos;
  
        pixelw = (w*80)/(radar_zoom+1); pixelh = h * pixelw / w;
        divx = pixelw/w; divy = pixelh/h;
  
  //      if (mapbit) blit(mapbit,score_table,plx/divx-w/2,ply/divy,sx,sy,w,h);
  
        // Mittelkreuz (Spielerposition)
        _putpixel(score_table,sx+w/2,sy+h/2,col_yellow);
        _putpixel(score_table,sx+w/2-1,sy+h/2-1,col_yellow);
        _putpixel(score_table,sx+w/2+1,sy+h/2-1,col_yellow);
        _putpixel(score_table,sx+w/2-1,sy+h/2+1,col_yellow);
        _putpixel(score_table,sx+w/2+1,sy+h/2+1,col_yellow);
  
        // Bildschirmgroesse
        abx = plx-(map_x[1]+scroll_x[1]);
        aby = ply-(map_y[1]+scroll_y[1]);
        abx2 = plx-(map_x[1]+scroll_x[1]+PLAYSCREEN_WIDTH);
        aby2 = ply-(map_y[1]+scroll_y[1]+PLAYSCREEN_HEIGHT);
        x1 = sx+w/2-abx/divx; y1 = sy+h/2-aby/divy;
        x2 = sx+w/2-abx2/divx; y2 = sy+h/2-aby2/divy;
        if (x1 < sx) x1 = sx; if (y1 < sy) y1 = sy;
        if (x2 > ex) x2 = ex; if (y2 > ey) y2 = ey;
        rect(score_table,x1,y1,x2,y2,col_grey);
  
        // Gegnerpositionen
        for (nr=0; nr < enemy_anz; nr++)
        {
          if (enemy[nr].active)
          {
            enx = enemy[nr].xpos+enemy[nr].image.width/2; eny = enemy[nr].ypos+enemy[nr].image.height/2;
            if ( (enx > plx-pixelw/2) && (enx < plx+pixelw/2) &&
                 (eny > ply-pixelh/2) && (eny < ply+pixelh/2) )
            {
              abx = plx-enx; aby = ply-eny;
              x1 = sx+w/2-(abx/divx); y1 = sy+h/2-(aby/divy);
              if ( (x1 >= sx) && (x1 <= ex) && (y1 >= sy) && (y1 <= ey) )
                _putpixel(score_table,x1,y1,col_orange);
            }
          } // if active
        } // for nr
  
        // Bases/Cargo
        for (nr=0; nr < base_anz; nr++)
        {
          enx = base[nr].x1/*+(base[nr].x2-base[nr].x1)/2*/; eny = base[nr].y;
          if ( (enx > plx-pixelw/2) && (enx < plx+pixelw/2) &&
               (eny > ply-pixelh/2) && (eny < ply+pixelh/2) )
          {
            abx = plx-enx; aby = ply-eny;
            x1 = sx+w/2-(abx/divx); y1 = sy+h/2-(aby/divy);
            x2 = x1+2;
            if ( (x1 >= sx) && (x1 <= ex) && (y1 >= sy) && (y1 <= ey) &&
                 (x2 >= sx) && (x2 <= ex) )
            hline(score_table,x1,y1,x2,(nr==0) ? col_yellow : col_green);
  
            if (base[nr].cargo) _putpixel(score_table,x1+1,y1-1,col_white);
          }
        } // for nr
  
        // Zoomfaktor
        mitoa(f,6-radar_zoom); strcat(f,"x");
        jhtextout(score_table,impact10h,f,sx,sy,col_white);
      } // if show_radar
      else
      {
        strcpy(f,"GRAVITY FORCE");
        jhtextout(score_table,impact10h,f,sx+5,sy+10,col_yellow);
  
        strcpy(f,"v"); strcat(f,VERSION);
        jhtextout(score_table,impact10h,f,sx+37,sy+25,col_orange);
      }
    } // sp_campaign
/*
    else if (game_mode == SP_RACE)
    {
      sprintf(f,"%s %i/%i", other_message[MSG_OTHER_LAP].text1, current_lap, max_lap);

      tw = text_length(impact10h,f);
      jhtextout(score_table,impact10h,f,middle(w,tw)+8,sy+8,col_orange);

      sprintf(f,"%02d:%02d:%02d", time_m, time_s, time_hs);
      jhtextout(score_table,impact14h,f,sx+13,sy+25,col_yellow);
    }
*/
  } // if show_panel
  else if (show_panel && show_debug_info)
  {
    draw_panel_text();
  }

}

// debug info
void draw_panel_text()
{
//  extern int fpst;

  text_mode(0);

  textout(score_table,font,ftoa(tmpstr,explosion_anz),10,10,col_white);
  textout(score_table,font,ftoa(tmpstr,fpst),10,20,col_white);
  textout(score_table,font,ftoa(tmpstr,map_x[1]),100,10,col_green);
  textout(score_table,font,ftoa(tmpstr,map_y[1]),100,20,col_green);
  textout(score_table,font,ftoa(tmpstr,scroll_x[1]),200,10,col_yellow);
  textout(score_table,font,ftoa(tmpstr,scroll_y[1]),200,20,col_yellow);
  textout(score_table,font,ftoa(tmpstr,global_saved),300,10,col_white);
  textout(score_table,font,ftoa(tmpstr,upanel_bar_hit),300,20,col_white);
  /*textout(score_table,font,ftoa(tmpstr,temp->dat.),400,20,col_green);
  textout(score_table,font,ftoa(tmpstr,temp_var),400,30,col_green);
*/
  // add_2_list(&rl,10+scroll_x[1],50+scroll_y[1],400,20);
}

void init_upanel()
{
  strcpy(upanel_text1,"");
  strcpy(upanel_text2,"");
  strcpy(upanel_text3,"");
  strcpy(upanel_text4,"");
  upanel_bar_hit = upanel_bar_maxhit = 1;
}

void update_upanel_time()
{
  int w=text_length(impact10h,upanel_text1);
  int tw=UBOX1_X2-UBOX1_X1;

  rectfill(uscore_table,UBOX1_X1,UBOX_Y1,UBOX1_X2,UBOX_Y2,0);
  jhtextout(uscore_table,impact10h,upanel_text1,middle(tw,w),UBOX_Y1-2,col_yellow);
}

void update_upanel_bar()
{
  int bw = ((BITMAP *)(paneldat[panel_statbar].dat))->w;
  int bh = ((BITMAP *)(paneldat[panel_statbar].dat))->h;
  float f = (float)upanel_bar_hit / (float)upanel_bar_maxhit;

  rectfill(uscore_table,UBOX2_X1,UBOX_Y1,UBOX2_X2,UBOX_Y2,0);

  blit(paneldat[panel_statbar].dat,uscore_table, 0,0, UBOX2_X1+1,UBOX_Y1+4,
       bw - (int)((float)bw * (float)f), bh);
}

void update_upanel_infobox()
{
  int w=text_length(impact10h,upanel_text2);
  int w2=0;

  rectfill(uscore_table,UBOX3_X1,UBOX_Y1,UBOX3_X2,UBOX_Y2,0);
  jhtextout(uscore_table,impact10h,upanel_text2,UBOX3_X1+5,UBOX_Y1-2,col_white);

  if (strlen(upanel_text3) > 0)
  {
    w2 = text_length(impact10h,upanel_text3);
    jhtextout(uscore_table,impact10h,upanel_text3,UBOX3_X1+15+w,UBOX_Y1-2,col_yellow);
  }

  if (strlen(upanel_text4) > 0)
  {
    if (game_mode == SP_RACE)
      jhtextout(uscore_table,impact10h,upanel_text4,UBOX3_X1+135,UBOX_Y1-2,col_green);
    else
      jhtextout(uscore_table,impact10h,upanel_text4,UBOX3_X1+20+w+w2,UBOX_Y1-2,col_green);
  }

}

