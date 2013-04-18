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
 * usb_func.cpp
 *
 *  Created on: Jun 26, 2010
 *      Author: Tony Huang
 */

#include "common.h"

extern "C"{
#include "usbdrv/usbdrv.h"
}

#include "stk500protocol.h"

// enable the following code if you need wireless support
// And... you need some modification to make them work :-p
#if 0
#include "sunjar_bridge/rf_cmd.h"
#include "working_mode.h"
#endif

static _u8    hidStatus;
static _u8    requestType;

/* defines for 'requestType' */
#define REQUEST_TYPE_LINE_CODING    0   /* CDC GET/SET_LINE_CODING */
#define REQUEST_TYPE_HID_FIRST      1   /* first block of HID reporting */
#define REQUEST_TYPE_HID_SUBSEQUENT 2   /* subsequent block of HID reporting */
#define REQUEST_TYPE_HID_DEBUGDATA  3   /* read/write data from/to debug interface */
#define REQUEST_TYPE_VENDOR         4   /* vendor request for get/set debug data */

#include "usb_func_desc.h"

static inline int stkReadBlock(_u8* p, _u8 len) {
	int c;

    do{
        c = stkGetTxByte();
        if(c < 0){
            return c;
        }
        *p++ = c;
    }while(--len);
}


/* Note: REPORT_COUNT does not include report-ID byte */
/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

_u8 usbFunctionDescriptor(usbRequest_t *rq)
{
	_u8 *p, len;
	_u8 descrStringType, descrType;

	descrStringType = rq->wValue.bytes[0];
    descrType = rq->wValue.bytes[1];

	#define USB_RESPONSE_WITH_LEN(DATA, LEN) do { len = LEN; p = (_u8*)(DATA); } while(0)
	#define USB_RESPONSE(DATA) USB_RESPONSE_WITH_LEN(DATA, sizeof(DATA))
	#define USB_RESPONSE_STRING(DATA) USB_RESPONSE_WITH_LEN(DATA, USB_PROP_LENGTH(sizeof(DATA)))
	#define COND_USB_RESPONSE_STRING(COND, STR1, STR2) do { if (COND) { USB_RESPONSE_STRING(STR1); } else { USB_RESPONSE_STRING(STR2); } } while (0)

    switch(descrType) {
		case USBDESCR_DEVICE:
			USB_RESPONSE(deviceDescrHID);
			break;
		case USBDESCR_CONFIG:
			USB_RESPONSE(configDescrHID);
			break;
		case USBDESCR_STRING:
			switch (descrStringType)
			{
				case 0:
					USB_RESPONSE_STRING(usbDescriptorString0);
					break;
				case 1:
                    USB_RESPONSE_STRING(usbDescriptorStringVendor_RP);
                    // replaced by the following code if you need changable device info
					// COND_USB_RESPONSE_STRING(bridgeWorkingMode == MODE_AVR_DOPER, usbDescriptorStringVendor, usbDescriptorStringVendor_RP);
					break;
				case 2:
                    USB_RESPONSE_STRING(usbDescriptorStringDevice_RP);
                    // replaced by the following code if you need changable device info
					// COND_USB_RESPONSE_STRING(bridgeWorkingMode == MODE_AVR_DOPER, usbDescriptorStringDevice, usbDescriptorStringDevice_RP);
					break;
				case 3:
                    USB_RESPONSE_STRING(usbDescriptorStringSerialNumber_RP);
                    // replaced by the following code if you need changable device info
					// COND_USB_RESPONSE_STRING(bridgeWorkingMode == MODE_AVR_DOPER, usbDescriptorStringSerialNumber, usbDescriptorStringSerialNumber_RP);
					break;
			}
			break;
        default:
        	USB_RESPONSE_WITH_LEN(configDescrHID + 18, 9);
        	break;
    }

	#undef COND_USB_RESPONSE_STRING
	#undef USB_RESPONSE_STRING
	#undef USB_RESPONSE
	#undef USB_RESPONSE_WITH_LEN

    usbMsgPtr = p;
    return len;
}

_u8 usbFunctionSetup(_u8 data[8])
{
    usbRequest_t  *rq      = (usbRequest_t *)data;
    _u8           rqType   = rq->bmRequestType & USBRQ_TYPE_MASK;
    _u8			  rqStatus = rq->bRequest;

    switch (rqType) {
		case USBRQ_TYPE_CLASS:		/* class request type */
			if (rqStatus == USBRQ_HID_GET_REPORT || rqStatus == USBRQ_HID_SET_REPORT) {
				hidStatus = rq->wValue.bytes[0];		/* store report ID */
				requestType = hidStatus == 5 ? REQUEST_TYPE_HID_DEBUGDATA : REQUEST_TYPE_HID_FIRST;
				/* the driver counts the total number of bytes for us */
				return 0xff;
			}
			break;
    }

    return 0;
}


_u8 usbFunctionRead(_u8 *data, _u8 len)
{
	switch (requestType) {
		case REQUEST_TYPE_LINE_CODING:
			// nothing to do
			break;
		case REQUEST_TYPE_HID_FIRST:
		case REQUEST_TYPE_HID_SUBSEQUENT:
	        _u8 *p = data;
	        _u8 remaining = len;

	        if(requestType == REQUEST_TYPE_HID_FIRST){
	            int cnt = stkGetTxCount();

	            *p++ = hidStatus;   /* report ID */
	            *p++ = utilHi8(cnt) ? 255 : cnt;

	            remaining -= 2;
	            requestType = REQUEST_TYPE_HID_SUBSEQUENT;
	        }

	        if(hidStatus){
	        	if (stkReadBlock(p, remaining) < 0) hidStatus = 0;
	        }
	        return len;
	}

	return 0;
}

_u8 usbFunctionWrite(_u8 *data, _u8 len)
{
	switch (requestType) {
		case REQUEST_TYPE_LINE_CODING:
			// nothing to do
			break;
		case REQUEST_TYPE_HID_FIRST:
		case REQUEST_TYPE_HID_SUBSEQUENT:
	        _u8 *p = data, rval = len != 8;
	        if(requestType == REQUEST_TYPE_HID_FIRST){
	            hidStatus = p[1]; /* second byte is data length */
	            p += 2;
	            len -= 2;
	            requestType = REQUEST_TYPE_HID_SUBSEQUENT;
	        }
	        do{
	            if(!hidStatus)
	                break;
	            stkSetRxChar(*p++);
	            hidStatus--;
	        }while(--len);
	        return rval;    /* the last packet must have 7 bytes insted of 8 */
	}

	return 1;
}

void usbFunctionWriteOut(_u8 *data, _u8 len)
{

}
