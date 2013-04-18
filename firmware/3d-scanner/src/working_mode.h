/*
 *    RoboPeak Project
 *    http://www.robopeak.com
 *
 *    3D Scanner
 *    An applet of RoboticPeak Project
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
 * working_mode.h
 *
 *  Created on: Jun 26, 2010
 *      Author: Tony Huang
 */

#ifndef WORKING_MODE_H_
#define WORKING_MODE_H_

enum BRIDGE_WORKING_MODE_T
{
    MODE_RF_AP  = 0,
    MODE_AVR_DOPER = 1,
};

extern volatile _u8    bridgeWorkingMode;

#endif /* WORKING_MODE_H_ */
