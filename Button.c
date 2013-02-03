#include <avr/io.h>
#include <avr/interrupt.h>
#include "Button.h"

void ButtonInit(void)
{
	// Configure the MCUCR for falling edge on INT0
	MCUCR |= (1<<ISC01);
	
	// Enable the interrupt for INT0
	GIMSK |= (1<<INT0);
	
	button_state = 0;
}

ISR (EXT_INT0_vect)
{
	if(!button_state)
	{
		button_state = 1;
	}
	else
	{
		button_state = 0;
	}
	
	// Clear the interrupt flag
	GIFR |= (1<<INTF0);
}