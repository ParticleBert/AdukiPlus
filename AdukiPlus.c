#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include "SoftPWM.h"
#include "RandomFader.h"
#include "Button.h"

int main(void)
{
	// Initialize the Watchdog
	wdt_reset();
	
	// Set the direction for the LEDs to OUT
	DDRA |= (1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3);

	SoftPWMInit();
	ADCEnable();
	ButtonInit();
	
	// Enable Interrupts
	sei();
	
	actual_colour = GetNextColour(4);
	softpwm.ctr[actual_colour] = 127;
	
    while(1)
    {
		next_colour = GetNextColour(actual_colour);
		SoftPWMBlendChannels(actual_colour, next_colour);
		actual_colour = next_colour;
   }
}
