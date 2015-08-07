
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
//#include <util/delay.h>     // for _delay_ms()

#include "./pgmspace.h"   // required by usbdrv.h
#include "usbdrv.h"
#include "cpu_io.h"
#include "dbg.h"
#include "pwm_ctrl.h"

#include "config.h"

usbMsgLen_t usbFunctionSetup( uchar data[8] );

void __attribute__((noreturn)) main( void )
{
    cli();
    initLeds();
    initPwm();

    wdt_enable( WDTO_1S );

    // USB initialization.
    usbInit();
    usbDeviceDisconnect();  // enforce re-enumeration, do this while interrupts are disabled!
    unsigned char b = 150;
    while ( b-- )
    {
        //_delay_ms( 1 );
        wdt_reset();
    }
    cpuIoInit();

    usbDeviceConnect();
    sei();

    for ( ;; )
    {
        // main event loop
        usbPoll();
        wdt_reset();
        cpuIoPoll();
        //_delay_ms( 1 );
    }
}

usbMsgLen_t usbFunctionSetup( uchar data[8] )
{
    usbRequest_t    * rq = (void *)data;
    static uchar    dataBuffer[8];  // buffer must stay valid when usbFunctionSetup returns
    usbMsgPtr = dataBuffer;
    if( (rq->bmRequestType & USBRQ_TYPE_MASK) != 0 /*== USBRQ_TYPE_CLASS*/ )
    {    // HID class request
        if(rq->bRequest == USBRQ_HID_GET_REPORT)
        {
            // wValue: ReportType (highbyte), ReportID (lowbyte)
            return USB_NO_MSG;  // use usbFunctionRead() to obtain data
        }
        else if(rq->bRequest == USBRQ_HID_SET_REPORT)
        {
            return USB_NO_MSG;  // use usbFunctionWrite() to receive data from host
        }
    }
    return 0;
    //return USB_NO_MSG;
}

uchar usbFunctionRead(uchar *data, uchar len)
{
    //blinkLed1();
    cpuIoPop( data, len );
    return len;
}

uchar usbFunctionWrite( uchar *data, uchar len )
{
    //if ( ( data[0] == 0 ) && ( data[1] == FUNC_SET_PWR ) )
    //    toggleDbgLed();
    cpuIoPush( data, len );
    return 1;
}








