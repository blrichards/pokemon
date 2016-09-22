// UART.c
// Runs on LM4F120/TM4C123
// Provide functions that setup and interact with UART
// Last Modified: 4/12/2016 
// Student names: Ben Richards & Nick Amaya
// Last modification date: 4/17/2016

#include <stdint.h>
#include "tm4c123gh6pm.h"

// UART initialization function 
// Input: none
// Output: none
void UART_Init(void){ 
	SYSCTL_RCGCUART_R |= 0x02;  // activate UART1
	SYSCTL_RCGCGPIO_R |= 0x04;	// activate port C
	UART1_CTL_R &= ~0x0001;			// disable UART1
	UART1_IBRD_R = 50;					// set integer part of baud rate
	UART1_FBRD_R = 0;						// set decimal part of baud rate
	UART1_LCRH_R = 0x70;				// update baud rate register
	//UART1_IM_R |= 0x10;					// enable Rx interrupts
	//UART1_IFLS_R = (UART1_IFLS_R&(~0x2F)) | 0x10; // interrupt when rx fifo 1/2 full
	GPIO_PORTC_DEN_R |= 0x20; // just tx PC5
	GPIO_PORTC_AMSEL_R &= ~0x20;
	GPIO_PORTC_AFSEL_R |= 0x20;																				// enable alternate function for PC5
	GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF0FFFFF) | 0x00200000;  // alternate function = UART1, Tx: PC5
	UART1_CTL_R |= 0x201;																							// enable UART1, Tx
	//NVIC_PRI1_R = (NVIC_PRI1_R&(~0xE00000)) | 0x600000; // interrupt priority 3
	//NVIC_EN0_R = 0x40;																	// enable UART1 interrupts
}

//------------UART_InChar------------
// Wait for new input,
// then return ASCII code
// Input: none
// Output: char read from UART
// *** Need not be busy-wait if you know when to call
//char UART_InChar(void){
//	return((unsigned char)(UART1_DR_R&0xFF));
//}

//------------UART_OutChar------------
// Wait for new input,
// then return ASCII code
// Input: none
// Output: char read from UART
void UART_PlaySound(char data){  
	while ((UART1_FR_R&0x20) != 0) {};// busy wait
	UART1_DR_R = data;
}
