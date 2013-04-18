/*
 *  Driver for controlling the 3D-scanner
 *  By Shikai Chen
 *  
 */

#pragma once

#include "stk500_hid.h"

class C3DScanner: public rp::net::Stk500_HID {
public:
    C3DScanner();
    ~C3DScanner();


    bool setAngle(_u16 angle10);
    bool enableLaser();
    bool disableLaser();
};