/*
 *    GRAVITY FORCE, gfpal.h
 *
 *
 *
 *    see source.txt for further information
 */

int fade_out_active,fade_in_active,p_fade_out_active,p_fade_in_active;
float fade_speed,fade_pos,fade_opos,p_fade_speed,p_fade_pos,p_fade_opos;
signed int fade_count,fade_count_to,fade_type,p_fade_count,p_fade_count_to,p_fade_type;
int fade_from_nr,fade_to_nr;

// Header aus gfpal.c

void nfade_from_range(PALLETE source, PALLETE dest, float speed, int from, int to);
void nfade_in_range(PALLETE p, float speed, int from, int to);
void nfade_out_range(float speed, int from, int to);
void nnfade_from_range(PALLETE temp, PALLETE t, float speed, int from, int to);
void nnfade_up(int from, int to);
void nnfade_down(int from, int to);
void nfade_from(PALLETE source, PALLETE dest, float speed);
void nfade_in(PALLETE p, float speed);
void nfade_out(float speed);
void CheckFade();
