#include <stdio.h>
#include "NUC100Series.h"
#include "SYS_init.h"
#include "displayNumber.h"

#define LXT_STATUS 1 << 1
#define HXT_STATUS 1 << 0
#define TIMER3_COUNTS 4000000 - 1

void setupClock(void);
void setupGPIO(void);
void setupTimer(void);

enum state { ON, OFF };
enum state state = ON;

volatile count = 0;

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
    CLK->CLKDIV |= (0x10 << 0);  // divide 12/(hclk(2) + 1) =>4 mhz
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
    PB->PMD &= (~(0b11 << 30));  // button
    PB->IEN |= (1 << 15);
    // PB->PMD |= (0b00 << 24);
    // button interrupt number 3
    NVIC->ISER[0] |= (1 << 3);
    NVIC->IP[0] &= ~(0b11 << 30);
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
    // priority level 0b00, so we clear all

    TIMER3->TCMPR = TIMER3_COUNTS;
    TIMER3->TCSR |= (1<<30); // start count
    // 0.5 hz, 1 seconds on, 1 seconds off
    // 1 x 4000000 = 4000000
}

void TMR3_IRQHandler(void) {
    if (state == ON) {
        state = OFF;
        turnOff();
			
    } else {
        state = ON;
        turnOnU14();
        updateDigit(count);
    }
		PC->DOUT ^= (1<<12);
    TIMER3->TISR |= (1 << 0);  // Timer interrupt flag
}

void EINT1_IRQHandler(void) {
    if (count == 10) {
        count = 0;
    }
    count++;
    CLK_SysTickDelay(50000);
    PB->ISRC |= (1 << 15);  // port interrupt flag
}