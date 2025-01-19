#include <stdio.h>

#include "NUC100Series.h"
#include "SYS_init.h"

#define HXT_STATUS (1 << 0)  // High-Speed External Crystal stable flag
#define LXT_STATUS (1 << 1)  // Low-Speed External Crystal stable flag
#define TIMER0_COUNT 100000 - 1  // Timer compare value for generating interrupts
// old code use 1000000 - 1

void enableClockSource() {
    // Enable HXT (High-Speed External Oscillator)
    CLK->PWRCON |= (1 << 0);  // (external 12 MHz crystal)
    // Wait for HXT to stabilize
    while (!(CLK->CLKSTATUS & HXT_STATUS))

    // Select LXT as the clock source
    CLK->CLKSEL0 &= ~(0x7 << 0);  // Clear clock source selection bits
    // Clear 000 to set HXT (12 kHz) as clock source

    // Configure power mode (optional)
    CLK->PWRCON &= ~(1 << 7);  // Disable power-down mode if previously enabled

    // Set Clock Divider to 1 (no division)
    CLK->CLKDIV &= ~(0xF << 0);  // Clear clock divider bits
    CLK->CLKDIV |= 11 << 0; // Here we divide 12mhz source with 11+1 = 100000
}

void enableTimer0() {
    // Select HXT as the clock source for Timer 0
    // HXT frequency is 12 MHz
    CLK->CLKSEL1 &= ~(
        0x7
        << 8);  // Clear bits [10:8] to select HXT (12 MHz) as the clock source
    // By default, 0x000 (HXT) is already selected, so the |= operation is not
    // strictly necessary.
		CLK->CLKSEL1 |= 0b010 << 8; //hclk
		//old code doesnt use this line 

    // Enable Timer 0 Clock
    CLK->APBCLK |= (1 << 2);  // Enable the Timer 0 clock by setting bit 2 in
                              // the APBCLK register

    // Configure Timer 0
    // Clear the prescaler (TCSR[7:0]), effectively setting it to 0
    TIMER0->TCSR &= ~(0xFF << 0);
    // Enable Timer 0 counter mode (TCSR[26] = 1)
    TIMER0->TCSR |= (1 << 26);

    // Set Timer 0 Mode
    // Select periodic mode (TCSR[27:26] = 0b01)
    TIMER0->TCSR &= ~(0x3 << 27);  // Clear the mode selection bits
    TIMER0->TCSR |= (0x1 << 27);   // Set Timer 0 to periodic mode

    // Disable Timer 0 counter reset function (TCSR[24] = 0)
    TIMER0->TCSR &= ~(1 << 24);

    // Enable Continuous Update of TDR
    TIMER0->TCSR |= (1 << 16);  // Enable continuous update of the TDR register
                                // (Timer Data Register)

    // Enable Timer 0 Interrupt
    TIMER0->TCSR |=
        (1 << 29);  // Enable interrupt on Timer 0 timeout (TCSR[29] = 1)
    NVIC->ISER[0] =
        (1 << 8);  // Enable Timer 0 interrupt in the NVIC (Interrupt No. 8)

    // Set Compare Value
    TIMER0->TCMPR = TIMER0_COUNT;  // Set the compare value for Timer 0
}