/*
 *    GRAVITY FORCE, gfedhd.h
 *
 *    -- main header file for editor
 *
 *
 *    see source.txt for further information
 */

enum Actions {
  PPOINTS,
  BASES,
  OBJECTS,
  SPOBJECTS,
  ENEMIES,
  ENPATHS
};

int mox,moy;

int current_action;
int  c;

int base_anz,object_anz,spobject_anz,enemy_anz;
int cur_base,cur_object,cur_spobject,cur_enemy;
int start_base,start_object,start_spobject,start_enemy;
int max_base,max_object,max_spobject,max_enemy;

BITMAP *temp_sprite;

