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
 *    Driver for controlling the 3D-scanner
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


#include "stdafx.h"
#include "common.h"
#include "scanner_client.h"


using namespace rp::net;

const int USB_VENDOR_ID=0x16c0;
const int USB_PRODUCT_ID=0x05df;
const char * RPBRIDGE_VENDER_STRING = "robopeak.net";
const char * RPBRIDGE_DEV_STRING = "RP-3DScaner";



#define RPINFRA_STK_CMD_GET_ADC         0x10    //get the current AVR ADC0 (PC1) Raw Data
#define RPINFRA_STK_CMD_SET_PWM         0x11    //set the current AVR PWM output #0 (PB3)
// 3D-Scanner Specific
#define SCANNER_CMD_SET_HEADING         0x20
#define SCANNER_CMD_ENABLE_LASER        0x21
#define SCANNER_CMD_DISABLE_LASER       0x22



C3DScanner::C3DScanner(): Stk500_HID(USB_VENDOR_ID, USB_PRODUCT_ID, RPBRIDGE_VENDER_STRING, RPBRIDGE_DEV_STRING)
{}

C3DScanner::~C3DScanner() {}

bool C3DScanner::setAngle(_u16 angle10)
{
    return IS_OK(send( SCANNER_CMD_SET_HEADING, sizeof(_u16), (_u8 *)&angle10));
}

bool C3DScanner::enableLaser()
{
    return IS_OK(send( SCANNER_CMD_ENABLE_LASER, 0, NULL));
}

bool C3DScanner::disableLaser()
{
    return IS_OK(send( SCANNER_CMD_DISABLE_LASER, 0, NULL));
}


