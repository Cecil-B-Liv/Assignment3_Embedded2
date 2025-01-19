#include <stdio.h>
#include "SYS_init.h"
#include "NUC100Series.h"

void startCounter() {
    // Start Timer 0 counting by setting TCSR[30] = 1
    // TCSR[30] is the "CEN" bit (Counter Enable)
    TIMER0->TCSR |= (1 << 30);
}

void pauseCounter() {
    // Pause Timer 0 counting by clearing TCSR[30] = 0
    // TCSR[30] is the "CEN" bit (Counter Enable)
    TIMER0->TCSR &= ~(1 << 30);
}