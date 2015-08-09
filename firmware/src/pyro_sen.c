
#include "pyro_sen.h"
#include "dbg.h"

#define PYRO_PORT PORTC
#define PYRO_DDR  DDRC
#define PYRO_PIN  PINC
#define PYRO_PAD  (1<<3)

static uint8_t pyro = 0;

void initPyroSen( void )
{
    PYRO_PORT &= ~PYRO_PAD;
    PYRO_DDR  &= ~PYRO_PAD;
}

void processPyroSen( void )
{
    uint8_t en = PYRO_PIN & PYRO_PAD;
    setDbgLed( en ? 1 : 0 );
    pyro |= ( en ) ? 1 : 0;
}

uint8_t pyroSen( void )
{
    uint8_t res = pyro;
    pyro = 0;
    return res;
}




