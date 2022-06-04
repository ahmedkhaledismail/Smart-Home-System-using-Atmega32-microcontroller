#include "timer_driver.h"

void timer0_initializeCTC(void)
{
	OCR0 = 78; //to achieve tick equal to 10 msec
	
	//Set the mode as Compare match
	SET_BIT(TCCR0, WGM01);
	CLR_BIT(TCCR0, WGM00);
	
	//set as clk/1024
	SET_BIT(TCCR0, CS00);
	CLR_BIT(TCCR0, CS01);
	SET_BIT(TCCR0, CS02);
	sei();
	SET_BIT(TIMSK, OCIE0);
}

void timer0_stop(void)
{
	CLR_BIT(TCCR0, CS00);
	CLR_BIT(TCCR0, CS01);
	CLR_BIT(TCCR0, CS02);
}
