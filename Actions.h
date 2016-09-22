#ifndef _ACTIONS_
#define _ACTIONS_

// Initializes the 'Bedroom' map
void GoInside(void);

// Initializes the 'Pallet_Town' map
void GoOutside(void);

// Initializes the 'Route1' map
void GoToRoute1(void);

// Initializes the 'Bedroom' map
void GoUpstairs(void);

// Initializes 'Home' map
void GoDownstairs(void);

// uses random num generator to check whether to spawn a wild pokemon
void SpawnWild(void);

void GoToPallet(void);

// print dialog in text box
void Say(char *dialog);

void None(void);
#endif
