#include <stdio.h>
#include "SYS_init.h"
#include "NUC100Series.h"

void setupKeyMatrix(){
		//Set up key matrix
		PA->PMD &= (~(0b11<< 6));
    PA->PMD |= (0b01 << 6);    
		PA->PMD &= (~(0b11<< 8));
    PA->PMD |= (0b01 << 8);  		
		PA->PMD &= (~(0b11<< 10));
    PA->PMD |= (0b01 << 10);  
}

void setupGPIO() {
    //GPIO INITIALIZATION--

    // =Setup LEDs (GPC5, GPC6, GPC7) as Output
    // GPC5, GPC6, GPC7 configured as push-pull output
    PC->PMD &= ~(0xFF << 10);        // Clear PMD bits for GPC5, GPC6, GPC7
    PC->PMD |= (0b010101 << 10);     // Set GPC5, GPC6, GPC7 to output mode (binary 01 per pin)

    // Setup GPC8 as Output
    PC->PMD &= ~(0b11 << 18);        // Clear PMD bits for GPC8
    PC->PMD |= (0b01 << 18);         // Set GPC8 to output mode (binary 01)

    // Setup 7-Segment Display (PE0 to PE7) as Output
    PE->PMD &= ~(0xFFFF << 0);       // Clear PMD bits for PE0 to PE7
    PE->PMD |= (0b0101010101010101 << 0);  // Set PE0 to PE7 to output mode (binary 01 per pin)

    // Setup PB15 as Input with Interrupt
    PB->PMD &= ~(0b11ul << 30);      // Clear PMD bits for PB15 (input mode)
    PB->IEN |= (1 << 15);            // Enable interrupt for PB15 (rising edge)

    // Enable NVIC Interrupt for PB15
    NVIC->ISER[0] |= (1UL << 3);     // Enable IRQ3 for PB15 in NVIC
    NVIC->IP[0] &= ~(0x3UL << 30);   // Set PB15 interrupt priority to the highest (default 0)

    // Enable Debounce for PB15
    PB->DBEN |= (1UL << 15);         // Enable debounce for PB15
    GPIO->DBNCECON |= (1UL << 4);    // Enable debounce function
    GPIO->DBNCECON |= (5UL << 0);    // Set debounce sampling cycle (default: 5 clocks)
}
