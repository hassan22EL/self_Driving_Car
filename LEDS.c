/*
 * File:   LEDS.c
 * Author: Hassan Elsaied
 *
 * Created on April 15, 2018, 6:24 PM
 */


#include <xc.h>
#include "LEDS.h"
#define _XTAL_FREQ  16000000
void INIT_LEDS() {
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB7 = 0;
    TRISDbits.TRISD4 = 0;
    TRISDbits.TRISD5 = 0;
    LED_Back_Right = 0;
    LED_Back_left = 0;
    LED_Forward_Right = 0;
    LED_Forward_left = 0;
}

void Car_Wait() {
    LED_Back_Right = 1;
    LED_Back_left = 1;
    LED_Forward_Right = 1;
    LED_Forward_left = 1;
    __delay_ms(1000);
    LED_Back_Right = 0;
    LED_Back_left = 0;
    LED_Forward_Right = 0;
    LED_Forward_left = 0;
}

void Car_Left() {
    LED_Forward_left = 1;
    LED_Back_left = 1;
    __delay_ms(1000);
    LED_Back_left = 0;
    LED_Forward_left = 0;
}

void Car_Right() {
    LED_Forward_Right = 1;
    LED_Back_Right = 1;
    __delay_ms(1000);
    LED_Back_Right = 0;
    LED_Forward_Right = 0;
}

void Car_Back() {
    LED_Back_Right = 1;
    LED_Back_left = 1;

}

void Car_Forward() {
    LED_Forward_Right = 1;
    LED_Forward_left = 1;
}
 