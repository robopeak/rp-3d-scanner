/*
 * Name: timer.h
 * Project: AVR-Doper
 * Author: Christian Starkjohann <cs@obdev.at>
 * Creation Date: 2006-06-26
 * Tabsize: 4
 * Copyright: (c) 2006 by Christian Starkjohann, all rights reserved.
 * License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
 * Revision: $Id: timer.h 566 2008-04-26 14:21:47Z cs $
 */

/*
General Description:
This module implements functions to control timing behavior. It is based on
a timer interrupt.
*/

#ifndef __timer_h_included__
#define __timer_h_included__


extern volatile _u8   timerTimeoutCnt;
extern volatile _u16  timerLongTimeoutCnt;

/* ------------------------------------------------------------------------- */

#define TIMER_TICK_US       (64000000.0/(double)F_CPU)

void timerSetupTimeout(_u8 msDuration);
void timerTicksDelay(_u8 ticks);
void    timerSetupLongTimeout(_u8 ms100Duration);

#define TIMER_US_DELAY(us)  timerTicksDelay((_u8)((us)/TIMER_TICK_US))

/* ------------------------------------------------------------------------- */

static inline _u8   timerTimeoutOccurred(void)
{
    return timer0_overflow_count >= timerTimeoutCnt;
}



static inline _u8   timerLongTimeoutOccurred(void)
{
    return timer0_millis >= timerLongTimeoutCnt;
}

/* ------------------------------------------------------------------------- */

#endif /* __timer_h_included__ */
