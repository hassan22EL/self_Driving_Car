/*
 * File:   Main.c
 * Author: Hassan Elsaied
 *
 * Created on April 15, 2018, 6:16 PM
 */


#include <xc.h>

#include "Sensor.h"
#include "LEDS.h"
#include "Motor.h"
#include "../inulde/PWM_CCP.X/PWM1.h"
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
    int Distance, Distance_FR;
    INITSensor();
    INIT_LEDS();
    InitCar();
    while (1) {
        TMR1H = 0x00;
        TMR1L = 0x00;
        Distance = ForwardSensor();
        Distance_FR = RightForwardSensor();
        if (Distance > 350) {
            GOForward();
            SteerStright();
            LED_Back_Right = 0;
            LED_Back_left = 0;
            LED_Forward_Right = 1;
            LED_Back_Right = 0;
            PWM1_Set_Duty(200);
        } else if (Distance <= 350 && Distance > 131) {
            GOForward();
            SteerStright();
            LED_Back_Right = 0;
            LED_Back_left = 0;
            LED_Forward_Right = 0;
            LED_Back_Right = 1;
            PWM1_Set_Duty(150);
        } else if (Distance <= 131 && Distance > 85) {
            GOForward();
            SteerStright();
            PWM1_Set_Duty(80);
            LED_Back_Right = 1;
            LED_Back_left = 0;
            LED_Forward_Right = 0;
            LED_Back_Right = 0;
        } else if (Distance <= 85 && Distance > 45) {
            GOForward();
            SteerStright();
            PWM1_Set_Duty(40);
            LED_Back_Right = 1;
            LED_Back_left = 1;
            LED_Forward_Right = 0;
            LED_Back_Right = 0;
        } else if (Distance <= 45) {
            if (Distance_FR > 50) {
                GOForward();
                Steerleft();
                PWM1_Set_Duty(20);
                LED_Forward_left = 1;
                LED_Back_left = 1;
                __delay_ms(2000);
                SteerStright();
                LED_Forward_left = 0;
                LED_Back_left = 0;
            } else {
                Stop();
                LED_Back_Right = 1;
                LED_Back_left = 1;
                LED_Forward_Right = 0;
                LED_Forward_left = 0;

            }


        }
    }
}

