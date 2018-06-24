/*
 * File:   Conuter.c
 * Author: Hassan
 *
 * Created on 23 ?????, 2018, 11:28 ?
 */


#include <xc.h>
#include "Counter.h"
#define _XTAL_FREQ 16000000

void INIT() {
    TRISDbits.TRISD1 = 0; //as output 
    TRISAbits.TRISA0 = 0; //as output
    TRISCbits.TRISC4=0;
    TRISDbits.TRISD2=0;
    TRISDbits.TRISD3=0;
    TRISB = 0x00;
}

char Display(char minut) {
    char x;
    x = counter[minut];
    return x;
}

void Minuet_Green() {
    
    for (int i = 0; i < 30; i++) {
        RED=0;
        Yellow=0;
        Green=1;
        for(int j=0;j<50;j++){
        onces = i / 10;
        PORTB = Display(onces);
        Digit1 = 1;
        __delay_ms(10);
        Digit1 = 0;
        Tens = i % 10;
        PORTB = Display(Tens);
        Digit2 = 1;
        __delay_ms(10);
        Digit2 = 0;
        }
    }
}

void Minuet_Yellow() {
    //open Blutooth ;
    //Connection Blutooth ;
    for (int i = 0; i < 10; i++) {
        RED=0;
        Yellow=1;
        Green=0;
        for(int j=0;j<50;j++){
        onces = i / 10;
        PORTB = Display(onces);
        Digit1 = 1;
        __delay_ms(10);
        Digit1 = 0;
        Tens = i % 10;
        PORTB = Display(Tens);
        Digit2 = 1;
        __delay_ms(10);
        Digit2 = 0;

        }
    }
   

}

void Minuet_Red() {
    //Send Blutooth Data Stop ;
    for (int i = 0; i < 30; i++) {
        RED=1;
        Yellow=0;
        Green=0;
        for(int j=0;j<50;j++){
        onces = i / 10;
        PORTB = Display(onces);
        Digit1 = 1;
        __delay_ms(10);
        Digit1 = 0;
        Tens = i % 10;
        PORTB = Display(Tens);
        Digit2 = 1;
        __delay_ms(10);
        Digit2 = 0;

        }
    }
}




