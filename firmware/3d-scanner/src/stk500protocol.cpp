/*
 *  RoboPeak Bridge 
 *  STK500v2 Sample code for extensions
 *
 *  Due to firmware size limitation, the Sunjar RF AP also uses STK500v2 protocol to 
 *  communicate with the PC.
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
 * Name: stk500protocol.c
 * Project: AVR-Doper
 * Author: Christian Starkjohann <cs@obdev.at>
 * Creation Date: 2006-06-19
 * Tabsize: 4
 * Copyright: (c) 2006 by Christian Starkjohann, all rights reserved.
 * License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
 * Revision: $Id: stk500protocol.c 566 2008-04-26 14:21:47Z cs $
 */

#include "common.h"
#include "stk500protocol.h"
#include "rp_stk500_protocol.h"




/* The following versions are reported to the programming software: */
#define STK_VERSION_HW      1
#define STK_VERSION_MAJOR   2
#define STK_VERSION_MINOR   4

#define BUFFER_SIZE     281 /* results in 275 bytes max body size */
#define RX_TIMEOUT      200 /* timeout in milliseconds */

#define STK_TXMSG_START 5


static _u8        rxBuffer[BUFFER_SIZE];
static _u16       rxPos;
static utilWord_t rxLen;
static _u8        rxBlockAvailable;

static _u8        txBuffer[BUFFER_SIZE];
static _u16       txPos, txLen;

stkParam_t      stkParam = {{
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    STK_VERSION_HW, STK_VERSION_MAJOR, STK_VERSION_MINOR, 0, 50, 0, 1, 0x80,
                    2, 0, 0xaa, 0, 0, 0, 0, 0,
                }};
utilDword_t     stkAddress;

/* ------------------------------------------------------------------------- */

void    stkIncrementAddress(void)
{
    stkAddress.dword++;
}

/* ------------------------------------------------------------------------- */

void stkSetTxMessage(_u16 len)
{
    _u8   *p = txBuffer, sum = 0;

    *p++ = STK_STX;
    *p++ = rxBuffer[1];  /* sequence number */
    *p++ = utilHi8(len);
    *p++ = len;
    *p++ = STK_TOKEN;
    txPos = 0;
    len += 6;
    txLen = len--;
    p = txBuffer;
    while(len--){
        sum ^= *p++;
    }
    *p = sum;
}

/* ------------------------------------------------------------------------- */

static void setParameter(_u8 index, _u8 value)
{
    if(index == STK_PARAM_OSC_PSCALE){
        // NOT SUPPORT
    }else if(index == STK_PARAM_OSC_CMATCH){
        // OCR2 = value;
        // TODO!
    }
    index &= 0x1f;
    stkParam.bytes[index] = value;
}

static _u8 getParameter(_u8 index)
{
    if(index == STK_PARAM_OSC_PSCALE)
        // NOT SUPPORT
        return 0;//HW_GET_T2_PRESCALER();
    if(index == STK_PARAM_OSC_CMATCH)
        // NOT SUPPORT
        return 0;//OCR2;
    index &= 0x1f;
    return stkParam.bytes[index];
}

/* ------------------------------------------------------------------------- */

/* Use defines for the switch statement so that we can choose between an
 * if()else if() and a switch/case based implementation. switch() is more
 * efficient for a LARGE set of sequential choices, if() is better in all other
 * cases.
 */
#if 0
#define SWITCH_START        if(0){
#define SWITCH_CASE(value)  }else if(cmd == (value)){
#define SWITCH_CASE2(v1,v2) }else if(cmd == (v1) || cmd == (v2)){
#define SWITCH_CASE3(v1,v2,v3) }else if(cmd == (v1) || cmd == (v2) || (cmd == v3)){
#define SWITCH_CASE4(v1,v2,v3,v4) }else if(cmd == (v1) || cmd == (v2) || cmd == (v3) || cmd == (v4)){
#define SWITCH_DEFAULT      }else{
#define SWITCH_END          }
#else
#define SWITCH_START        switch(cmd){{
#define SWITCH_CASE(value)  }break; case (value):{
#define SWITCH_CASE2(v1,v2) }break; case (v1): case(v2):{
#define SWITCH_CASE3(v1,v2,v3) }break; case (v1): case(v2): case(v3):{
#define SWITCH_CASE4(v1,v2,v3,v4) }break; case (v1): case(v2): case(v3): case(v4):{
#define SWITCH_DEFAULT      }break; default:{
#define SWITCH_END          }}
#endif

void stkEvaluateRxMessage(void) /* not static to prevent inlining */
{
    _u8       i, cmd;
    utilWord_t  len = {2};  /* defaults to cmd + error code */
    void        *param;

    //
    LED_RX_ON();

    /*
        NOTICE: Please do NOT forget to set the following values
    
        txBuffer[STK_TXMSG_START]     : cmd
        &txBuffer[STK_TXMSG_START+1]  : payload data
        len.word                      : sizeof(cmd) + sizeof(payload data)
        len.byte[0]                   : len.word & 0xFF
    */

    cmd = rxBuffer[STK_TXMSG_START];
    param = &rxBuffer[STK_TXMSG_START + 1];
    
    // The default return value
    txBuffer[STK_TXMSG_START] = cmd;
    txBuffer[STK_TXMSG_START + 1] = STK_STATUS_CMD_OK;
    
    
    // reduce the code size by removing the switch-case jump table
    if (cmd == RPINFRA_STK_CMD_ECHO){
        // simple echo back
    }

    else if (cmd == RPINFRA_STK_CMD_GET_ADC){
        *(_u16 *)&txBuffer[STK_TXMSG_START+1] = analogRead(A_ADC0);
        len.bytes[0] = sizeof(unsigned long) + 1;
    }else if (cmd == RPINFRA_STK_CMD_SET_PWM){
        PWM_ENABLE( IO_PWM0 );
        PWM_SET( IO_PWM0, rxBuffer[STK_TXMSG_START + 1] );
        
    }else if (cmd == SCANNER_CMD_SET_HEADING) {
        // set heading
        onSetHeading(*(_u16 *)&rxBuffer[STK_TXMSG_START + 1]);
    }else if (cmd == SCANNER_CMD_ENABLE_LASER) {
        onEnableLaser();
    }else if (cmd == SCANNER_CMD_DISABLE_LASER) {
        onDisableLaser();
    }

    LED_RX_OFF();

    //    
    stkSetTxMessage(len.word);
}

/* ------------------------------------------------------------------------- */

void    stkSetRxChar(_u8 c)
{
    static _u8 s_sum = 0;
    
    if(timerLongTimeoutOccurred()){
        rxPos = 0;
    }
    if(rxPos == 0){     /* only accept STX as the first character */
        if(c == STK_STX){
            s_sum = 0;
        }else{
            goto FINAL;
        }
    }
    
        if(rxPos < BUFFER_SIZE){
            rxBuffer[rxPos++] = c;
            s_sum ^= c;
            if(rxPos == 4){     /* do we have length byte? */
                rxLen.bytes[0] = rxBuffer[3];
                rxLen.bytes[1] = rxBuffer[2];
                rxLen.word += 6;
                if(rxLen.word > BUFFER_SIZE){    /* illegal length */
                    rxPos = 0;      /* reset state */
                }
            }else if(rxPos == 5){   /* check whether this is the token byte */
                if(c != STK_TOKEN){
                    rxPos = 0;  /* reset state */
                }
            }else if(rxPos > 4 && rxPos == rxLen.word){ /* message is complete */
                rxPos = 0;
                if(s_sum == 0){   /* check sum is correct, evaluate rx message */
                    rxBlockAvailable = 1;
                }else{          /* checksum error */
                    txBuffer[STK_TXMSG_START] = STK_ANSWER_CKSUM_ERROR;
                    txBuffer[STK_TXMSG_START + 1] = STK_ANSWER_CKSUM_ERROR;
                    stkSetTxMessage(2);
                }
            }
        }else{  /* overflow */
            rxPos = 0;  /* reset state */
        }


FINAL:
    timerSetupLongTimeout(RX_TIMEOUT);
}

int stkGetTxCount(void)
{
    return txLen - txPos;
}

int stkGetTxByte(void)
{
    _u8   c;

    if(txLen == 0){
        return -1;
    }
    c = txBuffer[txPos++];
    if(txPos >= txLen){         /* transmit ready */
        txPos = txLen = 0;
    }
    return c;
}

void    stkPoll(void)
{
    if(rxBlockAvailable){
        rxBlockAvailable = 0;
        stkEvaluateRxMessage();
    }
}

/* ------------------------------------------------------------------------- */
