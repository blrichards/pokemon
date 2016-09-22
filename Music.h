/*
Contains all song arrays and controls all sound outputs
*/

#ifndef Music_h
#define Music_h

#include <stdint.h>

// song arrays 

// go here ^

// initialize systick and any other timers
void musicInit(void);

// sets variable currentSong for use by SysTick handler
void playSong(uint32_t *song);

// set reload value of systick to 0 stopping current sound output
void stopSong(void);

#endif
