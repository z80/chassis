
#ifndef __CPU_IO_H_
#define __CPU_IO_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>     // for _delay_ms()

#include "./pgmspace.h"   // required by usbdrv.h
#include "./usbdrv.h"

void cpuIoInit( void );
void cpuIoPoll( void );
void cpuIoReset( void );
void cpuIoPush( uchar * in, uchar cnt );
void cpuIoPop( uchar * out, uchar cnt );

#endif


