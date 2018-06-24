/*
 * File:   Test.c
 * Author: Hassan
 *
 * Created on 24 ?????, 2018, 12:58 ?
 */


#include <xc.h>

#include "Counter.h"

void main(void) {
    INIT();
    while(1){
        Minuet_Red();
        Minuet_Yellow();
        Minuet_Green();
    }
}
