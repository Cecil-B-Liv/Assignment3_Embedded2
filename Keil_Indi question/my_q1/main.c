
#include <stdio.h>
#include "NUC100Series.h"
#include "SYS_init.h"

#define LXT_STATUS 1 << 1
#define HXT_STATUS 1 << 0
#define TIMER1_COUNTS 65536 - 1

void setupClock(void);
void setupGPIO(void);
void setupTimer(void);

volatile int count;

int main(void) {
    SYS_UnlockReg();  // Unlock protected regs
    setupClock();
    setupGPIO();
    setupTimer();
    SYS_LockReg();  // Lock protected registers

    while (1) {
        if (!(PB->PIN & (1 << 15))) {   // polling
            TIMER1->TCSR |= (1 << 30);  // start counting
        }
    }
}

void setupClock() {
    CLK->PWRCON |= (1 << 0);
    while (!(CLK->CLKSTATUS & HXT_STATUS));
    // 12 MHz HXT

    CLK->PWRCON |= (1 << 1);
    while (!(CLK->CLKSTATUS & LXT_STATUS));
    // 32768

    CLK->CLKSEL0 &= (~(0b111 << 0));  // 000 is choosing 12 mhz
    CLK->PWRCON &= ~(1 << 7);         // Normal mode
    // Clock frequency divider
    CLK->CLKDIV &= (~(0xF << 0));  // no needS
}

// input 00
// output 01
void setupGPIO() {
    // GPIOC.15
    PC->PMD &= (~(0b11 << 30));  // led
    PC->PMD |= (0b01 << 30);
    // GPIOC.12
    PC->PMD &= (~(0b11 << 24));  // signal
    PC->PMD |= (0b01 << 24);

    // GPIOB.15:
    PB->PMD &= (~(0b11 << 24));  // button
    // PB->PMD |= (0b00 << 24);
}

void setupTimer() {
    // check manual clksel1
    CLK->CLKSEL1 &= ~(0b111 << 12);
    CLK->CLKSEL1 |= (0b001 << 12);
    CLK->APBCLK |= (1 << 3);

    // Pre-scale =11
    TIMER1->TCSR &= ~(0xFF << 0);
    // TIMER1->TCSR |= 11 << 0;

    // reset Timer 0
    TIMER1->TCSR |= (1 << 26);

    // define Timer 0 operation mode
    // 00 1shot
    // 01 periodic
    // 10 toggle
    // 11 continous counting
    TIMER1->TCSR &= ~(0b11 << 27);
    TIMER1->TCSR |= (0b01 << 27);
    TIMER1->TCSR &= ~(1 << 24);  // disable counter mode

    // TDR to be updated continuously while timer counter is counting
    TIMER1->TCSR |= (1 << 16);

    // Enable TE bit (bit 29) of TCSR
    // The bit will enable the timer interrupt flag TIF
    TIMER1->TCSR |= (1 << 29);
    // check manual page 95
    NVIC->ISER[0] |= (1 << 9);
    // check index of ip in interrupt number and check the bit
    NVIC->IP[2] &= ~(0b11 << 14);
    TIMER1->TCMPR = TIMER1_COUNTS;
    // 0.25 hz, 2 seconds on, 2 seconds off
    // 2 x 32768 = 65536
}

void TMR1_IRQHandler(void) {
    if (count < 10) {
        PC->DOUT ^= (1 << 12);  // signal for pc12 for measure
        PC->DOUT ^= (1 << 15);  // led signal
        TIMER1->TISR |= (1 << 0);
        // clear interrupt flag and enable the next interrupt
    }
    count++;
}