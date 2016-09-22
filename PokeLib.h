#include <stdint.h>

#ifndef _POKE_
#define _POKE_

// simple image
typedef struct image {
	const unsigned short *imagePt;
	int width;
	int height;
} Image_t;

// pokemon move in battle
typedef struct move {
	char *name;
	unsigned char type;
	unsigned char category;
	unsigned char power;
	unsigned short accuracy;	//	1 < x < 100
	unsigned short PP;
} Move_t;

// used to represent a pokemon that can battle and hold experience
typedef struct pokemon {
	char *name;
	int level;
	unsigned char type;
	int health;
	int exp;

	//Index Guide
	//0 - HP
	//1 - Attack
	//2 - Defense
	//3 - Sp. Attack
	//4 - Sp. Defense
	//5 - Agility
	int baseStats[6];
	Move_t *moveSet[4];
	int movelvls[5];
	Image_t *Images[4];

	//unsigned short catchrate; // 1 < x < 100
} Pokemon_t;

typedef struct pokeball {
	char * name;
	unsigned char successRate; // 1 < x < 100
	Image_t *Graphic;
} Pokeball_t;

typedef struct battle {
	char *name;
	Pokemon_t *pokemon[6];
	unsigned char numPokemon;
	Pokeball_t *pokeballs[10];
	Image_t *Images[4];
} Battle_t;

// person on overworld map
typedef struct person {
	const unsigned short *stand[4];
	const unsigned short *walk[4][2];
	unsigned long xCoor;
	unsigned long yCoor;
	unsigned long width;
	unsigned long height;
	char *dialogue;
	unsigned char canChangeDir;
	unsigned char cDir;
	unsigned char isTrainer;
	unsigned long cLeg;
	unsigned char walking;
	signed char upDown;
	signed char leftRight;
	Battle_t *battleInfo;
	unsigned char isDefeated;
} Person_t;

// tile on the overworld map
typedef struct tile {
	const unsigned short *pxMap;
	unsigned char occupied;
	unsigned char has_action;
	void (*action)(void);
	unsigned short hasOverlay;
	const unsigned short *overlay;
	unsigned char has_dialogue;
	char *dialogue;
	unsigned short isFlower;
	unsigned short has_person;
	Person_t *person;
	struct tile *linkTile;
	unsigned short hasLink;
	unsigned short linkDir;
	struct tile *oldTile;
	void (*say)(char *dialogue);
} Tile_t;

// overworld map
typedef struct map{
	unsigned long width;
	unsigned long height;
	Tile_t *tiles[32][32];
	Person_t *people[5];
	long numPeople;
} Map_t;

#endif
