// DEFINES ////////////////////////////////////////////////////////////////////

#define F_CPU 8000000L                  // Systemtakt in Hz
#define F_PWM 100                       // PWM-Frequenz in Hz
#define PWM_STEPS 512                   // PWM-Schritte pro Zyklus
#define T_PWM (F_CPU/(F_PWM*PWM_STEPS)) // Systemtakte pro PWM-Takt

// VARIABLES //////////////////////////////////////////////////////////////////

enum enum_dir {UP, DOWN};
enum enum_colour {RED, YELLOW, BLUE, GREEN};

enum enum_colour actual_colour, next_colour;
// Structure containing the SoftPWM-Variables.
struct struct_softpwm
{
	volatile uint8_t ctr[4];
	enum enum_dir dir[4];
} softpwm;

uint16_t			compare[4];		// 4 PWM Channels
volatile uint16_t	compbuff[4];	// The corresponding compare registers
volatile uint8_t	request_channel_update;

// FUNCTIONS //////////////////////////////////////////////////////////////////

void SoftPWMInit(void);
// void SoftPWMUpdateChannel(uint8_t channel);
void SoftPWMBlendChannels(uint8_t from_channel, uint8_t to_channel);