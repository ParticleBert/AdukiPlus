#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "RandomFader.h"
#include "SoftPWM.h"

volatile uint8_t adc_value;

void ADCEnable(void)
{
	// Enable the ADC
	ADCSRA |= (1<<ADEN);
	// Set the reference Voltage to 1.1V. 
	ADMUX |= (1<<REFS1)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0);
	// Disable the digital Input
	DIDR0 |= (1<<ADC0D)|(1<<ADC3D);
	// Set the divider to 64. 125kHz at 8MHz.
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1);
}

uint8_t GetNextColour(uint8_t actual_colour_local)
{
	volatile uint16_t adc_value;
	
	do 
	{
		// Start Conversion
		ADCSRA |= (1<<ADSC);
		while(!(ADCSRA & (1<<ADIF)))
		{
			;	// Wait for the ADC to finish
		}
		ADCSRA |= (1<<ADIF);	// Clear the ADIF-Flag
		adc_value = ADCL;
		adc_value += (8<<ADCH);
		// Leave only the last bits.
		adc_value &= 0x3;
	} 
	while (adc_value == actual_colour_local);
	
	return (uint8_t)adc_value;
}

