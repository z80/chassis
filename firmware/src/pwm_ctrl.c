
#include "pwm_ctrl.h"

#include <avr/interrupt.h>
#include "cpu_io.h"
#include "dbg.h"

// 12MHz.
// I need 1-2ms with relatively big precission.
// And space between two PWM cycles about 20ms.
// 12000 per ms. So without prescaler it is totally fine.

#define PWM_PORT PORTC
#define PWM_DDR  DDRC
#define PWM_PAD_0 (1<<4)
#define PWM_PAD_1 (1<<5)

#define PWM_MASK ( PWM_PAD_0 | PWM_PAD_1 )

typedef struct
{
    uint8_t  pads; // Which PAD should be cleared.
    uint16_t ocr; // What should be OCR value to trigger this event.
} PWMPad;

#define PWM_CNT 2

static PWMPad  pwmPads[PWM_CNT];
volatile static PWMPad  pwmPadsSorted[PWM_CNT];
volatile static uint8_t pwmCnt;
volatile static uint8_t pwmIndex;
volatile static uint8_t pwmState;


#define STATE_OFF      0
#define STATE_PWM      1
#define STATE_PAUSE    2

#define PAUSE_DURATION  3750

#define TIMER_PWM( trig )     TCNT1=0; OCR1A=trig; TCCR1B=((1<<CS11) | (1<<CS10));
#define TIMER_PAUSE( trig )   TCNT1=0; OCR1A=trig; TCCR1B=((1<<CS11) | (1<<CS10));
#define TIMER_OFF()           TCNT1=0; TCCR1B=0;



ISR(TIMER1_COMPA_vect)
{
    if ( pwmState == STATE_PAUSE )
    {
        pwmState = STATE_PWM;
        PWM_PORT = PWM_PORT | PWM_MASK; // Enable all PWM pads.
        // Set timer.
        TIMER_PWM( pwmPadsSorted[0].ocr );
        // Set PWM index to the very beginning.
        pwmIndex = 0;
    }
    else if ( pwmState == STATE_PWM )
    {
        PWM_PORT = PWM_PORT & (pwmPadsSorted[pwmIndex].pads);
        pwmIndex++;
        if ( pwmIndex >= pwmCnt )
        {
            // Change mode to PAUSE.
            TIMER_PAUSE( PAUSE_DURATION );
            pwmState = STATE_PAUSE;
        }
        else
            // Select next time interval.
            // Timer prescaler should be wide enough to allow this.
            OCR1A = pwmPadsSorted[pwmIndex].ocr;
    }
    else
    {
        PWM_PORT &= (~PWM_MASK);
        TIMER_OFF();
    }
}

void initPwm( void )
{
    PWM_PORT &= ~(PWM_MASK);
    PWM_DDR  |= PWM_MASK;
    pwmPads[0].pads = PWM_PAD_0;
    pwmPads[1].pads = PWM_PAD_1;
    pwmState = STATE_OFF;

    // Setup timer.
    TCCR1A = 0;                 // I don't use these advanced modes here. So <- 0.
    TCCR1B = 0;                 // 0 - timer off. CS10 - no prescaler. CS11 = 1/8 prescaler, CS11 | CS10 = 1/64 prescaler.
    TCNT1  = 0x0000;            // Counter register. Start counting from 0.
    OCR1A  = 0x0000;            // Compare register.
    TIMSK  = (1<<OCIE1A);       // Interrupt mask register. Output compare interrupt enable.
}

void setPwmEn( uint8_t en )
{
    // If state is active, wait for inactive.
    /*
    while ( pwmState == STATE_PWM )
    {
        usbPoll();
        cpuIoPoll();
    }
    */

    // Turn timer off to avoid any glitches. I'm not sure if it should
    // be turned off regardless of situation but just in case...
    TIMER_OFF();
    if ( en )
    {
        pwmState = STATE_PAUSE;
        TIMER_PAUSE( PAUSE_DURATION );
    }
    else
    {
        // Initialize timer.
        // Just in case reset port value to 0.
        pwmState = STATE_OFF;
        //PWM_PORT = PWM_PORT & (~PWM_MASK);
    }
}

void setPwm0( uint16_t interval )
{
    pwmPads[0].ocr = interval;
}

void setPwm1( uint16_t interval )
{
    pwmPads[1].ocr = interval;
}

void updatePwm( void )
{
    // Check if it is necessary to restart timer after parameters apply.
    uint8_t en = (pwmState == STATE_OFF) ? 0 : 1;

    // If state is active, wait for inactive.
    /*
    while ( pwmState == STATE_PWM )
    {
        usbPoll();
        cpuIoPoll();
    }
    */
    // Turn timer off.
    TIMER_OFF();

    // Setup and sort PWM durations.
    // Initialize PADS for appropriate PWM channels.
    pwmPads[0].pads = PWM_PAD_0;
    pwmPads[1].pads = PWM_PAD_1;

    uint8_t i, j;
    for ( i=0; i<PWM_CNT; i++ )
        pwmPadsSorted[i] = pwmPads[i];

    // Sort PADS.
    for ( i=0; i<(PWM_CNT-1); i++ )
    {
        for ( j=1; j<PWM_CNT; j++ )
        {
            if ( pwmPadsSorted[i].ocr > pwmPadsSorted[j].ocr )
            {
                PWMPad tmp = pwmPadsSorted[i];
                pwmPadsSorted[i] = pwmPadsSorted[j];
                pwmPadsSorted[j] = tmp;
            }
        }
    }

    // Merge pads with matching duration.
    pwmCnt = 0;
    uint8_t mask = 0; // This is mask here.
    for ( i=0; i<PWM_CNT; i++ )
    {
        mask |= pwmPadsSorted[i].pads;
        if ( pwmPadsSorted[pwmCnt].ocr >= pwmPadsSorted[i].ocr )
        {
            pwmPadsSorted[pwmCnt].pads = mask;
        }
        else
        {
            pwmCnt++;
            pwmPadsSorted[pwmCnt].pads = mask;
        }
    }
    pwmCnt++;

    // Invert mask values to make & with port value.
    for ( i=0; i<pwmCnt; i++ )
        pwmPadsSorted[i].pads = ~pwmPadsSorted[i].pads;

    // If enabled start timer.
    if ( en )
        pwmState = STATE_PAUSE;
        TIMER_PAUSE( PAUSE_DURATION );
}




