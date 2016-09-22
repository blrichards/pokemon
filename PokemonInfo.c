
#include "PokeLib.h"
#include "PokemonSprites.h"
#include "BattleImages.h"


//******************Images Dictionary******************
Image_t CharmanderBack = {
	Charmander_Backside, 47,43
};

Image_t CharmanderFront = {
	Charmander_Frontside, 36,39
};
Image_t CharmanderIn1 = {
	Charmander_In1, 21,17
};
Image_t CharmanderIn2 = {
	Charmander_In2, 21,18
};

Image_t SquirtleBack = {
	Squirtle_Backside, 45,33
};
Image_t SquirtleFront = {
	Squirtle_Frontside, 38, 36
};
Image_t SquirtleIn1 = {
	Squirtle_In1, 21,17
};
Image_t SquirtleIn2 = {
	Squirtle_In2, 21,16
};

Image_t BulbasaurBack = {
	Bulbasaur_Back, 45,32
};

Image_t BulbasaurFront = {
	Bulbasaur_Front, 34,33
};

Image_t BulbasaurIn1 = {
	Bulbasaur_In1, 20,17
};

// Image_t GARY1 = {
// 	Gary_Pic1, 28, 57
// };
// Image_t GARY2 = {
// 	Gary_Pic2, 38, 60
// };
// Image_t GARY3 = {
// 	Gary_Pic3,  35, 37
// };

Image_t PROF_OAK = {
	Prof_Oak1, 40, 63
};
// Image_t TEAMROCKET_M = {
// 	TeamRocket_Male, 40, 59
// };
// Image_t KID = {
// 	Kid, 34, 48
// };
// Image_t NERD = {
// 	Nerd, 30, 61
// };
//
// Image_t BUGTRAINER = {
// 	BugCatcher, 39,48
// };

Image_t NullIm = {
	null, 0, 0
};

//******************************************************



//*******************Moves Dictionary*******************
// Types:
//
//0 - FIRE
//1 - WATER
//2 - GRASS
//
//
//
#define NUL			 				0
#define PHYSICAL				1
#define SPECIAL	 				2
#define DRAIN		 				3
#define O_ATT_STATUS		4
#define O_DEF_STATUS		5
#define U_ATT_STATUS		6
#define U_DEF_STATUS		7
#define O_SPA_STATUS		8
#define O_SPD_STATUS		9
#define U_SPA_STATUS		10
#define U_SPD_STATUS		11

#define FIRE		0
#define WATER		1
#define GRASS		2
#define NORMAL	3
//SQUIRTLE
//1
Move_t TAIL_WHIP = {
	"TAILWHIP\0", NORMAL, O_DEF_STATUS, 0, 100, 30
};

//
Move_t TACKLE = {
	"TACKLE\0",NORMAL, PHYSICAL, 35, 95, 35
};

Move_t BUBBLE = {
	"BUBBLE\0", WATER, SPECIAL, 20, 100, 30
};

Move_t WATER_GUN = {
	"WATER GUN\0", WATER, SPECIAL, 40, 100, 25
};
Move_t HYDRO_PUMP = {
	"HYDRO_PUMP\0", WATER, SPECIAL, 120, 80, 5
};

//CHARMANDER
//1
Move_t SCRATCH = {
	"SCRATCH\0", NORMAL, PHYSICAL, 40, 100, 35
};

//1
Move_t GROWL = {
	"GROWL\0", NORMAL, O_ATT_STATUS, 0, 100, 35
};

//9
Move_t EMBER = {
	"EMBER\0", FIRE, SPECIAL, 40, 100, 25
};


//30
Move_t SLASH = {
	"SLASH\0", NORMAL, PHYSICAL, 70, 100, 20
};

//38
Move_t FLAMETHROWER = {
	"FLAMETHROWER\0", FIRE, SPECIAL, 95, 100, 15
};


//Bulbasor
//tackle lvl 1
//growl lvl 1

// #! IMPLEMENT LEECH SEED, STATUS?
//7
Move_t LEECH_SEED = {
	"LEECH SEED\0", GRASS, DRAIN, 0, 90,
};

//13
Move_t VINE_WHIP = {
	"VINE WHIP\0", GRASS, PHYSICAL, 100, 10
};

//27
Move_t RAZOR_LEAF = {
	"RAZOR LEAF\0", GRASS, PHYSICAL, 95, 55
};

/*Move_t CUT = {
	"CUT\0", NORMAL, PHYSICAL, 50, 95
};

Move_t LEER = {
	"LEER\0", NORMAL, STATUS, 0, 100
}; */

Move_t NULL = {
	"-\0", 0,NUL,0,0
};
//*******************************************************

//**************POKEBALLS DICTIONARY*********************

Pokeball_t Pokeball = {
	"POKEBALL\0",
	50,
	&NullIm
};
