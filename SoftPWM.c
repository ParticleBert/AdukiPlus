#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "Button.h"
#include "SoftPWM.h"

#define PORTA_MASK (1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3);
// #define PORTB_MASK (1<<PB2)
#define CH0_SET (pinlevelA |= (1 << PA0)) // map CH0 to PA0
#define CH1_SET (pinlevelA |= (1 << PA1)) // map CH1 to PA1
#define CH2_SET (pinlevelA |= (1 << PA2)) // map CH2 to PA2
#define CH3_SET (pinlevelA |= (1 << PA3)) // map CH3 to PA3

// VARIABLES //////////////////////////////////////////////////////////////////

// const uint8_t pwmtable[128] PROGMEM = {0,0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,3,3,3,4,4,5,5,6,7,7,8,8,9,10,11,11,12,13,14,15,16,17,18,19,20,21,22,24,25,26,27,29,30,31,33,34,36,37,39,40,42,44,45,47,49,51,53,55,56,58,60,62,65,67,69,71,73,75,78,80,82,85,87,90,92,95,97,100,103,105,108,111,114,117,120,122,125,128,132,135,138,141,144,147,151,154,157,161,164,168,171,175,179,182,186,190,193,197,201,205,209,213,217,221,225,229,233,238,242,246,251,255};
// const uint16_t pwmtable[128] PROGMEM = {0,0,0,0,0,0,1,1,1,2,2,2,3,3,4,5,5,6,7,8,9,10,11,12,13,14,16,17,18,20,21,23,25,26,28,30,32,34,36,38,40,43,45,47,50,52,55,57,60,63,66,69,72,75,78,81,85,88,91,95,98,102,106,110,113,117,121,125,130,134,138,142,147,151,156,161,165,170,175,180,185,190,196,201,206,212,217,223,228,234,240,246,252,258,264,270,277,283,289,296,303,309,316,323,330,337,344,351,358,366,373,381,388,396,404,412,419,427,436,444,452,460,469,477,486,494,503,512};
const uint16_t pwmtable[128] PROGMEM = {0,0,0,0,1023,1023,1023,1022,1022,1021,1020,1019,1018,1017,1016,1015,1013,1012,1010,1008,1006,1004,1002,1000,998,995,993,990,987,984,981,978,975,971,968,964,960,956,952,948,943,939,934,930,925,920,914,909,904,898,892,887,881,874,868,862,855,848,842,835,827,820,813,805,797,790,782,773,765,757,748,739,730,721,712,703,693,684,674,664,654,644,633,623,612,601,590,579,568,556,544,533,521,509,496,484,471,459,446,433,419,406,392,379,365,351,337,322,308,293,278,263,248,233,217,202,186,170,154,137,121,104,88,71,53,36,19,1};
	
ISR (TIM0_COMPA_vect)
{
	static uint8_t pinlevelA = PORTA_MASK;
	// static uint8_t pinlevelB = PORTB_MASK;
	static uint16_t softcount = 0x3FF;
	
	// PORTB = pinlevelB;
	PORTA = pinlevelA;
	
	// Mask for 10 Bit.
	softcount = (++softcount & 0x3FF);
	
	// Softcount runs from 0 to 0x3FF
	if(softcount == 0)
	{
		// Reload compare
		compare[0] = compbuff[0];
		compare[1] = compbuff[1];
		compare[2] = compbuff[2];
		compare[3] = compbuff[3];
		
		pinlevelA = 0;
		// pinlevelB = 0;
	}
	else
	{
		// Compare the softcounter to the compare register
		// If the softcounter is reached, set the bit in pinlevelA
		// pinlevelA will be shiftet out in the next cycle
		if(compare[0] == softcount) CH0_SET;
		if(compare[1] == softcount) CH1_SET;
		if(compare[2] == softcount) CH2_SET;
		if(compare[3] == softcount) CH3_SET;
	}
}

ISR (TIM1_COMPA_vect)
{
	request_channel_update = 1;
}

void SoftPWMInit(void)
{
	// Initialize Variables
	compbuff[0] = 0;
	compbuff[1] = 0;
	compbuff[2] = 0;
	compbuff[3] = 0;

	// Init Timer/Counter 0
	// We set the WGM-Bits to 010. This activates the WGM-Mode 2, called CTC.
	TCCR0A |= (1<<WGM01);
	// We don't need prescaling for the Timers
	TCCR0B |= (1<<CS00);
	// Enable the Interrupt for the Overflow A of Timer 0
	TIMSK0 |= (1<<OCIE0A);
	// Set the OCR0A to 160. 160 * 125ns should give 20us as resolution for the PWM.
	// 512 * 20us gives 10.42ms, this represents 97.6Hz. Close enough to 100Hz.
	OCR0A = 100;
	
	// Init Timer/Counter 1
	// Here we need the WGM-Mode 4. Also we divide the CLOCK / 256.
	TCCR1B |= (1<<WGM12) | (1<<CS12);
	TIMSK1 |= (1<<OCIE1A);
	OCR1A = 0x1000;
}

void SoftPWMBlendChannels(uint8_t from_channel, uint8_t to_channel)
{
	// Finish, when the to-counter reaches 127.
	while (softpwm.ctr[to_channel] != 127)
	{
		if(request_channel_update)
		{
			// Decrease the from-pointer from 127 to 0
			--softpwm.ctr[from_channel];
			// Increase the to-pointer from 0 to 127
			++softpwm.ctr[to_channel];
			// Update the comparebuffers of the SoftPWM
			compbuff[from_channel] = pgm_read_word(&pwmtable[softpwm.ctr[from_channel]]);
			compbuff[to_channel] = pgm_read_word(&pwmtable[softpwm.ctr[to_channel]]);
						
			request_channel_update = 0;
		}
	}
	
}
