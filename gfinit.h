/*
 *    GRAVITY FORCE, gfinit.h
 *
 *    -- some init variables
 *
 *
 *    see source.txt for further information
 */

char level_passwords[30][20];

int timing_method;
int XRES,YRES;
int SOUND_VOLUME;
int KAMIKAZE_HITS;
int MAX_ROCKET_LOCK;
int MAX_LANDING_ANGLE;
int MAX_LANDING_SPEED;
int ZOOM_ACTIVE;
int GRAV_4_PIXELS;
int REFUEL_TIME;
float ALIEN_ROCKET_TURN;
int SHOW_DEBRIS;

int USE_JOYSTICK;
int USE_MOUSE;

int WRITE_LOG;

int PLAY_MENU_MUSIC;

// Header aus gfinit.c

char *encrypt_password(char *pwd);
char *decrypt_password(char *pwd);
void init_passwords();
void setup_global_stuff_once();
void setup_global_stuff();
void init_colors();
void init_bullets();
void init_ship(int c);
void callback_func();
void init_sounds();
void clear_arrays();
void init_tables();
void init_spec(int tst);
void init(int tst);
void init_first();
void free_all();
void error_exit(int el, char *t1, char *t2, int i1, int i2);
void init_controls();

