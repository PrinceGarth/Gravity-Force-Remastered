/*
 *    GRAVITY FORCE, gfstat.c
 *
 *    -- statistic screens
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfstat.h"
#include "gfmes.h"
#include "gf.h"
#include "gfrace.h"
#include "gfmisc.h"
#include "gfsound.h"
#include "gfcontrl.h"
#include <ctype.h>

extern int start_lifes;

int strtrimr(char *pszStr)
{
      int   i, j;                               /* Local counters */
      j = i = strlen(pszStr) - 1; /* Calculate the length of the string */
      while (isspace(pszStr[i]) && (i >= 0))
            pszStr[ i-- ] = '\0';               /*- Replace blank with '\0' */

      return(j - i);                            /* Return no of replacements */
}


int strtriml(char *pszStr)
{
      int   i = 0, j;                                 /* Local counters */
      j = strlen(pszStr) - 1; /* Calculate the length of the string */
      while (isspace(pszStr[i]) && (i <= j))
            i++;                          /*- Count no of leading blanks */
      if (0 < i)                          /* IF leading blanks are found */
            strcpy(pszStr, &pszStr[i]);   /*- Shift string to the left */

      return(i);                          /* Return no of replacements */
}

int strtrim(char *pszStr)
{
      int   iBlank;
      iBlank  = strtrimr(pszStr);               /* Remove trailing blanks */
      iBlank += strtriml(pszStr);               /* Remove leading blanks */
      return(iBlank);
}

void sort(HISCORE_ENTRY v[], int n)
{
  int i,k,minIndex;
  HISCORE_ENTRY memo;

  for (i=0; i <= n-1; i++)
  {
    minIndex = i;
    for (k=i; k <= n; k++)
    {
      if (v[k].score > v[minIndex].score) minIndex = k;
    }
    memo = v[i];
    v[i] = v[minIndex];
    v[minIndex] = memo;
  }
}

void sort_race(RACE_HISCORE_ENTRY v[], int n)
{
  int i,k,minIndex;
  long int sc1,sc2;
  RACE_HISCORE_ENTRY memo;

  for (i=0; i <= n-1; i++)
  {
    minIndex = i;
    for (k=i; k <= n; k++)
    {
      sc1 = v[k].m*60*100 + v[k].s*100 + v[k].hs;
      sc2 = v[minIndex].m*60*100 + v[minIndex].s*100 + v[minIndex].hs;
      if (sc1 == 0) sc1 = 99999999;
      if (sc2 == 0) sc2 = 99999999;

      if (sc1 < sc2) minIndex = k;
    }
    memo = v[i];
    v[i] = v[minIndex];
    v[minIndex] = memo;
  }
}


int add_2_hiscore(char *name, int score, int mode)
{
  int m,s,hs,nr;

  // normal (mission)
  if (mode == 0)
  {
    // add him on position 10 (9)
    strcpy(hiscore_file.hiscore[current_level].nr[9].name,name);
    hiscore_file.hiscore[current_level].nr[9].score = score;

    // sort list
    sort(hiscore_file.hiscore[current_level].nr,9);

    // search entry and return position
    for (nr=0; nr < 10; nr++)
      if (hiscore_file.hiscore[current_level].nr[nr].score == score)
        return nr+1;
  }
  // race
  else if (mode == 1)
  {
    strcpy(hiscore_file.race_hiscore[current_sprace_level].nr[9].name,name);
    m = score / (60*100);
    s = (score - (m*60*100)) / 100;
    hs = (score - (m*60*100) - (s*100));
    hiscore_file.race_hiscore[current_sprace_level].nr[9].m = m;
    hiscore_file.race_hiscore[current_sprace_level].nr[9].s = s;
    hiscore_file.race_hiscore[current_sprace_level].nr[9].hs = hs;

    sort_race(hiscore_file.race_hiscore[current_sprace_level].nr,9);

    // search entry and return position
    for (nr=0; nr < 10; nr++)
      if (hiscore_file.race_hiscore[current_sprace_level].nr[nr].m == m &&
          hiscore_file.race_hiscore[current_sprace_level].nr[nr].s == s &&
          hiscore_file.race_hiscore[current_sprace_level].nr[nr].hs == hs)
        return nr+1;
  }

  return 0;
}

void input_name(char *dtext, char *name)
{
  int c,tc;
  char text[30] = "";
  int maxtext = 25;
  int l = text_length(impact14h,dtext);
  BITMAP *tmp = create_bitmap(SCREEN_W,SCREEN_H);

  blit(vscreen,tmp,0,0,0,0,SCREEN_W,SCREEN_H);

  strtrim(config_file.config.last_hiscore_name);
  strcpy(text,config_file.config.last_hiscore_name);
  wait_keys_up();

  // back box
  rectfill(vscreen,SCREEN_W/2 - l/2 - 20, 150, SCREEN_W/2 + l/2 + 20, 240, col_blue);

  // title text
  jhtextout(vscreen,impact14h,dtext,
            SCREEN_W/2-l/2,
            160,
            col_yellow);

  // input box
//  rectfill(vscreen,SCREEN_W/2 - l/2 - 5, 200, SCREEN_W/2 + l/2 + 5, 200+text_height(impact14h), col_black);

  while (!(key[KEY_ENTER] || key[KEY_ESC]))
  {
    // input text
    jhtextout(vscreen,impact14h,text,
              middle(l,text_length(impact14h,text))+SCREEN_W/2 - l/2,
              202,
              col_white);

    blit(vscreen,screen,0,0,0,0,SCREEN_W,SCREEN_H);

    // restore input box
    rectfill(vscreen,SCREEN_W/2 - l/2 - 15, 200, SCREEN_W/2 + l/2 + 15, 200+text_height(impact14h), col_blue);

    if (keypressed())
    {
      c = readkey();
      tc = strlen(text);

      if (c>>8 == KEY_BACKSPACE)
      {
        text[tc-1] = '\0';
      }
      else if (tc <= maxtext && !key[KEY_ESC])
      {
        text[tc] = c;
        text[tc+1] = '\0';
      } // else

    } // if keypressed

  } // !key_enter

  strtrim(text);
  strcpy(name,text);

  strcpy(config_file.config.last_hiscore_name,text);

  // restore screen
  blit(tmp,vscreen,0,0,0,0,SCREEN_W,SCREEN_H);
  destroy_bitmap(tmp);
}

char *CalcScoreString(char *tmp, char *f, long int score)
{
  /* Stuff deleted, see docs */
  
  return "n/a";
}

char *CalcRaceScoreString(char *tmp, char *f, int m, int s, int hs)
{
  /* Stuff deleted, see docs */
  
  return "n/a";
}

void generate_statistics()
{
  char tmp[200],tmp2[100],f[100];
  int nr;
  int temp,zusatz;
  float ftemp;
  extern float level_time_factor;
  extern int killable_enemies;

  set_palette_range(mainpal,0,255,TRUE);
  clear(screen);

  text_mode(-1);
  wait_keys_up();

  while (!keypressed())
  {
    clear(vscreen);

    // Title
    strcpy(f,stat_message[MSG_STAT_STATISTICS].text1);
    jhtextout(vscreen,impact14h,f,
              middle(SCREEN_W,text_length(impact14h,f))-1,
              20-1,
              col_green);
    jhtextout(vscreen,impact14h,f,
              middle(SCREEN_W,text_length(impact14h,f)),
              20,
              col_yellow);

    rect(vscreen,10,55,630,400,col_white);
    rect(vscreen,11,56,629,399,col_yellow);

    rectfill(vscreen,20,360,620,390,col_blue);

    // Scoring Items
    for (nr=0; nr < 8; nr++)
    {
      strcpy(f,stat_message[MSG_STAT_STATUS+nr].text1);
      jhtextout(vscreen,impact14h,f,
                25,
                80 + nr*32,
                col_green);
    } // for nr
    strcpy(f,stat_message[MSG_STAT_SCORE].text1);
    jhtextout(vscreen,impact14h,f,
              25,
              77 + 9*32,
              col_yellow);


    // Scores
    player_stat[0].score = 0;

    // Status
    if (player_stat[0].mission_status == END_GOOD)
    {
      strcpy(f,mission_message[MSG_MISSION_COMPLETE].text1);
      jhtextout(vscreen,arial14h,f,
                300,
                80 + 0*32,
                col_white);
      strcpy(f,"2000");
      jhtextout(vscreen,impact14h,f,
                610-text_length(impact14h,f),
                80 + 0*32,
                col_yellow);
      player_stat[0].score+=2000;
    }
    else
    {
      strcpy(f,mission_message[MSG_MISSION_FAILED].text1);
      jhtextout(vscreen,arial14h,f,
                300,
                80 + 0*32,
                col_orange);
      strcpy(f,"0");
      jhtextout(vscreen,impact14h,f,
                610-text_length(impact14h,f),
                80 + 0*32,
                col_yellow);
      player_stat[0].score+=0;
    }

    // Lifes
    zusatz = 0;
    sprintf(f,"%d x 400",player_stat[0].lifes);
    if (player_stat[0].lifes == start_lifes) // Bonus!
    {
      strcat (f," (BONUS! 700)");
      zusatz = 700;
    }
    jhtextout(vscreen,arial14h,f,
              300,
              80 + 1*32,
              col_white);
    sprintf(f,"%d",player_stat[0].lifes*400 + zusatz);
    jhtextout(vscreen,impact14h,f,
              610-text_length(impact14h,f),
              80 + 1*32,
              col_yellow);
    player_stat[0].score += player_stat[0].lifes*400 + zusatz;

    // Fuel
    sprintf(f,"%d x 4",player_stat[0].fuel);
    jhtextout(vscreen,arial14h,f,
              300,
              80 + 2*32,
              col_white);
    sprintf(f,"%d",player_stat[0].fuel*4);
    jhtextout(vscreen,impact14h,f,
              610-text_length(impact14h,f),
              80 + 2*32,
              col_yellow);
    player_stat[0].score += player_stat[0].fuel*4;

    // Enemies killed
    zusatz = 0;
    sprintf(f,"%d x 50",player_stat[0].enemies_killed);
    if (player_stat[0].enemies_killed >= killable_enemies) // Bonus!
    {
      strcat (f," (BONUS! 1000)");
      zusatz = 1000;
    }
    jhtextout(vscreen,arial14h,f,
              300,
              80 + 3*32,
              col_white);
    sprintf(f,"%d",player_stat[0].enemies_killed*50 + zusatz);
    jhtextout(vscreen,impact14h,f,
              610-text_length(impact14h,f),
              80 + 3*32,
              col_yellow);
    player_stat[0].score += player_stat[0].enemies_killed*50 + zusatz;

    // Enemies killed quality
    sprintf(f,"%d x 8",player_stat[0].enemies_killed_quality);
    jhtextout(vscreen,arial14h,f,
              300,
              80 + 4*32,
              col_white);
    sprintf(f,"%d",player_stat[0].enemies_killed_quality*8);
    jhtextout(vscreen,impact14h,f,
              610-text_length(impact14h,f),
              80 + 4*32,
              col_yellow);
    player_stat[0].score += player_stat[0].enemies_killed_quality*8;

    // Cargo saved
    temp = (player_stat[0].cargo_saved*100)/player_stat[0].cargo_active;
    if (temp > 0) sprintf(f,"%d%%",temp); else strcpy(f,"0%");
    jhtextout(vscreen,arial14h,f,
              300,
              80 + 5*32,
              col_white);
    temp = (((player_stat[0].cargo_saved*100)/player_stat[0].cargo_active)*20);
    if (temp > 0) sprintf(f,"%d",temp); else strcpy(f,"0");
    jhtextout(vscreen,impact14h,f,
              610-text_length(impact14h,f),
              80 + 5*32,
              col_yellow);
    if (temp > 0) player_stat[0].score += temp;

    // Hit Percentage
    if (player_stat[0].anz_shot > 0)
    {
      ftemp = (float)(((float)player_stat[0].anz_hit / (float)player_stat[0].anz_shot)*100.0);
    }
    else ftemp = 0;
    if (ftemp > 0) sprintf(f,"%.1f%%",ftemp); else strcpy(f,"0%");
    jhtextout(vscreen,arial14h,f,
              300,
              80 + 6*32,
              col_white);

    if (player_stat[0].anz_shot > 0)
    {
      ftemp = (float)( ((float)player_stat[0].anz_hit / (float)player_stat[0].anz_shot)*100.0*20.0);
    }
    else ftemp = 0;
    if (ftemp > 0) sprintf(f,"%.0f",ftemp); else strcpy(f,"0");
    jhtextout(vscreen,impact14h,f,
              610-text_length(impact14h,f),
              80 + 6*32,
              col_yellow);
    if (ftemp > 0) player_stat[0].score += ftemp;

    // -Time
    sprintf(f,"%ds x %.1f",player_stat[0].time,(float)player_stat[0].time*level_time_factor);
    jhtextout(vscreen,arial14h,f,
              300,
              80 + 7*32,
              col_white);
    sprintf(f,"-%d",(int)((float)player_stat[0].time*(float)(player_stat[0].time*level_time_factor)));
    jhtextout(vscreen,impact14h,f,
              610-text_length(impact14h,f),
              80 + 7*32,
              col_orange);
    player_stat[0].score -= (int)((float)player_stat[0].time*(float)(player_stat[0].time*level_time_factor));

    // Overall
    sprintf(f,"%ld",player_stat[0].score);
    jhtextout(vscreen,impact14h,f,
              610-text_length(impact14h,f),
              77 + 9*32,
              col_white);

    // Hiscore!
    if (player_stat[0].score > hiscore_file.hiscore[current_level].nr[9].score)
    {
      blit(vscreen,screen,0,0,0,0,SCREEN_W,SCREEN_H);
      input_name(stat_message[MSG_STAT_ENTER_NAME].text1,f);

      sprintf(tmp,"%s (%s %i)",
                  stat_message[MSG_STAT_INHISCORE].text1,
                  stat_message[MSG_STAT_PLACE].text1,
                  add_2_hiscore(f,player_stat[0].score,0));

      jhtextout(vscreen,arial14h,tmp,
                middle(SCREEN_W,text_length(arial14h,tmp)),
                77+9*32,
                col_white);

      // Calculate Scorestring
      CalcScoreString(tmp,f,player_stat[0].score);
      strcpy(tmp2,stat_message[MSG_STAT_HISCORECODE].text1);
      strcat(tmp2," ");
      strcat(tmp2,tmp);
      jhtextout(vscreen,arial14h,tmp2,
                middle(SCREEN_W,text_length(arial14h,tmp2)),
                72+11*32,
                col_white);
    }



    // Press key...
    strcpy(tmp,stat_message[MSG_STAT_PRESS_KEY].text1);
    jhtextout(vscreen,impact10h,tmp,
              middle(SCREEN_W,text_length(impact10h,tmp)),
              460,
              col_white);

    blit(vscreen,screen,0,0,0,0,SCREEN_W,SCREEN_H);


    wait_keys_up(); while (!key[KEY_ENTER]) rest(1);
  }

  wait_keys_up();

  clear(screen); clear(vscreen);

//  free_all();
//  exit(0);
}

void generate_race_statistics()
{
  char tmp[200],tmp2[200],f[100];
  int nr;
  long int sc1,sc2;

  set_palette_range(mainpal,0,255,TRUE);
  clear(screen);

  text_mode(-1);
  wait_keys_up();

  while (!keypressed())
  {
    clear(vscreen);

    // Title
    strcpy(f,stat_message[MSG_STAT_STATISTICS].text1);
    jhtextout(vscreen,impact14h,f,
              middle(SCREEN_W,text_length(impact14h,f))-1,
              20-1,
              col_green);
    jhtextout(vscreen,impact14h,f,
              middle(SCREEN_W,text_length(impact14h,f)),
              20,
              col_yellow);

    rect(vscreen,10,55,630,400,col_white);
    rect(vscreen,11,56,629,399,col_yellow);

    rectfill(vscreen,20,360,620,390,col_blue);

    // Scoring Items
    for (nr=1; nr <= max_lap; nr++)
    {
      sprintf(f,"%s %d",stat_message[MSG_STAT_LAP].text1,nr);
      jhtextout(vscreen,impact14h,f,
                25,
                80 + nr*32,
                col_green);

      sprintf(f,"%02d:%02d:%02d", lap_stat[nr].m, lap_stat[nr].s, lap_stat[nr].hs);
      jhtextout(vscreen,arial14h,f,
                540,
                80 + nr*32,
                col_white);

    } // for nr
    strcpy(f,stat_message[MSG_STAT_OVERALL_TIME].text1);
    jhtextout(vscreen,impact14h,f,
              25,
              77 + 9*32,
              col_yellow);

    sprintf(f,"%02d:%02d:%02d",time_m,time_s,time_hs);
    jhtextout(vscreen,impact14h,f,
              540,
              77 + 9*32,
              col_white);

    // Hiscore!
    sc1 = time_m*60*100 + time_s*100 + time_hs;
    sc2 = hiscore_file.race_hiscore[current_sprace_level].nr[9].m*60*100 +
          hiscore_file.race_hiscore[current_sprace_level].nr[9].s*100 +
          hiscore_file.race_hiscore[current_sprace_level].nr[9].hs;

    if (sc2 == 0) sc2 = 9999999;

    if (sc1 < sc2)
    {
      blit(vscreen,screen,0,0,0,0,SCREEN_W,SCREEN_H);
      input_name(stat_message[MSG_STAT_ENTER_NAME].text1,f);

      sprintf(tmp,"%s (%s %i)",
                  stat_message[MSG_STAT_INHISCORE].text1,
                  stat_message[MSG_STAT_PLACE].text1,
                  add_2_hiscore(f,sc1,1));

      jhtextout(vscreen,arial14h,tmp,
                middle(SCREEN_W,text_length(arial14h,tmp)),
                77+9*32,
                col_white);

      blit(vscreen,screen,0,0,0,0,SCREEN_W,SCREEN_H);

      // Calculate Scorestring
      CalcRaceScoreString(tmp,f,time_m,time_s,time_hs);
      strcpy(tmp2,stat_message[MSG_STAT_HISCORECODE].text1);
      strcat(tmp2," ");
      strcat(tmp2,tmp);
      jhtextout(vscreen,arial14h,tmp2,
                middle(SCREEN_W,text_length(arial14h,tmp2)),
                72+11*32,
                col_white);

    }



    // Press key...
    strcpy(tmp,stat_message[MSG_STAT_PRESS_KEY].text1);
    jhtextout(vscreen,impact10h,tmp,
              middle(SCREEN_W,text_length(impact10h,tmp)),
              460,
              col_white);

    blit(vscreen,screen,0,0,0,0,SCREEN_W,SCREEN_H);


    wait_keys_up(); while (!key[KEY_ENTER]) rest(1);
  }

  wait_keys_up();

  clear(screen); clear(vscreen);

//  free_all();
//  exit(0);
}


void generate_2pdog_statistics()
{
  char f[100];
  int nr;
  int temp,temp2;
  int sc1=0, sc2=0;

  set_palette_range(mainpal,0,255,TRUE);
  clear(screen);

  text_mode(-1);

  stop_sound_sample(snd_thrusters);
  wait_keys_up();

  while (!keypressed())
  {
    clear(vscreen);

    // Title
    strcpy(f,stat_message[MSG_STAT_STATISTICS].text1);
    jhtextout(vscreen,impact14h,f,
              middle(SCREEN_W,text_length(impact14h,f))-1,
              20-1,
              col_green);
    jhtextout(vscreen,impact14h,f,
              middle(SCREEN_W,text_length(impact14h,f)),
              20,
              col_yellow);

    rect(vscreen,10,55,630,400,col_white);
    rect(vscreen,11,56,629,399,col_yellow);

    rectfill(vscreen,20,358,620,388,col_blue);

    // Player 1, Player 2
    sprintf(f,"%s 1",stat_message[MSG_STAT_PLAYER].text1);
    jhtextout(vscreen,impact14h,f,
              300, 80, col_green);

    sprintf(f,"%s 2",stat_message[MSG_STAT_PLAYER].text1);
    jhtextout(vscreen,impact14h,f,
              500, 80, col_green);

    // Frags
    sprintf(f,"%s (x50)",stat_message[MSG_STAT_FRAGS].text1);
    jhtextout(vscreen,impact14h,f,
              25, 100+1*32, col_green);

    sprintf(f,"%i",player_stat[0].frags);
    jhtextout(vscreen,impact14h,f,
              300, 100+1*32, col_yellow);
    sprintf(f,"%i",player_stat[1].frags);
    jhtextout(vscreen,impact14h,f,
              500, 100+1*32, col_yellow);

    sc1 += player_stat[0].frags*50;
    sc2 += player_stat[1].frags*50;

    // Suicides
    sprintf(f,"%s (- x50)",stat_message[MSG_STAT_SUICIDES].text1);
    jhtextout(vscreen,impact14h,f,
              25, 100+2*32, col_green);

    sprintf(f,"%i",player_stat[0].suicides);
    jhtextout(vscreen,impact14h,f,
              300, 100+2*32, col_orange);
    sprintf(f,"%i",player_stat[1].suicides);
    jhtextout(vscreen,impact14h,f,
              500, 100+2*32, col_orange);

    sc1 -= player_stat[0].suicides*50;
    sc2 -= player_stat[1].suicides*50;


    // Hit ratio
    sprintf(f,"%s (x10)",stat_message[MSG_STAT_HITPERCENT].text1);
    jhtextout(vscreen,impact14h,f,
              25, 100+3*32, col_green);

    if (player_stat[0].anz_shot > 0)
      sprintf(f,"%.1f%%",(float)(((float)player_stat[0].anz_hit / (float)player_stat[0].anz_shot)*100.0));
    else
      sprintf(f,"0%%");
    jhtextout(vscreen,impact14h,f,
              300, 100+3*32, col_yellow);

    if (player_stat[1].anz_shot > 0)
      sprintf(f,"%.1f%%",(float)(((float)player_stat[1].anz_hit / (float)player_stat[1].anz_shot)*100.0));
    else
      sprintf(f,"0%%");
    jhtextout(vscreen,impact14h,f,
              500, 100+3*32, col_yellow);

    if (player_stat[0].anz_shot == 0) player_stat[0].anz_shot = 1;
    if (player_stat[1].anz_shot == 0) player_stat[1].anz_shot = 1;
    sc1 += 10*((float)(((float)player_stat[0].anz_hit / (float)player_stat[0].anz_shot)*100.0));
    sc2 += 10*((float)(((float)player_stat[1].anz_hit / (float)player_stat[1].anz_shot)*100.0));

    // Extras taken
    sprintf(f,"%s (x50)",stat_message[MSG_STAT_EXTRASTAKEN].text1);
    jhtextout(vscreen,impact14h,f,
              25, 100+4*32, col_green);

    sprintf(f,"%i",player_stat[0].extras);
    jhtextout(vscreen,impact14h,f,
              300, 100+4*32, col_yellow);
    sprintf(f,"%i",player_stat[1].extras);
    jhtextout(vscreen,impact14h,f,
              500, 100+4*32, col_yellow);

    sc1 += player_stat[0].extras*50;
    sc2 += player_stat[1].extras*50;

    /* Time played
    strcpy(f,stat_message[MSG_STAT_TIMEPLAYED].text1);
    jhtextout(vscreen,arial14h,f,
              25, 100+5*32+15, col_white);

    sprintf(f,"%02d:%02d",player_stat[0].time / 60,player_stat[0].time % 60);
    jhtextout(vscreen,arial14h,f,
              300, 100+5*32+15, col_yellow);
    */

    // Score
    strcpy(f,stat_message[MSG_STAT_SCORE].text1);
    jhtextout(vscreen,impact14h,f,
              25, 80+9*32-5, col_white);

    sprintf(f,"%i",sc1);
    jhtextout(vscreen,impact14h,f,
              300, 80+9*32-5, col_white);
    sprintf(f,"%i",sc2);
    jhtextout(vscreen,impact14h,f,
              500, 80+9*32-5, col_white);


    // Press key...
    strcpy(f,stat_message[MSG_STAT_PRESS_KEY].text1);
    jhtextout(vscreen,impact10h,f,
              middle(SCREEN_W,text_length(impact10h,f)),
              460,
              col_white);

    blit(vscreen,screen,0,0,0,0,SCREEN_W,SCREEN_H);


    wait_keys_up(); while (!key[KEY_ENTER]) rest(1);
  }

  wait_keys_up();

  clear(screen); clear(vscreen);

//  free_all();
//  exit(0);
}

