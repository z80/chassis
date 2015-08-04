
#ifndef __PWM_CTRL_H_
#define __PWM_CTRL_H_

#include <avr/io.h>


void initPwm( void );
void setPwmEn( uint8_t en );
void setPwm0( uint16_t interval );
void setPwm1( uint16_t interval );
void updatePwm( void );


#endif


