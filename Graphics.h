// functions to procedurally generate screen

#ifndef Graph_h
#define Graph_h

// initialize map
void Map_Init(unsigned char map, unsigned long startR, unsigned long startC, unsigned char dir);

// move one tile on the map
unsigned char Move(char dir);

void Reload_Map(void);

void CheckForAction(void);

#endif
