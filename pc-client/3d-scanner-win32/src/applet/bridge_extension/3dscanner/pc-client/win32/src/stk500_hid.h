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

#pragma once

namespace rp{ namespace net{

extern "C"
{
#include "hiddata.h"
}

#include <cstring>
using namespace std;

#include <pshpack1.h>
struct stk500v2_pkt
{
    enum{
        STK_STX = 27,
        STK_TOKEN = 14,
        
        STK_STATUS_CMD_OK = 0,
        STK_STATUS_CMD_FAILED = 0xC0,
        STK_STATUS_CKSUM_ERROR = 0xC1,
        STK_STATUS_CMD_UNKNOWN = 0xC9,

        STK_ANSWER_CKSUM_ERROR = 0xB0,
        // no specific reason, due to the RF firmware implementation
        MAX_STK_PKT_SIZE = 275,  // = max payload size + checksum byte
    };
//-------------
//  Data Area
    _u8 _start_byte;
    _u8 _seq_id;
    _u8 _len_high;
    _u8 _len_low;
    _u8 _stk_token;
    _u8 _cmd;
    _u8 _data[MAX_STK_PKT_SIZE];

//-------------
    stk500v2_pkt(){
        setPkt(STK_STATUS_CMD_OK, 0, NULL);
    }

    stk500v2_pkt( _u8 cmd, _u16 size, const _u8 * data, _u8 seq_id = -1)
    {
        setPkt(cmd, size, data, seq_id);
    }

    _u16 setPkt(  _u8 cmd, _u16 size, const _u8 * data, _u8 seq_id = -1)
    {
        _cmd = cmd;
        _seq_id = seq_id;
        if (size > MAX_STK_PKT_SIZE-1) size = MAX_STK_PKT_SIZE-1;
        if (size && data){
            memcpy( _data, data, size);
        }
        _setDataSize(size);
        _buildPkt();

        return getPktSize();
    }

    bool isValidPkt()
    {
        _u8 checksum=0;
        _u8 *rawpkt = reinterpret_cast<_u8 *>(this);
        _u16 length = getPktSize();
        while(length--)
        {
            checksum ^= *rawpkt++;
        }
        return checksum==0;
    }

    _u16 getDataSize()
    {
        _u16 payload_size =  ((_u16)_len_high)<<8 | _len_low;
        if (payload_size<1) payload_size = 1;
        return payload_size-1;
    }

    _u16 getPktSize()
    {
        return getDataSize() + 7;
    }


private:
    void _setDataSize(_u16 size)
    {
        size += 1;

        _len_high = size>>8;
        _len_low  = size & 0xFF;
    }

    void _buildPkt()
    {
        _start_byte = STK_STX;
        _stk_token = STK_TOKEN;
        _u16 length = getPktSize() - 1;
        _u8 checksum=0;
        _u8 *rawpkt = reinterpret_cast<_u8 *>(this);

        while(length--)
        {
            checksum ^= *rawpkt++;
        }

        _data[getDataSize()] = checksum;
    }
};

#include <poppack.h>



class Stk500_HID
{
public:
    Stk500_HID(int vid, int pid, const char * vendorname, const char * dev_name);
    ~Stk500_HID();

    int  enum_dev( proc_dev_filter_t proc );
    bool bind( const char * dev_path = NULL );
    

    u_result send( _u8 cmd, _u16 payload_len, const _u8 * payload );
    u_result recv( stk500v2_pkt & ans);

    void close();

protected:
    static const int reportDataSizes[4];
    static int chooseDataSize(int len);

    bool isDevOpened() { return _is_dev_opened; }
    bool recv_stk_pkt(stk500v2_pkt & ans);



    void * usb_dev_handle;
    int _vid;
    int _pid;
    
    char * _vendorname, *_dev_name;

    bool _is_dev_opened;
};

}}