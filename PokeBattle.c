#include <stdint.h>
#include "ST7735.h"
#include "Random.h"
#include "Joystick.h"
#include "BattleImages.h"
#include "PokeBattle.h"
#include "User.h"
#include "PokeStructs.h"
#include "Graphics.h"
//#include "TrainerImages.c"
#define NULL		 				0
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




//******************Arrays******************
//Enemies[] - array of Enemy Trainers
//WildGrass[] - array of Wild Pokemon in wild grass setting

Person_t *Enemies[] = {&Player};
Pokemon_t *WildGrass[] = {&Squirtle, &Squirtle, &Charmander, &Charmander, &Squirtle};
//Pokemon_t *Environment = WildGrass;

#define ATTACK			0
#define DEFENSE			1
#define SPATTACK		2
#define SPDEFENSE		3
static double STAB;
static double typeEffectiveness;
static unsigned char Critical;
static double rand;
static int damage;

#define FIRE		0
#define WATER		1
#define GRASS		2
#define NORMAL	3
char *types[10] = {
	 "FIRE\0", "WATER\0", "GRASS\0", "NORMAL\0"
};
//************************************************


//****************************FSMs******************************
//logic used to control the different screens

struct state {
	int x;
	int y;
	int (*AButton)(void);
	int (*BButton)(void);
	int nextSt[4];
};
typedef struct state state_t;

//ShiftMenu_FSM
#define ShiftSt 	0
#define CancelSt  1
state_t ShiftMenu[2] = {
	{98,106, &ShiftMenu_SHIFT_,&ShiftMenu_CANCEL, {CancelSt,-1,CancelSt,-1}},
	{98,116, &ShiftMenu_CANCEL,&ShiftMenu_CANCEL, {ShiftSt,-1,ShiftSt,-1}}
};

#define Yes	0
#define No	1
//Run_FSM
state_t RunMenu[2] = {
	{98, 106, &RunMenu_YES, &RunMenu_NO, {No, -1,No,-1}},
	{98, 116, &RunMenu_NO, &RunMenu_NO, {Yes,-1,Yes,-1}}
};

//Change_Pokemon_FSM
state_t ChangePokemon[2] = {
	{98, 106, &ChangePokemon_YES, &ChangePokemon_NO, {No,-1,No,-1}},
	{98, 116, &ChangePokemon_NO, &ChangePokemon_NO, {Yes,-1,Yes,-1}}
};

//FourOptionMenu FSMS
#define UpL    0
#define UpR    1
#define DownL  2
#define DownR  3
state_t BattleMenu[4] = {
	{64, 107, &BattleMenu_FIGHT,&BattleMenu_RUN, {DownL,UpR,-1,-1}},  //upL
	{120, 107, &BattleMenu_BAG,&BattleMenu_RUN, {DownR,-1,-1,UpL}},     //upR
	{64, 120, &BattleMenu_POKEMON,&BattleMenu_RUN, {-1,DownR,UpL,-1}}, //downL
	{120, 120, &BattleMenu_RUN,&BattleMenu_RUN, {-1,-1,UpR,DownL}},  //downR
};
state_t MoveMenu[4] = {
	{8,106,&MoveMenu_USEMOVE,&MoveMenu_CANCEL, {DownL,UpR,-1,-1}},
	{58,106,&MoveMenu_USEMOVE,&MoveMenu_CANCEL, {DownR,-1,-1,UpL}},
	{8,118,&MoveMenu_USEMOVE,&MoveMenu_CANCEL, {-1,DownR,UpL,-1}},
	{58,118,&MoveMenu_USEMOVE,&MoveMenu_CANCEL, {-1,-1,UpR,DownL}}
};

#define SPOT0  0
#define SPOT1  1
#define SPOT2  2
#define SPOT3  3
#define SPOT4  4
#define SPOT5  5
#define CANCEL 6

//FSM used to control PokemonMenu
//SPOTS are the different Pokemon spots within PokemonMenu
state_t PokemonMenu[7] = {
	{5,5, &PokemonMenu_SPOTX,&PokemonMenu_CANCEL, {SPOT1,SPOT1,SPOT5,-1}},	  //SPOT0
	{5,5, &PokemonMenu_SPOTX,&PokemonMenu_CANCEL, {SPOT2,-1,SPOT0,SPOT0}},	  //SPOT1
	{5,5, &PokemonMenu_SPOTX,&PokemonMenu_CANCEL, {SPOT3,-1,SPOT1,SPOT0}},	  //SPOT2
	{5,5, &PokemonMenu_SPOTX,&PokemonMenu_CANCEL, {SPOT4,-1,SPOT2,SPOT0}},	  //SPOT3
	{5,5, &PokemonMenu_SPOTX,&PokemonMenu_CANCEL, {SPOT5,-1,SPOT3,SPOT0}},	  //SPOT4
	{5,5, &PokemonMenu_SPOTX,&PokemonMenu_CANCEL, {CANCEL,-1,SPOT4,SPOT0}},		//SPOT5
	//#! ADD B BUTTON ACTION
	{5,5, &PokemonMenu_CANCEL,&PokemonMenu_CANCEL, {SPOT0,-1,SPOT5,SPOT0}},		//CANCEL
};

//#! expand if you have time
state_t BagMenu[1] = {
	{63, 20, &BagMenu_POKEBALL, &BagMenu_CANCEL, {-1, -1, -1, -1}}
};
static int cs;
static int cFSM;
//******************************************************************




//*********************Static_Variables*****************************
//UserPokemon - instance of the Players Pokemon in the 0 slot.
//EnemyPokemon - instance of either a trainer's pokemon slot 0 or a wild pokemon.
static Pokemon_t UserPokemon[6];
static Pokemon_t EnemyPokemon[6];
static Battle_t User;
static Battle_t Enemy;
//******************************************************************

//***********************Colors****************************
static uint16_t green;
static uint16_t grey;
static uint16_t turquoise;
static uint16_t txtColor;
static uint16_t bgColor;
static uint16_t bleakPurple;
static uint16_t black;
static uint16_t jadedGreen;
static uint16_t healthRed;
static uint16_t healthYellow;
static uint16_t highlightOrange;
static uint16_t brownGoldBd;
static uint16_t expBlue;
static uint16_t faintRed;
static uint16_t	bgBagGrey;
static uint16_t	bdBagTan;
static uint16_t	mainBgPeach;
static uint16_t	bgBagGreen;
static uint16_t	bagGreen;
static uint16_t	blueBagBg;
static uint16_t	blueBagBd;
static uint16_t	blueBagMain;
//***********************Organize This*********************************
static int x, y, w, h, xmargin, ymargin;

static int cursorX;
static int cursorY;

static char *Shift = "SHIFT \0";
static char *Cancel = "CANCEL\0";
static char *YES = "YES\0";
static char *NO = "NO\0";
char *pokeball = "POKEBALL x6\0";
static char *Reward;
static int gainedXP;
static char *gainedXPs;
static int stack[5];
static int *top;
static int catchable;
static int success;
//**************************
static int TwoOptionMenu_spot;
//#! IMPLEMENT ANIMATIONS	
//static int animate;
static int youWin;
//#! IMPLEMENT YOU LOSE SCREEN
static int youLose;
static char *battleMenuWords[4] = {"FIGHT\0","BAG\0","POKEMON\0","RUN\0"};

#define ERROR						-1
#define GRAPHICS				0
#define BATTLEMENU			1
#define MOVEMENU 				2
#define BAGMENU					3
#define POKEMONMENU 		4
#define SHIFTMENU				5
#define RUNMENU					6
#define TWOOPTIONMENU		7
#define CHANGEPOKEMON		8
#define RUNENDSEQ				9

void Battle_RunFSM(Person_t trainer) {
	cFSM = Battle_LoadScreen(trainer);
	while(1) {
		switch(cFSM) {
			case GRAPHICS:
				return;
			case BATTLEMENU: 
				cFSM = FourOptionMenu_Controller(BATTLEMENU); 
			break;
			case MOVEMENU: 
				cFSM = FourOptionMenu_Controller(MOVEMENU);
			break;
			case POKEMONMENU: 
				cFSM = PokemonMenu_Controller(); 
			break;
			case SHIFTMENU:
				cFSM = TwoOptionMenu_Controller(SHIFTMENU);
			break;
			case RUNMENU:
				cFSM = TwoOptionMenu_Controller(RUNMENU);
			break;
			case CHANGEPOKEMON:
				cFSM = TwoOptionMenu_Controller(CHANGEPOKEMON);
			break;
			case BAGMENU:
				cFSM = BagMenu_Controller();
			break;
			case RUNENDSEQ:
				cFSM = Battle_EndConditions();
				break;
			default:
				cFSM = catchError();
			break;
		}
		if(cFSM == RUNENDSEQ) break;
	}
	Battle_ExitScreen(trainer);
}

//
//							FourOptionMenu_Controller();
//*******************************************************

static int FourOptionMenu_Controller(unsigned char menu) {
	unsigned int xdir;
	unsigned int ydir;
	cs = 0;
	
	while(1) {
		Joystick_In(&xdir,&ydir);
		FourOptionMenu_MoveCursor(Joystick_getDirection(xdir,ydir), menu);
		if(AisPushed()) {
			while(AisPushed());
			switch(menu) {
				case BATTLEMENU:
					return BattleMenu[cs].AButton();
				case MOVEMENU:
					return MoveMenu[cs].AButton();
				default:
					return ERROR;
			}
		}
		if(BisPushed()) {
			while(BisPushed());
			switch(menu) {
				case BATTLEMENU:
					return BattleMenu[cs].BButton();
				case MOVEMENU:
					return MoveMenu[cs].BButton();
				default:
					return ERROR;
			}
		}
		Delay1(20);
	}
}

static int FourOptionMenu_MoveCursor(int dir, unsigned char menu) {
	if(dir == 4 || BattleMenu[cs].nextSt[dir] == -1);
	else {
		switch(menu) {
			case BATTLEMENU:
				ST7735_DrawBitmap(BattleMenu[BattleMenu[cs].nextSt[dir]].x,BattleMenu[BattleMenu[cs].nextSt[dir]].y,Battle_Cursor,6,10);
				ST7735_FillRect(BattleMenu[cs].x,BattleMenu[cs].y-9,6,10,bgColor);
				cs = BattleMenu[cs].nextSt[dir];
			break;
			case MOVEMENU:
				ST7735_DrawBitmap(MoveMenu[MoveMenu[cs].nextSt[dir]].x,MoveMenu[MoveMenu[cs].nextSt[dir]].y,Battle_Cursor,6,10);
				ST7735_FillRect(MoveMenu[cs].x,MoveMenu[cs].y-9,6,10,bgColor);
				cs = MoveMenu[cs].nextSt[dir];
			break;
		}
	}
	return 0;
}
//
//								 MoveMenu_ActionFunctions
//*******************************************************
int MoveMenu_USEMOVE(void) {
	//#! STATUS MOVES!
	if(UserPokemon[0].moveSet[cs]->name[0] == '-') {
		printDialogue("Please pick a Usable Move", "\0","\0","\0","\0");
		return BATTLEMENU;
	}
	
	printRect(0,88,160,40,black,ST7735_Color565(200,168,72),jadedGreen);
	printDialogue(UserPokemon[0].name," used \0", UserPokemon[0].moveSet[cs]->name, "!\0","\0");
	
	
	if(UserPokemon[0].moveSet[cs]->category >= O_ATT_STATUS) {
			cursorX = 5;
			cursorY = 93;
			printRect(0,88,160,40,black,ST7735_Color565(200,168,72),jadedGreen);
		
		chooseSTATUS(UserPokemon[0].moveSet[cs]->category, UserPokemon[0],EnemyPokemon[0]);
	} else {
		
		damage = calcDam(UserPokemon[0],EnemyPokemon[0],cs);
		int newHealth = EnemyPokemon[0].health - damage;
		if(!Battle_UpdateEnemyHealth(newHealth)) {
			EnemyPokemon[0].health = 0;
			if(Enemy.numPokemon > 0) {
				for(int i = 0; i < Enemy.numPokemon; i++) {
					if(EnemyPokemon[i].health > 0) {
						switchEnemyPokemon();
						return BATTLEMENU;
					} else {
						youWin = 1;
						return RUNENDSEQ;
					}
				}
			} else {
				youWin = 1;
				return RUNENDSEQ;
			}
		}
		EnemyPokemon[0].health -= damage;
}
	
	//#! RUN FIGHT SEQUENCE
	return Battle_RunFightSequence();
}

static int MoveMenu_CANCEL(void) {
	
	printRect(0,88,160,40,black,ST7735_Color565(200,168,72),jadedGreen);
	printBattleMenu();
	
	return BATTLEMENU;
}

//
//									Battle_ActionFunctions
//*******************************************************
static int BattleMenu_FIGHT() {
	printMoveMenu();
	printPPMenu();
	
	return MOVEMENU;
}

static int BattleMenu_BAG() {
	printBagMenu();
	return BAGMENU;
}

static int BattleMenu_POKEMON() {
	printPokemonMenu();
	return POKEMONMENU;
}

static int BattleMenu_RUN() {
	printTwoOptionMenu(YES, NO);
	return RUNMENU;
}

//
//							PokemonMenu_Controller();
//*******************************************************
static int PokemonMenu_Controller() {
	unsigned int xdir;
	unsigned int ydir;
	cs = 0;
	
	while(1) {
		Joystick_In(&xdir, &ydir);
		PokemonMenu_MoveSpots(Joystick_getDirection(xdir,ydir));
		if(AisPushed()) {
			while(AisPushed());
			return PokemonMenu[cs].AButton();
		}
		if(BisPushed()) {
			while(BisPushed());
			return PokemonMenu[cs].BButton();
		}
		Delay1(20);
	}
}

static int PokemonMenu_MoveSpots(int dir) {
	unsigned short color1 = ST7735_Color565(120,208,232);
	unsigned short color2 = ST7735_Color565(72,72,96);
	unsigned short color3 = ST7735_Color565(64,200,248);
	unsigned short color4 = ST7735_Color565(56,144,216);
	
	if(dir == 4 || PokemonMenu[cs].nextSt[dir] == -1);
	else {
		if(PokemonMenu[cs].nextSt[dir] < User.numPokemon) {
			PokemonMenu_Highlight(PokemonMenu[cs].nextSt[dir],highlightOrange,highlightOrange,color1);
			PokemonMenu_Highlight(cs,color2,color3,color4);
			cs = PokemonMenu[cs].nextSt[dir];
		}
		if(PokemonMenu[cs].nextSt[dir] == 7){
			PokemonMenu_Highlight(PokemonMenu[cs].nextSt[dir],highlightOrange,highlightOrange,ST7735_Color565(112,88,176));
			PokemonMenu_Highlight(cs,ST7735_Color565(72,72,96),ST7735_Color565(72,72,96),ST7735_Color565(160,112,240));
			cs = PokemonMenu[cs].nextSt[dir];
		}
	}
	return 0;
}
static int PokemonMenu_Highlight(int spot,unsigned short bdColor1, unsigned short bdColor2, unsigned short mainColor) {
	int xcoord;
	int ycoord;
	int width;
	int height;
	int xmarg;
	int ymarg;
	int xchar;
	int ychar;
	int ylevel;
	int xlevel;
	int HPx;
	int HPy;
	int j;
	
	switch(spot) {
		case 0:
			xcoord = 3;
			ycoord = 27;
			width = 64;
			height = 40;
			xmarg = 3;
			ymarg = 13;
			xchar = 6;
			ychar = 5;
			ylevel = 15;
			xlevel = 30;
			HPx = -1;
			HPy = 36;
		break;
		case 7:
			xcoord = 5;
			ycoord = 5;
			width = 5;
			height = 5;
			xmarg = 5;
			ymarg = 5;
			//print cancel
		break;
		default:
			xcoord = 67;
			ycoord = 7+(23*(spot-1));
			width = 90;
			height = 20;
			xmarg = 1;
			ymarg = 8;
			xchar = 7;
			ychar = 3;
			ylevel = 0;
			xlevel = 60;
			HPx = 22;
			HPy = 17;
		break;
	}

	if(UserPokemon[spot].health == 0) {
		mainColor = faintRed;
	}
	
	ST7735_FillRect(xcoord,ycoord,width,height,mainColor);
	printEmptyPokemonBox(xcoord, ycoord, width, height, bdColor1, bdColor2);
	
	//Draw HP Box
	ST7735_DrawBitmap(xcoord+HPx,ycoord+HPy,PokemonMenu_HP,65,5);
	printHP(UserPokemon[spot].health,xcoord+HPx+15,ycoord+HPy-3);
	j=0;
	while(UserPokemon[spot].name[j] != '\0'){
		ST7735_DrawChar(xcoord+xchar+5*j,ycoord+ychar,UserPokemon[spot].name[j],0xFFFF,mainColor,0x01);
		j++;
	}
	
	//print level;
	int tens_place = UserPokemon[spot].level / 10 + 0x30;
	int ones_place = UserPokemon[spot].level % 10 + 0x30;
	j = 0;
	ST7735_DrawChar(xcoord+5*j+xlevel, ycoord+ychar+ylevel, 'L', 0xFFFF, mainColor, 0x01); j++;
	ST7735_DrawChar(xcoord+5*j+xlevel, ycoord+ychar+ylevel, 'v', 0xFFFF, mainColor, 0x01); j++;
	ST7735_DrawChar(xcoord+5*j+xlevel, ycoord+ychar+ylevel, '.', 0xFFFF, mainColor, 0x01); j++;
	
	ST7735_DrawChar(xcoord+5*j+xlevel, ycoord+ychar+ylevel, tens_place, 0xFFFF, mainColor, 0x01); j++;
	ST7735_DrawChar(xcoord+5*j+xlevel, ycoord+ychar+ylevel, ones_place, 0xFFFF, mainColor, 0x01); j++;
	ST7735_DrawBitmapT(xcoord+xmarg,ycoord+ymarg,UserPokemon[spot].Images[2]->imagePt,UserPokemon[spot].Images[2]->width,UserPokemon[spot].Images[2]->height);
	
	return 0;
}

//
//									PokemonMenu_ActionFunctions
//*******************************************************
static int PokemonMenu_SPOTX(void) {
	//prompt user if he wants to shift pokemon or cancel
	printTwoOptionMenu(Shift, Cancel);
	
	return SHIFTMENU;
}

static int PokemonMenu_CANCEL(void) {
	printBattleScreen();
	printRect(0,88,160,40,black,brownGoldBd,jadedGreen);
	printBattleMenu();
	return BATTLEMENU;
}
					

//
//							BagMenu_Controller();
//*******************************************************

static int BagMenu_Controller() {
	unsigned int xdir;
	unsigned int ydir;
	cs = 0;
	
	while(1) {
		Joystick_In(&xdir, &ydir);
		BagMenu_MoveCursor(Joystick_getDirection(xdir,ydir));
		if(AisPushed()) {
			while(AisPushed());
			return BagMenu[cs].AButton();
		}
		if(BisPushed()) {
			while(BisPushed());
			return BagMenu[cs].BButton();
		}
		Delay1(20);
	}
}

static int BagMenu_MoveCursor(int dir) {
		if(dir == 4 || BagMenu[cs].nextSt[dir] == -1);
		else {
			ST7735_DrawBitmap(BagMenu[BagMenu[cs].nextSt[dir]].x,BagMenu[BagMenu[cs].nextSt[dir]].y,BagMenu_Cursor,6,10);
			ST7735_FillRect(BagMenu[cs].x,BagMenu[cs].y-9,6,10,bgColor);
			cs = BagMenu[cs].nextSt[dir];
		}
		return 0;
}
//
//									Bag_ActionFunctions
//*******************************************************

//
//							TwoOptionMenu_Controller();
//*******************************************************
static int TwoOptionMenu_Controller(unsigned char menu) {
	unsigned int xdir;
	unsigned int ydir;
	
	TwoOptionMenu_spot = cs;
	cs = 0;
	while(1) {
		Joystick_In(&xdir,&ydir);
		TwoOptionMenu_MoveCursor(Joystick_getDirection(xdir,ydir),menu);
		if(AisPushed()) {
			while(AisPushed());
			switch(menu) {
				case SHIFTMENU:
					return ShiftMenu[cs].AButton();
				case RUNMENU:
					return RunMenu[cs].AButton();
				case CHANGEPOKEMON:
					return ChangePokemon[cs].AButton();
				default:
					return ERROR;
			}
		}
		if(BisPushed()) {
			while(BisPushed());
			switch(menu) {
				case SHIFTMENU:
					return ShiftMenu[cs].BButton();
				case RUNMENU:
					return RunMenu[cs].BButton();
				case CHANGEPOKEMON:
					return ChangePokemon[cs].BButton();
				default:
					return ERROR;
			}
		}
		Delay1(5);
	}
}
static int TwoOptionMenu_MoveCursor(int dir, unsigned char menu) {
	if(dir == 4 || ShiftMenu[cs].nextSt[dir] == -1);
	else {
		switch(menu) {
			case SHIFTMENU:
				ST7735_DrawBitmap(ShiftMenu[ShiftMenu[cs].nextSt[dir]].x,ShiftMenu[ShiftMenu[cs].nextSt[dir]].y,Battle_Cursor,6,10);
				ST7735_FillRect(ShiftMenu[cs].x,ShiftMenu[cs].y-9,6,10,bgColor);
				cs = ShiftMenu[cs].nextSt[dir];
			break;
			case RUNMENU:
				ST7735_DrawBitmap(RunMenu[RunMenu[cs].nextSt[dir]].x,RunMenu[RunMenu[cs].nextSt[dir]].y,Battle_Cursor,6,10);
				ST7735_FillRect(RunMenu[cs].x,RunMenu[cs].y-9,6,10,bgColor);
				cs = RunMenu[cs].nextSt[dir];
			break;
			case CHANGEPOKEMON:
				ST7735_DrawBitmap(ChangePokemon[ChangePokemon[cs].nextSt[dir]].x,ChangePokemon[ChangePokemon[cs].nextSt[dir]].y,Battle_Cursor,6,10);
				ST7735_FillRect(ChangePokemon[cs].x,ChangePokemon[cs].y-9,6,10,bgColor);
				cs = ChangePokemon[cs].nextSt[dir];
			default:
				return ERROR;
		}
	}
	return 0;
}
static void SayDialogue(char * dialogue, unsigned short txtColor, unsigned short bgColor) {
	int i = 0;
	int ymarg = 5;
	
	//while (AisPushed() || BisPushed()){};
	//while (!AisPushed() && !BisPushed()){};
	
	while (dialogue[i] != 0) {
		ST7735_DrawChar(cursorX+xmargin, cursorY+ymarg, dialogue[i],txtColor,bgColor,0x01);
		if (BisPushed() || AisPushed()) {
			Delay1(30);
		} else {
			Delay1(5);
		}
		i++;
		cursorX += 6;
		if ((cursorX > 110 && dialogue[i] == ' ') || cursorX > 140) {
			cursorX = 5; 
			cursorY += 10;
			if (cursorX > 120) {
				while (AisPushed() || BisPushed()){};
				while (!AisPushed() && !BisPushed()){};
				ST7735_FillRect(0, 88, 160, 40, 0xffff);
				cursorY = 93;
			}
			if (dialogue[i] == ' ') {
				i++;
			}
		}
	}
	
}

//
//							ShiftMenu_ActionFunctions
//*******************************************************
static int ShiftMenu_CANCEL(void) {
	printPokemonMenu();
	return POKEMONMENU;
}
static int ShiftMenu_SHIFT(int spot) {
	
	//#! MAKE A CHECK FOR USERPOKEMON[0]
	if(spot == 0) {
		/*cursorX = 5;
		cursorY = 93;
		printRect(0,88,160,40,black,brownGoldBd,jadedGreen); */
		return POKEMONMENU;
	}
	
	Pokemon_t temp = UserPokemon[0];
	UserPokemon[0] = UserPokemon[spot];
	UserPokemon[spot] = temp;
	
	ST7735_FillScreen(black);
	Delay1(1);
	
	ST7735_FillScreen(0xFFFF);
	printEnemyHealthBox();
	ST7735_DrawBitmap(110, 40, EnemyPokemon[0].Images[1]->imagePt, EnemyPokemon[0].Images[1]->width, EnemyPokemon[0].Images[1]->height);
	printDialogue(UserPokemon[spot].name," that's enough! Come back!\0", "\0","\0","\0");
	Delay1(1);
	
	printUserHealthBox();
	ST7735_DrawBitmap(3, 88, UserPokemon[0].Images[0]->imagePt, UserPokemon[0].Images[0]->width, UserPokemon[0].Images[0]->height);
	
	return BATTLEMENU;
}

static int ShiftMenu_SHIFT_(void) {
	return ShiftMenu_SHIFT(TwoOptionMenu_spot);
}

//
//
//*******************************************************
//#! CHECK IF CORRECT INDEXING
static int Battle_RunFightSequence(void) {
	int i;
	int chooseMove[4] = {0, 0, 0, 0};
	int j = 0;
	
	int random = Random() % 10;
	switch(random) {
		case 0:
		case 1:
		case 2:
			for(i = 0; i < 4; i++) {
				if(EnemyPokemon[0].moveSet[i]->category != NULL && EnemyPokemon[0].moveSet[i]->category >= O_ATT_STATUS) {
					chooseMove[i] = i+1;
					j++;
				}
			}
			break;
		default:
			for(i = 0; i < 4; i++) {
				if(EnemyPokemon[0].moveSet[i]->name[0] != '-' && EnemyPokemon[0].moveSet[i]->category < O_ATT_STATUS) {
					chooseMove[i] = i+1;
					j++;
				}
			}
		break;
	} 
	int chosenMove = 0;
	while(!chooseMove[chosenMove])
		chosenMove = (Random() % i) + 1;
	
	printRect(0,88,160,40,black,ST7735_Color565(200,168,72),jadedGreen);
	cursorX = 5;
	cursorY = 93;
	printDialogue("Foe \0",EnemyPokemon[0].name," used \0",EnemyPokemon[0].moveSet[chosenMove]->name,"!");
	
	if(EnemyPokemon[0].moveSet[chosenMove]->category >= O_ATT_STATUS) {
		printRect(0,88,160,40,black,ST7735_Color565(200,168,72),jadedGreen);
		chooseSTATUS(EnemyPokemon[0].moveSet[chosenMove]->category, EnemyPokemon[0], UserPokemon[0]);
	}
	else {
		damage = calcDam(EnemyPokemon[0], UserPokemon[0],chosenMove);
		if(!(Battle_UpdateUserHealth(UserPokemon[0].health - damage))) {
			UserPokemon[0].health = 0;
		} else {
			UserPokemon[0].health -= damage;
		}
		
		if(Critical == 2) {
			printRect(0,88,160,40,black,ST7735_Color565(200,168,72),jadedGreen);
			cursorX = 5;
			cursorY = 93;
			printDialogue("Critical hit!\0","\0","\0","\0","\0");
			while(!AisPushed());
			while(AisPushed());
		} 
		
		if(typeEffectiveness);
		else {
			printRect(0,88,160,40,black,ST7735_Color565(200,168,72),jadedGreen);
			cursorX = 5;
			cursorY = 93;
			if(typeEffectiveness == 0.5) {
				printDialogue("Its not very effective...\0","\0","\0","\0","\0");
			} else if(typeEffectiveness == 1.5) 
				printDialogue("Its super effective!\0","\0","\0","\0","\0");
		}
	}
	while(!AisPushed());
	while(AisPushed());
	
	if(UserPokemon[0].health <= 0) {
		for(i = 0; i < User.numPokemon; i++) {
			if(UserPokemon[i].health > 0) {
				printPokemonMenu();
				return POKEMONMENU;
			} else {
				youLose = 1;
				return RUNENDSEQ;
			}
		}
	}
	
	return BATTLEMENU;
}

static int Battle_LoadScreen(Person_t trainer) {
	int i;
	int rand = Random()%5;
	youLose = 0;
	youWin = 0;
	
	User = *(Player.battleInfo);
	for(i = 0; i < User.numPokemon; i++) {
		UserPokemon[i] = *(Player.battleInfo->pokemon[i]);
	} 
	
	if(trainer.battleInfo->numPokemon == 0) {
		
		catchable = 1;
		EnemyPokemon[0] = *(WildGrass[rand]);
		if(EnemyPokemon[0].level > UserPokemon[0].level) {
			//#! implement EXP POINTS
			UserPokemon[0].exp = (UserPokemon[0].level - EnemyPokemon[0].level) * 10;
		} else
			UserPokemon[0].exp = 10;
		
	} else {
		
		Enemy = *(trainer.battleInfo);
		for(i = 0; i < Enemy.numPokemon; i++) {
			EnemyPokemon[i] = *(Enemy.pokemon[i]);
		}
		catchable = 0;
		// #! gainedXP = Enemy.gainedExp;
	}
	
	gainedXP = 10;
	
	for(i = 0; i < 4; i++) {
		ST7735_FillScreen(0x0000);
		Reload_Map();
		Delay1(5);
	}
	
	ST7735_FillScreen(0xFFFF);
	printRect(0,88,160,40,black,brownGoldBd,jadedGreen);
	Delay1(1);
	Image_t *entImage;
	if(trainer.battleInfo->numPokemon == 0) {	//if its a wild pokemon
		entImage = EnemyPokemon[0].Images[1];
	} else {
		entImage = trainer.battleInfo->Images[0];
	}
	
	x = 140;
	int x1 = 0;
	while(x > 10 && x1 < 150-entImage->width) {
		ST7735_FillRect(x+10,40,36,49,0xFFFF);
		ST7735_DrawBitmap(x,88,Ash_Back1,36,48);
		ST7735_FillRect(x1-10,40-entImage->height,entImage->width,entImage->height,0xFFFF);
		ST7735_DrawBitmap(x1,40,entImage->imagePt,entImage->width,entImage->height);
		Delay1(6);
		x -= 10;
		x1 += 10;
	}
		cursorX = 5;
		cursorY = 93;
		//printRect(0,88,160,40,black,brownGoldBd,jadedGreen);
	if(trainer.battleInfo->numPokemon == 0) {
		printDialogue("A wild \0", EnemyPokemon[0].name," appeared!\0","\0","\0");
	} else {
		printDialogue(Enemy.name," would like to battle!\0","\0","\0","\0");
		y = 30;
		w = 50;
		x = w;
		ST7735_DrawFastHLine(0,y,w,black);
		for(i = 0; i < 6; i++) {
			if(i < Enemy.numPokemon) {
				ST7735_DrawBitmap(x-10,y-3,Small_Pokeball,7,7);
				x -= 10;
			}
		}
		
	}
	
	printEnemyHealthBox();
	
	while(!AisPushed());
	while(AisPushed());
	printRect(0,88,160,40,black,brownGoldBd,jadedGreen);
	printDialogue("Go! \0", UserPokemon[0].name,"!\0","\0","\0");
	ST7735_FillRect(0,40,88,48,0xFFFF);
	ST7735_DrawBitmap(x,88,Ash_Back2,63,44);
	Delay1(3);
	ST7735_FillRect(0,40,88,48,0xFFFF);
	ST7735_DrawBitmap(x-1,88,Ash_Back3,52,47);
	Delay1(3);
	ST7735_FillRect(0,40,88,48,0xFFFF);
	ST7735_DrawBitmap(x-2,88,Ash_Back4,64,49);
	Delay1(3);
	ST7735_FillRect(0,40,88,48,0xFFFF);
	ST7735_DrawBitmap(x-3,88,Ash_Back5,55,43);
	Delay1(3);
	ST7735_FillRect(0,40,88,48,0xFFFF);
	Delay1(2);
		
	ST7735_DrawBitmap(x,88,UserPokemon[0].Images[0]->imagePt,UserPokemon[0].Images[0]->width,UserPokemon[0].Images[0]->height);
	printUserHealthBox();
	printBattleMenu();
	return BATTLEMENU;
}

static int Battle_EndConditions() {
	if(success) return RUNENDSEQ;
	
	cursorX = 5;
	cursorY = 93;
	printRect(0,88,160, 40,black,brownGoldBd,jadedGreen);
	
	if(youWin) {
	//#! TEST TO SEE IF INT TO STRING CONVERTER WORKS (IT DOESNT)
		if(User.numPokemon > 0) {
			printDialogue("Player defeated \0", Enemy.name, "\0","\0","\0");
			while(!AisPushed());
			while(AisPushed());
			printDialogue(User.name," got $",Reward ,"500 for winning!","\0");
			while(!AisPushed());
			while(AisPushed());
			Battle_UpdateExp();
			cvString(gainedXP);
			printRect(0,88,160, 40,black,brownGoldBd,jadedGreen);
			printDialogue(UserPokemon[0].name, " gained ", gainedXPs," EXP. points!","\0");
		} else {
			printDialogue("Wild \0", EnemyPokemon[0].name," fainted!\0", "\0","\0");
			while(!AisPushed());
			while(AisPushed());
			printDialogue(UserPokemon[0].name, " gained ", gainedXPs," EXP. points!","\0");
		} 
	}
	
	if(youLose) {
		printDialogue(UserPokemon[0].name," fainted!","\0", "\0","\0");
		while(!AisPushed());
		while(AisPushed());
		for(int i = 0; i < User.numPokemon; i++) {
			if(UserPokemon[i].health > 0) {
				printPokemonMenu();
				return POKEMONMENU;
			} else {
				cursorX = 5;
				cursorY = 93;
				printRect(0,88,160, 40,black,brownGoldBd,jadedGreen);
				printDialogue("You ran out of usable Pokemon!\0","\0", "\0","\0","\0");
				while(!AisPushed());
				while(AisPushed());
				printRect(0,88,160, 40,black,brownGoldBd,jadedGreen);
				printDialogue(User.name, " scurried away..\0","\0", "\0","\0");
			}
		}
	}
	return RUNENDSEQ;
}

static int Battle_ExitScreen(Person_t trainer) {
	
	while(!AisPushed());
	while(AisPushed());
	
	ST7735_FillScreen(0x0000);
	for(int i = 0; i < Player.battleInfo->numPokemon; i++) {
		//#! NEED TO REMEMBER ORIGINAL ORDER OR ELSE SHIFTING POKEMON WILL MESS THIS UP
		Player.battleInfo->pokemon[i]->health = UserPokemon[i].health;
		Player.battleInfo->pokemon[i]->exp = UserPokemon[i].exp;
		Player.battleInfo->pokemon[i]->level = UserPokemon[i].level;
	}
	Delay1(1);
	Reload_Map();
	return 0;
}

//
//								Layout_Functions
//*******************************************************
static int printHP(int health, int xcoord, int ycoord) {
	
	unsigned short color;
	if(health < 8)
				color = healthRed;
			 else if(health < 30)
				color = healthYellow;
			 else
				color = green;
	
	ST7735_FillRect(xcoord,ycoord,health,3,color);
	ST7735_FillRect(xcoord+health,ycoord,MAX_HEALTH-health,3, grey);
	return 0;
}
static int Battle_UpdateUserHealth(int newHealth) {
	
	if(newHealth > 48)
		return -1;
	if(newHealth < 0)
		newHealth = 0;
	
	int health, difference;
	health = UserPokemon[0].health;
	difference = UserPokemon[0].health - newHealth;
	if(difference < 0) {
		difference ^= -1;
		difference++;
	}
	
		x = 99;
		y = 72;
	
		while(health != newHealth) {
			printHP(health,x,y);
				
			if(health > newHealth)
				health--;
			else
				health++;
				
			//Delay1
			for (int i = 0; i < 800000; i++);
		}
	return newHealth;
}

static int Battle_UpdateEnemyHealth(int newHealth) {
	//enemy x= 3,y=32;
	//Player x=52, y=85;
	
	if(newHealth > 48)
		newHealth = 48;
	if(newHealth < 0)
		newHealth = 0;
	
	int health;
	health = EnemyPokemon[0].health;

		x = 43;
		y = 20;
		
		unsigned short color;
		while(health != newHealth) {
			
			if(health < 8)
				color = healthRed;
			 else if(health < 30)
				color = healthYellow;
			 else
				color = green;
	
			ST7735_FillRect(x,y,health,3,color);
			ST7735_FillRect(x+health,y,MAX_HEALTH-health,3, grey);
			
			if(health > newHealth)
				health--;
			else
				health++;
			for(int i = 0; i < 800000; i++);
		}
	return newHealth;
}

static int Battle_UpdateExp() {
	int XP = UserPokemon[0].exp;
	int newXP = XP + gainedXP;
	int XP_barlength = 48;
	
	while(XP != newXP) {
			/*	while(XP != XP_barlength) {
					ST7735_FillRect(110,80,XP,2,expBlue);
					ST7735_FillRect(110+XP,80,MAX_EXP-XP,3, grey);
					XP++;
					for(int i = 0; i < 800000; i++);
				}
				//#! LEVEL UP IMPLEMENTATION
				UserPokemon[0].level++;
				gainedXP -= XP_barlength;
			} */
		ST7735_FillRect(110,80,XP,2,expBlue);
		ST7735_FillRect(110+XP,80,MAX_EXP-XP,3, grey);
		XP++;
		for(int i = 0; i < 800000; i++);
		if(XP == XP_barlength) {
			newXP -= XP_barlength;
			XP = 0;
		}
	}
	return 0;
}



static int printRect(int xcoord, int ycoord, int width, int height, unsigned short bgColor, unsigned short bdColor, unsigned short mainColor) {
	x = xcoord;
	y = ycoord;
	w = width;
	h = height;
	
	ST7735_FillRect(x,y,w,h,bgColor);
	xmargin = 2;
	ymargin = 2;
	ST7735_FillRect(x+xmargin,y+ymargin,w-2*xmargin,h-2*ymargin,bdColor);
	xmargin = 4;
	ymargin = 4;
	ST7735_FillRect(x+xmargin,y+ymargin,w-2*xmargin,h-2*ymargin,mainColor);
	
	return 0;
}

static int printEnemyHealthBox() {
	int xmargin, ymargin;
	int i = 0;
	int tens_place = EnemyPokemon[0].level / 10 + 0x30;
	int ones_place = EnemyPokemon[0].level % 10 + 0x30;
		
	x = 3;
	y = 32;
	xmargin = x + 7;
	ymargin = y - 24;
	
	//print background
	ST7735_DrawBitmap(x,y,HealthBox_Enemy,103,32);
		
	//print name
	while(EnemyPokemon[0].name[i] != '\0') {
		ST7735_DrawChar(xmargin+(5*i),ymargin,EnemyPokemon[0].name[i],txtColor,bgColor, 0x01);
		i++;
	}
		
	//print level, assumes level is less than 100
	if(EnemyPokemon[0].level < 100) {
			
		ST7735_DrawChar(xmargin+55, ymargin, 'L', txtColor, bgColor, 0x01);
		ST7735_DrawChar(xmargin+60, ymargin, 'v', txtColor, bgColor, 0x01);
		ST7735_DrawChar(xmargin+65, ymargin, '.', txtColor, bgColor, 0x01);

		ST7735_DrawChar(xmargin+70, ymargin, tens_place, txtColor, bgColor, 0x01);
		ST7735_DrawChar(xmargin+75, ymargin, ones_place, txtColor, bgColor, 0x01);
	}
	
	if(EnemyPokemon[0].health < 0 || EnemyPokemon[0].health > 48)
		return 1;
	
	int  health, difference;
	health = MAX_HEALTH;
	difference = MAX_HEALTH - EnemyPokemon[0].health;
	if(difference < 0) {
		difference ^= -1;
		difference++;
	}
	
	while(health != EnemyPokemon[0].health) {
		ST7735_FillRect(x+40,y-12,EnemyPokemon[0].health,3,green);
		ST7735_FillRect(x+40+EnemyPokemon[0].health,y-12,difference,3, grey);
		
		health--;
	}
	return 0;
}

static int printUserHealthBox() {
	int i = 0;
	int tens_place = UserPokemon[0].level / 10 + 0x30;
	int ones_place = UserPokemon[0].level % 10 + 0x30;
	
	x = 52;
	y = 85;
	
	xmargin = x + 15;
	ymargin = y - 24;
		
	ST7735_DrawBitmap(x,y,HealthBox_User,104,30);
		
	while(UserPokemon[0].name[i] != '\0') {
		ST7735_DrawChar(xmargin+(5*i),ymargin,UserPokemon[0].name[i],txtColor,bgColor, 0x01);
		i++;
	}
		
	if(UserPokemon[0].level < 100) {
			
		ST7735_DrawChar(xmargin+55, ymargin, 'L', txtColor, bgColor, 0x01);
		ST7735_DrawChar(xmargin+60, ymargin, 'v', txtColor, bgColor, 0x01);
		ST7735_DrawChar(xmargin+65, ymargin, '.', txtColor, bgColor, 0x01);

		ST7735_DrawChar(xmargin+70, ymargin, tens_place, txtColor, bgColor, 0x01);
		ST7735_DrawChar(xmargin+75, ymargin, ones_place, txtColor, bgColor, 0x01);
	}
	
	ST7735_FillRect(x+32,y-3,UserPokemon[0].exp,2,ST7735_Color565(64,200,248));
	
	if(UserPokemon[0].health < 0 || UserPokemon[0].health > 48)
		return 1;
	
	int health, difference;
	health = MAX_HEALTH;
	difference = MAX_HEALTH - UserPokemon[0].health;
	if(difference < 0) {
		difference ^= -1;
		difference++;
	}
		
		while(health != UserPokemon[0].health) {
			printHP(health,x+47,y-13);
			health--;
				
			for (int i = 0; i < 1000; i++);
		}
	return 0;
}

static int printMoveMenu() {
	
	printRect(0,88,110,40,black,bleakPurple,bgColor);
	
	xmargin = 15;
	ymargin = 10;
	int j;
	for(int i = 0; i < 4; i++) {
		j = 0;
		while(UserPokemon[0].moveSet[i]->name[j] != '\0') {
			ST7735_DrawChar(x+xmargin+(5*j),y+ymargin,UserPokemon[0].moveSet[i]->name[j],txtColor,bgColor,0x01);
			j++;
		}
		if(i % 2 == 0)
			xmargin += 50;
		else {
			ymargin += 13;
			xmargin -= 50;
		}
	}
	ST7735_DrawBitmap(8,106,Battle_Cursor,6,10);
	return 0;
}

static int printEmptyPokemonBox(int xcoord, int ycoord, int width, int height, unsigned short OutCol, unsigned short InCol) {
	
	for(int i = 0; i < 2; i++) {
		ST7735_DrawFastHLine(xcoord,ycoord,width,OutCol);
		ST7735_DrawFastHLine(xcoord,ycoord+height,width,OutCol);
		ST7735_DrawFastVLine(xcoord,ycoord,height,OutCol);
		ST7735_DrawFastVLine(xcoord+width-1,ycoord,height,OutCol);
		xcoord=xcoord+1;
		ycoord=ycoord+1;
		width=width-2;
		height=height-2;
	}
	
	ST7735_DrawFastHLine(xcoord,ycoord,width,InCol);
	ST7735_DrawFastHLine(xcoord,ycoord+height,width,InCol);
	ST7735_DrawFastVLine(xcoord,ycoord,height,InCol);
	ST7735_DrawFastVLine(xcoord+width,ycoord,height,InCol);
	
	return 0;
}

static int printPokemonMenu() {
	//********* print background ********
	x = 20;
	y = 6;
	ST7735_FillScreen(turquoise);	//background
	ST7735_FillRect(x,y,160-2*y,118,ST7735_Color565(56,136,136));
	
	w = 20;
	h = 118;
	int i = 0;
	while(i < h) {
		ST7735_DrawFastHLine(x,y+i,160-x,ST7735_Color565(72,168,160));
		i += 2;
	}
	
	//********* print first pokemon box ********
	PokemonMenu_Highlight(0,highlightOrange,highlightOrange,ST7735_Color565(120,208,232));
	
	//********* print the rest of the boxes ********
	x=67;
	y=7;
	w=90;
	h=20;
	for(i = 0; i < 5; i++) {
		printEmptyPokemonBox(x,y+(23*i),90,20,ST7735_Color565(72,72,96),ST7735_Color565(64,200,248));
	}
	
	for(i = 1; i < User.numPokemon; i++) {
		PokemonMenu_Highlight(i,ST7735_Color565(72,72,96),ST7735_Color565(64,200,248),ST7735_Color565(56,144,216));
	}

	//******* Draw Cancel Button *******
	//#! IMPLEMENT CANCEL BUTTON
	x = 5;
	y = 95;
	w = 55;
	h = 30;
	
	printRect(x,y,w,h,ST7735_Color565(72,72,96),ST7735_Color565(72,72,96),ST7735_Color565(112,88,176));
	
	return 0;
}
static int printPPMenu() {
	printRect(110,88,50,40,black,bleakPurple,bgColor);
	
	// #! PRINT PP AND TYPE
	
	
	return 0;
}
static int printBattleMenu() {
	printRect(55,88,105,40,black,bleakPurple,bgColor);
	
	xmargin = 16;
	ymargin = 10;
	int j;
	for(int i = 0; i < 4; i++) {
		j = 0;
		while(battleMenuWords[i][j] != '\0') {
			ST7735_DrawChar(x+xmargin+(5*j),y+ymargin,battleMenuWords[i][j],txtColor,bgColor,0x01);
			j++;
		}
		if(i % 2 == 0)
			xmargin += 56;
		else {
			ymargin += 13;
			xmargin -= 56;
		}
	}
	ST7735_DrawBitmap(BattleMenu[0].x,BattleMenu[0].y,Battle_Cursor,6,10);
	return 0;
}

static int printBattleScreen() {
	ST7735_FillScreen(0xFFFF);
	printUserHealthBox();
	ST7735_DrawBitmap(3, 88, UserPokemon[0].Images[0]->imagePt, UserPokemon[0].Images[0]->width, UserPokemon[0].Images[0]->height);
	printEnemyHealthBox();
	ST7735_DrawBitmap(110, 40, EnemyPokemon[0].Images[1]->imagePt, EnemyPokemon[0].Images[1]->width, EnemyPokemon[0].Images[1]->height);
	return 0;
}

static int printBagMenu() {
	//#! PRINT THE BAG MENU
	//Person_t needs to be able to hold items, pokeballs, and *potions
	ST7735_FillScreen(bgBagGreen);
	int i;
	for(i = 0; i < 128; i+= 2) {
		ST7735_DrawFastHLine(0,i,160,bagGreen);
	}
	x = 50;
	y = 2,
	w = 110;
	h = 86;
	printRect(x,y,w,h,bgBagGrey,bdBagTan,mainBgPeach);	
	
	printRect(0,88,160,40, blueBagBg, blueBagBd, blueBagMain);
	ST7735_DrawBitmap(BagMenu[0].x, BagMenu[0].y,BagMenu_Cursor,6,10);
	
	i=0;
	while(pokeball[i] != '\0') {
		ST7735_DrawChar(BagMenu[0].x+10+5*i,BagMenu[0].y-8,pokeball[i],black,mainBgPeach,0x01);
		i++;
	}
	return 0;
}

static int printTwoOptionMenu(char *firstOption, char *secondOption) {
	int i = 0;
	x=90;
	y=88;
	w = 70;
	h = 40;
	printRect(x,y,w,h,black,bleakPurple,bgColor);
	
	xmargin = 18;
	ymargin = 10;
	while(firstOption[i] != '\0') {
		ST7735_DrawChar(x+xmargin+(5*i),y+ymargin,firstOption[i],black, bgColor, 0x01);
		ST7735_DrawChar(x+xmargin+(5*i),y+ymargin+12,secondOption[i],black,bgColor,0x01);
		i++;
	}
	ST7735_DrawBitmap(98,106,Battle_Cursor,6,10);
	return 0;
}

static int throwPokeBall() {
	
	//ST7735_FillScreen(0xFFFF);
	printBattleScreen();
	printRect(0,88,160,40,black,brownGoldBd,jadedGreen);
	x = 110;
	y = 20;
	w = 12;
	h = 13;
	
	ST7735_DrawBitmapT(x,y,Pokeball_Closed,12,12);
	Delay1(1);
	Delay1(1);
	ST7735_DrawBitmapT(x,y,Pokeball_Open, w,h);
	Delay1(1);
	Delay1(1);
	
	success = Random() % 10;
	switch(success) {
		case 0:
		case 1:
		case 2:
		case 3:
			success = 1;
		break;
		default:
			success = 0;
	}
	
	ST7735_DrawBitmapT(x,y,Pokeball_Closed,12,12);
	Delay1(1);
	Delay1(1);
	//ST7735_FillRect(x,y-h,w,h,0xFFFF);
	
	if(success) {
		if(User.numPokemon < 6) {
			UserPokemon[User.numPokemon] = EnemyPokemon[0];
			User.numPokemon++;
			cursorX = 5;
			cursorY = 93;
			printRect(0,88,160,40,black,brownGoldBd,jadedGreen);
			printDialogue("Congratualtions! you caught \0", EnemyPokemon[0].name, "!\0", "\0", "\0");
			printRect(0,88,160,40,black,brownGoldBd,jadedGreen);
			printDialogue(EnemyPokemon[0].name, " has been added to your Pokemon! ", "\0", "\0", "\0");
		} else {
			success = 0;
		}
	}
	
	if(!success) {
			cursorX = 5;
			cursorY = 93;
			printRect(0,88,160,40,black,brownGoldBd, jadedGreen);
			printDialogue("Wild \0", EnemyPokemon[0].name, " escaped!\0", "\0", "\0");
			//printDialogue(EnemyPokemon[0].name, " has been added to your Pokemon ", "!\0", "\0", "\0");
	}
	
	return RUNENDSEQ;
}
//use own cursorX and cursorY
static int printDialogue(char *string1, char *string2, char *string3,char *string4, char *string5) {
	cursorX = 5;
	cursorY = 93;
	while(!AisPushed() && !BisPushed());
	while(AisPushed() || BisPushed());
	SayDialogue(string1,0xFFFF,jadedGreen);
	SayDialogue(string2,0xFFFF,jadedGreen);
	SayDialogue(string3, 0xFFFF,jadedGreen);
	SayDialogue(string4, 0xFFFF,jadedGreen);
	SayDialogue(string5, 0xFFFF,jadedGreen);
	while(!AisPushed() && !BisPushed());
	while(AisPushed() || BisPushed());
	return 0;
}

//initialize colors and other static variables
void Battle_Init(void) {
	jadedGreen = ST7735_Color565(40,80,104);
	green = ST7735_Color565(112,248,168);
	grey = ST7735_Color565(80,104,88);
	txtColor = ST7735_Color565(64,64,64);
	bgColor = ST7735_Color565(248,248,216);
	turquoise = ST7735_Color565(32,104,96);
	bleakPurple = ST7735_Color565(112,104,128);
	healthRed = ST7735_Color565(208,40,40);
	healthYellow = ST7735_Color565(248,224,56);
	highlightOrange = ST7735_Color565(248,112,48);
	brownGoldBd = ST7735_Color565(200,168,72);
	black = 0x0000;
	expBlue = ST7735_Color565(64,200,248);
	faintRed = ST7735_Color565(192,104,15);
	bgBagGrey = ST7735_Color565(107,105,107);
	bdBagTan = ST7735_Color565(247,203,115);
	mainBgPeach = ST7735_Color565(255,251,206);
	blueBagBg = ST7735_Color565(0,81,115);
	blueBagBd = ST7735_Color565(16,170,222);
	blueBagMain = ST7735_Color565(0,121,198);
	bgBagGreen = ST7735_Color565(107,203,198);
	bagGreen = ST7735_Color565(66,178,165);
	
	*Reward = Random()%10 + 0x30;
	gainedXP = UserPokemon[0].level - EnemyPokemon[0].level;
	if(gainedXP < 1) gainedXP = 1;
	gainedXP = (gainedXP * gainedXP) + 8;
	top = stack;
}

static void push(int num) {
	*top = num;
	top++;
}
static int pop() {
	top--;
	return *top;
}

static void cvString(int num) {
	push(num);
	if(num < 10) {
		*gainedXPs = pop() + 0x30;
		gainedXPs++;
	} else {
		cvString(num / 10);
		*gainedXPs = (pop() % 10) + 0x30;
		gainedXPs++;
	}
}

static void Delay1(int speed) {
	int i;
	for (i = 6000000 / speed; i > 0; i--);
}; 

static int catchError() {
	cursorX = 5;
	cursorY = 93;
	printRect(0,88,160,40,black,brownGoldBd,jadedGreen);
	printDialogue("Oops, something went wrong!", "\0", "\0","\0","\0");
	while(!AisPushed());
	while(AisPushed());
	return BATTLEMENU;
}


static int calcDam(Pokemon_t User, Pokemon_t Foe, int move) {
	int temp;
	
	if(User.type == User.moveSet[move]->type)
		STAB = 1.5;
	else
		STAB = 1;
	
	switch(User.moveSet[move]->type) {
		case FIRE: typeEffectiveness = Fire(Foe.moveSet[move]->type); break;
		case WATER: typeEffectiveness = Water(Foe.moveSet[move]->type); break;
		case GRASS: typeEffectiveness = Grass(Foe.moveSet[move]->type); break;
		case NORMAL: typeEffectiveness = 1; break;
	}
	
	Critical = 1;
	rand = Random() % 10;
	if(!rand) {
		Critical = 2;
		rand = 1;
	}
	
	rand = 1 - rand/100;
	if(User.moveSet[cs]->category == PHYSICAL)
		temp = ((2*User.level+10)/250)*(User.baseStats[ATTACK]/Foe.baseStats[DEFENSE]) * User.moveSet[move]->power + 2;
	else
		temp = ((2*User.level+10)/250)*(User.baseStats[SPATTACK]/Foe.baseStats[SPDEFENSE]) * User.moveSet[move]->power + 2;
	temp = temp * STAB * typeEffectiveness * Critical * rand * 5;
	return temp;
}

#define EFFECTIVE 	1
#define NOT 				0.5
#define SUPER				2
static double Fire(unsigned char enemytype) {
	
	switch(enemytype) {
		case FIRE: return EFFECTIVE;
		case WATER: return NOT;
		case GRASS: return SUPER;
	}
	return 0;
}
static double Water(unsigned char enemytype) {
	switch(enemytype) {
		case FIRE: return SUPER;
		case WATER: return EFFECTIVE;
		case GRASS: return NOT;
	}
	return 0;
}
static double Grass(unsigned char enemytype) {
	switch(enemytype) {
		case FIRE: return NOT;
		case WATER: return SUPER;
		case GRASS: return EFFECTIVE;
	}
	return 0;
}

static int chooseSTATUS(int status, Pokemon_t User, Pokemon_t Foe) {
		switch(status) {
			case O_ATT_STATUS:
					Foe.baseStats[ATTACK] -= 10;
					printDialogue(User.name," lowered \0", Foe.name, " ATTACK!\0","\0");
				break;
			case O_DEF_STATUS:
					Foe.baseStats[DEFENSE] -= 10;
					printDialogue(User.name," lowered \0", Foe.name, " DEFENSE!\0","\0");
				break;
			case U_ATT_STATUS:
					User.baseStats[ATTACK] += 10;
					printDialogue(User.name," raised his ATTACK!\0","\0","\0", "\0");
				break;
			case U_DEF_STATUS:
					User.baseStats[DEFENSE] += 10;
					printDialogue(User.name," raised his DEFENSE!\0","\0","\0", "\0");
				break;
			case O_SPA_STATUS:
					Foe.baseStats[ATTACK] -= 10;
					printDialogue(User.name," lowered \0", Foe.name, " SP. ATTACK!\0","\0");
				break;
			case O_SPD_STATUS:
					Foe.baseStats[SPDEFENSE] -= 10;
					printDialogue(User.name," lowered \0", Foe.name, " SP. DEFENSE!\0","\0");
				break;
			case U_SPA_STATUS:
					User.baseStats[SPATTACK] += 10;
					printDialogue(User.name," raised his SP. ATTACK!\0","\0","\0", "\0");
				break;
			case U_SPD_STATUS:
					User.baseStats[SPDEFENSE] += 10;
					printDialogue(User.name," raised his SP. DEFENSE!\0","\0","\0", "\0");
				break;
		}
		return 0;
}

static int switchEnemyPokemon() {
	cursorX = 5;
	cursorY = 93;
	printRect(0,88,160,40,black,brownGoldBd,jadedGreen);
	printDialogue("Foe \0", EnemyPokemon[0].name, " fainted!\0", "\0","\0");
	printDialogue(UserPokemon[0].name, " gained \0", gainedXPs, " EXP. Points!\0", "\0");
	
	int nextPokemon;
	for(int i = 0; i < Enemy.numPokemon; i++) {
		if(EnemyPokemon[i].health > 0)
			nextPokemon = i;
	}
	
	Pokemon_t temp;
	temp = EnemyPokemon[0];
	EnemyPokemon[0] = EnemyPokemon[nextPokemon];
	EnemyPokemon[nextPokemon] = temp;
	
	cursorX = 5;
	cursorY = 93;
	printRect(0,88,160,40,black,brownGoldBd,jadedGreen);
	printDialogue(Enemy.name, " is about to use \0",EnemyPokemon[0].name, "\0","\0");
	printDialogue("Will \0", User.name, " change Pokemon?\0", "\0", "\0");
	
	return 0;
}

static int ChangePokemon_YES() {
	printPokemonMenu();
	return POKEMONMENU;
}

static int ChangePokemon_NO() {
	printBattleMenu();
	return BATTLEMENU;
}

static int RunMenu_YES() {
	Reload_Map();
	return GRAPHICS;
}

static int RunMenu_NO() {
	printBattleMenu();
	return BATTLEMENU;
}

static int BagMenu_POKEBALL() {
	//prompt user if he wants to use item
	//return to BagMenu Use Item prompt
	if(catchable) {
		return throwPokeBall();
	} else {
		printBattleScreen();
		printRect(0,88,160,40,black,brownGoldBd,bgColor);
		printDialogue("You can't you a pokeball on a trainer's pokemon!","\0","\0","\0","\0");
		printBattleScreen();
		return BATTLEMENU;
	}
};

static int BagMenu_CANCEL() {
	printBattleScreen();
	printRect(0,88,160,40,black,brownGoldBd,jadedGreen);
	printBattleMenu();
	return BATTLEMENU;
}
