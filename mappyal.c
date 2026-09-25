/* Mappy playback library functions and variables
 * (C)2000 Robin Burrows - rburrows@bigfoot.com
 * This version released 25/2/00 - for Mappy FMP maps
 * Please read the readmeal.txt file and look at the examples
 */

#include <stdint.h>
#include <stdio.h>
#include <allegro.h>

/* 64-bit port: BLKSTR/ANISTR keep 32-bit *offsets* (file layout); resolved on use */
#define ANBLK(a) ((BLKSTR *)(mapblockstrpt + *(int32_t *)(mapanimstrendpt + (a)->ancuroff)))
#define GFXBG(o) (mapblockgfxpt + (o))
#define GFXFG(o) ((o) ? mapblockgfxpt + (o) : (char *)0)

/* If you aren't using plain 'MapLoad', you can uncomment the
 * next line to remove a lot of code from your programme (about 20K from the exe ?)
 */
/* #define RBNOCUSTOMRENDER 1 */

#define MER_NONE 0		/* All the horrible things that can go wrong */
#define MER_OUTOFMEM 1
#define MER_MAPLOADERROR 2
#define MER_NOOPEN 3
#define MER_NOSCREEN 4
#define MER_NOACCELERATION 5
#define MER_CVBFAILED 6

#define AN_END -1			/* Animation types, AN_END = end of anims */
#define AN_NONE 0			/* No anim defined */
#define AN_LOOPF 1		/* Loops from start to end, then jumps to start etc */
#define AN_LOOPR 2		/* As above, but from end to start */
#define AN_ONCE 3			/* Only plays once */
#define AN_ONCEH 4		/* Only plays once, but holds end frame */
#define AN_PPFF 5			/* Ping Pong start-end-start-end-start etc */
#define AN_PPRR 6			/* Ping Pong end-start-end-start-end etc */
#define AN_PPRF 7			/* Used internally by playback */
#define AN_PPFR 8			/* Used internally by playback */
#define AN_ONCES 9		/* Used internally by playback */

#define MapDraw15BG MapDraw16BG	/* Same thing */
#define MapDraw15BGT MapDraw16BGT	/* Same thing */
#define MapDraw15FG MapDraw16FG	/* Same thing */

typedef struct {				/* Structure for data blocks */
int32_t bgoff, fgoff;			/* offsets from start of graphic blocks */
int32_t fgoff2, fgoff3; 		/* more overlay blocks */
uint32_t user1, user2;	/* user long data */
unsigned short int user3, user4;	/* user short data */
unsigned char user5, user6, user7;	/* user byte data */
unsigned char tl : 1;				/* bits for collision detection */
unsigned char tr : 1;
unsigned char bl : 1;
unsigned char br : 1;
unsigned char trigger : 1;			/* bit to trigger an event */
unsigned char unused1 : 1;
unsigned char unused2 : 1;
unsigned char unused3 : 1;
} BLKSTR;

typedef struct {		/* Animation control structure */
signed char antype;	/* Type of anim, AN_? */
signed char andelay;	/* Frames to go before next frame */
signed char ancount;	/* Counter, decs each frame, till 0, then resets to andelay */
signed char anuser;	/* User info */
int32_t ancuroff;	/* Points to current offset in list */
int32_t anstartoff;	/* Points to start of blkstr offsets list, AFTER ref. blkstr offset */
int32_t anendoff;	/* Points to end of blkstr offsets list */
} ANISTR;

typedef struct {			/* Generic structure for chunk headers */
char id1, id2, id3, id4;	/* 4 byte header id. */
int32_t headsize;		/* size of header chunk. */
} GENHEAD;

typedef struct {		/* Map header structure */
char mapverhigh;		/* map version number to left of . (ie X.0). */
char mapverlow;		/* map version number to right of . (ie 0.X). */
char lsb;			/* if 1, data stored LSB first, otherwise MSB first. */
char reserved;
short int mapwidth;	/* width in blocks. */
short int mapheight;	/* height in blocks. */
short int reserved1;
short int reserved2;
short int blockwidth;	/* width of a block (tile) in pixels. */
short int blockheight;	/* height of a block (tile) in pixels. */
short int blockdepth;	/* depth of a block (tile) in planes (ie. 256 colours is 8) */
short int blockstrsize;	/* size of a block data structure */
short int numblockstr;	/* Number of block structures in BKDT */
short int numblockgfx;	/* Number of 'blocks' in graphics (BODY) */
} MPHD;

/* All global variables used by Mappy playback are here */
int maperror, gfxinbitmaps;		/* Set to a MER_ error if something wrong happens */
short int mapwidth, mapheight, mapblockwidth, mapblockheight, mapdepth;
short int mapblockstrsize, mapnumblockstr, mapnumblockgfx;
PACKFILE * mapfilept;
short int * mappt = NULL;
short int ** maparraypt = NULL;
char * mapcmappt = NULL;
char * mapblockgfxpt = NULL;
char * mapblockstrpt = NULL;
char * mapanimstrpt = NULL;
char * mapanimstrendpt;
GENHEAD mapgenheader;
RGB mapcmap6bit[256];
short int * mapmappt[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
short int ** mapmaparraypt[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
BITMAP * abmTiles[1024];
/* End of Mappy globals */

int MapGenerateYLookup (void)
{
int i, j;

	for (i=0;i<8;i++) {
		if (mapmaparraypt[i]!=NULL) { free (mapmaparraypt[i]); mapmaparraypt[i] = NULL; }
		if (mapmappt[i]!=NULL) {
			mapmaparraypt[i] = malloc (mapheight*sizeof(short int *));
			if (mapmaparraypt[i] == NULL) return -1;
			for (j=0;j<mapheight;j++) mapmaparraypt[i][j] = (mapmappt[i]+(j*mapwidth));
			if (mapmappt[i] == mappt) maparraypt = mapmaparraypt[i];
		}
	}
	return 0;
}

BLKSTR * MapGetBlock (int x, int y)
{
short int * mymappt;
ANISTR * myanpt;

	if (maparraypt!= NULL) {
		mymappt = maparraypt[y]+x;
	} else {
		mymappt = mappt;
		mymappt += x;
		mymappt += y*mapwidth;
	}
	if (*mymappt>=0) return (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
		return ANBLK(myanpt); }
}

void MapSetBlock (int x, int y, int strvalue)
{
short int * mymappt;

	if (maparraypt!= NULL) {
		mymappt = maparraypt[y]+x;
	} else {
		mymappt = mappt;
		mymappt += x;
		mymappt += y*mapwidth;
	}
	if (strvalue>=0) *mymappt = strvalue*sizeof(BLKSTR);
	else *mymappt = strvalue*sizeof(ANISTR);
}

int MapChangeLayer (int newlyr)
{
	if (newlyr<0 || newlyr>7 || mapmappt[newlyr] == NULL) return -1;
	mappt = mapmappt[newlyr]; maparraypt = mapmaparraypt[newlyr];
	return newlyr;
}

uint32_t Mapbyteswapl (uint32_t i)
{
uint32_t j;
	j = 0; j = i&0xFF; j <<= 8; i >>= 8; j |= i&0xFF; j <<= 8; i >>= 8;
	j |= i&0xFF; j <<= 8; i >>= 8; j |= i&0xFF; return j;
}

void Mapconv8to6pal (unsigned char * palpt)
{
int i;
	for (i=0;i<256;i++)
	{
		mapcmap6bit[i].r=(*(palpt)>>2);
		mapcmap6bit[i].g=(*(palpt+1)>>2);
		mapcmap6bit[i].b=(*(palpt+2)>>2);
		palpt+=3;
	}
}

void MapFreeMem (void)
{
int i;
	for (i=0;i<8;i++) { if (mapmappt[i]!=NULL) { free (mapmappt[i]); mapmappt[i] = NULL; } }
	mappt = NULL;
	for (i=0;i<8;i++) { if (mapmaparraypt[i]!=NULL) { free (mapmaparraypt[i]); mapmaparraypt[i] = NULL; } }
	maparraypt = NULL;
	if (mapcmappt!=NULL) { free (mapcmappt); mapcmappt = NULL; }
	if (mapblockgfxpt!=NULL) { free (mapblockgfxpt); mapblockgfxpt = NULL; }
	if (mapblockstrpt!=NULL) { free (mapblockstrpt); mapblockstrpt = NULL; }
	if (mapanimstrpt!=NULL) { free (mapanimstrpt); mapanimstrpt = NULL; }
	i = 0; while (abmTiles[i]!=NULL) { destroy_bitmap (abmTiles[i]); abmTiles[i] = NULL; i++; }
}

void MapSetPal8 (void)
{
	if (screen!=NULL) { if ((bitmap_color_depth (screen)==8) && mapdepth == 8) set_palette (mapcmap6bit); }
}

void MapCorrectColours (void)
{
/* Remember, Intel stores shorts and longs the wrong way round!
 * ie. 0x12345678 will be 0x78563412
 */
	return;
}

void MapRestore (void)
{
int i, j, k;
unsigned char * newgfxpt;

	if (gfxinbitmaps!=1) return;
	i = 0; newgfxpt = mapblockgfxpt; while (abmTiles[i]!=NULL) {
		acquire_bitmap (abmTiles[i]);
		for (k=0;k<mapblockheight;k++) {
		for (j=0;j<mapblockwidth;j++) {
		switch (mapdepth) {
			case 8:
				putpixel (abmTiles[i], j, k, *((unsigned char *) newgfxpt));
				newgfxpt++;
				break;
			case 15:
			case 16:
				putpixel (abmTiles[i], j, k, *((unsigned short int *) newgfxpt));
				newgfxpt+=2;
				break;
			case 24:
				putpixel (abmTiles[i], j, k, makecol (newgfxpt[0], newgfxpt[1], newgfxpt[2]));
				newgfxpt+=3;
				break;
			case 32:
				putpixel (abmTiles[i], j, k, makecol (newgfxpt[1], newgfxpt[2], newgfxpt[3]));
				newgfxpt+=4;
				break;
		} } }
		release_bitmap (abmTiles[i]);
		i++;
	}
}

int MapRelocate2 (void)
{
int i, j, k;
BLKSTR * myblkstrpt;
ANISTR * myanpt;
unsigned char * newgfxpt;
int32_t * myanblkpt;

	i = mapnumblockstr;
	myblkstrpt = (BLKSTR *) mapblockstrpt;
	if (!gfxinbitmaps) {
	while (i)
	{
		myblkstrpt++; i--;
	} } else {
	i = 0; newgfxpt = mapblockgfxpt; while (i<mapnumblockgfx) {
		abmTiles[i+1] = NULL;
		if (gfxinbitmaps==1) {
			abmTiles[i] = create_video_bitmap (mapblockwidth, mapblockheight);
			if (abmTiles[i] == NULL) { MapFreeMem (); maperror = MER_CVBFAILED ; return -1; }
			set_clip (abmTiles[i], 0, 0, 0, 0);
			acquire_bitmap (abmTiles[i]);
		} else {
			abmTiles[i] = create_bitmap (mapblockwidth, mapblockheight);
			if (abmTiles[i] == NULL) { MapFreeMem (); maperror = MER_CVBFAILED ; return -1; }
			set_clip (abmTiles[i], 0, 0, 0, 0);
		}
		for (k=0;k<mapblockheight;k++) {
		for (j=0;j<mapblockwidth;j++) {
		switch (mapdepth) {
			case 8:
				putpixel (abmTiles[i], j, k, *((unsigned char *) newgfxpt));
				newgfxpt++;
				break;
			case 15:
			case 16:
				putpixel (abmTiles[i], j, k, *((unsigned short int *) newgfxpt));
				newgfxpt+=2;
				break;
			case 24:
				putpixel (abmTiles[i], j, k, makecol (newgfxpt[0], newgfxpt[1], newgfxpt[2]));
				newgfxpt+=3;
				break;
			case 32:
				putpixel (abmTiles[i], j, k, makecol (newgfxpt[1], newgfxpt[2], newgfxpt[3]));
				newgfxpt+=4;
				break;
		} } }
		if (gfxinbitmaps==1) release_bitmap (abmTiles[i]);
		i++;
	}
	i = mapnumblockstr; while (i) {
		(void)0;
		if (myblkstrpt->fgoff!=0)
		(void)0;
		if (myblkstrpt->fgoff2!=0)
	(void)0;
		if (myblkstrpt->fgoff3!=0)
	(void)0;
	myblkstrpt++; i--;
	}
	}

	if (mapanimstrpt!=NULL) {
	myanpt = (ANISTR *) mapanimstrendpt; myanpt--;
	while (myanpt->antype!=AN_END)
	{
		myanpt--;
	}
	}
	return 0;
}

int MapRelocate (void)
{
int i, j, cdepth, pixcol;
BLKSTR * myblkstrpt;
unsigned char * oldgfxpt;
unsigned char * mycmappt;
unsigned char * newgfxpt;
unsigned char * newgfx2pt;

	if (screen == NULL) { MapFreeMem (); maperror = MER_NOSCREEN; return -1; }
	if (!gfx_capabilities&GFX_HW_VRAM_BLIT && gfxinbitmaps==1)
		{ MapFreeMem (); maperror = MER_NOACCELERATION; return -1; }
	cdepth = bitmap_color_depth (screen);
		oldgfxpt = (unsigned char *) mapblockgfxpt;
		newgfxpt = (unsigned char *)
			malloc (mapblockwidth*mapblockheight*((mapdepth+1)/8)*mapnumblockgfx*((cdepth+1)/8));
		if (newgfxpt==NULL) { MapFreeMem (); maperror = MER_OUTOFMEM; return -1; }
		newgfx2pt = newgfxpt;
		mycmappt = (unsigned char *) mapcmappt; pixcol = 0;
		for (i=0;i<(mapblockwidth*mapblockheight*mapnumblockgfx);i++)
		{
			switch (mapdepth) {
			case 8:
				if (cdepth==8) pixcol = (int) *oldgfxpt; else {
				j = (*oldgfxpt)*3;
				pixcol = makecol (mycmappt[j], mycmappt[j+1], mycmappt[j+2]);
				if (j == 0 && cdepth!=8) pixcol = makecol (255, 0, 255); }
				oldgfxpt++;
				break;
			case 15:
				pixcol = makecol (((((int) *oldgfxpt)&0x7C)<<1),
					(((((int) *oldgfxpt)&0x3)<<3)|(((int) *(oldgfxpt+1))>>5))<<3,
					(((int) *(oldgfxpt+1)&0x1F)<<3));
				if (cdepth==8) { if (((((int) *oldgfxpt)&0x7C)<<1) == 0xF8 &&
					((((((int) *oldgfxpt)&0x3)<<3)|(((int) *(oldgfxpt+1))>>5))<<3) == 0 &&
					(((int) *(oldgfxpt+1)&0x1F)<<3) == 0xF8) pixcol = 0; }
				oldgfxpt += 2;
				break;
			case 16:
				pixcol = makecol ((((int) *oldgfxpt)&0xF8),
					(((((int) *oldgfxpt)&0x7)<<3)|(((int) *(oldgfxpt+1))>>5))<<2,
					(((int) *(oldgfxpt+1)&0x1F)<<3));
				if (cdepth==8) { if ((((int) *oldgfxpt)&0xF8) == 0xF8 &&
					((((((int) *oldgfxpt)&0x7)<<3)|(((int) *(oldgfxpt+1))>>5))<<2) == 0 &&
					(((int) *(oldgfxpt+1)&0x1F)<<3) == 0xF8) pixcol = 0; }
				oldgfxpt += 2;
				break;
			case 24:
				pixcol = makecol (*oldgfxpt, *(oldgfxpt+1), *(oldgfxpt+2));
				if (cdepth==8) { if (*oldgfxpt == 0xFF && *(oldgfxpt+1) == 0 &&
					*(oldgfxpt+2) == 0xFF) pixcol = 0; }
				oldgfxpt += 3;
				break;
			case 32:
				pixcol = makecol (*(oldgfxpt+1), *(oldgfxpt+2), *(oldgfxpt+3));
				if (cdepth==8) { if (*(oldgfxpt+1) == 0xFF && *(oldgfxpt+2) == 0 &&
					*(oldgfxpt+3) == 0xFF) pixcol = 0; }
				oldgfxpt += 4;
				break;
			}
			switch (cdepth) {
			case 8:
				*newgfxpt = (unsigned char) pixcol;
				newgfxpt++;
				break;
			case 15:
			case 16:
				*((unsigned short int *) newgfxpt) = (unsigned short int) pixcol;
				newgfxpt+=2;
				break;
			case 24:
				*newgfxpt = (unsigned char) (pixcol>>16)&0xFF;
				*(newgfxpt+1) = (unsigned char) (pixcol>>8)&0xFF;
				*(newgfxpt+2) = (unsigned char) pixcol&0xFF;
				newgfxpt+=3;
				break;
			case 32:
				*newgfxpt = 0;
				*(newgfxpt+1) = (unsigned char) (pixcol>>16)&0xFF;
				*(newgfxpt+2) = (unsigned char) (pixcol>>8)&0xFF;
				*(newgfxpt+3) = (unsigned char) pixcol&0xFF;
				newgfxpt+=3;
				break;
			}
		}
		free (mapblockgfxpt); mapblockgfxpt = (char *) newgfx2pt;
		i = mapnumblockstr;
		myblkstrpt = (BLKSTR *) mapblockstrpt;
		while (i)
		{
			myblkstrpt->bgoff = (myblkstrpt->bgoff/(mapblockwidth*mapblockheight*((mapdepth+1)/8)));
			myblkstrpt->bgoff *= (mapblockwidth*mapblockheight*((cdepth+1)/8));
			myblkstrpt->fgoff = (myblkstrpt->fgoff/(mapblockwidth*mapblockheight*((mapdepth+1)/8)));
			myblkstrpt->fgoff *= (mapblockwidth*mapblockheight*((cdepth+1)/8));
			myblkstrpt->fgoff2 = (myblkstrpt->fgoff2/(mapblockwidth*mapblockheight*((mapdepth+1)/8)));
			myblkstrpt->fgoff2 *= (mapblockwidth*mapblockheight*((cdepth+1)/8));
			myblkstrpt->fgoff3 = (myblkstrpt->fgoff3/(mapblockwidth*mapblockheight*((mapdepth+1)/8)));
			myblkstrpt->fgoff3 *= (mapblockwidth*mapblockheight*((cdepth+1)/8));
			myblkstrpt++; i--;
		}
		mapdepth = cdepth;

	return MapRelocate2 ();
}

int MapDecodeMPHD (void)
{
MPHD * hdrmempt;

	hdrmempt = (MPHD *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (hdrmempt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	pack_fread (hdrmempt, Mapbyteswapl(mapgenheader.headsize), mapfilept);
	mapwidth=hdrmempt->mapwidth;
	mapheight=hdrmempt->mapheight;
	mapblockwidth=hdrmempt->blockwidth;
	mapblockheight=hdrmempt->blockheight;
	mapdepth=hdrmempt->blockdepth;
	mapblockstrsize=hdrmempt->blockstrsize;
	mapnumblockstr=hdrmempt->numblockstr;
	mapnumblockgfx=hdrmempt->numblockgfx;
	free (hdrmempt);
	return 0;
}

int MapDecodeCMAP (void)
{
	mapcmappt = (unsigned char *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapcmappt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	pack_fread (mapcmappt, Mapbyteswapl(mapgenheader.headsize), mapfilept);
	Mapconv8to6pal (mapcmappt);
	return 0;
}

int MapDecodeBKDT (void)
{
	mapblockstrpt = malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapblockstrpt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	pack_fread (mapblockstrpt, Mapbyteswapl(mapgenheader.headsize), mapfilept);

	return 0;
}

int MapDecodeANDT (void)
{
	mapanimstrpt = malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapanimstrpt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	mapanimstrendpt = (char *) (mapanimstrpt+(Mapbyteswapl(mapgenheader.headsize)));
	pack_fread (mapanimstrpt, Mapbyteswapl(mapgenheader.headsize), mapfilept);

	return 0;
}

int MapDecodeBGFX (void)
{
	mapblockgfxpt = malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapblockgfxpt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	pack_fread (mapblockgfxpt, Mapbyteswapl(mapgenheader.headsize), mapfilept);

	return 0;
}

int MapDecodeBODY (void)
{
	mappt = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mappt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	pack_fread (mappt, Mapbyteswapl(mapgenheader.headsize), mapfilept);
	mapmappt[0] = mappt;
	return 0;
}
int MapDecodeLYR1 (void)
{
	mapmappt[1] = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapmappt[1]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	pack_fread (mapmappt[1], Mapbyteswapl(mapgenheader.headsize), mapfilept);
	return 0;
}
int MapDecodeLYR2 (void)
{
	mapmappt[2] = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapmappt[2]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	pack_fread (mapmappt[2], Mapbyteswapl(mapgenheader.headsize), mapfilept);
	return 0;
}
int MapDecodeLYR3 (void)
{
	mapmappt[3] = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapmappt[3]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	pack_fread (mapmappt[3], Mapbyteswapl(mapgenheader.headsize), mapfilept);
	return 0;
}
int MapDecodeLYR4 (void)
{
	mapmappt[4] = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapmappt[4]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	pack_fread (mapmappt[4], Mapbyteswapl(mapgenheader.headsize), mapfilept);
	return 0;
}
int MapDecodeLYR5 (void)
{
	mapmappt[5] = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapmappt[5]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	pack_fread (mapmappt[5], Mapbyteswapl(mapgenheader.headsize), mapfilept);
	return 0;
}
int MapDecodeLYR6 (void)
{
	mapmappt[6] = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapmappt[6]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	pack_fread (mapmappt[6], Mapbyteswapl(mapgenheader.headsize), mapfilept);
	return 0;
}
int MapDecodeLYR7 (void)
{
	mapmappt[7] = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapmappt[7]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	pack_fread (mapmappt[7], Mapbyteswapl(mapgenheader.headsize), mapfilept);
	return 0;
}

int MapDecodeNULL (void)
{
char * mynllpt;
	mynllpt = (char *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mynllpt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	pack_fread (mynllpt, Mapbyteswapl(mapgenheader.headsize), mapfilept);
	free (mynllpt);
	return 0;
}

int MapRealLoad (char * mapname)
{
int i;
int32_t mapfilesize, mapbytesread;
//char debugtxt[80];

	abmTiles[0] = NULL;
	MapFreeMem ();
	maperror = 0;
	
	mapfilept = pack_fopen (mapname, "rp");
	if (mapfilept==NULL) { mapfilept = pack_fopen (mapname, "r");
		if (mapfilept==NULL) { maperror = MER_NOOPEN; return -1; } }
	if (pack_fread (&mapgenheader, sizeof(GENHEAD), mapfilept)!=sizeof(GENHEAD))
	{ maperror = MER_MAPLOADERROR; pack_fclose (mapfilept); return -1; }

	if (mapgenheader.id1!='F') maperror = MER_MAPLOADERROR;
	if (mapgenheader.id2!='O') maperror = MER_MAPLOADERROR;
	if (mapgenheader.id3!='R') maperror = MER_MAPLOADERROR;
	if (mapgenheader.id4!='M') maperror = MER_MAPLOADERROR;

	mapfilesize = (Mapbyteswapl(mapgenheader.headsize))+8;

	if (maperror) { pack_fclose (mapfilept); return -1; }

	if (pack_fread (&mapgenheader, sizeof(GENHEAD)-4,mapfilept)!=sizeof(GENHEAD)-4)
	{ maperror = MER_MAPLOADERROR; pack_fclose (mapfilept); return -1; }
	maperror=MER_MAPLOADERROR;
	if (mapgenheader.id1=='F') { if (mapgenheader.id2=='M') { if (mapgenheader.id3=='A')
	{ if (mapgenheader.id4=='P') maperror=MER_NONE; } } }
	if (maperror) { pack_fclose (mapfilept); return -1; }
	mapbytesread = 12;

//	set_palette (desktop_palette);
	while (mapfilesize!=mapbytesread)
	{
//		sprintf (debugtxt, "mapfilesize = %d mapbytesread = %d", mapfilesize, mapbytesread);
//		alert ((char *) &mapgenheader, debugtxt, NULL, "&OK", NULL, 'o', 0);
		if (pack_fread (&mapgenheader, sizeof(GENHEAD),mapfilept)!=sizeof(GENHEAD))
		{ maperror = MER_MAPLOADERROR; pack_fclose (mapfilept); return -1; }
		mapbytesread += sizeof(GENHEAD);
		if (mapbytesread>=mapfilesize) break;

		i = 0;
		if (mapgenheader.id1=='M') { if (mapgenheader.id2=='P') { if (mapgenheader.id3=='H')
		{ if (mapgenheader.id4=='D') { MapDecodeMPHD (); i = 1; } } } }
		if (mapgenheader.id1=='C') { if (mapgenheader.id2=='M') { if (mapgenheader.id3=='A')
		{ if (mapgenheader.id4=='P') { MapDecodeCMAP (); i = 1; } } } }
		if (mapgenheader.id1=='B') { if (mapgenheader.id2=='K') { if (mapgenheader.id3=='D')
		{ if (mapgenheader.id4=='T') { MapDecodeBKDT (); i = 1; } } } }
		if (mapgenheader.id1=='A') { if (mapgenheader.id2=='N') { if (mapgenheader.id3=='D')
		{ if (mapgenheader.id4=='T') { MapDecodeANDT (); i = 1; } } } }
		if (mapgenheader.id1=='B') { if (mapgenheader.id2=='G') { if (mapgenheader.id3=='F')
		{ if (mapgenheader.id4=='X') { MapDecodeBGFX (); i = 1; } } } }
		if (mapgenheader.id1=='B') { if (mapgenheader.id2=='O') { if (mapgenheader.id3=='D')
		{ if (mapgenheader.id4=='Y') { MapDecodeBODY (); i = 1; } } } }
		if (mapgenheader.id1=='L') { if (mapgenheader.id2=='Y') { if (mapgenheader.id3=='R')
		{ if (mapgenheader.id4=='1') { MapDecodeLYR1 (); i = 1; } } } }
		if (mapgenheader.id1=='L') { if (mapgenheader.id2=='Y') { if (mapgenheader.id3=='R')
		{ if (mapgenheader.id4=='2') { MapDecodeLYR2 (); i = 1; } } } }
		if (mapgenheader.id1=='L') { if (mapgenheader.id2=='Y') { if (mapgenheader.id3=='R')
		{ if (mapgenheader.id4=='3') { MapDecodeLYR3 (); i = 1; } } } }
		if (mapgenheader.id1=='L') { if (mapgenheader.id2=='Y') { if (mapgenheader.id3=='R')
		{ if (mapgenheader.id4=='4') { MapDecodeLYR4 (); i = 1; } } } }
		if (mapgenheader.id1=='L') { if (mapgenheader.id2=='Y') { if (mapgenheader.id3=='R')
		{ if (mapgenheader.id4=='5') { MapDecodeLYR5 (); i = 1; } } } }
		if (mapgenheader.id1=='L') { if (mapgenheader.id2=='Y') { if (mapgenheader.id3=='R')
		{ if (mapgenheader.id4=='6') { MapDecodeLYR6 (); i = 1; } } } }
		if (mapgenheader.id1=='L') { if (mapgenheader.id2=='Y') { if (mapgenheader.id3=='R')
		{ if (mapgenheader.id4=='7') { MapDecodeLYR7 (); i = 1; } } } }
		if (!i) MapDecodeNULL ();
		if (maperror) { pack_fclose (mapfilept); return -1; }
		mapbytesread += Mapbyteswapl(mapgenheader.headsize);
	}
	pack_fclose (mapfilept);
	i = MapRelocate ();
	return i;
}

int MapLoad (char * mapname)
{
	gfxinbitmaps = 0;
	return MapRealLoad (mapname);
}

int MapLoadVRAM (char * mapname)
{
	gfxinbitmaps = 1;
	return MapRealLoad (mapname);
}

int MapLoadABM (char * mapname)
{
	gfxinbitmaps = 2;
	return MapRealLoad (mapname);
}

int MapMemDecodeMPHD (MPHD * mapmempt)
{
	mapwidth=mapmempt->mapwidth;
	mapheight=mapmempt->mapheight;
	mapblockwidth=mapmempt->blockwidth;
	mapblockheight=mapmempt->blockheight;
	mapdepth=mapmempt->blockdepth;
	mapblockstrsize=mapmempt->blockstrsize;
	mapnumblockstr=mapmempt->numblockstr;
	mapnumblockgfx=mapmempt->numblockgfx;
	return 0;
}

int MapMemDecodeCMAP (unsigned char * cmppt)
{
int i;
unsigned char * tempcmappt;
	mapcmappt = (unsigned char *) malloc (Mapbyteswapl(*(int32_t *)cmppt));
	if (mapcmappt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	cmppt += 4;
	tempcmappt = mapcmappt;
	for (i=0;i<(256*3);i++) { *tempcmappt = *cmppt; tempcmappt++; cmppt++; }
	Mapconv8to6pal (mapcmappt);
	set_palette (mapcmap6bit);
	return 0;
}

int MapMemDecodeBKDT (unsigned char * mapmempt)
{
int i, j;
unsigned char * temppt;
	i = Mapbyteswapl(*(int32_t *)mapmempt);
	mapmempt += 4;
	mapblockstrpt = malloc (i);
	if (mapblockstrpt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	temppt = (unsigned char *) mapblockstrpt;
	for (j=0;j<i;j++) { *temppt = *mapmempt; temppt++; mapmempt++; }

	return 0;
}

int MapMemDecodeANDT (unsigned char * mapmempt)
{
int i, j;
unsigned char * temppt;
	i = Mapbyteswapl(*(int32_t *)mapmempt);
	mapmempt += 4;
	mapanimstrpt = malloc (i);
	if (mapanimstrpt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	mapanimstrendpt = (char *) (mapanimstrpt+i);
	temppt = (unsigned char *) mapanimstrpt;
	for (j=0;j<i;j++) { *temppt = *mapmempt; temppt++; mapmempt++; }

	return 0;
}

int MapMemDecodeBGFX (unsigned char * mapmempt)
{
int i, j;
unsigned char * temppt;
	i = Mapbyteswapl(*(int32_t *)mapmempt);
	mapmempt += 4;
	mapblockgfxpt = malloc (i);
	if (mapblockgfxpt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	temppt = (unsigned char *) mapblockgfxpt;
	for (j=0;j<i;j++) { *temppt = *mapmempt; temppt++; mapmempt++; }
	return 0;
}

int MapMemDecodeBODY (unsigned char * mapmempt)
{
int i, j;
unsigned char * temppt;
	i = Mapbyteswapl(*(int32_t *)mapmempt);
	mapmempt += 4;
	mappt = malloc (i);
	if (mappt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	mapmappt[0] = mappt;
	temppt = (unsigned char *) mappt;
	for (j=0;j<i;j++) { *temppt = *mapmempt; temppt++; mapmempt++; }
	return 0;
}
int MapMemDecodeLYR (unsigned char * mapmempt, int maplayernum)
{
int i, j;
unsigned char * temppt;
	i = Mapbyteswapl(*(int32_t *)mapmempt);
	mapmempt += 4;
	mapmappt[maplayernum] = malloc (i);
	if (mapmappt[maplayernum]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	temppt = (unsigned char *) mappt;
	for (j=0;j<i;j++) { *temppt = *mapmempt; temppt++; mapmempt++; }
	return 0;
}

int MapRealDecode (unsigned char * mapmempt)
{
int i;
int32_t maplength;

	MapFreeMem ();
	maperror = 0;

	if (*mapmempt!='F') maperror = MER_MAPLOADERROR;
	if (*(mapmempt+1)!='O') maperror = MER_MAPLOADERROR;
	if (*(mapmempt+2)!='R') maperror = MER_MAPLOADERROR;
	if (*(mapmempt+3)!='M') maperror = MER_MAPLOADERROR;
	mapmempt += 4;
	maplength = (Mapbyteswapl(*(int32_t *)mapmempt)+8);

	if (maperror) return -1;
	mapmempt += 4;

	if (*mapmempt!='F') maperror = MER_MAPLOADERROR;
	if (*(mapmempt+1)!='M') maperror = MER_MAPLOADERROR;
	if (*(mapmempt+2)!='A') maperror = MER_MAPLOADERROR;
	if (*(mapmempt+3)!='P') maperror = MER_MAPLOADERROR;
	mapmempt+=4; i = 12;

	if (maperror) return -1;

	while (i!=maplength)
	{
		if ((*mapmempt=='M') && (*(mapmempt+1)=='P') && (*(mapmempt+2)=='H') &&
		(*(mapmempt+3)=='D')) MapMemDecodeMPHD ((MPHD *) (mapmempt+8));
		if ((*mapmempt=='C') && (*(mapmempt+1)=='M') && (*(mapmempt+2)=='A') &&
		(*(mapmempt+3)=='P')) MapMemDecodeCMAP (mapmempt+4);
		if ((*mapmempt=='B') && (*(mapmempt+1)=='K') && (*(mapmempt+2)=='D') &&
		(*(mapmempt+3)=='T')) MapMemDecodeBKDT (mapmempt+4);
		if ((*mapmempt=='A') && (*(mapmempt+1)=='N') && (*(mapmempt+2)=='D') &&
		(*(mapmempt+3)=='T')) MapMemDecodeANDT (mapmempt+4);
		if ((*mapmempt=='B') && (*(mapmempt+1)=='G') && (*(mapmempt+2)=='F') &&
		(*(mapmempt+3)=='X')) MapMemDecodeBGFX (mapmempt+4);
		if ((*mapmempt=='B') && (*(mapmempt+1)=='O') && (*(mapmempt+2)=='D') &&
		(*(mapmempt+3)=='Y')) MapMemDecodeBODY (mapmempt+4);
		if ((*mapmempt=='L') && (*(mapmempt+1)=='Y') && (*(mapmempt+2)=='R') &&
		(*(mapmempt+3)=='1')) MapMemDecodeLYR (mapmempt+4, 1);
		if ((*mapmempt=='L') && (*(mapmempt+1)=='Y') && (*(mapmempt+2)=='R') &&
		(*(mapmempt+3)=='2')) MapMemDecodeLYR (mapmempt+4, 2);
		if ((*mapmempt=='L') && (*(mapmempt+1)=='Y') && (*(mapmempt+2)=='R') &&
		(*(mapmempt+3)=='3')) MapMemDecodeLYR (mapmempt+4, 3);
		if ((*mapmempt=='L') && (*(mapmempt+1)=='Y') && (*(mapmempt+2)=='R') &&
		(*(mapmempt+3)=='4')) MapMemDecodeLYR (mapmempt+4, 4);
		if ((*mapmempt=='L') && (*(mapmempt+1)=='Y') && (*(mapmempt+2)=='R') &&
		(*(mapmempt+3)=='5')) MapMemDecodeLYR (mapmempt+4, 5);
		if ((*mapmempt=='L') && (*(mapmempt+1)=='Y') && (*(mapmempt+2)=='R') &&
		(*(mapmempt+3)=='6')) MapMemDecodeLYR (mapmempt+4, 6);
		if ((*mapmempt=='L') && (*(mapmempt+1)=='Y') && (*(mapmempt+2)=='R') &&
		(*(mapmempt+3)=='7')) MapMemDecodeLYR (mapmempt+4, 7);
		if (maperror) return -1;
		mapmempt += 4; i += Mapbyteswapl(*(int32_t *)mapmempt)+8;
		mapmempt += Mapbyteswapl(*(int32_t *)mapmempt); mapmempt += 4;
	}
	i = MapRelocate ();
	return i;
}

int MapDecode (unsigned char * mapmempt)
{
	gfxinbitmaps = 0;
	return MapRealDecode (mapmempt);
}

int MapDecodeVRAM (unsigned char * mapmempt)
{
	gfxinbitmaps = 1;
	return MapRealDecode (mapmempt);
}

int MapDecodeABM (unsigned char * mapmempt)
{
	gfxinbitmaps = 2;
	return MapRealDecode (mapmempt);
}

void MapInitAnims (void)
{
ANISTR * myanpt;
	if (mapanimstrpt==NULL) return;
	myanpt = (ANISTR *) mapanimstrendpt; myanpt--;
	while (myanpt->antype!=-1)
	{
		if (myanpt->antype==AN_PPFR) myanpt->antype = AN_PPFF;
		if (myanpt->antype==AN_PPRF) myanpt->antype = AN_PPRR;
		if (myanpt->antype==AN_ONCES) myanpt->antype = AN_ONCE;
		if ((myanpt->antype==AN_LOOPR) || (myanpt->antype==AN_PPRR))
		{
		myanpt->ancuroff = myanpt->anstartoff;
		if ((myanpt->anstartoff)!=(myanpt->anendoff)) myanpt->ancuroff=(myanpt->anendoff)-4;
		} else {
		myanpt->ancuroff = myanpt->anstartoff;
		}
		myanpt->ancount = myanpt->andelay;
		myanpt--;
	}
}

void MapUpdateAnims (void)
{
ANISTR * myanpt;

	if (mapanimstrpt==NULL) return;
	myanpt = (ANISTR *) mapanimstrendpt; myanpt--;
	while (myanpt->antype!=-1)
	{
		if (myanpt->antype!=AN_NONE) { myanpt->ancount--; if (myanpt->ancount<0) {
		myanpt->ancount = myanpt->andelay;
		if (myanpt->antype==AN_LOOPF)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff+=4;
			if (myanpt->ancuroff==myanpt->anendoff) myanpt->ancuroff = myanpt->anstartoff;
		} }
		if (myanpt->antype==AN_LOOPR)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff-=4;
			if (myanpt->ancuroff==((myanpt->anstartoff)-4))
				myanpt->ancuroff = (myanpt->anendoff)-4;
		} }
		if (myanpt->antype==AN_ONCE)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff+=4;
			if (myanpt->ancuroff==myanpt->anendoff) { myanpt->antype = AN_ONCES;
				myanpt->ancuroff = myanpt->anstartoff; }
		} }
		if (myanpt->antype==AN_ONCEH)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) {
			if (myanpt->ancuroff!=((myanpt->anendoff)-4)) myanpt->ancuroff+=4;
		} }
		if (myanpt->antype==AN_PPFF)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff+=4;
			if (myanpt->ancuroff==myanpt->anendoff) { myanpt->ancuroff -= 8;
			myanpt->antype = AN_PPFR;
			if (myanpt->ancuroff<myanpt->anstartoff) myanpt->ancuroff +=4; }
		} } else {
		if (myanpt->antype==AN_PPFR)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff-=4;
			if (myanpt->ancuroff==((myanpt->anstartoff)-4)) { myanpt->ancuroff += 8;
			myanpt->antype = AN_PPFF;
			if (myanpt->ancuroff>myanpt->anendoff) myanpt->ancuroff -=4; }
		} } }
		if (myanpt->antype==AN_PPRR)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff-=4;
			if (myanpt->ancuroff==((myanpt->anstartoff)-4)) { myanpt->ancuroff += 8;
			myanpt->antype = AN_PPRF;
			if (myanpt->ancuroff>myanpt->anendoff) myanpt->ancuroff -=4; }
		} } else {
		if (myanpt->antype==AN_PPRF)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff+=4;
			if (myanpt->ancuroff==myanpt->anendoff) { myanpt->ancuroff -= 8;
			myanpt->antype = AN_PPRR;
			if (myanpt->ancuroff<myanpt->anstartoff) myanpt->ancuroff +=4; }
		} } }
	} } myanpt--; }
}

BITMAP * MapMakeParallaxBitmap (BITMAP * sourcebm, int style)
{
BITMAP * newbm;

	if (mappt == NULL) return NULL;
	if (style < 0 || style > 1) return NULL;

	if (style) newbm = create_video_bitmap (sourcebm->w+mapblockwidth, sourcebm->h+mapblockheight);
	else newbm = create_bitmap (sourcebm->w+mapblockwidth, sourcebm->h+mapblockheight);

	if (newbm == NULL) return NULL;
	blit (sourcebm, newbm, 0, 0, 0, 0, sourcebm->w, sourcebm->h);
	blit (sourcebm, newbm, 0, 0, 0, sourcebm->h, sourcebm->w, mapblockheight);
	blit (newbm, newbm, 0, 0, sourcebm->w, 0, mapblockwidth, sourcebm->h+mapblockheight);
	return newbm;
}

void MapDrawParallax (BITMAP * mapdestpt, BITMAP * parbm, int mapxo, int mapyo, int mapx, int mapy,
	int mapw, int maph)
/* mapdestpt = standard allegro bitmap, MEMORY or VIDEO bitmap.
 * parbm = standard allegro bitmap. MEMORY or VIDEO bitmap.
 * mapxo = offset, in pixels, from the left edge of the map.
 * mapyo = offset, in pixels, from the top edge of the map.
 * mapx  = offset, in pixels, from the left edge of the BITMAP.
 * mapy  = offset, in pixels, from the top edge of the BITMAP.
 * mapw  = width, in pixels, of drawn area.
 * maph  = height, in pixels, of drawn area.
 */
{
int mycl, mycr, myct, mycb;
int i, i2, j;
int paraxo, paraxo2, parayo;
short int * mymappt, * mymappt2;
BLKSTR * blkdatapt;
ANISTR * myanpt;

	mycl = mapdestpt->cl; 
	mycr = mapdestpt->cr; 
	myct = mapdestpt->ct; 
	mycb = mapdestpt->cb;
	set_clip (mapdestpt, mapx, mapy, mapx+mapw-1, mapy+maph-1);

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);

	paraxo = ((mapxo-(mapxo%mapblockwidth))%(parbm->w-mapblockwidth))-((mapxo/2)%(parbm->w-mapblockwidth));
	parayo = ((mapyo-(mapyo%mapblockheight))%(parbm->h-mapblockheight))-((mapyo/2)%(parbm->h-mapblockheight));
	while (paraxo < 0) paraxo += (parbm->w-mapblockwidth);
	while (parayo < 0) parayo += (parbm->h-mapblockheight);

	i = mapx-(mapxo%mapblockwidth);
	j = mapy-(mapyo%mapblockheight);

	i2 = i; paraxo2 = paraxo; mymappt2 = mymappt;
	while (j < (mapy+maph)) {
		while (i < (mapx+mapw)) {
			if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
				blkdatapt = ANBLK(myanpt); }
			if (blkdatapt->trigger)
				blit (parbm, mapdestpt, paraxo, parayo, i, j, mapblockwidth, mapblockheight);
			paraxo += mapblockwidth;
			if (paraxo >= (parbm->w-mapblockwidth)) paraxo -= (parbm->w-mapblockwidth);
			i += mapblockwidth; mymappt++;
		}
		parayo += mapblockheight;
		if (parayo >= (parbm->h-mapblockheight)) parayo -= (parbm->h-mapblockheight);
		i = i2; paraxo = paraxo2; mymappt2 += mapwidth; mymappt = mymappt2;
		j += mapblockheight;
	}
	set_clip (mapdestpt, mycl, myct, mycr+1, mycb+1);
}

#ifndef RBNOCUSTOMRENDER
void MapDraw8BG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
	int mapw, int maph)
/* mapdestpt = standard allegro MEMORY bitmap, won't work on SCREEN bitmaps.
 * mapxo = offset, in pixels, from the left edge of the map.
 * mapyo = offset, in pixels, from the top edge of the map.
 * mapx  = offset, in pixels, from the left edge of the BITMAP.
 * mapy  = offset, in pixels, from the top edge of the BITMAP.
 * mapw  = width, in pixels, of drawn area.
 * maph  = height, in pixels, of drawn area.
 */
{
int i, j, k, l, mapvclip, maphclip, mapxblks, mapx2blks, mapyblks;
short int *mymappt;
short int *mymap2pt;
unsigned char * maplinecpypt;
/* short int *mymap2pt;
unsigned char * srcpt;
*/ unsigned char * mapgfxpt;
BLKSTR *blkdatapt;
ANISTR *myanpt;

/* first, draw blocks that are clipped horizontally (left) */
	maphclip = mapxo%mapblockwidth;	/* Number of pixels to clip from left */
	mapvclip = mapyo%mapblockheight;	/* Number of pixels to clip from top */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j=mapy-mapvclip; i=mapx-maphclip; while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned char *) GFXBG(blkdatapt->bgoff);

	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=maphclip;k<mapblockwidth;k++) {
		mapdestpt->line[(l+j)][(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; }
	j += mapblockheight; mymappt += mapwidth; }

/* now, draw blocks that are clipped horizontally (right) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j = mapy-mapvclip; i = mapx-maphclip; mapxblks = -1;
	while (i<((mapx+mapw)-mapblockwidth)) { mapxblks++; mymappt++; i +=mapblockwidth; }
	while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned char *) GFXBG(blkdatapt->bgoff);

	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=0;k<mapblockwidth;k++) {
		if ((k+i)>=(mapx+mapw)) break;
		mapdestpt->line[(l+j)][(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; }
	j += mapblockheight; mymappt += mapwidth; }

/* now draw blocks that are clipped vertically (top) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy; i = mapx-maphclip+mapblockwidth;
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned char *) GFXBG(blkdatapt->bgoff);
	mapgfxpt += mapblockwidth*mapvclip;

	for (l=0;l<(mapblockheight-mapvclip);l++) { 
		maplinecpypt = (unsigned char *) mapdestpt->line[(l+j)] + i;
		memcpy (maplinecpypt, mapgfxpt, mapblockwidth); mapgfxpt += mapblockwidth; }
	i += mapblockwidth; mymappt++; }

/* now draw blocks that are clipped vertically (bottom) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy-mapvclip; i = mapx-maphclip+mapblockwidth; mapyblks = -1;
	while (j<((mapy+maph)-mapblockheight))
		{ mapyblks++; mymappt += mapwidth; j += mapblockheight; }
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned char *) GFXBG(blkdatapt->bgoff);

	for (l=0;(l+j)<(mapy+maph);l++) { 
		maplinecpypt = (unsigned char *) mapdestpt->line[(l+j)] + i;
		memcpy (maplinecpypt, mapgfxpt, mapblockwidth); mapgfxpt += mapblockwidth; }
	i += mapblockwidth; mymappt++; }

/* now draw all unclipped blocks (should be pretty quick) */
/* ANY OPTIMISATION SHOULD BE DONE HERE! (time intensive) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+(((mapyo/mapblockheight)+1)*mapwidth)+1;

	mapx2blks = mapxblks;
	j = mapy-mapvclip+mapblockheight; while (mapyblks)
	{ mymap2pt = mymappt; i = mapx-maphclip+mapblockwidth; mapxblks = mapx2blks;
	while (mapxblks)
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned char *) GFXBG(blkdatapt->bgoff);

	for (l=0;l<mapblockheight;l++) {
		maplinecpypt = (unsigned char *) mapdestpt->line[(l+j)] + i;
		memcpy (maplinecpypt, mapgfxpt, mapblockwidth); mapgfxpt += mapblockwidth; }
	i += mapblockwidth; mymappt++; mapxblks--; }
	j += mapblockheight; mymappt = mymap2pt+mapwidth; mapyblks--; }
}

void MapDraw8BGT (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
	int mapw, int maph)
/* same as MapDraw8BG, except colour 0 pixels are transparent.
 * mapdestpt = standard allegro MEMORY bitmap, won't work on SCREEN bitmaps.
 * mapxo = offset, in pixels, from the left edge of the map.
 * mapyo = offset, in pixels, from the top edge of the map.
 * mapx  = offset, in pixels, from the left edge of the BITMAP.
 * mapy  = offset, in pixels, from the top edge of the BITMAP.
 * mapw  = width, in pixels, of drawn area.
 * maph  = height, in pixels, of drawn area.
 */
{
int i, j, k, l, mapvclip, maphclip, mapxblks, mapx2blks, mapyblks;
short int *mymappt;
short int *mymap2pt;
unsigned char * maplinecpypt;
/* short int *mymap2pt;
unsigned char * srcpt;
*/ unsigned char * mapgfxpt;
BLKSTR *blkdatapt;
ANISTR *myanpt;

/* first, draw blocks that are clipped horizontally (left) */
	maphclip = mapxo%mapblockwidth;	/* Number of pixels to clip from left */
	mapvclip = mapyo%mapblockheight;	/* Number of pixels to clip from top */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j=mapy-mapvclip; i=mapx-maphclip; while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned char *) GFXBG(blkdatapt->bgoff);

	if (mapgfxpt!= (unsigned char *) mapblockgfxpt) { 
	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=maphclip;k<mapblockwidth;k++) {
		if (mapgfxpt[k]) mapdestpt->line[(l+j)][(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; } }
	j += mapblockheight; mymappt += mapwidth; }

/* now, draw blocks that are clipped horizontally (right) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j = mapy-mapvclip; i = mapx-maphclip; mapxblks = -1;
	while (i<((mapx+mapw)-mapblockwidth)) { mapxblks++; mymappt++; i +=mapblockwidth; }
	while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned char *) GFXBG(blkdatapt->bgoff);

	if (mapgfxpt!= (unsigned char *) mapblockgfxpt) { 
	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=0;k<mapblockwidth;k++) {
		if ((k+i)>=(mapx+mapw)) break;
		if (mapgfxpt[k]) mapdestpt->line[(l+j)][(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; } }
	j += mapblockheight; mymappt += mapwidth; }

/* now draw blocks that are clipped vertically (top) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy; i = mapx-maphclip+mapblockwidth;
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned char *) GFXBG(blkdatapt->bgoff);
	mapgfxpt += mapblockwidth*mapvclip;

	if (mapgfxpt!= (unsigned char *) mapblockgfxpt) { 
	for (l=0;l<(mapblockheight-mapvclip);l++) { 
		maplinecpypt = (unsigned char *) mapdestpt->line[(l+j)] + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt) *maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; }

/* now draw blocks that are clipped vertically (bottom) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy-mapvclip; i = mapx-maphclip+mapblockwidth; mapyblks = -1;
	while (j<((mapy+maph)-mapblockheight))
		{ mapyblks++; mymappt += mapwidth; j += mapblockheight; }
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned char *) GFXBG(blkdatapt->bgoff);

	if (mapgfxpt!= (unsigned char *) mapblockgfxpt) { 
	for (l=0;(l+j)<(mapy+maph);l++) { 
		maplinecpypt = (unsigned char *) mapdestpt->line[(l+j)] + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt) *maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; }

/* now draw all unclipped blocks (should be pretty quick) */
/* ANY OPTIMISATION SHOULD BE DONE HERE! (time intensive) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+(((mapyo/mapblockheight)+1)*mapwidth)+1;

	mapx2blks = mapxblks;
	j = mapy-mapvclip+mapblockheight; while (mapyblks)
	{ mymap2pt = mymappt; i = mapx-maphclip+mapblockwidth; mapxblks = mapx2blks;
	while (mapxblks)
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned char *) GFXBG(blkdatapt->bgoff);

	if (mapgfxpt!= (unsigned char *) mapblockgfxpt) { for (l=0;l<mapblockheight;l++) {
		maplinecpypt = (unsigned char *) mapdestpt->line[(l+j)] + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt) *maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; mapxblks--; }
	j += mapblockheight; mymappt = mymap2pt+mapwidth; mapyblks--; }
}

void MapDraw8FG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
	int mapw, int maph, int mapfg)
/* mapdestpt = standard allegro MEMORY bitmap, won't work on SCREEN bitmaps.
 * mapxo = offset, in pixels, from the left edge of the map.
 * mapyo = offset, in pixels, from the top edge of the map.
 * mapx  = offset, in pixels, from the left edge of the BITMAP.
 * mapy  = offset, in pixels, from the top edge of the BITMAP.
 * mapw  = width, in pixels, of drawn area.
 * maph  = height, in pixels, of drawn area.
 * mapfg  = foreground number, 0, 1, or 2 */
{
int i, j, k, l, mapvclip, maphclip, mapxblks, mapx2blks, mapyblks;
short int *mymappt;
short int *mymap2pt;
unsigned char * maplinecpypt;
/* short int *mymap2pt;
unsigned char * srcpt;
*/ unsigned char * mapgfxpt;
BLKSTR *blkdatapt;
ANISTR *myanpt;

	if ((mapfg<0) || (mapfg>2)) return;
/* first, draw blocks that are clipped horizontally (left) */
	maphclip = mapxo%mapblockwidth;	/* Number of pixels to clip from left */
	mapvclip = mapyo%mapblockheight;	/* Number of pixels to clip from top */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j=mapy-mapvclip; i=mapx-maphclip; while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=maphclip;k<mapblockwidth;k++) {
		if (mapgfxpt[k]) mapdestpt->line[(l+j)][(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; } }
	j += mapblockheight; mymappt += mapwidth; }

/* now, draw blocks that are clipped horizontally (right) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j = mapy-mapvclip; i = mapx-maphclip; mapxblks = -1;
	while (i<((mapx+mapw)-mapblockwidth)) { mapxblks++; mymappt++; i +=mapblockwidth; }
	while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=0;k<mapblockwidth;k++) {
		if ((k+i)>=(mapx+mapw)) break;
		if (mapgfxpt[k]) mapdestpt->line[(l+j)][(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; } }
	j += mapblockheight; mymappt += mapwidth; }

/* now draw blocks that are clipped vertically (top) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy; i = mapx-maphclip+mapblockwidth;
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	mapgfxpt += mapblockwidth*mapvclip;
	for (l=0;l<(mapblockheight-mapvclip);l++) { 
		maplinecpypt = (unsigned char *) mapdestpt->line[(l+j)] + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt) *maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; }

/* now draw blocks that are clipped vertically (bottom) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy-mapvclip; i = mapx-maphclip+mapblockwidth; mapyblks = -1;
	while (j<((mapy+maph)-mapblockheight))
		{ mapyblks++; mymappt += mapwidth; j += mapblockheight; }
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	for (l=0;(l+j)<(mapy+maph);l++) { 
		maplinecpypt = (unsigned char *) mapdestpt->line[(l+j)] + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt) *maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; }

/* now draw all unclipped blocks (should be pretty quick) */
/* ANY OPTIMISATION SHOULD BE DONE HERE! (time intensive) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+(((mapyo/mapblockheight)+1)*mapwidth)+1;

	mapx2blks = mapxblks;
	j = mapy-mapvclip+mapblockheight; while (mapyblks)
	{ mymap2pt = mymappt; i = mapx-maphclip+mapblockwidth; mapxblks = mapx2blks;
	while (mapxblks)
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (unsigned char *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	for (l=0;l<mapblockheight;l++) {
		maplinecpypt = (unsigned char *) mapdestpt->line[(l+j)] + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt) *maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; mapxblks--; }
	j += mapblockheight; mymappt = mymap2pt+mapwidth; mapyblks--; }
}

void MapDraw16BG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
	int mapw, int maph)
/* mapdestpt = standard allegro MEMORY bitmap, won't work on SCREEN bitmaps.
 * mapxo = offset, in pixels, from the left edge of the map.
 * mapyo = offset, in pixels, from the top edge of the map.
 * mapx  = offset, in pixels, from the left edge of the BITMAP.
 * mapy  = offset, in pixels, from the top edge of the BITMAP.
 * mapw  = width, in pixels, of drawn area.
 * maph  = height, in pixels, of drawn area.
 */
{
int i, j, k, l, mapvclip, maphclip, mapxblks, mapx2blks, mapyblks;
short int *mymappt;
short int *mymap2pt;
unsigned short int * maplinecpypt;
/* short int *mymap2pt;
unsigned char * srcpt;
*/ unsigned short int * mapgfxpt;
BLKSTR *blkdatapt;
ANISTR *myanpt;

/* first, draw blocks that are clipped horizontally (left) */
	maphclip = mapxo%mapblockwidth;	/* Number of pixels to clip from left */
	mapvclip = mapyo%mapblockheight;	/* Number of pixels to clip from top */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j=mapy-mapvclip; i=mapx-maphclip; while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned short int *) GFXBG(blkdatapt->bgoff);

	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=maphclip;k<mapblockwidth;k++) {
		((unsigned short int *)mapdestpt->line[(l+j)])[(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; }
	j += mapblockheight; mymappt += mapwidth; }

/* now, draw blocks that are clipped horizontally (right) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j = mapy-mapvclip; i = mapx-maphclip; mapxblks = -1;
	while (i<((mapx+mapw)-mapblockwidth)) { mapxblks++; mymappt++; i +=mapblockwidth; }
	while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned short int *) GFXBG(blkdatapt->bgoff);

	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=0;k<mapblockwidth;k++) {
		if ((k+i)>=(mapx+mapw)) break;
		((unsigned short int *)mapdestpt->line[(l+j)])[(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; }
	j += mapblockheight; mymappt += mapwidth; }

/* now draw blocks that are clipped vertically (top) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy; i = mapx-maphclip+mapblockwidth;
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned short int *) GFXBG(blkdatapt->bgoff);
	mapgfxpt += mapblockwidth*mapvclip;

	for (l=0;l<(mapblockheight-mapvclip);l++) { 
		maplinecpypt = ((unsigned short int *) mapdestpt->line[(l+j)]) + i;
		memcpy (maplinecpypt, mapgfxpt, mapblockwidth*2); mapgfxpt += mapblockwidth; }
	i += mapblockwidth; mymappt++; }

/* now draw blocks that are clipped vertically (bottom) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy-mapvclip; i = mapx-maphclip+mapblockwidth; mapyblks = -1;
	while (j<((mapy+maph)-mapblockheight))
		{ mapyblks++; mymappt += mapwidth; j += mapblockheight; }
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned short int *) GFXBG(blkdatapt->bgoff);

	for (l=0;(l+j)<(mapy+maph);l++) { 
		maplinecpypt = ((unsigned short int *) mapdestpt->line[(l+j)]) + i;
		memcpy (maplinecpypt, mapgfxpt, mapblockwidth*2); mapgfxpt += mapblockwidth; }
	i += mapblockwidth; mymappt++; }

/* now draw all unclipped blocks (should be pretty quick) */
/* ANY OPTIMISATION SHOULD BE DONE HERE! (time intensive) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+(((mapyo/mapblockheight)+1)*mapwidth)+1;

	mapx2blks = mapxblks;
	j = mapy-mapvclip+mapblockheight; while (mapyblks)
	{ mymap2pt = mymappt; i = mapx-maphclip+mapblockwidth; mapxblks = mapx2blks;
	while (mapxblks)
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned short int *) GFXBG(blkdatapt->bgoff);

	for (l=0;l<mapblockheight;l++) {
		maplinecpypt = ((unsigned short int *) mapdestpt->line[(l+j)]) + i;
		memcpy (maplinecpypt, mapgfxpt, mapblockwidth*2); mapgfxpt += mapblockwidth; }
	i += mapblockwidth; mymappt++; mapxblks--; }
	j += mapblockheight; mymappt = mymap2pt+mapwidth; mapyblks--; }
}

void MapDraw16BGT (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
	int mapw, int maph)
/* same as MapDraw16BG, except PINK is transparent
 * mapdestpt = standard allegro MEMORY bitmap, won't work on SCREEN bitmaps.
 * mapxo = offset, in pixels, from the left edge of the map.
 * mapyo = offset, in pixels, from the top edge of the map.
 * mapx  = offset, in pixels, from the left edge of the BITMAP.
 * mapy  = offset, in pixels, from the top edge of the BITMAP.
 * mapw  = width, in pixels, of drawn area.
 * maph  = height, in pixels, of drawn area.
 */
{
int i, j, k, l, mapvclip, maphclip, mapxblks, mapx2blks, mapyblks;
short int *mymappt;
short int *mymap2pt;
unsigned short int * maplinecpypt;
/* short int *mymap2pt;
unsigned char * srcpt;
*/ unsigned short int * mapgfxpt;
unsigned short int mapmymaskcol;
BLKSTR *blkdatapt;
ANISTR *myanpt;

/* first, draw blocks that are clipped horizontally (left) */
	maphclip = mapxo%mapblockwidth;	/* Number of pixels to clip from left */
	mapvclip = mapyo%mapblockheight;	/* Number of pixels to clip from top */
	mapmymaskcol = (unsigned short int) makecol (255, 0, 255);

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j=mapy-mapvclip; i=mapx-maphclip; while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned short int *) GFXBG(blkdatapt->bgoff);

	if (mapgfxpt!= (unsigned short int *) mapblockgfxpt) { 
	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=maphclip;k<mapblockwidth;k++) {
		if (mapgfxpt[k]!=mapmymaskcol)
		((unsigned short int *)mapdestpt->line[(l+j)])[(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; } }
	j += mapblockheight; mymappt += mapwidth; }

/* now, draw blocks that are clipped horizontally (right) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j = mapy-mapvclip; i = mapx-maphclip; mapxblks = -1;
	while (i<((mapx+mapw)-mapblockwidth)) { mapxblks++; mymappt++; i +=mapblockwidth; }
	while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned short int *) GFXBG(blkdatapt->bgoff);

	if (mapgfxpt!= (unsigned short int *) mapblockgfxpt) { 
	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=0;k<mapblockwidth;k++) {
		if ((k+i)>=(mapx+mapw)) break;
		if (mapgfxpt[k]!=mapmymaskcol)
		((unsigned short int *)mapdestpt->line[(l+j)])[(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; } }
	j += mapblockheight; mymappt += mapwidth; }

/* now draw blocks that are clipped vertically (top) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy; i = mapx-maphclip+mapblockwidth;
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned short int *) GFXBG(blkdatapt->bgoff);
	mapgfxpt += mapblockwidth*mapvclip;

	if (mapgfxpt!= (unsigned short int *) mapblockgfxpt) { 
	for (l=0;l<(mapblockheight-mapvclip);l++) { 
		maplinecpypt = ((unsigned short int *) mapdestpt->line[(l+j)]) + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt!=mapmymaskcol)
		*maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; }

/* now draw blocks that are clipped vertically (bottom) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy-mapvclip; i = mapx-maphclip+mapblockwidth; mapyblks = -1;
	while (j<((mapy+maph)-mapblockheight))
		{ mapyblks++; mymappt += mapwidth; j += mapblockheight; }
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned short int *) GFXBG(blkdatapt->bgoff);

	if (mapgfxpt!= (unsigned short int *) mapblockgfxpt) { 
	for (l=0;(l+j)<(mapy+maph);l++) { 
		maplinecpypt = ((unsigned short int *) mapdestpt->line[(l+j)]) + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt!=mapmymaskcol)
		*maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; }

/* now draw all unclipped blocks (should be pretty quick) */
/* ANY OPTIMISATION SHOULD BE DONE HERE! (time intensive) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+(((mapyo/mapblockheight)+1)*mapwidth)+1;

	mapx2blks = mapxblks;
	j = mapy-mapvclip+mapblockheight; while (mapyblks)
	{ mymap2pt = mymappt; i = mapx-maphclip+mapblockwidth; mapxblks = mapx2blks;
	while (mapxblks)
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (unsigned short int *) GFXBG(blkdatapt->bgoff);

	if (mapgfxpt!= (unsigned short int *) mapblockgfxpt) { 
	for (l=0;l<mapblockheight;l++) {
		maplinecpypt = ((unsigned short int *) mapdestpt->line[(l+j)]) + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt!=mapmymaskcol)
			*maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; mapxblks--; }
	j += mapblockheight; mymappt = mymap2pt+mapwidth; mapyblks--; }
}

void MapDraw16FG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
	int mapw, int maph, int mapfg)
/* mapdestpt = standard allegro MEMORY bitmap, won't work on SCREEN bitmaps.
 * mapxo = offset, in pixels, from the left edge of the map.
 * mapyo = offset, in pixels, from the top edge of the map.
 * mapx  = offset, in pixels, from the left edge of the BITMAP.
 * mapy  = offset, in pixels, from the top edge of the BITMAP.
 * mapw  = width, in pixels, of drawn area.
 * maph  = height, in pixels, of drawn area.
 * mapfg  = foreground number, 0, 1, or 2 */
{
int i, j, k, l, mapvclip, maphclip, mapxblks, mapx2blks, mapyblks;
short int *mymappt;
short int *mymap2pt;
unsigned short int * maplinecpypt;
/* short int *mymap2pt;
unsigned char * srcpt;
*/ unsigned short int * mapgfxpt;
unsigned short int mapmymaskcol;
BLKSTR *blkdatapt;
ANISTR *myanpt;

	if ((mapfg<0) || (mapfg>2)) return;
/* first, draw blocks that are clipped horizontally (left) */
	maphclip = mapxo%mapblockwidth;	/* Number of pixels to clip from left */
	mapvclip = mapyo%mapblockheight;	/* Number of pixels to clip from top */
	mapmymaskcol = (unsigned short int) makecol (255, 0, 255);

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j=mapy-mapvclip; i=mapx-maphclip; while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=maphclip;k<mapblockwidth;k++) {
		if (mapgfxpt[k] != mapmymaskcol) 
		((unsigned short int *)mapdestpt->line[(l+j)])[(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; } }
	j += mapblockheight; mymappt += mapwidth; }

/* now, draw blocks that are clipped horizontally (right) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j = mapy-mapvclip; i = mapx-maphclip; mapxblks = -1;
	while (i<((mapx+mapw)-mapblockwidth)) { mapxblks++; mymappt++; i +=mapblockwidth; }
	while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=0;k<mapblockwidth;k++) {
		if ((k+i)>=(mapx+mapw)) break;
		if (mapgfxpt[k] != mapmymaskcol)
		((unsigned short int *)mapdestpt->line[(l+j)])[(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; } }
	j += mapblockheight; mymappt += mapwidth; }

/* now draw blocks that are clipped vertically (top) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy; i = mapx-maphclip+mapblockwidth;
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	mapgfxpt += mapblockwidth*mapvclip;
	for (l=0;l<(mapblockheight-mapvclip);l++) { 
		maplinecpypt = ((unsigned short int *) mapdestpt->line[(l+j)]) + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt != mapmymaskcol)
			*maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; }

/* now draw blocks that are clipped vertically (bottom) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy-mapvclip; i = mapx-maphclip+mapblockwidth; mapyblks = -1;
	while (j<((mapy+maph)-mapblockheight))
		{ mapyblks++; mymappt += mapwidth; j += mapblockheight; }
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	for (l=0;(l+j)<(mapy+maph);l++) { 
		maplinecpypt = ((unsigned short int *) mapdestpt->line[(l+j)]) + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt != mapmymaskcol)
			*maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; }

/* now draw all unclipped blocks (should be pretty quick) */
/* ANY OPTIMISATION SHOULD BE DONE HERE! (time intensive) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+(((mapyo/mapblockheight)+1)*mapwidth)+1;

	mapx2blks = mapxblks;
	j = mapy-mapvclip+mapblockheight; while (mapyblks)
	{ mymap2pt = mymappt; i = mapx-maphclip+mapblockwidth; mapxblks = mapx2blks;
	while (mapxblks)
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (unsigned short int *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	for (l=0;l<mapblockheight;l++) {
		maplinecpypt = ((unsigned short int *) mapdestpt->line[(l+j)]) + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt != mapmymaskcol)
			*maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; mapxblks--; }
	j += mapblockheight; mymappt = mymap2pt+mapwidth; mapyblks--; }
}

void MapDraw32BG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
	int mapw, int maph)
/* mapdestpt = standard allegro MEMORY bitmap, won't work on SCREEN bitmaps.
 * mapxo = offset, in pixels, from the left edge of the map.
 * mapyo = offset, in pixels, from the top edge of the map.
 * mapx  = offset, in pixels, from the left edge of the BITMAP.
 * mapy  = offset, in pixels, from the top edge of the BITMAP.
 * mapw  = width, in pixels, of drawn area.
 * maph  = height, in pixels, of drawn area.
 */
{
int i, j, k, l, mapvclip, maphclip, mapxblks, mapx2blks, mapyblks;
short int *mymappt;
short int *mymap2pt;
uint32_t * maplinecpypt;
/* short int *mymap2pt;
unsigned char * srcpt;
*/ uint32_t * mapgfxpt;
BLKSTR *blkdatapt;
ANISTR *myanpt;

/* first, draw blocks that are clipped horizontally (left) */
	maphclip = mapxo%mapblockwidth;	/* Number of pixels to clip from left */
	mapvclip = mapyo%mapblockheight;	/* Number of pixels to clip from top */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j=mapy-mapvclip; i=mapx-maphclip; while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (uint32_t *) GFXBG(blkdatapt->bgoff);

	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=maphclip;k<mapblockwidth;k++) {
		((uint32_t *)mapdestpt->line[(l+j)])[(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; }
	j += mapblockheight; mymappt += mapwidth; }

/* now, draw blocks that are clipped horizontally (right) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j = mapy-mapvclip; i = mapx-maphclip; mapxblks = -1;
	while (i<((mapx+mapw)-mapblockwidth)) { mapxblks++; mymappt++; i +=mapblockwidth; }
	while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (uint32_t *) GFXBG(blkdatapt->bgoff);

	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=0;k<mapblockwidth;k++) {
		if ((k+i)>=(mapx+mapw)) break;
		((uint32_t *)mapdestpt->line[(l+j)])[(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; }
	j += mapblockheight; mymappt += mapwidth; }

/* now draw blocks that are clipped vertically (top) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy; i = mapx-maphclip+mapblockwidth;
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (uint32_t *) GFXBG(blkdatapt->bgoff);
	mapgfxpt += mapblockwidth*mapvclip;

	for (l=0;l<(mapblockheight-mapvclip);l++) { 
		maplinecpypt = ((uint32_t *) mapdestpt->line[(l+j)]) + i;
		memcpy (maplinecpypt, mapgfxpt, mapblockwidth*4); mapgfxpt += mapblockwidth; }
	i += mapblockwidth; mymappt++; }

/* now draw blocks that are clipped vertically (bottom) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy-mapvclip; i = mapx-maphclip+mapblockwidth; mapyblks = -1;
	while (j<((mapy+maph)-mapblockheight))
		{ mapyblks++; mymappt += mapwidth; j += mapblockheight; }
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (uint32_t *) GFXBG(blkdatapt->bgoff);

	for (l=0;(l+j)<(mapy+maph);l++) { 
		maplinecpypt = ((uint32_t *) mapdestpt->line[(l+j)]) + i;
		memcpy (maplinecpypt, mapgfxpt, mapblockwidth*4); mapgfxpt += mapblockwidth; }
	i += mapblockwidth; mymappt++; }

/* now draw all unclipped blocks (should be pretty quick) */
/* ANY OPTIMISATION SHOULD BE DONE HERE! (time intensive) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+(((mapyo/mapblockheight)+1)*mapwidth)+1;

	mapx2blks = mapxblks;
	j = mapy-mapvclip+mapblockheight; while (mapyblks)
	{ mymap2pt = mymappt; i = mapx-maphclip+mapblockwidth; mapxblks = mapx2blks;
	while (mapxblks)
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (uint32_t *) GFXBG(blkdatapt->bgoff);

	for (l=0;l<mapblockheight;l++) {
		maplinecpypt = ((uint32_t *) mapdestpt->line[(l+j)]) + i;
		memcpy (maplinecpypt, mapgfxpt, mapblockwidth*4); mapgfxpt += mapblockwidth; }
	i += mapblockwidth; mymappt++; mapxblks--; }
	j += mapblockheight; mymappt = mymap2pt+mapwidth; mapyblks--; }
}

void MapDraw32BGT (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
	int mapw, int maph)
/* Same as MapDraw32BGT except PINK is transparent
 * mapdestpt = standard allegro MEMORY bitmap, won't work on SCREEN bitmaps.
 * mapxo = offset, in pixels, from the left edge of the map.
 * mapyo = offset, in pixels, from the top edge of the map.
 * mapx  = offset, in pixels, from the left edge of the BITMAP.
 * mapy  = offset, in pixels, from the top edge of the BITMAP.
 * mapw  = width, in pixels, of drawn area.
 * maph  = height, in pixels, of drawn area.
 */
{
int i, j, k, l, mapvclip, maphclip, mapxblks, mapx2blks, mapyblks;
short int *mymappt;
short int *mymap2pt;
uint32_t * maplinecpypt;
/* short int *mymap2pt;
unsigned char * srcpt;
*/ uint32_t * mapgfxpt;
BLKSTR *blkdatapt;
ANISTR *myanpt;

/* first, draw blocks that are clipped horizontally (left) */
	maphclip = mapxo%mapblockwidth;	/* Number of pixels to clip from left */
	mapvclip = mapyo%mapblockheight;	/* Number of pixels to clip from top */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j=mapy-mapvclip; i=mapx-maphclip; while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (uint32_t *) GFXBG(blkdatapt->bgoff);

	if (mapgfxpt != (uint32_t *) mapblockgfxpt) {
	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=maphclip;k<mapblockwidth;k++) {
		if (mapgfxpt[k] == 0xFF00FF)
		((uint32_t *)mapdestpt->line[(l+j)])[(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; } }
	j += mapblockheight; mymappt += mapwidth; }

/* now, draw blocks that are clipped horizontally (right) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j = mapy-mapvclip; i = mapx-maphclip; mapxblks = -1;
	while (i<((mapx+mapw)-mapblockwidth)) { mapxblks++; mymappt++; i +=mapblockwidth; }
	while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (uint32_t *) GFXBG(blkdatapt->bgoff);

	if (mapgfxpt != (uint32_t *) mapblockgfxpt) {
	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=0;k<mapblockwidth;k++) {
		if ((k+i)>=(mapx+mapw)) break;
		if (mapgfxpt[k] == 0xFF00FF)
		((uint32_t *)mapdestpt->line[(l+j)])[(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; } }
	j += mapblockheight; mymappt += mapwidth; }

/* now draw blocks that are clipped vertically (top) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy; i = mapx-maphclip+mapblockwidth;
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (uint32_t *) GFXBG(blkdatapt->bgoff);
	mapgfxpt += mapblockwidth*mapvclip;

	if (mapgfxpt != (uint32_t *) mapblockgfxpt) {
	for (l=0;l<(mapblockheight-mapvclip);l++) { 
		maplinecpypt = ((uint32_t *) mapdestpt->line[(l+j)]) + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*maplinecpypt == 0xFF00FF)
		*maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; }

/* now draw blocks that are clipped vertically (bottom) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy-mapvclip; i = mapx-maphclip+mapblockwidth; mapyblks = -1;
	while (j<((mapy+maph)-mapblockheight))
		{ mapyblks++; mymappt += mapwidth; j += mapblockheight; }
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (uint32_t *) GFXBG(blkdatapt->bgoff);

	if (mapgfxpt != (uint32_t *) mapblockgfxpt) {
	for (l=0;(l+j)<(mapy+maph);l++) { 
		maplinecpypt = ((uint32_t *) mapdestpt->line[(l+j)]) + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*maplinecpypt == 0xFF00FF)
		*maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; }

/* now draw all unclipped blocks (should be pretty quick) */
/* ANY OPTIMISATION SHOULD BE DONE HERE! (time intensive) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+(((mapyo/mapblockheight)+1)*mapwidth)+1;

	mapx2blks = mapxblks;
	j = mapy-mapvclip+mapblockheight; while (mapyblks)
	{ mymap2pt = mymappt; i = mapx-maphclip+mapblockwidth; mapxblks = mapx2blks;
	while (mapxblks)
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	mapgfxpt = (uint32_t *) GFXBG(blkdatapt->bgoff);

	if (mapgfxpt != (uint32_t *) mapblockgfxpt) {
	for (l=0;l<mapblockheight;l++) {
		maplinecpypt = ((uint32_t *) mapdestpt->line[(l+j)]) + i;
		for (k=0;k<mapblockwidth;k++) {
			if (*maplinecpypt == 0xFF00FF)
			*maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; mapxblks--; }
	j += mapblockheight; mymappt = mymap2pt+mapwidth; mapyblks--; }
}

void MapDraw32FG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
	int mapw, int maph, int mapfg)
/* mapdestpt = standard allegro MEMORY bitmap, won't work on SCREEN bitmaps.
 * mapxo = offset, in pixels, from the left edge of the map.
 * mapyo = offset, in pixels, from the top edge of the map.
 * mapx  = offset, in pixels, from the left edge of the BITMAP.
 * mapy  = offset, in pixels, from the top edge of the BITMAP.
 * mapw  = width, in pixels, of drawn area.
 * maph  = height, in pixels, of drawn area.
 * mapfg  = foreground number, 0, 1, or 2 */
{
int i, j, k, l, mapvclip, maphclip, mapxblks, mapx2blks, mapyblks;
short int *mymappt;
short int *mymap2pt;
uint32_t * maplinecpypt;
/* short int *mymap2pt;
unsigned char * srcpt;
*/ uint32_t * mapgfxpt;
BLKSTR *blkdatapt;
ANISTR *myanpt;

	if ((mapfg<0) || (mapfg>2)) return;
/* first, draw blocks that are clipped horizontally (left) */
	maphclip = mapxo%mapblockwidth;	/* Number of pixels to clip from left */
	mapvclip = mapyo%mapblockheight;	/* Number of pixels to clip from top */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j=mapy-mapvclip; i=mapx-maphclip; while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=maphclip;k<mapblockwidth;k++) {
		if (mapgfxpt[k] != 0xFF00FF) 
		((uint32_t *)mapdestpt->line[(l+j)])[(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; } }
	j += mapblockheight; mymappt += mapwidth; }

/* now, draw blocks that are clipped horizontally (right) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
	j = mapy-mapvclip; i = mapx-maphclip; mapxblks = -1;
	while (i<((mapx+mapw)-mapblockwidth)) { mapxblks++; mymappt++; i +=mapblockwidth; }
	while (j<(mapy+maph))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	for (l=0;l<mapblockheight;l++) { if (((l+j)>=mapy) && ((l+j)<(mapy+maph))) {
		for (k=0;k<mapblockwidth;k++) {
		if ((k+i)>=(mapx+mapw)) break;
		if (mapgfxpt[k] != 0xFF00FF)
		((uint32_t *)mapdestpt->line[(l+j)])[(k+i)] = mapgfxpt[k]; } }
		mapgfxpt += mapblockwidth; } }
	j += mapblockheight; mymappt += mapwidth; }

/* now draw blocks that are clipped vertically (top) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy; i = mapx-maphclip+mapblockwidth;
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	mapgfxpt += mapblockwidth*mapvclip;
	for (l=0;l<(mapblockheight-mapvclip);l++) { 
		maplinecpypt = ((uint32_t *) mapdestpt->line[(l+j)]) + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt != 0xFF00FF)
			*maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; }

/* now draw blocks that are clipped vertically (bottom) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth)+1;
	j = mapy-mapvclip; i = mapx-maphclip+mapblockwidth; mapyblks = -1;
	while (j<((mapy+maph)-mapblockheight))
		{ mapyblks++; mymappt += mapwidth; j += mapblockheight; }
	while (i<(mapx+mapw-mapblockwidth))
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	for (l=0;(l+j)<(mapy+maph);l++) { 
		maplinecpypt = ((uint32_t *) mapdestpt->line[(l+j)]) + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt != 0xFF00FF)
			*maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; }

/* now draw all unclipped blocks (should be pretty quick) */
/* ANY OPTIMISATION SHOULD BE DONE HERE! (time intensive) */

	mymappt = (short int *) mappt;
	mymappt += (mapxo/mapblockwidth)+(((mapyo/mapblockheight)+1)*mapwidth)+1;

	mapx2blks = mapxblks;
	j = mapy-mapvclip+mapblockheight; while (mapyblks)
	{ mymap2pt = mymappt; i = mapx-maphclip+mapblockwidth; mapxblks = mapx2blks;
	while (mapxblks)
	{
	if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
	blkdatapt = ANBLK(myanpt); }
	if (!mapfg) mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff);
	else if (mapfg == 1) mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff2);
	else mapgfxpt = (uint32_t *) GFXFG(blkdatapt->fgoff3);

	if (mapgfxpt != NULL) {
	for (l=0;l<mapblockheight;l++) {
		maplinecpypt = ((uint32_t *) mapdestpt->line[(l+j)]) + i;
		for (k=0;k<mapblockwidth;k++) {
		if (*mapgfxpt != 0xFF00FF)
			*maplinecpypt = *mapgfxpt; maplinecpypt++; mapgfxpt++; } } }
	i += mapblockwidth; mymappt++; mapxblks--; }
	j += mapblockheight; mymappt = mymap2pt+mapwidth; mapyblks--; }
}
#endif

void MapDrawBG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
	int mapw, int maph)
{
int i, j, mycl, mycr, myct, mycb, mapvclip, maphclip;
short int *mymappt;
short int *mymap2pt;
BLKSTR *blkdatapt;
ANISTR *myanpt;

	if (!gfxinbitmaps) {
#ifndef RBNOCUSTOMRENDER
		switch (mapdepth) {
			case 8:
				MapDraw8BG (mapdestpt, mapxo, mapyo, mapx, mapy, mapw, maph);
				break;
			case 15:
			case 16:
				MapDraw16BG (mapdestpt, mapxo, mapyo, mapx, mapy, mapw, maph);
				break;
			case 32:
				MapDraw32BG (mapdestpt, mapxo, mapyo, mapx, mapy, mapw, maph);
				break;
		}
#endif
	} else {
		mycl = mapdestpt->cl;
		mycr = mapdestpt->cr;
		myct = mapdestpt->ct;
		mycb = mapdestpt->cb;
		set_clip (mapdestpt, mapx, mapy, mapx+mapw-1, mapy+maph-1);
		mymappt = (short int *) mappt;
		mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
		mapvclip = mapyo%mapblockheight;
		maphclip = mapxo%mapblockwidth;

/* Draw left clipped column */
		i=(mapx-maphclip);
		for (j=(mapy-mapvclip);j<((mapy+maph)-mapblockheight);j+=mapblockheight) {
			if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			mymappt += mapwidth;
		}
/* Draw bottom clipped row */
		for (;i<((mapx+mapw)-mapblockwidth);i+=mapblockwidth) {
			if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			mymappt ++;
		}
/* Draw right clipped column */
		for (;j>(mapy-mapvclip);j-=mapblockheight) {
			if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			mymappt -= mapwidth;
		}
/* Draw top clipped row */
		for (;i>(mapx-maphclip);i-=mapblockwidth) {
			if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			mymappt--;
		}
/* Draw unclipped blocks */
		mymappt += mapwidth+1;
		mymap2pt = mymappt;
		set_clip (mapdestpt, 0, 0, 0, 0);
		for (j=((mapy-mapvclip)+mapblockheight);j<((mapy+maph)-mapblockheight);j+=mapblockheight) {
		for (i=((mapx-maphclip)+mapblockwidth);i<((mapx+mapw)-mapblockwidth);i+=mapblockwidth) {
			if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			mymappt++;
		}
		mymap2pt += mapwidth; mymappt = mymap2pt;
		}
		set_clip (mapdestpt, mycl, myct, mycr+1, mycb+1);
	}
}

void MapDrawBGT (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
	int mapw, int maph)
{
int i, j, mycl, mycr, myct, mycb, mapvclip, maphclip;
short int *mymappt;
short int *mymap2pt;
BLKSTR *blkdatapt;
ANISTR *myanpt;

	if (!gfxinbitmaps) {
#ifndef RBNOCUSTOMRENDER
		switch (mapdepth) {
			case 8:
				MapDraw8BGT (mapdestpt, mapxo, mapyo, mapx, mapy, mapw, maph);
				break;
			case 15:
			case 16:
				MapDraw16BGT (mapdestpt, mapxo, mapyo, mapx, mapy, mapw, maph);
				break;
			case 32:
				MapDraw32BGT (mapdestpt, mapxo, mapyo, mapx, mapy, mapw, maph);
				break;
		}
#endif
	} else {
		mycl = mapdestpt->cl;
		mycr = mapdestpt->cr;
		myct = mapdestpt->ct;
		mycb = mapdestpt->cb;
		set_clip (mapdestpt, mapx, mapy, mapx+mapw-1, mapy+maph-1);
		mymappt = (short int *) mappt;
		mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
		mapvclip = mapyo%mapblockheight;
		maphclip = mapxo%mapblockwidth;

/* Draw left clipped column */
		i=(mapx-maphclip);
		for (j=(mapy-mapvclip);j<((mapy+maph)-mapblockheight);j+=mapblockheight) {
			if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			if (blkdatapt->trigger)
				masked_blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			else
				blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			mymappt += mapwidth;
		}
/* Draw bottom clipped row */
		for (;i<((mapx+mapw)-mapblockwidth);i+=mapblockwidth) {
			if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			if (blkdatapt->trigger)
				masked_blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			else
				blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			mymappt ++;
		}
/* Draw right clipped column */
		for (;j>(mapy-mapvclip);j-=mapblockheight) {
			if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			if (blkdatapt->trigger)
				masked_blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			else
				blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			mymappt -= mapwidth;
		}
/* Draw top clipped row */
		for (;i>(mapx-maphclip);i-=mapblockwidth) {
			if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			if (blkdatapt->trigger)
				masked_blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			else
				blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			mymappt--;
		}
/* Draw unclipped blocks */
		mymappt += mapwidth+1;
		mymap2pt = mymappt;
		set_clip (mapdestpt, 0, 0, 0, 0);
		for (j=((mapy-mapvclip)+mapblockheight);j<((mapy+maph)-mapblockheight);j+=mapblockheight) {
		for (i=((mapx-maphclip)+mapblockwidth);i<((mapx+mapw)-mapblockwidth);i+=mapblockwidth) {
			if (*mymappt>=0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			if (blkdatapt->trigger)
				masked_blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			else
				blit ((BITMAP *) GFXBG(blkdatapt->bgoff), mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
			mymappt++;
		}
		mymap2pt += mapwidth; mymappt = mymap2pt;
		}
		set_clip (mapdestpt, mycl, myct, mycr+1, mycb+1);
	}
}

void MapDrawFG (BITMAP * mapdestpt, int mapxo, int mapyo, int mapx, int mapy,
	int mapw, int maph, int mapfg)
{
int i, j, mycl, mycr, myct, mycb, mapvclip, maphclip;
short int *mymappt;
short int *mymap2pt;
BLKSTR *blkdatapt;
ANISTR *myanpt;
BITMAP *mapgfxpt;

	if (!gfxinbitmaps) {
#ifndef RBNOCUSTOMRENDER
		switch (mapdepth) {
			case 8:
				MapDraw8FG (mapdestpt, mapxo, mapyo, mapx, mapy, mapw, maph, mapfg);
				break;
			case 15:
			case 16:
				MapDraw16FG (mapdestpt, mapxo, mapyo, mapx, mapy, mapw, maph, mapfg);
				break;
			case 32:
				MapDraw32FG (mapdestpt, mapxo, mapyo, mapx, mapy, mapw, maph, mapfg);
				break;
		}
#endif
	} else {
		mycl = mapdestpt->cl;
		mycr = mapdestpt->cr;
		myct = mapdestpt->ct;
		mycb = mapdestpt->cb;
		set_clip (mapdestpt, mapx, mapy, mapx+mapw-1, mapy+maph-1);
		mymappt = (short int *) mappt;
		mymappt += (mapxo/mapblockwidth)+((mapyo/mapblockheight)*mapwidth);
		mapvclip = mapyo%mapblockheight;
		maphclip = mapxo%mapblockwidth;
/* Draw left clipped column */
		i=(mapx-maphclip);
		for (j=(mapy-mapvclip);j<((mapy+maph)-mapblockheight);j+=mapblockheight) {
		if (*mymappt) {
			if (*mymappt>0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			if (!mapfg) mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff;
			else if (mapfg == 1) mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff2;
			else mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff3;
			if (((intptr_t) mapgfxpt)!=0)
			masked_blit (mapgfxpt, mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
		}
			mymappt += mapwidth;
		}
/* Draw bottom clipped row */
		for (;i<((mapx+mapw)-mapblockwidth);i+=mapblockwidth) {
		if (*mymappt) {
			if (*mymappt>0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			if (!mapfg) mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff;
			else if (mapfg == 1) mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff2;
			else mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff3;
			if (((intptr_t) mapgfxpt)!=0)
			masked_blit (mapgfxpt, mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
		}
			mymappt ++;
		}
/* Draw right clipped column */
		for (;j>(mapy-mapvclip);j-=mapblockheight) {
		if (*mymappt) {
			if (*mymappt>0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			if (!mapfg) mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff;
			else if (mapfg == 1) mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff2;
			else mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff3;
			if (((intptr_t) mapgfxpt)!=0)
			masked_blit (mapgfxpt, mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
		}
			mymappt -= mapwidth;
		}
/* Draw top clipped row */
		for (;i>(mapx-maphclip);i-=mapblockwidth) {
		if (*mymappt) {
			if (*mymappt>0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			if (!mapfg) mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff;
			else if (mapfg == 1) mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff2;
			else mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff3;
			if (((intptr_t) mapgfxpt)!=0)
			masked_blit (mapgfxpt, mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
		}
			mymappt--;
		}
/* Draw unclipped blocks */
		mymappt += mapwidth+1;
		mymap2pt = mymappt;
		set_clip (mapdestpt, 0, 0, 0, 0);
		for (j=((mapy-mapvclip)+mapblockheight);j<((mapy+maph)-mapblockheight);j+=mapblockheight) {
		for (i=((mapx-maphclip)+mapblockwidth);i<((mapx+mapw)-mapblockwidth);i+=mapblockwidth) {
		if (*mymappt) {
			if (*mymappt>0) blkdatapt = (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
			else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
			blkdatapt = ANBLK(myanpt); }
			if (!mapfg) mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff;
			else if (mapfg == 1) mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff2;
			else mapgfxpt = (BITMAP *)(intptr_t) blkdatapt->fgoff3;
			if (((intptr_t) mapgfxpt)!=0)
			masked_blit (mapgfxpt, mapdestpt, 0, 0, i, j, mapblockwidth, mapblockheight);
		}
		mymappt++;
		}
		mymap2pt += mapwidth; mymappt = mymap2pt;
		}
		set_clip (mapdestpt, mycl, myct, mycr+1, mycb+1);
	}
}
