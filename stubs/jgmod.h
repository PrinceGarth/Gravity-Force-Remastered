/* jgmod replacement (Linux port): XM playback via libxmp, see gfmusic.c */
typedef struct JGMOD JGMOD;
int install_mod(int voices);
JGMOD *load_mod(char *file);
void play_mod(JGMOD *m, int loop);
void stop_mod(void);
void set_mod_volume(int vol);
void remove_mod(void);
void destroy_mod(JGMOD *m);
