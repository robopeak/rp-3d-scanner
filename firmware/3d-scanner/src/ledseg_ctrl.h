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
 *    LED Seg Ctrl
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
 
#pragma once

#define BIT_SEG1_EN  (0x1L<<0)
#define BIT_SEG2_EN  (0x1L<<1)

void ledseg_init();
void ledseg_heartbeat();
void ledseg_rawoutput(_u16 seg1, _u16 seg2);
void ledseg_hexout(_u8 hex);
