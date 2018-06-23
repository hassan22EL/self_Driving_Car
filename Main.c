/*
 * File:   Main.c
 * Author: Hassan Elsaied
 *
 * Created on April 15, 2018, 6:16 PM
 */


#include    <xc.h>
#include "../inulde/PWM_CCP.X/PWM1.h"
#include "Blutooth.h"
#include "LEDS.h"
#include "Sensor.h"
#include "Motor.h"
#define _XTAL_FREQ  16000000
#pragma config FOSC =HS 
#pragma config WDTE =OFF
#pragma config PWRTE =OFF
#pragma config BOREN =ON
#pragma config LVP =OFF
#pragma config CPD=OFF 
#pragma config WRT=OFF
#pragma config CP=OFF 

void main(void) {
    int Distance_FS, Distance_RFS, Distance_LFS, Distance_RBS, Distance_LBS;
    INITSensor();
    INIT_LEDS();
    InitCar();
    while (1) {
        TMR1L = 0x00;
        TMR1H = 0x00;
        Distance_FS = ForwardSensor();
        if (Distance_FS > 350) {
            GOForward();
            SteerStraight();
            PWM1_Set_Duty(200);
            LED_OFF();
        } else if (Distance_FS <= 350 && Distance_FS > 200) {
            GOForward();
            SteerStraight();
            PWM1_Set_Duty(150);
            LED_OFF();
        } else if (Distance_FS <= 200 && Distance_FS > 130) {
            GOForward();
            SteerStraight();
            PWM1_Set_Duty(120);
            LED_OFF();
        } else if (Distance_FS <= 130 && Distance_FS < 80) {
            GOForward();
            SteerStraight();
            PWM1_Set_Duty(100);
            LED_OFF();
        } else if (Distance_FS <= 80 && Distance_FS > 50) {
            GOForward();
            SteerStraight();
            PWM1_Set_Duty(80);
            LED_OFF();
        } else if (Distance_FS <= 50 && Distance_FS > 30) {
            GOForward();
            SteerStraight();
            PWM1_Set_Duty(30);
            Car_Back();
        } else if (Distance_FS <= 30) {
            Distance_RFS = RightForwardSensor();
            Distance_RBS = RightBackSensor();
            GOForward();
            Car_Back();
            PWM1_Set_Duty(20);
            if (Distance_RBS > 20 && Distance_RFS > 20) {
                steerRight();
                Car_Right();
                SteerStraight();
            } else {
                GOForward();
                Distance_LFS = LeftForwardSensor();
                Distance_LBS = LeftBackSensor();
                if (Distance_LFS > 20 && Distance_LBS > 20) {
                    Steerleft();
                    Car_Left();
                    SteerStraight();
                } else {
                    PWM1_Set_Duty(0);
                    Stop();
                    Car_Wait();
                }
            }
        }

    }
}

