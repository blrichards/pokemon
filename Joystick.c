
#include "Joystick.h"

#define Down  0
#define Right 1
#define Up    2
#define Left  3

static uint16_t upper = 3000;
static uint16_t lower = 1000;

//**********Joystick_Init***********
//ADC on PE3 (channel 0 for ADC0), PE2 (channel 1 for ADDC0)
//using Sequencer 1, enable interrupts
void Joystick_Init(void) {
	
	//intialize PORTE
	volatile uint16_t delay;
	SYSCTL_RCGCGPIO_R |= 0x10;
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTE_AFSEL_R |= 0x0C;
	GPIO_PORTE_DEN_R &= ~0x0C;
	GPIO_PORTE_AMSEL_R |= 0x0C;
	GPIO_PORTE_DIR_R &= ~0x0C;
	
	//initialize ADC0
	SYSCTL_RCGCADC_R |= 0x01;
	delay = SYSCTL_RCGCADC_R;
	delay = SYSCTL_RCGCADC_R;
	delay = SYSCTL_RCGCADC_R;
	delay = SYSCTL_RCGCADC_R;
	
	ADC0_ACTSS_R &= ~0x02;                              //disable Sequencer 1 while configuring it
	ADC0_SSPRI_R = (ADC0_SSPRI_R & ~0xFFFF) + 0x0123;   //set priorities of all sequencers
	ADC0_EMUX_R &= ~0xF000;                             //set ADC to sample on a software trigger
	ADC0_SSMUX1_R = (ADC0_SSMUX1_R & ~0x00FF) + 0x10;   //FIFO spot 0 filled with channel 0, PE3
	                                                    //FIFO spot 1 filled with channel 1, PE2
	ADC0_SSCTL1_R = (ADC0_SSCTL1_R & ~0xF0) + 0x60;     //set SS1 to interrupt after 2 samples
	                                                    //set SS1 to sample 2 times
	ADC0_PC_R = (ADC0_PC_R & ~0x0F) + 0x01;             //ADC samples at 125 ksps
	ADC0_IM_R &= ~0x02;                                 //the status of SS1 does not affect the SS1 interrupt status

	ADC0_ACTSS_R |= 0x02;                               //enable Sequencer 1   
	
}

void Joystick_In(uint32_t *ydir, uint32_t *xdir) {
	ADC0_PSSI_R |= 0x02;          //tell ADC to begin sampling to SS1
	while(!(ADC0_RIS_R & 0x02));  //check when ADC is done sampling both samples
	*ydir = ADC0_SSFIFO1_R;      //pop off sample from FIFO queue
	*xdir = ADC0_SSFIFO1_R;
	ADC0_ISC_R |= 0x02;           //tell ADC that an interrupt has occurred
}

// PE2 for x direction, PE3 for y direction
// calculates direction based on values (+x: right, -x: left, y: up, -y: down)
// if x and y are equal, go in x direction
// outputs: 0: down
//          1: right
//					2: up
//					3: left
//					4: stay
uint8_t Joystick_getDirection(uint32_t xdir, uint32_t ydir) {
	if(ydir > upper) 
		return Up;
	if(ydir < lower)
		return Down;
	if(xdir > upper)
		return Left;
	if(xdir < lower)
		return Right;
	
	return 4; //stay
}

void ButtonsAB_Init(void) {
	volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x01;
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTE_DEN_R |= 0x30;
	GPIO_PORTE_DIR_R &= ~0x30;
	GPIO_PORTE_AFSEL_R &= ~0x30;
	GPIO_PORTE_AMSEL_R &= ~0x30;
}

uint8_t AisPushed() {
	return (GPIO_PORTE_DATA_R & 0x10) >> 4;
}

uint8_t BisPushed(void) {
	return (GPIO_PORTE_DATA_R & 0x20) >> 5;
}

