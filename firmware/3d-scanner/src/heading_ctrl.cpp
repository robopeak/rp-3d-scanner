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
 *    Servo Control
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


#include "common.h"
#include "heading_ctrl.h"

#define PTZ_SERVO_X IO_ADC1
#define PTZ_SERVO_Y 7
#include "drv/avr_servo_ptz.h"

#define MAX_PULSE_WIDTH_C 2350
#define MIN_PULSE_WIDTH_C 550L

#define MICROSECOND_PER_DEGREE  ((MAX_PULSE_WIDTH_C-MIN_PULSE_WIDTH_C) / SERVO_ANGLE_RANGE) 
static _u16 current_angle ;
void heading_init()
{
    servo_ptz_init();
    heading_set_angle(900);
    
}

void heading_set_angle(int angle10)
{
    if (angle10<0) angle10=0;
    if (angle10>1800) angle10 = 1800;
    current_angle = angle10;
    servo_ptz_set(0, (long)angle10 * MICROSECOND_PER_DEGREE/10 + MIN_PULSE_WIDTH_C);
}

_u16 heading_get_angle(){
    return current_angle;
}