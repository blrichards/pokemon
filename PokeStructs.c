#include "PokeLib.h"
#include "PokemonInfo.h"

#define FIRE		0
#define WATER		1
#define GRASS		2
#define NORMAL	3


/*Pokemon_t Wartortle = {
	
}; */
Pokemon_t Squirtle = {
	"SQUIRTLE\0", 5, WATER, 48, 0,
	{
		44, //HP
		48, //Attack
		65, //Defense
		50, //Sp. Attack
		64, //Sp. Defense
		43, //Speed
	},
	{
		&TACKLE, &TAIL_WHIP, &NULL, &NULL
	},
	{
		0
	},
	{
		&SquirtleBack,
		&SquirtleFront,
		&SquirtleIn1,
		&SquirtleIn2
	}
};

Pokemon_t Charmander = {
	"CHARMANDER\0", 5, FIRE, 48, 10,
	{
		39, //HP
		52, //Attack
		43, //Defense
		60, //Sp. Attack
		50, //Sp. Defense
		65, //Speed
	},
	{
		&SCRATCH, &GROWL, &NULL, &NULL
	},
	{
		0 //move levels
	},
	{
		&CharmanderBack,
		&CharmanderFront,
		&CharmanderIn1,
		&CharmanderIn2
	}
};

Pokemon_t Bulbasaur = {
	"BULBASAUR\0", 5, GRASS, 48, 10,
	{
		45, //HP
		49, //Attack
		49, //Defense
		65, //Sp. Attack
		65, //Sp. Defense
		45, //Speed
	},
	{
		&TACKLE, &GROWL, &NULL, &NULL
	},
	{
		0 //move levels
	},
	{
		//#! MAKE THESE ACTUALLY BULBASAUR
		&BulbasaurBack,
		&BulbasaurFront,
		&BulbasaurIn1,
		&CharmanderIn2
	}
};

Pokemon_t NullPk = {
	"-\0", 0, 0, 0, 0, 
	{
		0, //HP
		0, //Attack
		0, //Defense
		0, //Sp. Attack
		0, //Sp. Defense
		0, //Speed
	},
	{
		&NULL,&NULL, &NULL, &NULL
	},
	{
		0
	},
	{
		&NullIm,
		&NullIm,
		&NullIm,
		&NullIm
	}
};


