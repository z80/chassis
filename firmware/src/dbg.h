
#ifndef __DBG_H_
#define __DBG_H_

#include <avr/io.h>

void initLeds( void );
void setDbgLed( unsigned char val );
void setLed( unsigned char val );
void setPower( uint8_t val );
void toggleDbgLed( void );

#endif


