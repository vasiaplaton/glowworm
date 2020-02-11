#include <avr/io.h>
#include <util/delay.h>

#define PORTB                _SFR_IO8(0x18)
#define PB5                  5
#define PB4                  4
#define PB3                  3
#define PB2                  2
#define PB1                  1
#define PB0                  0
int prev=0;
byte flag=0;
int i;
/*
void delay_ms( int ms)
{
   volatile long unsigned int i;
   for(i=0;i<ms;i++)
      _delay_ms(1);
}
*/

/* LED RBG pins */
#define    LED_RED        PB1
#define    LED_GREEN    PB0
#define    LED_BLUE    PB2

int red = 253;
int blue = 0;
int green = 0;
///
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

void setup()
{
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
}

#define TOUCH_PANEL_LEVEL 5  // sensor sensivity

// !!! for 1GHz !!!
#define QUICK_TOUCH_LEVEL 2  // more 1 to ingnore false alarm
#define LONG_TOUCH_LEVEL 10  // count of sensor alarm for detect long touch
#define QUICK_TOUCH_DELAY 2 
#define LONG_TOUCH_IGNORE_DELAY 20 // ignore touch after long touch with led off


int touch_level = 0;
int time_remain = 0;
bool isIgnoreShortTouch = false;
bool isLedOn = false;

void loop()
{
  // accumulate sensor alarm
// touch_level = touch_level + (touch_measure(&panel) > TOUCH_PANEL_LEVEL ? 1 : -1);
  
 /// if (touch_level < 0)
 //   touch_level = 0;
  
  // if quick touch detect
  if (touch_measure(&panel) >= 10)
  {
    //time_remain = QUICK_TOUCH_DELAY;            
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
  // if long touch detect
  
  /*if (time_remain > 0)
  {
    time_remain--;
    if (time_remain <= 0)
    {
      time_remain = 0;
      isLedOn = false;
    }
  }*/


 if(millis() - prev >= 10 )
 {

  if (isLedOn)
  {
    i++;
    //rainbow_on();
   /* if (flag == 0)
    {
    i++;
    }
    else
    {
      i--;
    }
    if( i > 253 )
    {
      flag=1;
    }
     if( i < 1)
    {
      flag=0;
    }
    analogWrite(PB2, i);
   //PORTB |= (1<<PB0);*/
   analogWrite(LED_RED, red);
   //analogWrite(LED_BLUE, blue);
   analogWrite(LED_GREEN, green);
   if ( i < 253 ){
   green = i;
   }
    else if( i < 253 *2 )  {
      red = 253 * 2 - i;
    }
    else if ( i < 253 *3 )
    {
       blue = i - 253*2;
       blue = 10 * blue / 127;
      digitalWrite(LED_BLUE, HIGH);
      delay(blue);
      digitalWrite(LED_BLUE, LOW);
     // blue = i - 253*2;
    }
   if( i > 253*3 ){
    i = 0;
    red=253;
   }
  }
  else 
  {
    //rainbow_off();
   // PORTB &= ~(1<<PB0);

    digitalWrite(PB2, LOW);
     digitalWrite(PB1, LOW);
      digitalWrite(PB0, LOW);
  }

  
 }
}
