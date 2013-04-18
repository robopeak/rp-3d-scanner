/*
 * Name: stk500protocol.h
 * Project: AVR-Doper
 * Author: Christian Starkjohann <cs@obdev.at>
 * Creation Date: 2006-06-19
 * Tabsize: 4
 * Copyright: (c) 2006 by Christian Starkjohann, all rights reserved.
 * License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
 * Revision: $Id: stk500protocol.h 566 2008-04-26 14:21:47Z cs $
 */

/*
General Description:
This module implements parsing of the STK500v2 protocol and assembly of
response messages. It calls functions in the isp and hvprog modules to
perform the real work.
*/

#ifndef __stk500protocol_h_included__
#define __stk500protocol_h_included__

#include "util/timer.h"

typedef union{
    _u16  word;
    _u8   bytes[2];
}utilWord_t;

typedef union{
    _u32          dword;
    _u8           bytes[4];
}utilDword_t;

static inline _u8 utilHi8(_u16 x)
{
    utilWord_t converter;

    converter.word = x;
    return converter.bytes[1];
}

extern void stkSetRxChar(_u8 c);
extern int stkGetTxByte(void);
extern int stkGetTxCount(void);
extern void stkPoll(void);  /* must be called from main loop */

typedef union{
    _u8   bytes[32];
    struct{
        int   buildVersionLow;
        _u8   reserved1[14];
        _u8   hardwareVersion;
        _u8   softwareVersionMajor;
        _u8   softwareVersionMinor;
        _u8   reserved2;
        _u8   vTarget;
        _u8   vRef;
        _u8   oscPrescale;
        _u8   oscCmatch;
        _u8   sckDuration;
        _u8   reserved3;
        _u8   topcardDetect;
        _u8   reserved4;
        _u8   status;
        _u8   data;
        _u8   resetPolarity;
        _u8   controllerInit;
    }       s;
}stkParam_t;

/* ------------------------------------------------------------------------- */

extern utilDword_t  stkAddress;
extern stkParam_t   stkParam;

/* ------------------------------------------------------------------------- */
/* ----------------------- command parameter structs ----------------------- */
/* ------------------------------------------------------------------------- */

typedef struct stkEnterProgIsp{
    _u8   timeout;
    _u8   stabDelay;
    _u8   cmdExeDelay;
    _u8   synchLoops;
    _u8   byteDelay;
    _u8   pollValue;
    _u8   pollIndex;
    _u8   cmd[4];
}stkEnterProgIsp_t;

typedef struct stkLeaveProgIsp{
    _u8   preDelay;
    _u8   postDelay;
}stkLeaveProgIsp_t;

typedef struct stkChipEraseIsp{
    _u8   eraseDelay;
    _u8   pollMethod;
    _u8   cmd[4];
}stkChipEraseIsp_t;

typedef struct stkProgramFlashIsp{
    _u8   numBytes[2];
    _u8   mode;
    _u8   delay;
    _u8   cmd[3];
    _u8   poll[2];
    _u8   data[1];    /* actually more data than 1 byte */
}stkProgramFlashIsp_t;

typedef struct stkReadFlashIsp{
    _u8   numBytes[2];
    _u8   cmd;
}stkReadFlashIsp_t;

typedef struct stkReadFlashIspResult{
    _u8   status1;
    _u8   data[1];    /* actually more than 1 byte */
    /* uchar status2 */
}stkReadFlashIspResult_t;

typedef struct stkProgramFuseIsp{
    _u8   cmd[4];
}stkProgramFuseIsp_t;

typedef struct stkReadFuseIsp{
    _u8   retAddr;
    _u8   cmd[4];
}stkReadFuseIsp_t;

typedef struct stkMultiIsp{
    _u8   numTx;
    _u8   numRx;
    _u8   rxStartAddr;
    _u8   txData[1];  /* actually more than 1 byte */
}stkMultiIsp_t;

typedef struct stkMultiIspResult{
    _u8   status1;
    _u8   rxData[1];  /* potentially more than 1 byte */
    /* uchar status2 */
}stkMultiIspResult_t;

/* ------------------------------------------------------------------------- */

typedef struct stkEnterProgHvsp{
    _u8   stabDelay;
    _u8   cmdExeDelay;
    _u8   synchCycles;
    _u8   latchCycles;
    _u8   toggleVtg;
    _u8   powerOffDelay;
    _u8   resetDelay1;
    _u8   resetDelay2;
}stkEnterProgHvsp_t;

typedef struct stkLeaveProgHvsp{
    _u8   stabDelay;
    _u8   resetDelay;
}stkLeaveProgHvsp_t;

typedef struct stkChipEraseHvsp{
    _u8   pollTimeout;
    _u8   eraseTime;
}stkChipEraseHvsp_t;

typedef struct stkProgramFlashHvsp{
    _u8   numBytes[2];
    _u8   mode;
    _u8   pollTimeout;
    _u8   data[1];    /* actually more data than 1 byte */
}stkProgramFlashHvsp_t;

typedef struct stkReadFLashHvsp{
    _u8   numBytes[2];
}stkReadFlashHvsp_t;

#define stkReadFlashHvspResult_t    stkReadFlashIspResult_t

typedef struct stkProgramFuseHvsp{
    _u8   fuseAddress;
    _u8   fuseByte;
    _u8   pollTimeout;
}stkProgramFuseHvsp_t;

typedef struct stkReadFuseHvsp{
    _u8   fuseAddress;
}stkReadFuseHvsp_t;

/* ------------------------------------------------------------------------- */

typedef struct stkEnterProgPp{
    _u8   stabDelay;
    _u8   progModeDelay;
    _u8   latchCycles;
    _u8   toggleVtg;
    _u8   powerOffDelay;
    _u8   resetDelayMs;
    _u8   resetDelayUs;
}stkEnterProgPp_t;

#define stkLeaveProgPp_t        stkLeaveProgHvsp_t

typedef struct stkChipErasePp{
    _u8   pulseWidth;
    _u8   pollTimeout;
}stkChipErasePp_t;

#define stkProgramFlashPp_t     stkProgramFlashHvsp_t

#define stkReadFlashPp_t        stkReadFlashHvsp_t
#define stkReadFlashPpResult_t  stkReadFlashHvspResult_t

typedef struct stkProgramFusePp{
    _u8   address;
    _u8   data;
    _u8   pulseWidth;
    _u8   pollTimeout;
}stkProgramFusePp_t;

#define stkReadFusePp_t         stkReadFuseHvsp_t

/* ------------------------------------------------------------------------- */

#define STK_STX     27
#define STK_TOKEN   14

/* =================== [ STK general command constants ] =================== */

#define STK_CMD_SIGN_ON                         0x01
#define STK_CMD_SET_PARAMETER                   0x02
#define STK_CMD_GET_PARAMETER                   0x03
#define STK_CMD_SET_DEVICE_PARAMETERS           0x04
#define STK_CMD_OSCCAL                          0x05
#define STK_CMD_LOAD_ADDRESS                    0x06
#define STK_CMD_FIRMWARE_UPGRADE                0x07


/* =================== [ STK ISP command constants ] =================== */

#define STK_CMD_ENTER_PROGMODE_ISP              0x10
#define STK_CMD_LEAVE_PROGMODE_ISP              0x11
#define STK_CMD_CHIP_ERASE_ISP                  0x12
#define STK_CMD_PROGRAM_FLASH_ISP               0x13
#define STK_CMD_READ_FLASH_ISP                  0x14
#define STK_CMD_PROGRAM_EEPROM_ISP              0x15
#define STK_CMD_READ_EEPROM_ISP                 0x16
#define STK_CMD_PROGRAM_FUSE_ISP                0x17
#define STK_CMD_READ_FUSE_ISP                   0x18
#define STK_CMD_PROGRAM_LOCK_ISP                0x19
#define STK_CMD_READ_LOCK_ISP                   0x1A
#define STK_CMD_READ_SIGNATURE_ISP              0x1B
#define STK_CMD_READ_OSCCAL_ISP                 0x1C
#define STK_CMD_SPI_MULTI                       0x1D

/* =================== [ STK PP command constants ] =================== */

#define STK_CMD_ENTER_PROGMODE_PP               0x20
#define STK_CMD_LEAVE_PROGMODE_PP               0x21
#define STK_CMD_CHIP_ERASE_PP                   0x22
#define STK_CMD_PROGRAM_FLASH_PP                0x23
#define STK_CMD_READ_FLASH_PP                   0x24
#define STK_CMD_PROGRAM_EEPROM_PP               0x25
#define STK_CMD_READ_EEPROM_PP                  0x26
#define STK_CMD_PROGRAM_FUSE_PP                 0x27
#define STK_CMD_READ_FUSE_PP                    0x28
#define STK_CMD_PROGRAM_LOCK_PP                 0x29
#define STK_CMD_READ_LOCK_PP                    0x2A
#define STK_CMD_READ_SIGNATURE_PP               0x2B
#define STK_CMD_READ_OSCCAL_PP                  0x2C    

#define STK_CMD_SET_CONTROL_STACK               0x2D

/* =================== [ STK HVSP command constants ] =================== */

#define STK_CMD_ENTER_PROGMODE_HVSP             0x30
#define STK_CMD_LEAVE_PROGMODE_HVSP             0x31
#define STK_CMD_CHIP_ERASE_HVSP                 0x32
#define STK_CMD_PROGRAM_FLASH_HVSP              0x33
#define STK_CMD_READ_FLASH_HVSP                 0x34
#define STK_CMD_PROGRAM_EEPROM_HVSP             0x35
#define STK_CMD_READ_EEPROM_HVSP                0x36
#define STK_CMD_PROGRAM_FUSE_HVSP               0x37
#define STK_CMD_READ_FUSE_HVSP                  0x38
#define STK_CMD_PROGRAM_LOCK_HVSP               0x39
#define STK_CMD_READ_LOCK_HVSP                  0x3A
#define STK_CMD_READ_SIGNATURE_HVSP             0x3B
#define STK_CMD_READ_OSCCAL_HVSP                0x3C

/* =================== [ STK status constants ] =================== */

/* Success */
#define STK_STATUS_CMD_OK                       0x00

/* Warnings */
#define STK_STATUS_CMD_TOUT                     0x80
#define STK_STATUS_RDY_BSY_TOUT                 0x81
#define STK_STATUS_SET_PARAM_MISSING            0x82

/* Errors */
#define STK_STATUS_CMD_FAILED                   0xC0
#define STK_STATUS_CKSUM_ERROR                  0xC1
#define STK_STATUS_CMD_UNKNOWN                  0xC9

/* =================== [ STK parameter constants ] =================== */
#define STK_PARAM_BUILD_NUMBER_LOW              0x80
#define STK_PARAM_BUILD_NUMBER_HIGH             0x81
#define STK_PARAM_HW_VER                        0x90
#define STK_PARAM_SW_MAJOR                      0x91
#define STK_PARAM_SW_MINOR                      0x92
#define STK_PARAM_VTARGET                       0x94
#define STK_PARAM_VADJUST                       0x95
#define STK_PARAM_OSC_PSCALE                    0x96
#define STK_PARAM_OSC_CMATCH                    0x97
#define STK_PARAM_SCK_DURATION                  0x98
#define STK_PARAM_TOPCARD_DETECT                0x9A
#define STK_PARAM_STATUS                        0x9C
#define STK_PARAM_DATA                          0x9D
#define STK_PARAM_RESET_POLARITY                0x9E
#define STK_PARAM_CONTROLLER_INIT               0x9F

/* =================== [ STK answer constants ] =================== */

#define STK_ANSWER_CKSUM_ERROR                  0xB0


#endif /* __stk500protocol_h_included__ */
