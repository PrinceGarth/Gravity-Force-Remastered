/*
 *    GRAVITY FORCE, gfosd.c
 *
 *    -- Handling of OSD (On Screen Display)
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfosd.h"
#include "gfmes.h"
#include "gfsound.h"

#define OSD_HEIGHT 100
#define OSD_WIDTH  250
/*
#define SMALL_OSD_HEIGHT 20
#define SMALL_OSD_WIDTH  50
*/

void update_timer(void)
{
  osd_update_counter = TRUE;
}
END_OF_FUNCTION(update_timer);


void clear_osd(int r)
{
  clear(osd);
  if (r)
    rect(osd,0,0,osd->w-1,osd->h-1,col_white);
}

void init_osd()
{
  osd  = create_bitmap(OSD_WIDTH,OSD_HEIGHT);
  osdb = create_bitmap(OSD_WIDTH,OSD_HEIGHT);
  clear_osd(0);
}

void show_normal_osd_message(FONT *f1, FONT *f2, char *m1, char *m2, int le, int col1, int col2, int r)
{
  int l1,l2,x1,x2,y1,y2;

  l1 = text_length(f1,m1);
  l2 = text_length(f2,m2);
  x1 = osd->w/2 - l1/2;
  x2 = osd->w/2 - l2/2;
  y1 = 2; y2 = text_height(f1)+3;

  clear_osd(r);

  // Anzeigen, wenn Text nicht zu lang
  if (l1 < osd->w)
    jhtextout(osd,f1,m1,x1,y1,col1);

  if (m2)
    if ((l2 < osd->w) && (y2 < osd->h))
      jhtextout(osd,f2,m2,x2,y2,col2);

  message = TRUE;
  osd_counter = le;
}

// l1 = Speed, l2 = Time after eot
void show_typewriter_osd_message(FONT *f, char *m, int l1, int l2, int col, int y)
{
  int l,nr,nr2;

  strcpy(osd_buf.text,m);

  // Erste Zeile rausfischen
  nr = 0; nr2 = 0;
  while ( (osd_buf.text[nr] != '\0') && (osd_buf.text[nr] != '^') )
  {
    osd_buf.curline[nr2] = osd_buf.text[nr];
    nr++;
    nr2++;
  }
  osd_buf.curline[nr2] = '\0';

  l = text_length(f,osd_buf.curline);
  osd_buf.x = osd->w/2 - l/2;
  osd_buf.y = y;

  osd_buf.pos = osd_buf.gpos = 0;
  osd_buf.active = TRUE;
  osd_buf.l1 = l1; osd_buf.l2 = l2;
  osd_buf.col = col;
  osd_buf.f = f;

  LOCK_FUNCTION(update_timer);
  install_int_ex(update_timer,MSEC_TO_TIMER(l1));

  message = TRUE;
  osd_counter = 100;
}

void update_typewriter_osd_message()
{
  char bo[2],b[2];
  int l;
  int nr,nr2;

  if (osd_update_counter && osd_buf.active)
  {
    if ((osd_buf.pos > (signed)strlen(osd_buf.curline)) && (osd_buf.gpos < (signed)strlen(osd_buf.text)))
    {
      nr = osd_buf.gpos;
      nr2 = 0;
      while ( (osd_buf.text[nr] != '\0') && (osd_buf.text[nr] != '^') )
      {
        osd_buf.curline[nr2] = osd_buf.text[nr];
        nr++;
        nr2++;
      }
      osd_buf.curline[nr2] = '\0';
      osd_buf.pos = 0;
      l = text_length(osd_buf.f,osd_buf.curline);
      osd_buf.x = osd->w/2 - l/2;
      osd_buf.y += text_height(osd_buf.f)-1;
      b[0] = osd_buf.curline[osd_buf.pos]; b[1] = '\0';
//      play_sound_sample(snd_noweap,1000,0,playship[0].xpos,playship[0].ypos,100);
    }
    else
    {
      b[0] = osd_buf.curline[osd_buf.pos]; b[1] = '\0';
    }

    if (osd_buf.pos > 0)
    {
      bo[0] = osd_buf.curline[osd_buf.pos-1]; bo[1] = '\0';
      osd_buf.x += text_length(osd_buf.f,bo);
    }

    jhtextout(osd,osd_buf.f,b,osd_buf.x,osd_buf.y,osd_buf.col);
    play_sound_sample(snd_click,1000,0,playship[0].xpos,playship[0].ypos,100);

    if ((osd_buf.gpos < (signed)strlen(osd_buf.text)-1) && (message)) { osd_buf.pos++; osd_buf.gpos++; }
      else
      {
        osd_buf.active = FALSE;
        remove_int(update_timer);
        osd_counter = osd_buf.l2;
      }

    osd_update_counter = FALSE;
  }

}

/*

void clear_small_osd()
{
  clear(small_osd);
}

void init_small_osd(int x, int y, int w, int h)
{
  small_osd = create_bitmap(w,h);
  small_osdb = create_bitmap(w,h);
  small_osd_x = x;
  small_osd_y = y;
  small_osd_w = w;
  small_osd_h = h;
  clear_small_osd();
  small_osd_show = TRUE;
}

void kill_small_osd()
{
  destroy_bitmap(small_osd);
  destroy_bitmap(small_osdb);
  small_osd_show = FALSE;
}

void put_text_on_small_osd(char *t, FONT *f, int col, int line)
{
//  rectfill(small_osd,1,1,small_osd_w-3,small_osd_h-3,col_yellow);
  rectfill(small_osd,0,text_height(f)*line,small_osd_w,text_height(f)*(line+1),0);
  jhtextout(small_osd,f,t,0,text_height(f)*line,col);
}

*/
