#include "PokeLib.h"
#include "PokeStructs.h"
#include "PokemonInfo.h"
//**************TRAINERS****************
/*
	typedef struct battle {
	char *name;
	Pokemon_t *pokemon[6];
	unsigned char numPokemon;
	//Item_t *items[10]
	Pokeball_t *pokeballs[10];
	Image_t *Images[4];
} Battle_t;

*/
Battle_t GARY_b = {
	"RIVAL GARY\0", 
 {&Charmander, &Squirtle, &Bulbasaur}, 
	3, 
	{0},
	{&GARY1, &GARY2, &GARY3},
};

Person_t GARY = {
		{0},
	{0},
	0,
	0,
	0,
	0,
	"-\0",
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	&GARY_b
};


/*typedef struct battle {
	char *name;
	Pokemon_t *pokemon[6];
	unsigned char numPokemon;
	//Item_t *items[10]
	Pokeball_t *pokeballs[10];
	Image_t *Images[4];
} Battle_t; */

Battle_t Player_b = {
	"ASH\0",
	{ &Bulbasaur
	},
	1,
	{ 0
	},
	{ 0
	}
};


Person_t Player = {
	{0},
	{0},
	0,
	0,
	0,
	0,
	"-\0",
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	&Player_b
};



Battle_t BugTrainer_Tim_b = {
	"BUG CATCHER TIM\0",
	{	&Bulbasaur, &Bulbasaur
	},
	2,
	{0
	},
	{&BUGTRAINER, 
	},
};

Person_t BugTrainer_Tim = {
	{0},
	{0},
	0,
	0,
	0,
	0,
	"\0",
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	&BugTrainer_Tim_b
};

Battle_t Nerd_Chris_b = {
	"NERD CHRIS\0", 
	{ &Charmander, &Charmander
	},
	2,
	{0},
	{&NERD}
};

Person_t Nerd_Chris = {
	{0},
	{0},
	0,
	0,
	0,
	0,
	"\0",
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	&Nerd_Chris_b
};

Battle_t TeamRocket_Brad_b = {
	"TEAM ROCKET BRAD\0",
	{ &Squirtle
	}, 1,
	{0},
	{&TEAMROCKET_M}
};

Person_t TeamRocket_Brad = {
	{0},
	{0},
	0,
	0,
	0,
	0,
	"\0",
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	&TeamRocket_Brad_b
};

Battle_t Kid_Matt_b = {
	"YOUNGSTER MATT",
	{ &Bulbasaur
	}, 1,
	{0},
	{&KID}
};

Person_t Kid_Matt = {
	{0},
	{0},
	0,
	0,
	0,
	0,
	"\0",
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	&Kid_Matt_b
};
Battle_t Prof_Oak_b = {
	"PROF OAK\0",
	{ &Charmander, &Squirtle, &Bulbasaur, &Charmander, &Squirtle, &Bulbasaur },
	6,
	{0},
	{&PROF_OAK}
};

Person_t Prof_Oak = {
	{0},
	{0},
	0,
	0,
	0,
	0,
	"\0",
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	&Prof_Oak_b
};




















Person_t nullEn = {

	{0},
	{0},
	0,
	0,
	0,
	0,
	"\0",
	0,
	0,
	1, 
	0,
	0,
};
