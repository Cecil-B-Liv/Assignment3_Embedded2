#include <stdio.h>
#include "SYS_init.h"
#include "NUC100Series.h"


void turnOnU14(){
	PC->DOUT |= (1 << 4); // U14
	PC->DOUT &= ~(1 << 5); // U13
	PC->DOUT &= ~(1 << 6); // U12
	PC->DOUT &= ~(1 << 7); 	// U11
}

void turnOff(){
	PE->DOUT |= (1 << 0); // segment c
	PE->DOUT |= (1 << 1); // segment dot
	PE->DOUT |= (1 << 2); // segment f
	PE->DOUT |= (1 << 3); // segment a
	PE->DOUT |= (1 << 4); // segment b
	PE->DOUT |= (1 << 5); // segment d
	PE->DOUT |= (1 << 6); // segment e
	PE->DOUT |= (1 << 7); // segment g
}


void displayZero(){
	PE->DOUT &= ~(1 << 0); // segment c
	PE->DOUT |= (1 << 1); // segment dot
	PE->DOUT &= ~(1 << 2); // segment f
	PE->DOUT &= ~(1 << 3); // segment a
	PE->DOUT &= ~(1 << 4); // segment b
	PE->DOUT &= ~(1 << 5); // segment d
	PE->DOUT &= ~(1 << 6); // segment e
	PE->DOUT |= (1 << 7); // segment g
}

void displayOne(){
	PE->DOUT &= ~(1 << 0); // segment c
	PE->DOUT |= (1 << 1); // segment dot
	PE->DOUT |= (1 << 2); // segment f
	PE->DOUT |= (1 << 3); // segment a
	PE->DOUT &= ~(1 << 4); // segment b
	PE->DOUT |= (1 << 5); // segment d
	PE->DOUT |= (1 << 6); // segment e
	PE->DOUT |= (1 << 7); // segment g
}


void displayTwo(){
	PE->DOUT |= (1 << 0); // segment c
	PE->DOUT |= (1 << 1); // segment dot
	PE->DOUT |= (1 << 2); // segment f
	PE->DOUT &= ~(1 << 3); // segment a
	PE->DOUT &= ~(1 << 4); // segment b
	PE->DOUT &= ~(1 << 5); // segment d
	PE->DOUT &= ~(1 << 6); // segment e
	PE->DOUT &= ~(1 << 7); // segment g
}


void displayThree(){
	PE->DOUT &= ~(1 << 0); // segment c
	PE->DOUT |= (1 << 1); // segment dot
	PE->DOUT |= (1 << 2); // segment f
	PE->DOUT &= ~(1 << 3); // segment a
	PE->DOUT &= ~(1 << 4); // segment b
	PE->DOUT &= ~(1 << 5); // segment d
	PE->DOUT |= (1 << 6); // segment e
	PE->DOUT &= ~(1 << 7); // segment g
}


void displayFour(){
	PE->DOUT &= ~(1 << 0); // segment c
	PE->DOUT |= (1 << 1); // segment dot
	PE->DOUT &= ~(1 << 2); // segment f
	PE->DOUT |= (1 << 3); // segment a
	PE->DOUT &= ~(1 << 4); // segment b
	PE->DOUT |= (1 << 5); // segment d
	PE->DOUT |= (1 << 6); // segment e
	PE->DOUT &= ~(1 << 7); // segment g
}


void displayFive(){
	PE->DOUT &= ~(1 << 0); // segment c
	PE->DOUT |= (1 << 1); // segment dot
	PE->DOUT &= ~(1 << 2); // segment f
	PE->DOUT &= ~(1 << 3); // segment a
	PE->DOUT |= (1 << 4); // segment b
	PE->DOUT &= ~(1 << 5); // segment d
	PE->DOUT |= (1 << 6); // segment e
	PE->DOUT &= ~(1 << 7); // segment g
}


void displaySix(){
	PE->DOUT &= ~(1 << 0); // segment c
	PE->DOUT |= (1 << 1); // segment dot
	PE->DOUT &= ~(1 << 2); // segment f
	PE->DOUT &= ~(1 << 3); // segment a
	PE->DOUT |= (1 << 4); // segment b
	PE->DOUT &= ~(1 << 5); // segment d
	PE->DOUT &= ~(1 << 6); // segment e
	PE->DOUT &= ~(1 << 7); // segment g
}

void displaySeven(){
	PE->DOUT &= ~(1 << 0); // segment c
	PE->DOUT |= (1 << 1); // segment dot
	PE->DOUT |= (1 << 2); // segment f
	PE->DOUT &= ~(1 << 3); // segment a
	PE->DOUT &= ~(1 << 4); // segment b
	PE->DOUT |= (1 << 5); // segment d
	PE->DOUT |= (1 << 6); // segment e
	PE->DOUT |= (1 << 7); // segment g
}


void displayEight(){
	PE->DOUT &= ~(1 << 0); // segment c
	PE->DOUT |= (1 << 1); // segment dot
	PE->DOUT &= ~(1 << 2); // segment f
	PE->DOUT &= ~(1 << 3); // segment a
	PE->DOUT &= ~(1 << 4); // segment b
	PE->DOUT &= ~(1 << 5); // segment d
	PE->DOUT &= ~(1 << 6); // segment e
	PE->DOUT &= ~(1 << 7); // segment g
}

void displayNine(){
	PE->DOUT &= ~(1 << 0); // segment c
	PE->DOUT |= (1 << 1); // segment dot
	PE->DOUT &= ~(1 << 2); // segment f
	PE->DOUT &= ~(1 << 3); // segment a
	PE->DOUT &= ~(1 << 4); // segment b
	PE->DOUT &= ~(1 << 5); // segment d
	PE->DOUT |= (1 << 6); // segment e
	PE->DOUT &= ~(1 << 7); // segment g
}


void updateDigit(int count){
	switch(count){
		case 0:
			displayZero();
			break;
		case 1:
			displayOne();
			break;
		case 2:
			displayTwo();
			break;
		case 3:
			displayThree();
			break;
		case 4:
			displayFour();
			break;
		case 5:
			displayFive();
			break;
		case 6:
			displaySix();
			break;
		case 7:
			displaySeven();
			break;
		case 8:
			displayEight();
			break;
		case 9:
			displayNine();
			break;
		default:
			displayZero();
			break;
	}
}







