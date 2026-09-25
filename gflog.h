/*
 *    GRAVITY FORCE, gflog.h
 *
 *
 *
 *    see source.txt for further information
 */

FILE *logfile;

void init_log();
void close_log();
void write_log_str(int nr, char *text, int nr2);

