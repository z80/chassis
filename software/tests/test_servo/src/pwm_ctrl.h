
#ifndef __PWM_CTRL_H_
#define __PWM_CTRL_H_

#ifndef WIN32
    #include <inttypes.h>
#else
    typedef unsigned short uint16_t;
    typedef unsigned char  uint8_t;
#endif

void initPwm( void );
void setPwm0( uint16_t interval );
void setPwm1( uint16_t interval );
void updatePwm( void );
void processIsr( void );

#endif
