#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "Graphics.h"
#include "PokeBattle.h"
#include "Joystick.h"
#include "Maps.h"
#include "People.h"
#include "PokeStructs.h"
#include "UART.h"

#define MAX_WIDTH  128
#define MAX_HEIGHT 160
#define Pallet_Town 0
#define Route1      1
#define Bedroom     2
#define Home				3
#define Down  0
#define Right 1
#define Up    2
#define Left  3
#define Stand 4
#define User  0

static uint32_t cX;
static uint32_t cY;
static int dir;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
void printLoadScreen(void);

int main(void) {
	TExaS_Init();  // set system clock to 80 MHz
  Output_Init();
	Joystick_Init();
	ButtonsAB_Init();
	UART_Init();
	Random_Init(1);
	Battle_Init();
	Map_Init(Bedroom, 7, 5, Down);
	while (1) {
		Joystick_In(&cY, &cX);
		dir = Joystick_getDirection(cY, cX);
		while (dir == Stand) {
			if (AisPushed()) {
				CheckForAction();
			}
			Joystick_In(&cY, &cX);
			dir = Joystick_getDirection(cY, cX);
			Reload_Map();
		  UART_PlaySound(1);
		}
		Move(dir);
	}
}


