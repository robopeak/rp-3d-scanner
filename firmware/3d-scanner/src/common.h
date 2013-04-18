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
 *    Common Includes
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

#include "conf.h"
#include "infra.h"
#include <avr/sleep.h>
#include <avr/wdt.h>

// The RDTSC timer value
// Note: In most cases, please use getms() interface to read its value.
//       Except in RF interrupt handler
extern volatile unsigned long timer0_millis; //Real time clock in ms

extern volatile unsigned long timer0_overflow_count;



void onSetHeading( _u16 angle10);
void onEnableLaser();
void onDisableLaser();