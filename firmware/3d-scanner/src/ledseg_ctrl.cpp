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


#include "common.h"
#include "ledseg_ctrl.h"
#include "drv/avr_spi.h"

static _u16 _cached_segval[2];


extern const uint16_t PROGMEM legseg_digitbitmap[16] = {
    0xFC,     /* 0 : a,b,c,d,e,f */
    0x18,     /* 1 : b,c */
    0x36C,    /* 2 : a, b, g1, g2, d, e */
    0x33C,    /* 3 : a, b, g1, g2, c, d */
    0x398,    /* 4 : b, g1, g2, c, f */
    0x3B4,    /* 5 : a, g1, g2, c, d, f */
    0x3F4,    /* 6 : a, g1, g2, c, d, e, f */
    0x1C,     /* 7 : a, b, c */
    0x3FC,    /* 8 : a, b, c, d, e, f, g1, g2 */
    0x3BC,    /* 9 : a, b, c, d, f, g1, g2 */
    0x3DC,    /* A : a, b, c, e, f, g1, g2 */
    0x29E4,   /* B : a, d, e, f, g1, J, M */
    0xE4,     /* C : a, d, e, f */
    0x378,    /* D : b, c, d, e, g1, g2 */
    0x1E4,    /* E : a, d, e, f, g1 */
    0x1C4,    /* F : a, e, f, g1 */
}; 


void ledseg_init()
{
    DIGITAL_WRITE(ISP_MOSI, LOW);
    DIGITAL_WRITE(ISP_SCK, LOW);
    DIGITAL_WRITE(ISP_RST, LOW);
    PIN_MODE(ISP_RST,OUTPUT);
    PIN_MODE(ISP_MOSI, OUTPUT);
    PIN_MODE(ISP_SCK, OUTPUT);

    /* Enable SPI, Master, set clock rate */
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void ledseg_heartbeat()
{
    static _u8 odd = 0;

    if (odd) {
        spi_tx(~(_cached_segval[0] >> 8));
        spi_tx( ~((_cached_segval[0] & 0xFF) | BIT_SEG2_EN) );
        DIGITAL_WRITE(ISP_RST, LOW);
        _delay_us(1);
        DIGITAL_WRITE(ISP_RST, HIGH);
        odd = 0;
    } else {
        spi_tx(~(_cached_segval[1] >> 8));
        spi_tx( ~((_cached_segval[1] & 0xFF) | BIT_SEG1_EN) );
        DIGITAL_WRITE(ISP_RST, LOW);
        _delay_us(1);
        DIGITAL_WRITE(ISP_RST, HIGH);
        odd = 1;
    }
    
}

void ledseg_rawoutput(_u16 seg1, _u16 seg2)
{
    _cached_segval[0] = seg1;
    _cached_segval[1] = seg2;    
}

void ledseg_hexout(_u8 hex)
{
    _u16 val1 = pgm_read_word((uint16_t)&legseg_digitbitmap[hex & 0xF]);
    _u16 val2 = pgm_read_word((uint16_t)&legseg_digitbitmap[hex>>4]);
    ledseg_rawoutput(val1, val2);
}

