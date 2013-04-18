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
 *    RP Bridge RF AP bridge via USB-HID adaptor
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
#include "infra.h"
#include "stk500_hid.h"

#include "cstring"

namespace rp{ namespace net{

const int Stk500_HID::reportDataSizes[4] = {13, 29, 61, 125};

int Stk500_HID::chooseDataSize(int len)
{
    int i;
    for(i = 0; i < sizeof(reportDataSizes)/sizeof(reportDataSizes[0]); i++){
        if(reportDataSizes[i] >= len)
            return i;
    }
    return i - 1;
}

Stk500_HID::Stk500_HID(int vid, int pid, const char * vendorname, const char * dev_name)
: _is_dev_opened(false)
, usb_dev_handle(NULL)
, _vid(vid)
, _pid(pid)
, _vendorname(NULL)
, _dev_name(NULL)
{
    if (vendorname){
        _vendorname = new char [strlen(vendorname)+1];
        strcpy(_vendorname, vendorname);
    }

    if (dev_name){
        _dev_name = new char [strlen(dev_name)+1];
        strcpy(_dev_name, dev_name);
    }
}

Stk500_HID::~Stk500_HID(){
    close();
    delete _vendorname;
    delete _dev_name;
}

int  Stk500_HID::enum_dev( proc_dev_filter_t proc )
{
    return usbhidEnumDevice(_vid, _vendorname ,_pid, _dev_name, proc);
}

bool Stk500_HID::bind( const char * dev_path )
{
    close();
    if (dev_path){
        if (!usbhidOpenDeviceByPath((usbDevice_t **)&usb_dev_handle, dev_path))
        {
            _is_dev_opened = true;
        }
    }
    else
    {
        if (!usbhidOpenDevice( (usbDevice_t **)&usb_dev_handle, _vid, _vendorname ,_pid, _dev_name, 1))
        {
            _is_dev_opened = true;
        }
    }
    return _is_dev_opened;
}
    

u_result Stk500_HID::send( _u8 cmd, _u16 payload_len, const _u8 * payload )
{
    // Some code is from ser_avrdoper.c, copyright owner: Christian Starkjohann
    // TODO: rewrite the code to prevent copyright issue!
    if (!isDevOpened()) return RESULT_OPERATION_FAIL;

    stk500v2_pkt tx_pkt(cmd, payload_len, payload);
    
    int datalen = tx_pkt.getPktSize();
    _u8 * raw_data = reinterpret_cast<_u8 *>(&tx_pkt);
    while (datalen>0){
        _u8 buf[256];
        int rval, lenIndex = chooseDataSize(datalen);
        _u8 thisLen = datalen > reportDataSizes[lenIndex] ? reportDataSizes[lenIndex] : datalen;
        buf[0] = lenIndex + 1;   /* HID report ID */
        buf[1] = thisLen;
        memcpy(&buf[2], raw_data, thisLen);
        rval = usbhidSetReport((usbDevice_t *)usb_dev_handle, 
            (char *)buf, reportDataSizes[lenIndex] + 2);

        if (rval) return RESULT_OPERATION_FAIL;

        datalen -= thisLen;
        raw_data += thisLen;
    }
    return RESULT_OK;
}

bool Stk500_HID::recv_stk_pkt(stk500v2_pkt & ans)
{
    // Some code is from ser_avrdoper.c, copyright owner: Christian Starkjohann
    // TODO: rewrite the code to prevent copyright issue!
    _u8 * rawdata = reinterpret_cast<_u8 *>(&ans);
    _u8 recv_len = 0;
    int bytesPending = reportDataSizes[1];  /* guess how much data is buffered in device */


    while(bytesPending > 0){
        int len, usbErr, lenIndex = chooseDataSize(bytesPending);
        _u8 buf[128];

#if 0
        len = sizeof(stk500v2_pkt) - recv_len;  /* bytes remaining */
        if(reportDataSizes[lenIndex] + 2 > len) /* requested data would not fit into buffer */
            return false;
#endif

        len = reportDataSizes[lenIndex] + 2;
        usbErr = usbhidGetReport((usbDevice_t *)usb_dev_handle, lenIndex + 1, (char *)buf, &len);
        if(usbErr != 0){
            return false;
        }
        
        len -= 2;   /* compensate for report ID and length byte */
        bytesPending = buf[1] - len; /* amount still buffered */
        if(len > buf[1])             /* cut away padding */
            len = buf[1];
        if(recv_len + len > sizeof(stk500v2_pkt)){
            return false;
        }
        memcpy(rawdata + recv_len, buf + 2, len);
        recv_len += len;
    }

    return ans.isValidPkt();
}

u_result Stk500_HID::recv( stk500v2_pkt & ans)
{
    // Some code is from ser_avrdoper.c, copyright owner: Christian Starkjohann
    // TODO: rewrite the code to prevent copyright issue!
    if (!isDevOpened()) return RESULT_OPERATION_FAIL;

    if (!recv_stk_pkt(ans)) return RESULT_OPERATION_FAIL;
    return RESULT_OK;
}

void Stk500_HID::close()
{
    if (usb_dev_handle)
    {
        usbhidCloseDevice((usbDevice_t *)usb_dev_handle);
        usb_dev_handle = NULL;
    }
    _is_dev_opened = false;

}


}}