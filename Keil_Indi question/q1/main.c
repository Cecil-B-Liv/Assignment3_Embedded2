#include <stdio.h>
#include "NUC100Series.h"
#include "SYS_init.h"

#define HXT_STATUS 1 << 0
#define LXT_STATUS 1 << 1
#define TIMER1_COUNT 65536-1

void enableClockSource(void);
void enableGPIO(void);
void enableTimer1(void);
void TMR1_IRQHandler(void);

int count_cycle = 0;

int main (void){
	SYS_UnlockReg();
	enableClockSource();
	enableGPIO();
	enableTimer1();
	SYS_LockReg();
	
	while(1){
		if(!( PB->PIN &(1<<15))){
			TIMER1->TCSR |= (1 << 30);
		}
	}
	
}

void enableClockSource(){
	CLK->PWRCON |= (1 << 0);
	while(!(CLK->CLKSTATUS & HXT_STATUS));
	
	CLK->PWRCON |= (1 << 1);
	while(!(CLK->CLKSTATUS & LXT_STATUS));
	
	CLK->CLKSEL0 &= ~(0x7 << 0);
	
	CLK->PWRCON &= ~(1 << 7);

	CLK->CLKDIV &= ~(0xF << 0);
}

void enableGPIO(){
	PC->PMD &= ~(0b11 << 14);
	PC->PMD |= (0b01 << 14);

	PB->PMD &= ~(0b11ul<<30);
}

void enableTimer1(){
	CLK->CLKSEL1 &= ~(0x7 << 12);
	CLK->CLKSEL1 |= (0x1 << 12);
	

	CLK->APBCLK |= (1 << 3);

	TIMER1->TCSR &= ~(0xFF << 0);
	TIMER1->TCSR |= (1 << 26);

	TIMER1->TCSR &= ~(0x3 << 27);
	TIMER1->TCSR |= (0x1 << 27);
	TIMER1->TCSR &= ~(1 << 24);
	
	TIMER1->TCSR |= (1 << 16);
	
	TIMER1->TCSR |= (1 << 29);
	NVIC->ISER[0] = (1<<9);
	NVIC->IP[2] &= ~(0b11 << 14);
	

	TIMER1->TCMPR = TIMER1_COUNT;
}

void TMR1_IRQHandler(void) {
	if(count_cycle < 10){
		PC->DOUT ^= (1<<15);
		TIMER1->TISR |= (1 << 0);
	}
	count_cycle++;
}