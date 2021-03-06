
#ifndef __CTRLBOARD_IO_H_
#define __CTRLBOARD_IO_H_

#include "lowlevel_io.h"
#include <string>

class CtrlboardIo: public LowlevelIo
{
public:
    enum TThrottleType { TCurrentLoop=0, TAnalogLevel=1, TSoftware=2 };
    enum TThrottleMode { TThrottle=0, TSpeed=1 };
    typedef std::basic_string< unsigned char > TIo;

    CtrlboardIo();
    ~CtrlboardIo();

    bool version( std::string & ver );
    bool firmware( std::string & fir );

    bool setDbgLed( bool en );
    bool setPower( bool en );
    bool setServoEn( bool en );
    bool setLed( bool en );
    bool setServoRaw( int raw1, int raw2 );
    bool setServo( qreal v1, qreal v2 );
    bool sensor( bool & activated );

    // Group 1.
    bool setThrottleType( TThrottleType val );
    bool throttleType( TThrottleType & val );

    bool setThrottleMode( TThrottleMode val );
    bool throttleMode( TThrottleMode & val );

    bool setMaxThrottleCw( int val );
    bool maxThrottleCw( int & val );

    bool setMaxThrottleCcw( int val );
    bool maxThrottleCcw( int & val );

    bool setMaxSpeedCw( int val );
    bool maxSpeedCw( int & val );

    bool setMaxSpeedCcw( int val );
    bool maxSpeedCcw( int & val );

    bool setThrottleRampUpCw( int val );
    bool throttleRampUpCw( int & val );

    bool setThrottleRampUpCcw( int val );
    bool throttleRampUpCcw( int & val );

    bool setThrottleRampDownCw( int val );
    bool throttleRampDownCw( int & val );

    bool setThrottleRampDownCcw( int val );
    bool throttleRampDownCcw( int & val );

    bool setCommutationMode( int val );
    bool commutationMode( int & val );

    bool setThrottleLockout( bool val );
    bool throttleLockout( bool & val );

    bool setStallThreshold( int val );
    bool stallThreshold( int & val );

    bool setCurrentLimit( int en );
    bool currentLimit( int & en );

    bool setUndervoltageCtrl( int en );
    bool undervoltageCtrl( int & en );

    bool setMotorOvertemp( int val );
    bool motorOvertemp( int & val );

    bool setControllerOvertemp( int val );
    bool controllerOvertemp( int & val );

    bool setPassword( const std::string & val );
    bool password( std::string & val );

    // Group 2.
    //bool setEnabled( bool en );
    //bool enabled( bool & en );

    bool setThrottle( int val );
    bool throttle( int & val );

    bool setSpeed( int val );
    bool speed( int & val );

    bool setDirection( bool cw );
    bool direction( bool & cw );

    // Group 3.
    bool voltage( int & val );
    bool enKeySeat_1( bool & val );
    bool enKeySeat_2( bool & val );
    bool controllerT( int & val );
    bool motorT( int & val );
    bool extInp_1( int & val );
    bool extInp_2( int & val );
    bool errorCode( int & val );

    // Group 3-eeprom.
    bool hours( int & val );
    bool cycles( int & val );
    bool modelRev( std::string & val );
};



#endif


