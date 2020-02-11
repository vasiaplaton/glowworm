#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#define PORTB                _SFR_IO8(0x18)
#define PB5                  5
#define PB4                  4
#define PB3                  3
#define PB2                  2
#define PB1                  1
#define PB0                  0
uint8_t i2=0;
int i;
int i1;

//////



#include <avr/io.h>
#include <util/delay.h>

/* LED RBG pins */
#define    LED_RED        PB0
#define    LED_GREEN    PB1
#define    LED_BLUE    PB2

/* Rainbow settings */
#define    MAX        (256)
#define    STEP        (4)

/* Fading states */
#define    REDtoYELLOW    (0)
#define    YELLOWtoGREEN    (1)
#define    GREENtoCYAN    (2)
#define    CYANtoBLUE    (3)
#define    BLUEtoVIOLET    (4)
#define    VIOLETtoRED    (5)

/* Global variables */
uint16_t red = MAX;
uint16_t green = 0;
uint16_t blue = 0;
uint16_t state = 0;
void
rainbow(int n)
{

    switch (state) {
    case REDtoYELLOW: green += n; break;
    case YELLOWtoGREEN: red -= n; break;
    case GREENtoCYAN: blue += n; break;
    case CYANtoBLUE: green -= n; break;
    case BLUEtoVIOLET: red += n; break;
    case VIOLETtoRED: blue -= n; break;
    default: break;
    }

    if (red >= MAX || green >= MAX || blue >= MAX || red <= 0 || green <= 0 || blue <= 0) {
        state = (state + 1) % 6; // Finished fading a color so move on to the next
    }
}

//////
/*
void delay_ms( int ms)
{
   volatile long unsigned int i;
   for(i=0;i<ms;i++)
      _delay_ms(1);
}
*/

typedef struct {
	volatile uint8_t *port; //PORTx register for pin
	volatile uint8_t portmask; //mask for the bit in port
	volatile uint8_t mux; //ADMUX value for the channel
	volatile uint8_t muxgnd; //ADMUX value for the channel
} touch_channel_t;
static touch_channel_t panel;

uint16_t touch_measure(touch_channel_t *channel);

static inline void adc_channel(uint8_t channel)
{
	ADMUX &= ~(0b11);
	ADMUX |= 0b11 & channel;
}
 
static inline uint16_t adc_get(void)
{
	ADCSRA |= (1<<ADSC); 
	while(!(ADCSRA & (1<<ADIF))); 
	ADCSRA |= (1<<ADIF); 
	return ADC; 
}

uint16_t touch_measure(touch_channel_t *channel)
{
	uint8_t i;
	uint16_t retval; 
	retval = 0;
	
	for (i=0 ; i<4 ; i++)
	{
		*(channel->port) |= channel->portmask;
		_delay_ms(1);
		*(channel->port) &= ~(channel->portmask);
		
		adc_channel(channel->muxgnd);
		adc_get(); 
		
		adc_channel(channel->mux);
		retval += adc_get();
	}
	return retval /4;
}

int main (void) {
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1);
	ADCSRA |= (1<<ADEN); 
	DDRB |= (1<<PB3);
	PORTB &= ~(1<<PB3);
	
	DDRB |= (1<<PB0);
	DDRB |= (1<<PB1);
	DDRB |= (1<<PB2);
	panel.mux = 2;
	panel.muxgnd = 3;
	panel.port = &PORTB;
	panel.portmask = (1<<PB4);

#define TOUCH_PANEL_LEVEL 7  // sensor sensivity

// !!! for 1GHz !!!
#define QUICK_TOUCH_LEVEL 3  // more 1 to ingnore false alarm
#define LONG_TOUCH_LEVEL 10  // count of sensor alarm for detect long touch
#define QUICK_TOUCH_DELAY 40 
#define LONG_TOUCH_IGNORE_DELAY 20 // ignore touch after long touch with led off


int touch_level = 0;
int time_remain = 0;
bool isIgnoreShortTouch = false;
bool isLedOn = false;

while (true)
{
	// accumulate sensor alarm

	
	// if quick touch detect
	if (touch_measure(&panel) > 8)
	{
		isLedOn = true;
	}
	   else
   {
    touch_level++;
    //_delay_ms(1);// isLedOn = false;
   }
   if ( touch_level > 300 )
   {
    touch_level = 0;
    isLedOn = false;
   }
	
	if (isLedOn){
 while (1) {

        /* Rainbow algorithm */

        if (i < red) {
            PORTB &= ~(1 << LED_RED);
        } else {
             PORTB |= 1 << LED_RED;
        }

        if (i < green) {
            PORTB &= ~(1 << LED_GREEN);
        } else {
             PORTB |= 1 << LED_GREEN;
        }

        if (i < blue) {
            PORTB &= ~(1 << LED_BLUE);
        } else {
             PORTB |= 1 << LED_BLUE;
        }

        if (i >= MAX) {
            rainbow(STEP);
            i = 0;
	    i1++;
	   break;
        }
	//if (i1 >= 2) {
	//i1=0;
	//break;
	//}

        i++;
    }
		
		//PORTB |= (1<<PB2);
}	
else{ 
		 PORTB &= ~(1 << LED_BLUE);
		PORTB &= ~(1 << LED_GREEN);
		 PORTB &= ~(1 << LED_RED);
		//PORTB &= ~(1<<PB2);
}
} 
}
