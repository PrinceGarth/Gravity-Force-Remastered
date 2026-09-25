/* Header file for mappyAL V0.?? */
/* (C)2000 Robin Burrows  -  rburrows@bigfoot.com */

#ifdef __cplusplus
extern "C" {
#endif

#define MER_NONE 0              /* All the horrible things that can go wrong */
#define MER_OUTOFMEM 1
#define MER_MAPLOADERROR 2
#define MER_NOOPEN 3
#define MER_NOSCREEN 4
#define MER_NOACCELERATION 5
#define MER_CVBFAILED 6

#define AN_END -1                       /* Animation types, AN_END = end of anims */
#define AN_NONE 0                       /* No anim defined */
#define AN_LOOPF 1              /* Loops from start to end, then jumps to start etc */
#define AN_LOOPR 2              /* As above, but from end to start */
#define AN_ONCE 3                       /* Only plays once */
#define AN_ONCEH 4              /* Only plays once, but holds end frame */
#define AN_PPFF 5                       /* Ping Pong start-end-start-end-start etc */
#define AN_PPRR 6                       /* Ping Pong end-start-end-start-end etc */
#define AN_PPRF 7                       /* Used internally by playback */
#define AN_PPFR 8                       /* Used internally by playback */
#define AN_ONCES 9              /* Used internally by playback */

#define MapDraw15BG MapDraw16BG /* Same thing */
#define MapDraw15BGT MapDraw16BGT       /* Same thing */
#define MapDraw15FG MapDraw16FG /* Same thing */

typedef struct {                                /* Structure for data blocks */
long int bgoff, fgoff;                  /* offsets from start of graphic blocks */
long int fgoff2, fgoff3;                /* more overlay blocks */
unsigned long int user1, user2; /* user long data */
unsigned short int user3, user4;        /* user short data */
unsigned char user5, user6, user7;      /* user byte data */
unsigned char tl : 1;                           /* bits for collision detection */
unsigned char tr : 1;
unsigned char bl : 1;
unsigned char br : 1;
unsigned char trigger : 1;                      /* bit to trigger an event */
unsigned char unused1 : 1;
unsigned char unused2 : 1;
unsigned char unused3 : 1;
} BLKSTR;

typedef struct {                /* Animation control structure */
signed char antype;     /* Type of anim, AN_? */
signed char andelay;    /* Frames to go before next frame */
signed char ancount;    /* Counter, decs each frame, till 0, then resets to andelay */
signed char anuser;     /* User info */
long int ancuroff;      /* Points to current offset in list */
long int anstartoff;    /* Points to start of blkstr offsets list, AFTER ref. blkstr offset */
long int anendoff;      /* Points to end of blkstr offsets list */
} ANISTR;

typedef struct {                        /* Generic structure for chunk headers */
char id1, id2, id3, id4;        /* 4 byte header id. */
long int headsize;              /* size of header chunk. */
} GENHEAD;

typedef struct {                /* Map header structure */
char mapverhigh;                /* map version number to left of . (ie X.0). */
char mapverlow;         /* map version number to right of . (ie 0.X). */
char lsb;                       /* if 1, data stored LSB first, otherwise MSB first. */
char reserved;
short int mapwidth;     /* width in blocks. */
short int mapheight;    /* height in blocks. */
short int reserved1;
short int reserved2;
short int blockwidth;   /* width of a block (tile) in pixels. */
short int blockheight;  /* height of a block (tile) in pixels. */
short int blockdepth;   /* depth of a block (tile) in planes (ie. 256 colours is 8) */
short int blockstrsize; /* size of a block data structure */
short int numblockstr;  /* Number of block structures in BKDT */
short int numblockgfx;  /* Number of 'blocks' in graphics (BODY) */
} MPHD;

/* All global variables used bt Mappy playback are here */
extern int maperror;            /* Set to a MER_ error if something wrong happens */
extern short int mapwidth, mapheight, mapblockwidth, mapblockheight, mapdepth;
extern short int mapblockstrsize, mapnumblockstr, mapnumblockgfx;
extern short int * mappt;
extern short int ** maparraypt;
extern char * mapcmappt;
extern char * mapblockgfxpt;
extern char * mapblockstrpt;
extern char * mapanimstrpt;
extern char * mapanimstrendpt;
extern PACKFILE * mapfilept;
extern RGB mapcmap6bit[256];
extern short int *mapmappt[8];
extern short int ** mapmaparraypt[8];
extern BITMAP * abmTiles[1024];
/* End of Mappy globals */

unsigned long int Mapbyteswapl (unsigned long int i);
void Mapconv8to6pal (unsigned char * palpt);
void MapFreeMem (void);
void MapSetPal8 (void);
void MapCorrectColours (void);
int MapRelocate (void);
int MapLoad (char * mapname);
int MapLoadVRAM (char * mapname);
int MapLoadABM (char * mapname);
int MapDecode (unsigned char * mapmempt);
int MapDecodeVRAM (unsigned char * mapmempt);
int MapDecodeABM (unsigned char * mapmempt);
int MapDecodeBODY(void);
int MapGenerateYLookup (void);
int MapChangeLayer (int);
BLKSTR * MapGetBlock (int x, int y);
void MapSetBlock (int x, int y, int strvalue);
void MapRestore (void);
void MapInitAnims (void);
void MapUpdateAnims (void);
void MapDrawBG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph);
void MapDrawBGT (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph);
void MapDrawFG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph, int mapfg);
void MapDraw8BG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph);
void MapDraw8BGT (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph);
void MapDraw8FG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph, int mapfg);
void MapDraw15BG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph);
void MapDraw15BGT (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph);
void MapDraw15FG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph, int mapfg);
void MapDraw16BG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph);
void MapDraw16BGT (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph);
void MapDraw16FG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph, int mapfg);
void MapDraw32BG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph);
void MapDraw32BGT (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph);
void MapDraw32FG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph, int mapfg);
BITMAP * MapMakeParallaxBitmap (BITMAP *, int);
void MapDrawParallax (BITMAP * mapdestpt, BITMAP * parbm, int mapxo, int mapyo, int mapx, int mapy,
        int mapw, int maph);

#ifdef __cplusplus
}
#endif
