//------------------------------------------- main.c CODE STARTS
//---------------------------------------------------------------------------
#include <stdio.h>
#include "NUC100Series.h"
// #include "MCU_init.h"

#define SYSTICK_LVR 1105920 - 1

volatile int count = 0;

int main(void) {
    // System initialization start-------------------
    SYS_UnlockReg();
    // enable clock sources
    CLK->PWRCON |= (1 << 0);
    while (!(CLK->CLKSTATUS & (1 << 0)));

    CLK->PWRCON |= (1 << 2);
    while (!(CLK->CLKSTATUS & (1 << 4)));

    // Select CPU clock
    CLK->CLKSEL0 &= ~(0b111 << 0);
    CLK->CLKSEL0 |= (0b111 << 0);

    CLK->PWRCON &= ~(1 << 7);
    // CPU clock frequency divider
    CLK->CLKDIV &= ~(0xF);
    CLK->CLKDIV |= (0b11 << 0);
    // System initialization end---------------------

    // GPIO initialization start --------------------
    PC->PMD &= ~(0b11 << 24);
    PC->PMD |= 0b01 << 24;
		PB->PMD &= (~(0x03ul << 30));
		
    // GPIO initialization end ----------------------

    // System Tick initialization start--------------
    // STCLK as SysTick clock source
    SysTick->CTRL &= ~(1 << 2);
    CLK->CLKSEL0 &= ~(0b111 << 3);
    CLK->CLKSEL0 |= (0b011 << 3); // hclk

    // SysTick Reload Value
    SysTick->LOAD = SYSTICK_LVR - 1;
    SysTick->VAL = 0;
    // Start SysTick
    SysTick->CTRL |= (1 << 0);
    // System Tick initialization end----------------
    SYS_LockReg();  // Lock protected registers

    while (1) {
        if (!(PB->PIN & (1 << 15))) {
            while (count < 32) {
                while (!(SysTick->CTRL & (1 << 16)));
                PC->DOUT ^= 1 << 12;
                count++;
            }
        }
    }
}
//------------------------------------------- main.c CODE ENDS
//---------------------------------------------------------------------------
