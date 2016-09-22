#include <stdint.h>
#include "PokeLib.h"
#include "People.h"
#include "Graphics.h"
#include "PokeBattle.h"
#include "random.h"
#include "ST7735.h"
#include "Joystick.h"
#include "UART.h"

#define Pallet_Town 0
#define Route1	  	1
#define Bedroom     2
#define Home        3
#define Down  0
#define Right 1
#define Up    2
#define Left  3
#define Beep  2

void Delay(int speed) {
	int i;
	for (i = 6000000 / speed; i > 0; i--);
}

void GoInside(void) {
	Reload_Map();
	Delay(1);
	Map_Init(Home, 8, 3, Up);
}

void GoToRoute1(void) {
	Reload_Map();
	Delay(1);
	Map_Init(Route1, 5, 5, Up);
}

void GoDownstairs(void) {
	Reload_Map();
	Delay(1);
	Map_Init(Home, 2, 9, Left);
}

void GoUpstairs(void) {
	Reload_Map();
	Delay(1);
	Map_Init(Bedroom, 2, 9, Right);
}

void GoOutside(void) {
	Reload_Map();
	Delay(1);
	Map_Init(Pallet_Town, 9, 5, Down);
}

void GoToPallet(void) {
	Reload_Map();
	Delay(1);
	Map_Init(Pallet_Town, 2, 15, Down);
}


void SpawnWild(void) {
	int random_num = Random() % 20;
	if (random_num == 2) {
		Battle_RunFSM(Prof_Oak);
	}
}

void Say(char *dialogue) {
	ST7735_FillRect(0, 88, 160, 40, 0xffff);
	int i = 0;
	int x = 5;
	int y = 93;
	i = 0;
	while (AisPushed() || BisPushed()){};
	while (!AisPushed() && !BisPushed()){};
	UART_PlaySound(Beep);
	while (dialogue[i] != 0) {
		if (dialogue[i] == '\n') {
			y += 10;
			x = 5;
		} else if (dialogue[i] == '\t') {
			x += 18;
		} else {
			ST7735_DrawChar(x, y, dialogue[i], 0x0000, 0xffff, 1);
		}
		if (BisPushed() || AisPushed()) {
			Delay(30);
		} else {
			Delay(5);
		}
		i++;
		x += 6;
		if (x > 110 && dialogue[i] == ' ') {
			x = 5;
			y += 10;
			if (y > 120) {
				while (AisPushed() || BisPushed()){};
				while (!AisPushed() && !BisPushed()){};
				UART_PlaySound(Beep);
				ST7735_FillRect(0, 88, 160, 40, 0xffff);
				y = 93;
			}
			if (dialogue[i] == ' ') {
				i++;
			}
		}
	}
	while (AisPushed() || BisPushed()){};
	while (!AisPushed() && !BisPushed()) {};
	UART_PlaySound(Beep);
	Reload_Map();
	while (AisPushed() || BisPushed()){};
}

//void TrainerBattle(Person_t *enemy) {
	//TriggerBattle(enemy);
//}

void None(void) {
	return;
}
