
#include "dbg.h"
#include <avr/io.h>

#define LED_PORT     PORTD
#define LED_DDR      DDRD
#define LED_PAD      (1<<4)
#define DBG_LED_PAD  (1<<3)
#define PWR_PORT     PORTC
#define PWR_DDR      DDRC
#define PWR_PAD      (1<<1)

void initLeds( void )
{
    LED_PORT &= ~(LED_PAD | DBG_LED_PAD);
    LED_DDR  |= LED_PAD | DBG_LED_PAD;

    PWR_PORT &= ~(PWR_PAD);
    PWR_DDR  |= PWR_PAD;
}

void setDbgLed( unsigned char val )
{
    uint8_t led = (val) ? DBG_LED_PAD : 0;
    LED_PORT = (LED_PORT & (~(DBG_LED_PAD))) | led;
}

void setLed( unsigned char val )
{
    uint8_t led = (val) ? LED_PAD : 0;
    LED_PORT = (LED_PORT & (~(LED_PAD))) | led;
}

void setPower( uint8_t val )
{
    uint8_t led = (val) ? PWR_PAD : 0;
    PWR_PORT = (PWR_PORT & (~(PWR_PAD))) | led;
}

void toggleDbgLed( void )
{
    // Here it is reversed to perform toggle.
    uint8_t led = (LED_PORT & DBG_LED_PAD) ? 0 : DBG_LED_PAD;
    LED_PORT = (LED_PORT & (~(DBG_LED_PAD))) | led;
}






