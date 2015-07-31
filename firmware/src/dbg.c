
#include "dbg.h"
#include <avr/io.h>

#define LED_PORT PORTD
#define LED_DDR  DDRD
#define LED_PAD  (1<<4)
#define DBG_LED_PAD  (1<<3)

void initLeds( void )
{
    LED_DDR  |= LED_PAD | DBG_LED_PAD;
    LED_PORT |= 0b00000000;
}

void setDbgLed( unsigned char val )
{
    LED_PORT = (LED_PORT & (~(DBG_LED_PAD))) | val;
}

void setLed( unsigned char val )
{
    LED_PORT = (LED_PORT & (~(LED_PAD))) | val;
}

void blinkDbgLed( void )
{
    uint8_t led = (LED_PORT & DBG_LED_PAD) ? 0 : DBG_LED_PAD;
    LED_PORT = (LED_PORT & (~(DBG_LED_PAD))) | led;
}






