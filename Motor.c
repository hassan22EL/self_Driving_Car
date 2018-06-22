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
/*Forward Motion*/
void GOForward() {
    Forward = 1;   //RD6 as out one 
    Backward = 0;   //RD7 as out zero 
}
/*Backward Motion*/
void GoBackWard() {
    Forward = 0;  //RD6 as out zero 
    Backward = 1;  //RD7 as out one 
}
/*Stop Car */
void Stop() {

    Forward = 0; //RD6 as out zero 
    Backward = 0; // RD6 as out zero 
}
/*Car steer Right */
void steerRight() {
    Right = 1; //RD4 as out one 
    Left = 0;  //RD4 as out zero 
}
 /*Car steer left */
void Steerleft() {
    Right = 0;  //RD4 as out zero  
    Left = 1;   //RD4 as out one 
}
/*Car Steer straight*/
void SteerStraight() {
      //Forward motor as stop 
    Right = 0; 
     Left = 0; 
}
