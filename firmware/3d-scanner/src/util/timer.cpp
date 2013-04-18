/*
 * Name: timer.c
 * Project: AVR-Doper
 * Author: Christian Starkjohann <cs@obdev.at>
 * Creation Date: 2006-06-26
 * Tabsize: 4
 * Copyright: (c) 2006 by Christian Starkjohann, all rights reserved.
 * License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
 * Revision: $Id: timer.c 566 2008-04-26 14:21:47Z cs $
 */


#include "../common.h"

volatile _u8  timerTimeoutCnt;

volatile _u16  timerLongTimeoutCnt;

/* ------------------------------------------------------------------------- */

void timerTicksDelay(_u8 ticks)
{
char    d, until;

    cli();
#ifdef SFIOR
    SFIOR = _BV(PSR10);     /* reset prescaler */
#else
    GTCCR = _BV(PSRSYNC);
#endif
    until = TCNT0 + ticks;
    sei();
    do{
        d = TCNT0 - until;  /* enforce 8 bit arithmetics */
    }while(d < 0);
}

/* ------------------------------------------------------------------------- */
#if 0
/* main configures Timer 0 with 1/64 prescaler
 * -> 12MHz/64 = 187.5kHz --> 5.333us
 * overflow every 256 counts = 732.422Hz --> 1.365ms
 */
UTIL_INTERRUPT(SIG_OVERFLOW0)   /* run with global interrupt enable */
{
static _u8    prescaler = 1;

    cli();
    if(timerTimeoutCnt != 0)
        timerTimeoutCnt--;
    sei();  /* allow interrupts here */
    cli();
    if(--prescaler == 0){   /* scale down to 10 Hz */
        prescaler = (_u8)((double)F_CPU / 64 / 256 / 10 + 0.5);
        sei();
        if(timerLongTimeoutCnt != 0)
            timerLongTimeoutCnt--;
    }else{
        sei();
    }
}
#endif
/* ------------------------------------------------------------------------- */

void    timerSetupTimeout(_u8 msDuration)
{
   
/* We can either have 12 Mhz clock or roughly 16 MHz (15/16/16.5). The
 * following is precise enough.
 */
#if F_CPU < 14000000
    msDuration -= msDuration >> 2;  /* approximate milliseconds with our timebase */
    /* 1 - 1/4 = 0.75; 1.365 ms * 0.75 = 1.024 ms --> 2.4% deviation */
#endif
    msDuration++;   /* Our resolution is 1.4 ms. Add one unit to avoid almost zero delays for low values */
    
    timerTimeoutCnt = msDuration;
    timer0_overflow_count = 0;
}

void    timerSetupLongTimeout(_u8 ms100Duration)
{
    timerLongTimeoutCnt = (_u16)ms100Duration*100;
    timer0_millis = 0;
}
/* ------------------------------------------------------------------------- */
