//------------------------------------------- main.c CODE STARTS -------------
#include <stdbool.h>
#include <stdio.h>

#include "GPIO_config.h"
#include "NUC100Series.h"
#include "display.h"

#define HXT_STATUS 1 << 0
#define TIMER3_COUNTS 3200000 - 1

void EINT1_IRQHandler(void);
volatile int buttonPresses = 0;
volatile bool U14State = false;
volatile bool U13State = false;
volatile bool U12State = false;
volatile bool U11State = false;

int main(void) {
    // System initialization start-------------------
    SYS_UnlockReg();  // Unlock protected regs
    CLK->PWRCON |= (1 << 0);
    while (!(CLK->CLKSTATUS & HXT_STATUS));

    // Select CPU clock
    // 12 MHz HXT
    CLK->CLKSEL0 &= (~(0b111 << 0));
    CLK->PWRCON &= ~(1 << 7);  // Normal mode

    // Clock frequency divider
    CLK->CLKDIV &= (~(0xF << 0));

    // System initialization end---------------------

    // Timer 3 initialization start--------------
    // TM1 Clock selection and configuration
    CLK->CLKSEL1 &= ~(0b111 << 20);  // for time0 use bit 8, timer2 bit 16,
                                     // timer3 bit 20(clear timer old selection)
    CLK->CLKSEL1 |=
        (0b010 << 20);  // for time0 use bit 8, timer2 bit 16, timer3 bit 20(set
                        // new timer clk source selection)
    CLK->APBCLK |= (1 << 5);  // clock enable for timer0 use bit 2, timer2 bit
                              // 4, timer3 bit 5

    // No prescaler for T1
    TIMER3->TCSR &= ~(0xFF << 0);
    TIMER3->TCSR |= (3 << 0);

    // reset Timer 3
    TIMER3->TCSR |= (1 << 26);

    // define Timer 3 operation mode
    TIMER3->TCSR &= ~(0b11 << 27);  // clear previous setting
    TIMER3->TCSR |= (0b01 << 27);  // 00: mode 1(oneshot), 01: mode 2(periodic),
                                   // 10: mode 3(toggle), 11: mode 4(auto)
    TIMER3->TCSR &= ~(1 << 24);    // enable counter

    // TDR to be updated continuously while timer counter is counting
    TIMER3->TCSR |= (1 << 16);

    // Enable TE bit (bit 29) of TCSR
    // The bit will enable the timer interrupt flag TIF
    TIMER3->TCSR |= (1 << 29);

    // TimeOut = 1s --> Counter's TCMPR = 4000000-1
    TIMER3->TCMPR = TIMER3_COUNTS;

    // start counting
    TIMER3->TCSR |= (1 << 30);
    // Timer 1 initialization end----------------
    // Setup modes for GPIOs button
    PB->PMD &= (~(0b11 << 30));  // Set PB15 to input mode
    // Setup button interrupt from GPB15
    PB->IMD &= (~(1 << 15));  // Set interrupt to edge-triggered
    PB->IEN |= (1 << 15);     // Enable falling edge interrupt for PB15
    PB->IEN &= (~(1 << 31));  // Disable rising edge interrupt for PB15
    // Interrupt enabling and priority setting

    NVIC->ISER[0] |= 1 << 3;
    NVIC->IP[0] &= (~(0b11 << 30));

    // Button Debounce
    PB->DBEN |= (1 << 15);
    GPIO->DBNCECON &= ~(1 << 4);
    GPIO->DBNCECON &= ~(0xF << 0);
    GPIO->DBNCECON |= (0xC << 0);

    SYS_LockReg();  // Lock protected registers

    while (1) {
        if ((TIMER3->TISR &
             (1 << 0)))  // Wait for the Overflow flag (TIF) to be set
        {
            if (buttonPresses == 0) {
                U14State = !U14State;  // Toggle the state of U14
                if (U14State) {
                    turnOnU14();
                    PE->DOUT = pattern[0];
                } else {
                    turnOff();
                }
            }
            if (buttonPresses == 1) {
                U13State = !U13State;  // Toggle the state of U13
                if (U13State) {
                    turnOnU13();
                    PE->DOUT = pattern[1];
                } else {
                    turnOff();
                }
            }
            if (buttonPresses == 2) {
                U13State = !U13State;  // Toggle the state of U13
                if (U13State) {
                    turnOnU12();
                    PE->DOUT = pattern[2];
                } else {
                    turnOff();
                }
            }
            if (buttonPresses == 3) {
                U13State = !U13State;  // Toggle the state of U13
                if (U13State) {
                    turnOnU11();
                    PE->DOUT = pattern[3];
                } else {
                    turnOff();
                }
            }
            if (buttonPresses == 4) {
                buttonPresses = 0;
            }
            TIMER3->TISR |= (1 << 0);  // Clear the flag by writing 1 to it
        }
    }
}

// Interrupt Service Rountine of GPIO port B pin 15
void EINT1_IRQHandler(void) {
    // Increment when button is pressed
    buttonPresses++;
    PB->ISRC |= (1 << 15);
}
//------------------------------------------- main.c CODE ENDS
