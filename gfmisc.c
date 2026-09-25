/*
 *    GRAVITY FORCE, gfmisc.c
 *
 *    -- miscellaneous functions
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfmisc.h"

char *ftoa(char *tmp, float n)
{
//  char tmp[200];

  sprintf(tmp,"%.4f",n);
  return tmp;
}

char *mitoa(char *tmp, int n)
{
//  char tmp[200] = "";

  sprintf(tmp,"%d",n);
//  return itoa(n,tmp,10);
  return tmp;
}

void make_screenshot()
{
  BITMAP *bmp;
  int dump = 0;
  char text[20];
  PALETTE pal;

  do
  {
    dump++;
    sprintf(text, "gf%06d.tga", dump);
  } while (exists(text));

  sprintf(text, "gf%06d.tga", dump);

  get_palette(pal);
  bmp = create_sub_bitmap(screen, 0, 0, SCREEN_W, SCREEN_H);
  save_tga(text,bmp,pal);
  destroy_bitmap(bmp);
}

int count_cargo()
{
  int nr;
  int res=0;

  for (nr=0; nr < base_anz; nr++)
    if (base[nr].cargo < 10 && base[nr].cargo > 0) res += base[nr].cargo;

  return res;
}

