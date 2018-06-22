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
    TRISB0 = 0; // this pin output Trigger FS
    TRISB1 = 1; // this pin input Echo FS 
    TRISB2 = 0; // this pin output Trigger LFS
    TRISB3 = 1; // this pin input Echo LFS
    TRISB4 = 0; // this pin output Trigger RFS
    TRISB5 = 1; // this pin input Echo RFS 
    TRISB6 = 0; // this pin output Trigger RBS
    TRISB7 = 1; // this pin input Echo RBS
    TRISC0 = 0; // this pin output Trigger LBS
    TRISC1 = 1; // this pin input Echo LBS
    T1CON = 0x10; // open Timer one 
}
/*Forward Sensor*/
int ForwardSensor() {
    int Distance;
    Trigger_FS = 1;
    __delay_us(10);
    Trigger_FS = 0;
    while (!Echo_FS);
    T1CONbits.TMR1ON = 1; //open Timer one if Echo signal high
    while (Echo_FS);
    T1CONbits.TMR1ON = 0; // off Timer one if Echo Signal Low
    Time = TMR1L | (TMR1H << 8);
    Distance = Time;
    Distance = (Distance / 58.82);
    Distance = Distance + 1;
    return Distance;
}
/*  Right Forward Sensor*/
int RightForwardSensor() {
    int Distance;
    Trigger_RFS = 1;
    __delay_us(10);
    Trigger_RBS = 0;
    while (!Echo_RFS);
    T1CONbits.TMR1ON = 1; //open Timer one if Echo signal high
    while (Echo_RFS);
    T1CONbits.TMR1ON = 0; // off Timer one if Echo Signal Low
    Time = TMR1L | (TMR1H << 8);
    Distance = Time;
    Distance = (Distance / 58.82);
    Distance = Distance + 1;
    return Distance;
}
/* Left Forward Sensor*/
int LeftForwardSensor() {
    int Distance;
    Trigger_LFS = 1;
    __delay_us(10);
    Trigger_LFS = 0;
    while (!Echo_LFS);
    T1CONbits.TMR1ON = 1;
    while (Echo_LFS);
    Time = TMR1L | (TMR1H << 8);
    Distance = Time;
    Distance = (Distance / 58.82);
    Distance = Distance + 1;
    return Distance;
}
/*Right Back  Sensor*/
int RightBackSensor() {
    int Distance;
    Trigger_RBS = 1;
    __delay_us(10);
    Trigger_RBS = 0;
    while (!Echo_RBS);
    T1CONbits.TMR1ON = 1;
    while (Echo_RBS);
    Time = TMR1L | (TMR1H << 8);
    Distance = Time;
    Distance = Distance / 58.82;
    Distance = Distance + 1;
    return Distance;
}
/*Right left Sensor*/
int LeftBackSensor() {
    int Distance;
    Trigger_LBS = 1;
    __delay_us(10);
    Trigger_LBS = 0;
    while (!Echo_LBS);
    T1CONbits.TMR1ON = 1;
    while (Echo_LBS);
    Time = TMR1L | (TMR1H << 8);
    Distance = Time;
    Distance = Distance / 58.82;
    Distance = Distance + 1;
    return Distance;


}
