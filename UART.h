// UART.c
// Runs on LM4F120/TM4C123
// Provides Prototypes for functions implemented in UART.c
// Last Modified: 4/17/2016 
// Student names: Ben Richards & Nick Amaya

#include <stdint.h>
#include "tm4c123gh6pm.h"

// UART initialization function 
// Input: none
// Output: none
void UART_Init(void);

//------------UART_InChar------------
// Wait for new input,
// then return ASCII code
// Input: none
// Output: char read from UART
// *** Need not be busy-wait if you know when to call
//char UART_InChar(void);

//------------UART_OutChar------------
// Wait for new input,
// then return ASCII code
// Input: none
// Output: char read from UART
void UART_PlaySound(char data);
