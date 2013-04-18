/*
 *    RoboPeak Project
 *    http://www.robopeak.com
 *
 *    3D Scanner
 *    An applet of RoboticPeak Project
 *
 *    By Shikai Chen (shikai.chn@gmail.com)
 *
 *
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
   Copyright statements of the reference source:
*/
/* Name: main.c
 * Project: AVR-Doper
 * Author: Christian Starkjohann <cs@obdev.at>
 * Creation Date: 2006-06-21
 * Tabsize: 4
 * Copyright: (c) 2006 by Christian Starkjohann, all rights reserved.
 * License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
 * Revision: $Id: main.c 702 2008-11-26 20:16:54Z cs $
 */


#include "common.h"

extern "C"{
#include "usbdrv/usbdrv.h"
}

#include "stk500protocol.h"
#include "ledseg_ctrl.h"
#include "heading_ctrl.h"


#define KEY_LEFT  18
#define KEY_RIGHT 19
#define LASER_EN  16

void onSetHeading( _u16 angle10)
{
    heading_set_angle(angle10);
    ledseg_hexout(angle10/10);
}

void onEnableLaser()
{
    DIGITAL_WRITE(LASER_EN, HIGH);
}

void onDisableLaser()
{
    DIGITAL_WRITE(LASER_EN, LOW);
}

void setup(void)
{
    infra_init();

    // disconnect all IO port
    PORTB = 0;
    DDRB = 0;
    PORTC = 0;
    DDRC = 0;
    PORTD = 0;
    DDRD = 0;

    //LED_INIT();
    
    cbi(USB_PORT, USB_PORT_P);
    cbi(USB_PORT, USB_PORT_N);
    cbi(USB_DDR, USB_PORT_P);
    cbi(USB_DDR, USB_PORT_N);

    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */

    // Init key state
    DIGITAL_WRITE(KEY_LEFT, HIGH);
    DIGITAL_WRITE(KEY_RIGHT, HIGH);
    
    enable_adc();
    ledseg_init();
    heading_init();
}

static inline void postsetup(void)
{
    // Connect the USB
	wdt_reset();
	delay(128);
	delay(128);
	usbDeviceConnect();


    // show RP text
    ledseg_rawoutput(0x3CC, 0x23CC);

    // enable Laser
    PIN_MODE(LASER_EN, OUTPUT);
    DIGITAL_WRITE(LASER_EN, LOW);
}

static void poll_keyevent()
{
    static _u32 last = 0;

    if (getms()-last > 10) {
       last = getms();
    }else {return;}
    if ( DIGITAL_READ(KEY_LEFT) == 0) {
        onSetHeading(heading_get_angle()+5);
    } else if (DIGITAL_READ(KEY_RIGHT) == 0) {
        onSetHeading(heading_get_angle()-5);
    }
    
    
}

int main(void)
{
    wdt_enable(WDTO_2S);

#if ENABLE_DEBUG_INTERFACE
    serialInit();
#endif

    setup();


    postsetup();
    usbInit();
    sei();

    for(;;){    /* main event loop */
        wdt_reset();
        usbPoll();
        stkPoll();
        ledseg_heartbeat();
        poll_keyevent();
    }
    
    return 0;
}
