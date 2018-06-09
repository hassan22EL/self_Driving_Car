/*
 * File:   Sensor.c
 * Author: Hassan Elsaied
 *
 * Created on April 15, 2018, 6:05 PM
 */


#include <xc.h>
#include "Sensor.h"
#define _XTAL_FREQ  16000000

void INITSensor() {
    TRISB0 = 1;
    TRISB1 = 0;
    TRISB2 = 0;
    TRISB3 = 1;
    T1CON = 0x10;
}

int ForwardSensor() {
    int Distance;
    Trigger = 1;
    __delay_us(10);
    Trigger = 0;
    while (!Echo);
    T1CONbits.TMR1ON = 1; //open Timer one if Echo signal high
    while (Echo);
    T1CONbits.TMR1ON = 0; // off Timer one if Echo Signal Low
    Time = TMR1L | (TMR1H << 8);
    Distance = Time;
    Distance = (Distance / 58.82);
    Distance = Distance + 1;
    return Distance;
}

int RightForwardSensor() {
    int Distance;
    Trigger_FR = 1;
    __delay_us(10);
    Trigger_FR = 0;
    while (!Echo_FR);
    T1CONbits.TMR1ON = 1; //open Timer one if Echo signal high
    while (Echo_FR);
    T1CONbits.TMR1ON = 0; // off Timer one if Echo Signal Low
    Time = TMR1L | (TMR1H << 8);
    Distance = Time;
    Distance = (Distance / 58.82);
    Distance = Distance + 1;
    return Distance;
}
