

#include "pwm_ctrl.h"

int main( void )
{
  initPwm();
  setPwm0( 7 );
  setPwm1( 5 );
  updatePwm();
  return 0;
}
