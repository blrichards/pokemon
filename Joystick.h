// Analog Joystick driver

#ifndef Joy_h
#define Joy_h

#include <stdint.h>
#include "tm4c123gh6pm.h"

void Joystick_Init(void);

void Joystick_In(uint32_t *xdir, uint32_t *ydir);

// PE2 for x direction, PE3 for y direction
// calculates direction based on values (+x: right, -x: left, y: up, -y: down)
// if x and y are equal, go in x direction
// outputs: 0: down
//          1: right
//					2: up
//					3: left
//					4: stay
uint8_t Joystick_getDirection(uint32_t xdir, uint32_t ydir);

// PE4 is A Button, PE5 is B Button
void ButtonsAB_Init(void);

uint8_t AisPushed(void);

uint8_t BisPushed(void);


#endif
