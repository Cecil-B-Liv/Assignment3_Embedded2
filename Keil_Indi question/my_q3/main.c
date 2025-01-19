#include <stdio.h>
#include <stdbool.h>
#include "NUC100Series.h"
#include "SYS_init.h"
#include "displayNumber.h"

#define LXT_STATUS 1 << 1
#define HXT_STATUS 1 << 0
#define TIMER3_COUNTS 4000000 - 1

void setupClock(void);
void setupGPIO(void);
void setupTimer(void);

volatile bool U14State = false;
volatile bool U13State = false;
volatile bool U12State = false;
volatile bool U11State = false;

volatile int count = 0;
void EINT1_IRQHandler(void);

int main(void) {
    SYS_UnlockReg();  // Unlock protected regs
    setupClock();
    setupGPIO();
    setupTimer();
    SYS_LockReg();  // Lock protected registers

    while (1) {
    }
}

void setupClock() {
    CLK->PWRCON |= (1 << 0);
    while (!(CLK->CLKSTATUS & HXT_STATUS));
    // 12 MHz HXT

    CLK->CLKSEL0 &= (~(0b111 << 0));  // 000 is choosing 12 mhz
    CLK->PWRCON &= ~(1 << 7);         // Normal mode

    // Clock frequency divider
    CLK->CLKDIV &= (~(0xF << 0));
    CLK->CLKDIV |= (0b10 << 0);  // divide 12/(hclk(2) + 1) =>4 mhz
}

// input 00
// output 01
void setupGPIO() {
    // GPIOC.12
    PC->PMD &= (~(0b11 << 24));  // signal
    PC->PMD |= (0b01 << 24);
    // 7segment
    PE->PMD &= ~(0xFFFF << 0);
    PE->PMD |= 0b0101010101010101 << 0;
    // GPIOB.15:
    // PB->PMD &= ~(0b11 << 30);  // button
    //PB->IEN |= (1 << 15);
		PB->IMD &= (~(1 << 15));  // Set interrupt to edge-triggered
    PB->IEN |= (1 << 15);     // Enable falling edge interrupt for PB15
    PB->IEN &= (~(1 << 31));  // Disable rising edge interrupt for PB15
    // button interrupt number 3
    NVIC->ISER[0] |= (1 << 3);
    NVIC->IP[0] &= ~(0b11 << 30);
	
	  // Button Debounce
    PB->DBEN |= (1 << 15);
    GPIO->DBNCECON &= ~(1 << 4);
    GPIO->DBNCECON &= ~(0xF << 0);
    GPIO->DBNCECON |= (0xC << 0);
}

void setupTimer() {
    // check manual clksel1
    CLK->CLKSEL1 &= ~(0b111 << 20);
    CLK->CLKSEL1 |= (0b010 << 20);
    CLK->APBCLK |= (1 << 5);

    TIMER3->TCSR &= ~(0xFF << 0);
    // TIMER3->TCSR |=  << 0;

    // reset Timer 0
    TIMER3->TCSR |= (1 << 26);

    // define Timer 0 operation mode
    // 00 1shot
    // 01 periodic
    // 10 toggle
    // 11 continous counting
    TIMER3->TCSR &= ~(0b11 << 27);
    TIMER3->TCSR |= (0b01 << 27);
    TIMER3->TCSR &= ~(1 << 24);  // disable counter mode

    // Data load enable (Timer data register update)
    TIMER3->TCSR |= (1 << 16);

    // Enable TE bit (bit 29) of TCSR
    // The bit will enable the timer interrupt flag TIF
    TIMER3->TCSR |= (1 << 29);
    // check manual page 95
    NVIC->ISER[0] |= (1 << 11);
    // check index of ip in interrupt number and check the bit
    NVIC->IP[2] &= ~(0b11 << 30);

    TIMER3->TCMPR = TIMER3_COUNTS;
    TIMER3->TCSR |= (1 << 30);  // start count
    // 0.5 hz, 1 seconds on, 1 seconds off
    // 1 x 4000000 = 4000000
}

void TMR3_IRQHandler(void) {
    if (count == 0) {
        if (U14State) {
            U14State = !U14State;
            turnOff();
        } else {
            U14State = !U14State;
            turnOnU14();
            updateDigit(0);
        }
        //PC->DOUT ^= (1 << 12);  // signal for pc12 for measure
    }

    if (count == 1) {
        if (U13State) {
            U13State = !U13State;
            turnOff();
        } else {
            U13State = !U13State;
            turnOnU13();
            updateDigit(1);
        }
        //PC->DOUT ^= (1 << 12);  // signal for pc12 for measure
    }

    if (count == 2) {
        if (U12State) {
            U12State = !U12State;
            turnOff();
        } else {
            U12State = !U12State;
            turnOnU12();
            updateDigit(2);
        }
        //PC->DOUT ^= (1 << 12);  // signal for pc12 for measure
    }

    if (count == 3) {
        if (U11State) {
            U11State = !U11State;
            turnOff();
        } else {
            U11State = !U11State;
            turnOnU11();
            updateDigit(3);
        }
        //PC->DOUT ^= (1 << 12);  // signal for pc12 for measure
    }

    if (count == 4) {
        count = 0;
    }
    TIMER3->TISR |= (1 << 0);  // Timer interrupt flag
}

void EINT1_IRQHandler(void) {
    count++;
    PB->ISRC |= (1 << 15);  // port interrupt flag
}