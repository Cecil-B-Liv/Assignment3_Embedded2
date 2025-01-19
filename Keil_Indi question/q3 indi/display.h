#include <stdio.h>
#include "NUC100Series.h"

// Global Array to display on 7segment for NUC140 MCU
int pattern[] = {
    0b10000010,  // Number 0          // ---a----
    0b11101110,  // Number 1          // |      |
    0b00000111,  // Number 2          // f      b
    0b01000110,  // Number 3          // |      |
    0b01101010,  // Number 4          // ---g----
    0b01010010,  // Number 5          // |      |
    0b00010010,  // Number 6          // e      c
    0b11100110,  // Number 7          // |      |
    0b00000010,  // Number 8          // ---d-----dot(h)
    0b01000010,   // Number 9
    0b11111101,   // dot 
		0b11111111    // turnoff
}; 
void turnOff(){
	PC->DOUT &= ~(1 << 4); // U14
	PC->DOUT &= ~(1 << 5); // U13
	PC->DOUT &= ~(1 << 6); // U12
	PC->DOUT &= ~(1 << 7); 	// U11
}

void turnOnU14(){
	PC->DOUT |= (1 << 4); // U14
	PC->DOUT &= ~(1 << 5); // U13
	PC->DOUT &= ~(1 << 6); // U12
	PC->DOUT &= ~(1 << 7); 	// U11
}

void turnOnU13(){
	PC->DOUT &= ~(1 << 4); // U14
	PC->DOUT |= (1 << 5); // U13
	PC->DOUT &= ~(1 << 6); // U12
	PC->DOUT &= ~(1 << 7); 	// U11
}

void turnOnU12(){
	PC->DOUT &= ~(1 << 4); // U14
	PC->DOUT &= ~(1 << 5); // U13
	PC->DOUT |= (1 << 6); // U12
	PC->DOUT &= ~(1 << 7); 	// U11
}

void turnOnU11(){
	PC->DOUT &= ~(1 << 4); // U14
	PC->DOUT &= ~(1 << 5); // U13
	PC->DOUT &= ~(1 << 6); // U12
	PC->DOUT |= (1 << 7); 	// U11
}
void updateDigit(int count){
	switch(count){
		case 0:
			PE->DOUT = pattern[0];
			break;
		case 1:
			PE->DOUT = pattern[1];
			break;
		case 2:
			PE->DOUT = pattern[2];
			break;
		case 3:
			PE->DOUT = pattern[3];
			break;
		case 4:
			PE->DOUT = pattern[4];
			break;
		case 5:
			PE->DOUT = pattern[5];
			break;
		case 6:
			PE->DOUT = pattern[6];
			break;
		case 7:
			PE->DOUT = pattern[7];
			break;
		case 8:
			PE->DOUT = pattern[8];
			break;
		case 9:
			PE->DOUT = pattern[9];
			break;
		default:
			PE->DOUT = pattern[0];
			break;
	}
}
void showNumbers(int countU11, int countU12, int countU13, int countU14){
	turnOnU14();
	updateDigit(countU14);
	PE->DOUT = pattern[11];
	
	turnOnU13();
	updateDigit(countU13);
	PE->DOUT = pattern[10];
	PE->DOUT = pattern[11];
	
	turnOnU12();
	updateDigit(countU12);
	PE->DOUT = pattern[11];

	turnOnU11();
	updateDigit(countU11);
	PE->DOUT = pattern[10];
	PE->DOUT = pattern[11];	
}

showNumberU14(int countU14){
	turnOnU14();
	updateDigit(countU14);
	PE->DOUT = pattern[11];
}

showNumberU13(int countU13){
	turnOnU13();
	updateDigit(countU13);
	PE->DOUT = pattern[11];
}

showNumberU12(int countU12){
	turnOnU12();
	updateDigit(countU12);
	PE->DOUT = pattern[11];
}

showNumberU11(int countU11){
	turnOnU11();
	updateDigit(countU11);
	PE->DOUT = pattern[11];
}

void turnOn_led5 (void) {
			PC->DOUT &= ~(1 << 12); //turn on led 5
			PC->DOUT |= (1 << 13);
			PC->DOUT |= (1 << 14);
			PC->DOUT |= (1 << 15);
}

void turnOn_led6 (void) {
			PC->DOUT &= ~(1 << 13); //turn on led 6
			PC->DOUT |= (1 << 12);
			PC->DOUT |= (1 << 14);
			PC->DOUT |= (1 << 15);
}

void turnOn_led7 (void) {
			PC->DOUT &= ~(1 << 14); //turn on led 7
			PC->DOUT |= (1 << 12);
			PC->DOUT |= (1 << 13);
			PC->DOUT |= (1 << 15);
}

void turnOn_led8 (void) {
			PC->DOUT &= ~(1 << 15); //turn on led 8
			PC->DOUT |= (1 << 13);
			PC->DOUT |= (1 << 14);
			PC->DOUT |= (1 << 12);
}
