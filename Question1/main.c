//------------------------------------------- main.c CODE STARTS ---------------------------------------------------------------------------
#include <stdio.h>
#include "NUC100Series.h"

void System_Config(void);
void UART0_Config(void);
void UART0_SendChar(int ch);
char UART0_GetChar();


int main(void)
{		
		// SYSTEM CONFIGURATION
		// Unlock protected system registers for configuration
		SYS_UnlockReg(); // Unlock protected registers to allow modification

		// Enable clock sources
		CLK->PWRCON |= (0x01 << 0); // Enable external high-speed crystal (HXT)
		while (!(CLK->CLKSTATUS & (1 << 0))); // Wait for HXT to stabilize

		// PLL CONFIGURATION STARTS
		// Configure the PLL (Phase-Locked Loop) for system clock
		CLK->PLLCON &= ~(1 << 19); // Set PLL input source to HXT (0: HXT)
		CLK->PLLCON &= ~(1 << 16); // Normal PLL mode (0: Normal mode)

		// Clear existing PLL frequency settings and set desired frequency multiplier
		CLK->PLLCON &= (~(0x01FF << 0)); // Clear PLL feedback divider (FBDIV)
		CLK->PLLCON |= 48; // Set PLL feedback divider to 48 (PLL clock = 48 MHz)

		// Enable PLL output
		CLK->PLLCON &= ~(1 << 18); // 0: Enable PLL output (PLLOUT)
		while (!(CLK->CLKSTATUS & (0x01 << 2))); // Wait for PLL to stabilize
		// PLL CONFIGURATION ENDS

		// Select CPU clock source
		CLK->CLKSEL0 &= (~(0x07 << 0)); // Clear CPU clock source selection
		CLK->CLKSEL0 |= (0x02 << 0);    // Select PLL as the CPU clock source

		// Configure clock frequency division
		CLK->CLKDIV &= (~0x0F << 0);    // Set HCLK clock divider to 1 (HCLK = CPU clock)

		// UART0 CLOCK CONFIGURATION
		// Configure UART0 clock source and enable it
		CLK->CLKSEL1 &= (~(0x07 << 24)); // Clear UART0 clock source selection
		CLK->CLKSEL1 |= (0x03 << 24);    // Select UART0 clock source as 22.1184 MHz
		CLK->CLKDIV &= ~(0x0F << 8);     // Set UART0 clock divider to 1 (UART0 clock = 22.1184 MHz)
		CLK->APBCLK |= (0x01 << 16);     // Enable UART0 clock

		// Lock protected registers after configuration
		SYS_LockReg(); // Lock protected registers to prevent accidental changes


		// UART0 CONFIGURATION
		// Configure Port B Pin 1 (PB.1) as UART0 TX (output) and Pin 0 (PB.0) as UART0 RX
		PB->PMD &= ~(0b11 << 2);       // Clear mode bits for PB.1 (reset to input)
		PB->PMD |= (0b01 << 2);        // Set PB.1 as output
		SYS->GPB_MFP |= (1 << 1) | (1 << 0);  // Enable UART0 function on PB.0 (RX) and PB.1 (TX)

		// UART0 Operation Configuration
		UART0->LCR = 0b11;             // Configure 8 data bits
		UART0->LCR &= ~(1 << 2);       // Set ONE stop bit
		UART0->LCR &= ~(1 << 3);       // Disable parity check (NO parity bit)
		UART0->FCR |= (1 << 1) | (1 << 2);  // Clear both RX and TX FIFOs
		UART0->FCR &= ~(0xF << 16);    // Set FIFO Trigger Level to 1 byte (default)

		// Baud Rate Configuration for 115200 bps
		UART0->BAUD &= ~(0b11 << 28);  // Clear mode selection (use divider mode)
		UART0->BAUD &= ~(0xFFFF << 0);
		UART0->BAUD |= 10;  // Set divider to 10 (assuming clock is 22.1184 MHz)

		// Enable UART0 Interrupts
		UART0->IER |= (1 << 0);        // Enable Receive Data Available interrupt

		// Configure NVIC for UART0 Interrupt
		NVIC->ISER[0] = (1 << 12);     // Enable UART0 interrupt in NVIC (Interrupt No. 12)
	
		GPIO_SetMode(PC,BIT12,GPIO_MODE_OUTPUT);

		while(1){
		}
}

// Send a character via UART0
void UART0_SendChar(int ch) {
    // Wait until the TX FIFO is not full
    while (UART0->FSR & (0x01 << 23)); 
    // Write the character to the DATA register
    UART0->DATA = ch;
    // If the character is a newline '\n', also send a carriage return '\r'
    if (ch == '\n') {
        while (UART0->FSR & (0x01 << 23)); // Wait for TX FIFO to be ready
        UART0->DATA = '\r';               // Send the carriage return
    }
}

// Receive a character from UART0
char UART0_GetChar() {
    while (1) {
        // Check if RX FIFO is not empty
        if (!(UART0->FSR & (0x01 << 14))) {
            // Read and return the received character from the DATA register
            return (UART0->DATA);
        }
    }
}

// UART0 Interrupt Handler
void UART02_IRQHandler(void) {
    // Check if the Receive Data Available interrupt is triggered
    if (UART0->ISR & (1 << 0)) {
        // Read the received character
        char data = UART0_GetChar();
        
        // Echo the received character back via UART0
        UART0_SendChar(data);
    }
}

//------------------------------------------- main.c CODE ENDS --------------------------------------------------------------------------