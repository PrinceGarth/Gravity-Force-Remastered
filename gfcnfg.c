/*
 *    GRAVITY FORCE, gfcnfg.c
 *
 *    -- reads and writes the configuration file as well as the
 *       highscore file (routines not included in source release)
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfcnfg.h"
#include "gfmes.h"
#include "gfstat.h"
#include "gfinit.h"
#include "gfmisc.h"
#include "gfcontrl.h"
#include "gf.h"
#include "gfg2pdog.h"

void write_level_data()
{
  FILE *datf;
  int nr;

  strcpy(tmp_dir,map_dir);
  datf = fopen((char*)strcat(tmp_dir,cfg_name),"w");

  // bissel konvertieren
  for (nr=0; nr < enemy_anz; nr++)
    if (enemy[nr].start_waypnt == 0) enemy[nr].start_waypnt = 1;

  // Header schreiben
  putc(base_anz,datf);
  putc(global_active,datf);
  putc(node_anz,datf);
  putc(object_anz,datf);
  putc(spobject_anz,datf);
  putc(enemy_anz,datf);

  fwrite(&base,sizeof(coord_type),base_anz,datf);
  fwrite(&path_node,sizeof(NODE),node_anz,datf);
  fwrite(&object,sizeof(SPRITE_TYPE),object_anz,datf);
  fwrite(&spobject,sizeof(SPRITE_TYPE),spobject_anz,datf);
  fwrite(&enemy,sizeof(SPRITE_TYPE),enemy_anz,datf);

  fclose(datf);
}

void read_level_data()
{
  PACKFILE *datf;
  extern int killable_enemies;

  packfile_password("tanja");
//  strcpy(tmp_dir,map_dir);
  sprintf(tmp_dir,"%s%s#%s",dat_dir,"gfmaps.dat",cfg_name);
//  datf = pack_fopen((char*)strcat(tmp_dir,cfg_name),"rp");
  datf = pack_fopen(tmp_dir,"r");
  if (!datf) { error_exit(31,error_message[MSG_ERROR_CFGFILE].text1,cfg_name,31,0); }

  packfile_password(NULL);

  // Header lesen
  base_anz = pack_getc(datf);
  global_active = pack_getc(datf);
  node_anz = pack_getc(datf);
  object_anz = pack_getc(datf);
  spobject_anz = pack_getc(datf);
  enemy_anz = pack_getc(datf);

  if (killable_enemies == 0) killable_enemies = enemy_anz;

  pack_fread(&base,sizeof(coord_type)*base_anz,datf);
  pack_fread(&path_node,sizeof(NODE)*node_anz,datf);
  pack_fread(&object,sizeof(SPRITE_TYPE)*object_anz,datf);
  pack_fread(&spobject,sizeof(SPRITE_TYPE)*spobject_anz,datf);
  pack_fread(&enemy,sizeof(SPRITE_TYPE)*enemy_anz,datf);

  pack_fclose(datf);

}


void write_config_file()
{
  int nr,nr2,nr3;
  extern int cur_max_levels;
  extern int GRAV_4_PIXELS;
  extern int ZOOM_ACTIVE;
  char temp[20];
  FILE *score_file2;

  config_file.config.language = LANGUAGE;
  config_file.config.pixanz = PIXEL_DIVISOR;
  config_file.config.svolume = SOUND_VOLUME;
  config_file.config.current_level = current_level;
  config_file.config.current_sprace_level = current_sprace_level;
  config_file.config.current_mprace_level = current_mprace_level;
  config_file.config.current_dogfight_level = current_dogfight_level;
  config_file.config.current_qdogfight_level = current_qdogfight_level;
  config_file.config.current_training_level = current_training_level;
  config_file.config.pixel_grav = GRAV_4_PIXELS;
  config_file.config.zoom_effect = ZOOM_ACTIVE;
  config_file.config.show_debris = SHOW_DEBRIS;
  config_file.config.play_menu_music = PLAY_MENU_MUSIC;

  config_file.config.def_frag_limit = frag_limit;
  config_file.config.def_time_limit = time_limit;

  strcpy(tmp_dir,dat_dir);
  cfg_file = fopen((char*)strcat(tmp_dir,"gf.cfg"),"wb");

  if (cfg_file)
  {
    fwrite(&config_file,sizeof(config_file),1,cfg_file);
    fclose(cfg_file);
  }

  // scorefile

  /* deleted */ 


  // Keyboard mapping
  strcpy(tmp_dir,dat_dir);
  cfg_file = fopen((char*)strcat(tmp_dir,"key.cfg"),"wb");

  if (cfg_file)
  {
    fwrite(&pcontrol,sizeof(pcontrol),1,cfg_file);
    fclose(cfg_file);
  }

}

void read_config_file()
{
  int nr,nr2,nr3;
  FILE *score_file2;
  char temp[20],temp2[20];
  unsigned char tempc;
  unsigned char ttempc;
  int ret;
  int file_max_levels = 0;
  extern int GRAV_4_PIXELS;
  extern int ZOOM_ACTIVE;

  strcpy(tmp_dir,dat_dir);
  cfg_file = fopen((char*)strcat(tmp_dir,"gf.cfg"),"rb");
  if (cfg_file)
  {
    fread(&config_file,sizeof(config_file),1,cfg_file);
    fclose(cfg_file);
  }

  LANGUAGE = config_file.config.language;
  PIXEL_DIVISOR = config_file.config.pixanz;
  SOUND_VOLUME = config_file.config.svolume;
  current_level = config_file.config.current_level;
  current_sprace_level = config_file.config.current_sprace_level;
  current_mprace_level = config_file.config.current_mprace_level;
  current_dogfight_level = config_file.config.current_dogfight_level;
  current_qdogfight_level = config_file.config.current_qdogfight_level;
  current_training_level = config_file.config.current_training_level;
  PLAY_MENU_MUSIC = config_file.config.play_menu_music;

  GRAV_4_PIXELS = config_file.config.pixel_grav;
  ZOOM_ACTIVE = config_file.config.zoom_effect;
  SHOW_DEBRIS = config_file.config.show_debris;

  frag_limit = config_file.config.def_frag_limit;
  time_limit = config_file.config.def_time_limit;

  // Hiscore-File

  /* deleted */


  // Keyboard mapping
  strcpy(tmp_dir,dat_dir);
  cfg_file = fopen((char*)strcat(tmp_dir,"key.cfg"),"rb");

  if (cfg_file)
  {
    fread(&pcontrol,sizeof(pcontrol),1,cfg_file);
    fclose(cfg_file);
  }

}

