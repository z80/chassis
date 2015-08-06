
#ifndef __PWM_CTRL_H_
#define __PWM_CTRL_H_

#include <inttypes.h>

void initPwm( void );
void setPwm0( uint16_t interval );
void setPwm1( uint16_t interval );
void updatePwm( void );
void processIsr( void );

#endif
