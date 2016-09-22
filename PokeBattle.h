#include "PokeLib.h"

#ifndef PokeBattle_h
#define PokeBattle_h

#define MAX_HEALTH 48
#define MAX_HEIGHT 160
#define MAX_WIDTH  128
#define MAX_EXP    50

//****************Main_Controller************************
void Battle_RunFSM(Person_t trainer);


//**************FSM_Controllers*************************
static int PokemonMenu_Controller(void);
static int Bag_Controller(void);
static int TwoOptionMenu_Controller(unsigned char menu);
static int FourOptionMenu_Controller(unsigned char menu);
static int BagMenu_Controller(void);

//**************FSM_HelperFunctions***********************
static int FourOptionMenu_MoveCursor(int dir, unsigned char menu);
static int TwoOptionMenu_MoveCursor(int dir, unsigned char menu);
static int PokemonMenu_MoveSpots(int dir);
static int PokemonMenu_Highlight(int spot,unsigned short bdColor1, unsigned short bdColor2, unsigned short mainColor);
static int BagMenu_MoveCursor(int dir);

//*************Battle_GameMechanicFunctions************
void Battle_Init(void);
static int Battle_LoadScreen(Person_t trainer);
static int Battle_RunFightSequence(void);
static int Battle_ExitScreen(Person_t trainer);
static double Fire(unsigned char enemytype);
static double Water(unsigned char enemytype);
static double Grass(unsigned char grasstype);
static int switchEnemyPokemon(void);
static int Battle_EndConditions(void);

//***************FSM_ActionFunctions***********************
static int BattleMenu_RUN(void);
static int BattleMenu_FIGHT(void);
static int BattleMenu_BAG(void);
static int BattleMenu_POKEMON(void);

static int MoveMenu_USEMOVE(void);
static int MoveMenu_CANCEL(void);

static int PokemonMenu_SPOTX(void);
static int PokemonMenu_CANCEL(void);

static int ShiftMenu_SHIFT_(void);
static int ShiftMenu_CANCEL(void);

static int ChangePokemon_YES(void);
static int ChangePokemon_NO(void);

static int RunMenu_YES(void);
static int RunMenu_NO(void);

static int BagMenu_POKEBALL(void);
static int BagMenu_CANCEL(void);

//****************Layout_Functions**********************

static int printBattleMenu(void);
static int printPPMenu(void);
static int printPokemonMenu(void);
static int printMoveMenu(void);
static int printBattleScreen(void);

static int Battle_UpdateEnemyHealth(int newHealth);
static int Battle_UpdateUserHealth(int newHealth);
static int Battle_UpdateExp(void);

static int printRect(int xcoord, int ycoord, int width, int height, unsigned short bgColor, unsigned short bdColor, unsigned short mainColor);
static int printEmptyPokemonBox(int xcoord, int ycoord, int width, int height, unsigned short OutCol, unsigned short InCol);
static int printHP(int health, int xcoord, int ycoord);
static int printDialogue(char *string1, char *string2, char *string3,char *string4, char *string5);
static int printEnemyHealthBox(void);
static int printUserHealthBox(void);
static int printBagMenu(void);
static int printTwoOptionMenu(char *firstOption, char *secondOption);

//***************Controller_LinkFunctions**********************

static int returnToBattleScreen(void);
static int Battle_ReturnFromMoveMenu(void);

//****************Misc**********************************
static void Delay1(int speed);
static int catchError(void);
static int calcDam(Pokemon_t User, Pokemon_t Foe, int move);
static int chooseSTATUS(int status, Pokemon_t User, Pokemon_t Foe);
static int updateExp(void);
static void cvString(int num);

#endif
