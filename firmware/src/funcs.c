
#include "funcs.h"
#include "config.h"
#include "dbg.h"
#include "pwm_ctrl.h"

// For debug purpose only
#define MEM_SZ 128
#define EEP_SZ 128
uchar g_ramParams[ MEM_SZ ];
uchar g_eepromParams[ EEP_SZ ];
// / For debug purpose only

static void version( uchar * io );
static void firmware( uchar * io );
/*
static void setParam( uchar * io );
static void param( uchar * io );
static void setEepromParam( uchar * io );
static void eepromParam( uchar * io );
*/

static void set_DbgLed( uchar * io );
static void set_Led( uchar * io );
static void set_Pwr( uchar * io );
static void set_PwmEn( uchar * io );
static void set_Pwm( uchar * io );

void invoke( uchar id, uchar * io )
{
    switch ( id )
    {
    /*
    case FUNC_SET_PARAM:
        setParam( io );
        break;
    case FUNC_PARAM:
        param( io );
        break;
    case FUNC_SET_EEPROM_PARAM:
        setEepromParam( io );
        break;
    case FUNC_EEPROM_PARAM:
        eepromParam( io );
        break;
    */

    case FUNC_FIRMWARE:
        firmware( io );
        break;
    case FUNC_VERSION:
        version( io );
        break;
    case FUNC_SET_DBG_LED:
        set_DbgLed( io );
        break;
    case FUNC_SET_LED:
        set_Led( io );
        break;
    case FUNC_SET_PWR:
        set_Pwr( io );
        break;
    case FUNC_SET_PWM_EN:
        set_PwmEn( io );
        break;
    case FUNC_SET_PWM:
        set_Pwm( io );
        break;
    }
}

static void version( uchar * io )
{
    //blinkLed1();
    const uchar maxLen = 16;
    uchar ver[] = VERSION;
    uchar i = 0;
    do {
        io[i] = ver[i];
        i++;
    } while ( ( ver[i] != '\0' ) && ( i < maxLen ) );
}

static void firmware( uchar * io )
{
    const uchar maxLen = 16;
    uchar ver[] = FIRMWARE;
    uchar i = 0;
    do {
        io[i] = ver[i];
    } while ( ( ver[i] != '\0' ) && ( i < maxLen ) );
}

/*
static void setParam( uchar * io )
{
    blinkLed0();
    uchar at = io[0];
    uchar sz = io[1];
    uchar i;
    for ( i=0; i<sz; i++ )
        g_ramParams[ at+i ] = io[i+2];
}

static void param( uchar * io )
{
    blinkLed1();
    uchar at = io[0];
    uchar sz = io[1];
    uchar i;
    for ( i=0; i<sz; i++ )
        io[i] = g_ramParams[ at+i ];
}

static void setEepromParam( uchar * io )
{
    uchar at = io[0];
    uchar sz = io[1];
    uchar i;
    for ( i=0; i<sz; i++ )
        g_eepromParams[ at+i ] = io[i+2];
}

static void eepromParam( uchar * io )
{
    uchar at = io[0];
    uchar sz = io[1];
    uchar i;
    for ( i=0; i<sz; i++ )
        io[i] = g_eepromParams[ at+i ];
}
*/

static void set_DbgLed( uchar * io )
{
    setDbgLed( io[0] );
}

static void set_Led( uchar * io )
{
    setLed( io[0] );
}

static void set_Pwr( uchar * io )
{
    setPower( io[0] );
}

static void set_PwmEn( uchar * io )
{
    //setPwmEn( io[0] );
}

static void set_Pwm( uchar * io )
{
    /*
    uint16_t pwm = (uint16_t)(io[0]) + (((uint16_t)(io[1])) << 8);
    setPwm0( pwm );
    pwm = (uint16_t)(io[2]) + (((uint16_t)(io[3])) << 8);
    setPwm1( pwm );
    updatePwm();
    */
}




