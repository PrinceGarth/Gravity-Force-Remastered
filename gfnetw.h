
// Network
typedef struct {
  int ready;
  int id;
} netplayer_type;

netplayer_type  network_player[MAX_NETWORK_PLAYERS];

int network_game;

// Header aus gfnetw.c
