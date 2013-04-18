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
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _RP_BRIDGE_CONF_H
#define _RP_BRIDGE_CONF_H


//-------------------------------------------------
// GENERAL FEATURES

//#define _DEBUG

//-------------------------------------------------

// Jumper used for mode selection
#define JUMPER_MODE_SEL         16   //PC2

#define PORT_LED        PORTD
#define DDR_LED         DDRD
#define LED_TX          PD7
#define LED_RX          PD6

#define LED_INIT()               \
    do{                          \
        cbi(PORT_LED, LED_TX);   \
        cbi(PORT_LED, LED_RX);   \
        cbi(DDR_LED, LED_TX);    \
        cbi(DDR_LED, LED_RX);    \
    }while(0)

#define LED_RX_ON()    sbi(DDR_LED, LED_RX)
#define LED_TX_ON()    sbi(DDR_LED, LED_TX)

#define LED_RX_OFF()    cbi(DDR_LED, LED_RX)
#define LED_TX_OFF()    cbi(DDR_LED, LED_TX)

//mapping according to The Sunjar++ Main Circuit 1.0 Alpha (Rev 1)

// ArduinoLite Pin mapping:
// ATMEL ATMEGA8 & 168 
//
//                  +-\/-+
//            PC6  1|    |28  PC5 (AI 5/*D19)
//      (D 0) PD0  2|    |27  PC4 (AI 4/*D18)
//      (D 1) PD1  3|    |26  PC3 (AI 3/*D17)
//      (D 2) PD2  4|    |25  PC2 (AI 2/*D16)
// PWM+ (D 3) PD3  5|    |24  PC1 (AI 1/*D15)
//      (D 4) PD4  6|    |23  PC0 (AI 0/*D14)
//            VCC  7|    |22  GND
//            GND  8|    |21  AREF
//     *(D20) PB6  9|    |20  AVCC
//     *(D21) PB7 10|    |19  PB5 (D 13)
// PWM+ (D 5) PD5 11|    |18  PB4 (D 12)
// PWM+ (D 6) PD6 12|    |17  PB3 (D 11) PWM
//      (D 7) PD7 13|    |16  PB2 (D 10) PWM
//      (D 8) PB0 14|    |15  PB1 (D 9) PWM
//                  +----+

//-------------------------------------------------
// SUNJAR BRIDGE
#define PKT_FLG_DDR  DDRD
#define PKT_FLG_IN   PIND
#define PKT_FLG_PORT PORTD
#define PKT_FLG_PIN  PD3

#define RF_RESET_DDR  DDRD
#define RF_RESET_PORT PORTD
#define RF_RESET_PIN  PD4

#define SS_PIN          PC0
#define SPI_SS_PORT     PORTC
#define SPI_SS_DDR      DDRC
#define SPI_SS_PIN      PINC

#define ON_PKT_INT       1
#define ON_PKT_VECTOR    INT1_vect


#define PUBLIC_RF_CHANNEL  1   //this channel is used to configure the sunjarpp
#define DEFAULT_RF_CHANNEL 5

#define RF_CHANNEL      DEFAULT_RF_CHANNEL
#define RF_RX_TX_ADDR   0xBEEF


// represented in ArduinoLite Pin
#define ISP_RST   10
#define ISP_MOSI  11
#define ISP_MISO  12
#define ISP_SCK   13

#define USB_PORT_NAME           B  //USB Port 
#define USB_PORT                PORTB
#define USB_DDR                 DDRB
#define USB_PORT_P              1  //USB D+ Pin number
#define USB_PORT_N              0  //USB D- Pin number

// The following marcos are not used yet.
// The avr-usb always uses Int0 
#define ON_USB_INT              0
#define ON_USB_VECTOR           INT0_vect


//-------------------------------------------------
// GENERAL IO BOARD

// ADC & PWM  (ArduinoLite Pins)
#define IO_PWM0    11//PB3
#define IO_PWM1    12//PB4  

#define IO_ADC0    15
#define IO_ADC1    17


#define A_ADC0   1
#define A_ADC1   3
#define A_ADC2   6
#define A_ADC3   7
#endif