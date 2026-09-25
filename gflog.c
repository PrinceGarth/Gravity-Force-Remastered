/*
 *    GRAVITY FORCE, gflog.c
 *
 *    -- never really used this :-)
 *
 *
 *
 *    see source.txt for further information
 */

#include "stdio.h"
#include "gfinit.h"
#include "gflog.h"

void init_log()
{
  if (!logfile)
  {
    logfile = fopen("gf.log","a");
  }
}

void close_log()
{
  if (logfile)
  {
    fclose(logfile);
  }
}

void write_log_str(int nr, char *text, int nr2)
{
  init_log();
  fprintf(logfile, "%i  %s  %i\n", nr, text, nr2);
  close_log();
}


