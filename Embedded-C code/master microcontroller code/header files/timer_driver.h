

#ifndef TIMER_DRIVER_H_
#define TIMER_DRIVER_H
#include <avr/io.h>
#include <avr/interrupt.h>
#include "std_macros.h"
#include "STD_Types.h"

/*
	Function Name        : timer0_initializeCTC
	Function Returns     : void
	Function Arguments   : void
	Function Description : Initialize and start the timer0 for compare match.
*/
void timer0_initializeCTC(void);

/*
	Function Name        : timer0_stop
	Function Returns     : void
	Function Arguments   : void
	Function Description : Stop timer0.
*/
void timer0_stop(void);

#endif /* TIMER_DRIVER_H_ */
