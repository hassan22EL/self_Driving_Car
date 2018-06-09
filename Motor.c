/*
 * File:   Motor.c
 * Author: Hassan Elsaied
 *
 * Created on April 15, 2018, 5:57 PM
 */


#include <xc.h>
#include "Motor.h"
#include "../inulde/PWM_CCP.X/PWM1.h"


void InitCar() {
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    PWM1_Init(5000);
    PWM1_OPEN();
    PWM1_Set_Duty(80);
    Stop();
}

void GOForward() {
    Forward = 1;
    Backward = 0;
}

void GoBackWard() {
    Forward = 0;
    Backward = 1;
}

void Stop() {

    Forward = 0;
    Backward = 0;
}

void steerRight() {
    Right = 1;
    Left = 0;
}

void Steerleft() {
    Right = 0;
    Left = 1;
}

void SteerStright() {
    Right = 0;
    Left = 0;
}
