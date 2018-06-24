/*
 * File:   Blutooth.c
 * Author: Hassan
 *
 * Created on 23 ?????, 2018, 02:06 ?
 */




#include <xc.h>
#define _XTAL_FREQ  16000000
#include "../inulde/EUSART.X/EUSART.h"
#include "Blutooh.h"
void Blutooth_INIT() {
   
    EUSART_Buad_Rate(38400);
     Sbits.sync = 0;
    EUSART_INIT();
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    RCIE = 1;
    TXIE = 1;

}

void Blutooth_load_char(char byte) {
    EUSART_Write(byte);
}

void Blutooth_load_String(char *String) {
    EUSART_Write_Text(String);
}

void boradcast_Blutooth() {
    TXREG = 13;
    __delay_ms(500);
}

char ReadChar_Blutooth() {
    if (OERR) {
        CREN = 0;
        CREN = 1;
    }
    if (RCIF == 1) {
        return EUSART_Read();
    } else {
        return 0;
    }


}


