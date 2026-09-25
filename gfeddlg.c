/*
 *    GRAVITY FORCE, gfeddlg.c
 *
 *    -- dialog functions of "editor"
 *
 *
 *
 *    see source.txt for further information
 */

#include "gfhead.h"
#include "gfedhd.h"

void inc_node_anz()
{
  node_anz++;
}

void dec_node_anz()
{
  if (node_anz > 0) node_anz--;
}

void inc_base_anz()
{
  base_anz++;
}

void dec_base_anz()
{
  if (base_anz > 0) base_anz--;
}

void inc_object_anz()
{
  object_anz++;
}

void dec_object_anz()
{
  if (object_anz > 0) object_anz--;
}

void inc_spobject_anz()
{
  spobject_anz++;
}

void dec_spobject_anz()
{
  if (spobject_anz > 0) spobject_anz--;
}

void inc_enemy_anz()
{
  enemy_anz++;
}

void dec_enemy_anz()
{
  if (enemy_anz > 0) enemy_anz--;
}


DIALOG main_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)   (bg)   (key)   (flags)     (d1)     (d2)    (dp)    (dp2)  (dp3)*/
   { d_button_proc,       5,  430,   65,   15,   200,    21,      0,        0,       1,       1,   "PPOINTS" },
   { d_button_proc,       5,  450,   65,   15,   200,    21,      0,        0,       1,       1,   "BASES" },
   { d_button_proc,      80,  430,   65,   15,   200,    21,      0,        0,       1,       1,   "OBJECTS" },
   { d_button_proc,      80,  450,   65,   15,   200,    21,      0,        0,       1,       1,   "SPOBJECTS" },
   { d_button_proc,      80,  470,   65,   15,   200,    21,      0,        0,       1,       1,   "ENEMIES" },
   { NULL,                0,    0,    0,    0,     0,     0,      0,        0,       0,       0,   NULL }
};

DIALOG ppoints_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)   (bg)   (key)   (flags)     (d1)     (d2)    (dp)    (dp2)  (dp3)*/
   { d_radio_proc,        5,  430,   65,   15,   200,    21,      0,        0,       1,       1,   "ADDPNTS" },
   { d_radio_proc,        5,  450,   65,   15,   200,    21,      0,        0,       1,       1,   "SAVE" },
   { d_button_proc,     350,  450,   65,   15,   200,    21,      0,        0,       0,       0,   "EXIT" },
   { d_keyboard_proc,     0,    0,    0,    0,     0,     0,    '+',        0,       0,       0,   inc_node_anz },
   { d_keyboard_proc,     0,    0,    0,    0,     0,     0,    '-',        0,       0,       0,   dec_node_anz },
   { NULL,                0,    0,    0,    0,     0,     0,      0,        0,       0,       0,   NULL }
};

DIALOG bases_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)   (bg)   (key)   (flags)     (d1)     (d2)    (dp)    (dp2)  (dp3)*/
   { d_radio_proc,        5,  430,   65,   15,   200,    21,      0,        0,       2,       1,   "ADDBASE" },
   { d_radio_proc,        5,  450,   65,   15,   200,    21,      0,        0,       2,       1,   "SAVE" },
   { d_button_proc,     350,  450,   65,   15,   200,    21,      0,        0,       0,       0,   "EXIT" },
   { d_keyboard_proc,     0,    0,    0,    0,     0,     0,    '+',        0,       0,       0,   inc_base_anz },
   { d_keyboard_proc,     0,    0,    0,    0,     0,     0,    '-',        0,       0,       0,   dec_base_anz },
   { NULL,                0,    0,    0,    0,     0,     0,      0,        0,       0,       0,   NULL }
};

DIALOG objects_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)   (bg)   (key)   (flags)     (d1)     (d2)    (dp)    (dp2)  (dp3)*/
   { d_radio_proc,        5,  430,   65,   15,   200,    21,      0,        0,       3,       1,   "ADDOBJCT" },
   { d_radio_proc,        5,  450,   65,   15,   200,    21,      0,        0,       3,       1,   "SAVE" },
   { d_button_proc,     350,  450,   65,   15,   200,    21,      0,        0,       0,       0,   "EXIT" },
   { d_keyboard_proc,     0,    0,    0,    0,     0,     0,    '+',        0,       0,       0,   inc_object_anz },
   { d_keyboard_proc,     0,    0,    0,    0,     0,     0,    '-',        0,       0,       0,   dec_object_anz },
   { NULL,                0,    0,    0,    0,     0,     0,      0,        0,       0,       0,   NULL }
};

DIALOG spobjects_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)   (bg)   (key)   (flags)     (d1)     (d2)    (dp)    (dp2)  (dp3)*/
   { d_radio_proc,        5,  430,   65,   15,   200,    21,      0,        0,       3,       1,   "ADDSPOBJCT" },
   { d_radio_proc,        5,  450,   65,   15,   200,    21,      0,        0,       3,       1,   "SAVE" },
   { d_button_proc,     350,  450,   65,   15,   200,    21,      0,        0,       0,       0,   "EXIT" },
   { d_keyboard_proc,     0,    0,    0,    0,     0,     0,    '+',        0,       0,       0,   inc_spobject_anz },
   { d_keyboard_proc,     0,    0,    0,    0,     0,     0,    '-',        0,       0,       0,   dec_spobject_anz },
   { NULL,                0,    0,    0,    0,     0,     0,      0,        0,       0,       0,   NULL }
};

DIALOG enemies_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)   (bg)   (key)   (flags)     (d1)     (d2)    (dp)    (dp2)  (dp3)*/
   { d_radio_proc,        5,  430,   65,   15,   200,    21,      0,        0,       4,       1,   "ADDENEMY" },
   { d_radio_proc,        5,  450,   65,   15,   200,    21,      0,        0,       4,       1,   "SAVE" },
   { d_button_proc,     350,  450,   65,   15,   200,    21,      0,        0,       0,       0,   "EXIT" },
   { d_keyboard_proc,     0,    0,    0,    0,     0,     0,    '+',        0,       0,       0,   inc_enemy_anz },
   { d_keyboard_proc,     0,    0,    0,    0,     0,     0,    '-',        0,       0,       0,   dec_enemy_anz },
   { NULL,                0,    0,    0,    0,     0,     0,      0,        0,       0,       0,   NULL }
};

void do_dlg()
{

// main_dialog ...................................................

  // ppoints
  if (main_dialog[0].flags & D_SELECTED)
  {
    current_action = PPOINTS;
    main_dialog[0].flags = 0;
    broadcast_dialog_message(MSG_DRAW,c);
    scare_mouse(); clear(screen); unscare_mouse();
    dialog_message(ppoints_dialog,MSG_DRAW,c,NULL);
  }

  // bases
  if (main_dialog[1].flags & D_SELECTED)
  {
    current_action = BASES;
    main_dialog[1].flags = 0;
    broadcast_dialog_message(MSG_DRAW,c);
    scare_mouse(); clear(screen); unscare_mouse();
    dialog_message(bases_dialog,MSG_DRAW,c,NULL);
  }

  // objects
  if (main_dialog[2].flags & D_SELECTED)
  {
    current_action = OBJECTS;
    main_dialog[2].flags = 0;
    broadcast_dialog_message(MSG_DRAW,c);
    scare_mouse(); clear(screen); unscare_mouse();
    dialog_message(objects_dialog,MSG_DRAW,c,NULL);
  }

  // objects
  if (main_dialog[3].flags & D_SELECTED)
  {
    current_action = SPOBJECTS;
    main_dialog[3].flags = 0;
    broadcast_dialog_message(MSG_DRAW,c);
    scare_mouse(); clear(screen); unscare_mouse();
    dialog_message(objects_dialog,MSG_DRAW,c,NULL);
  }

  // enemies
  if (main_dialog[4].flags & D_SELECTED)
  {
    current_action = ENEMIES;
    main_dialog[4].flags = 0;
    broadcast_dialog_message(MSG_DRAW,c);
    scare_mouse(); clear(screen); unscare_mouse();
    dialog_message(enemies_dialog,MSG_DRAW,c,NULL);
  }

// ppoints_dialog ................................................

  // addpnts
  if (ppoints_dialog[0].flags & D_SELECTED)
  {
    ppoints_addpnts();
    dialog_message(ppoints_dialog,MSG_DRAW,c,NULL);
  }

  // writepnts
  if (ppoints_dialog[1].flags & D_SELECTED)
  {
    ppoints_write();
    ppoints_dialog[0].flags = 0;
    ppoints_dialog[1].flags = 0;
    dialog_message(ppoints_dialog,MSG_DRAW,c,NULL);
  }

  // exitppoints
  if (ppoints_dialog[2].flags & D_SELECTED)
  {
    current_action = -1;
    ppoints_dialog[0].flags = 0;
    ppoints_dialog[1].flags = 0;
    ppoints_dialog[2].flags = 0;
    scare_mouse(); clear(screen); clear(score_table); unscare_mouse();
    dialog_message(main_dialog,MSG_DRAW,c,NULL);
  }


// bases_dialog .....................................................

  // addbases
  if (bases_dialog[0].flags & D_SELECTED)
  {
    bases_addbase();
    dialog_message(bases_dialog,MSG_DRAW,c,NULL);
  }

  // writebases
  if (bases_dialog[1].flags & D_SELECTED)
  {
    bases_write();
    bases_dialog[0].flags = 0;
    bases_dialog[1].flags = 0;
    dialog_message(bases_dialog,MSG_DRAW,c,NULL);
  }

  // exitbases
  if (bases_dialog[2].flags & D_SELECTED)
  {
    current_action = -1;
    bases_dialog[0].flags = 0;
    bases_dialog[1].flags = 0;
    bases_dialog[2].flags = 0;
    scare_mouse(); clear(screen); clear(score_table); unscare_mouse();
    dialog_message(main_dialog,MSG_DRAW,c,NULL);
  }


// objects_dialog .....................................................

  // addobjects
  if (objects_dialog[0].flags & D_SELECTED)
  {
    objects_addobject();
    dialog_message(objects_dialog,MSG_DRAW,c,NULL);
  }

  // writeobjects
  if (objects_dialog[1].flags & D_SELECTED)
  {
    objects_write();
    objects_dialog[0].flags = 0;
    objects_dialog[1].flags = 0;
    dialog_message(objects_dialog,MSG_DRAW,c,NULL);
  }

  // exitobjects
  if (objects_dialog[2].flags & D_SELECTED)
  {
    current_action = -1;
    objects_dialog[0].flags = 0;
    objects_dialog[1].flags = 0;
    objects_dialog[2].flags = 0;
    scare_mouse(); clear(screen); clear(score_table); unscare_mouse();
    dialog_message(main_dialog,MSG_DRAW,c,NULL);
  }

// spobjects_dialog ...................................................

  // addspobjects
  if (spobjects_dialog[0].flags & D_SELECTED)
  {
    spobjects_addobject();
    dialog_message(spobjects_dialog,MSG_DRAW,c,NULL);
  }

  // writespobjects
  if (spobjects_dialog[1].flags & D_SELECTED)
  {
    spobjects_write();
    spobjects_dialog[0].flags = 0;
    spobjects_dialog[1].flags = 0;
    dialog_message(spobjects_dialog,MSG_DRAW,c,NULL);
  }

  // exitspobjects
  if (spobjects_dialog[2].flags & D_SELECTED)
  {
    current_action = -1;
    spobjects_dialog[0].flags = 0;
    spobjects_dialog[1].flags = 0;
    spobjects_dialog[2].flags = 0;
    scare_mouse(); clear(screen); clear(score_table); unscare_mouse();
    dialog_message(main_dialog,MSG_DRAW,c,NULL);
  }


// enemys_dialog .....................................................

  // addenemys
  if (enemies_dialog[0].flags & D_SELECTED)
  {
    enemies_addenemy();
    dialog_message(enemies_dialog,MSG_DRAW,c,NULL);
  }

  // writeenemys
  if (enemies_dialog[1].flags & D_SELECTED)
  {
    enemies_write();
    enemies_dialog[0].flags = 0;
    enemies_dialog[1].flags = 0;
    dialog_message(enemies_dialog,MSG_DRAW,c,NULL);
  }

  // exitenemys
  if (enemies_dialog[2].flags & D_SELECTED)
  {
    current_action = -1;
    enemies_dialog[0].flags = 0;
    enemies_dialog[1].flags = 0;
    enemies_dialog[2].flags = 0;
    scare_mouse(); clear(screen); clear(score_table); unscare_mouse();
    dialog_message(main_dialog,MSG_DRAW,c,NULL);
  }


}
