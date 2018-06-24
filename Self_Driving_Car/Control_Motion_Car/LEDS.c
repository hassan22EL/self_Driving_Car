/*
 * File:   LEDS.c
 * Author: Hassan Elsaied
 *
 * Created on April 15, 2018, 6:24 PM
 */


#include <xc.h>
#include "LEDS.h"
#define _XTAL_FREQ  16000000
#include "../inulde/ADC.X/ADC.h"

void INIT_LEDS() {
    ADC_Init(1, 0); //select channel AN1 and select Clock fosc/2 
    ANDEL = 0x00; // All pins Analog 
    TRISDbits.TRISD0 = 0; // pin output on  LED BL_LED
    TRISDbits.TRISD1 = 0; //  pin output on  LED BR_LED 
    TRISDbits.TRISD4 = 0; // pin output on  LED FR_LED
    TRISDbits.TRISD5 = 0; //pin output on  LED FL_LED
    TRISAbits.TRISA1 = 1; //this pins input 
    BL_LED = 0; //oFf BL_LED 
    BR_LED = 0; //oFf BR_LED
    FR_LED = 0; //oFf FR_LED
    FL_LED = 0; //oFf FL_LED
}

void LED_OFF() {
    BL_LED = 0; //oFf BL_LED 
    BR_LED = 0; //oFf BR_LED
    FR_LED = 0; //oFf FR_LED
    FL_LED = 0; //oFf FL_LED
}

void Car_Wait() {
    BL_LED = 1;
    BR_LED = 1;
    FL_LED = 1;
    FR_LED = 1;
    __delay_ms(1000);
    BL_LED = 0;
    BR_LED = 0;
    FL_LED = 0;
    FR_LED = 0;
}

void Car_Left() {

    FL_LED = 1;
    BL_LED = 1;
    __delay_ms(1000);
    FL_LED = 0;
    BL_LED = 0;
}

void Car_Right() {
    FR_LED = 1;
    BR_LED = 1;
    __delay_ms(1000);
    FR_LED = 0;
    BR_LED = 0;
}

void Car_Back() {
    BR_LED = 1;
    BL_LED = 1;
}

void Car_Forward() {
    int a = Read_ADC(1);
    if (a == 0) {

        FR_LED = 1;
        FL_LED = 1;
    } else {
        FR_LED = 0;
        FL_LED = 0;
    }
}

void Car_BackS() {
    int a = Read_ADC(1);
    if (a == 0) {
        BR_LED = 1;
        BL_LED = 1;
    } else {
        BR_LED = 0;
        BL_LED = 0;
    }

}
