#include <stdio.h>

#include "NUC100Series.h"
#include "SYS_init.h"
#include "clockState.h"
#include "displayDigit.h"
#include "enableClockAndTimer.h"
#include "setupGPIOandKeyMatrix.h"

#define isK1Pressed (!(PA->PIN & (1 << 2)))  // key1
#define isK5Pressed (!(PA->PIN & (1 << 1)))  // key5
#define isK9Pressed (!(PA->PIN & (1 << 0)))  // key9
#define LAP_LIMIT 5                          // Assessment Requirement

// Unit counter
volatile int minutes = 0;       // U11
volatile int tensOfSecond = 0;  // U12
volatile int unitSecond = 0;    // U13
volatile int ticks = 0;         // U14

// Temporary unit counter for pause mode
volatile int tempMinutes = 0;
volatile int tempTensOfSecond = 0;
volatile int tempUnitSecond = 0;
volatile int tempTicks = 0;

// Index for debug/output
volatile int currentDisplayIndex = 0;
volatile int recordIndex = 0;

// Array to store 5 lap records
int lapRecord[LAP_LIMIT][4] = {0};

// enum of modes for timewatch transistion
enum state {MODE_IDLE,MODE_START,MODE_PAUSE,MODE_DISPLAY };
enum state currentState = MODE_IDLE;

// Declare functions
void displayRecord();
void save(int, int, int, int);
void reset(void);
void reset(void);
void saveCurrentTime();
void reloadCurrentTime();

static void startPauseMode() {  // Handle Start/Pause button (Key 1)
    PA->DOUT &= ~(1 << 3);
    PA->DOUT |= ((1 << 4)|(1 << 5));

    if (isK1Pressed) {
			// Handle Start/Pause button (Key 1)
        if (currentState != MODE_DISPLAY && currentState == MODE_START) {
            pauseCounter();
            currentState = MODE_PAUSE;
        } else if (currentState != MODE_DISPLAY &&
                   (currentState == MODE_PAUSE || currentState == MODE_IDLE)) {
            startCounter();
            currentState = MODE_START;
        }
    }
}

static void displayMode() {  // Handle Display button (Key 5)
    PA->DOUT |= ((1 << 3)|(1 << 5)); 
		PA->DOUT &= ~(1 << 4);

    if (isK5Pressed) {
			// Toggle between Display and Pause modes
        if (currentState != MODE_DISPLAY && currentState != MODE_START) {
            saveCurrentTime();
            currentState = MODE_DISPLAY;
            displayRecord();
        } else if (currentState == MODE_DISPLAY) {
            reloadCurrentTime();
            currentState = MODE_PAUSE;
        }
    }
}

static void pauseResetMode() {  // Handle Reset/Lap button (key 9) 
    PA->DOUT |= ((1 << 3)|(1 << 4));
    PA->DOUT &= ~(1 << 5);

    if (isK9Pressed) {
        if (currentState == MODE_PAUSE) {
            reset();
            currentState = MODE_IDLE;
        } else if (currentState == MODE_START) {
            save(minutes, tensOfSecond, unitSecond, ticks);
        }
    }
}

static void rotateDisplay() {  // Rotate through lap records (PB15 interrupt)
    if (currentState == MODE_DISPLAY) {
        if (currentDisplayIndex < 4) {
            currentDisplayIndex++;
        } else {
            currentDisplayIndex = 0;
        }
        displayRecord();
    }
}

// Save the current time
void saveCurrentTime() {
    tempMinutes = minutes;
    tempTensOfSecond = tensOfSecond;
    tempUnitSecond = unitSecond;
    tempTicks = ticks;
}

// Reload the saved time
void reloadCurrentTime() {
    minutes = tempMinutes;
    tensOfSecond = tempTensOfSecond;
    unitSecond = tempUnitSecond;
    ticks = tempTicks;
    currentDisplayIndex = 0;
}

// Save a lap record
void save(int minutes, int tensOfSecond, int unitSecond, int ticks) {
		recordIndex = (recordIndex + 1) % LAP_LIMIT;  
		lapRecord[recordIndex][0] = minutes; 	
	  lapRecord[recordIndex][1] = tensOfSecond;
		lapRecord[recordIndex][2] = unitSecond;
		lapRecord[recordIndex][3] = ticks;
}

// Reset the stopwatch
void reset() {
    minutes = 0;
    tensOfSecond = 0;
    unitSecond = 0;
    ticks = 0;

    for (int i = 0; i < LAP_LIMIT; i++) {
        for (int j = 0; j < 4; j++) {
            lapRecord[i][j] = 0;
        }
    }
}

// Display a lap record
void displayRecord() { 
    minutes = currentDisplayIndex + 1;
    tensOfSecond = lapRecord[currentDisplayIndex][1];
    unitSecond = lapRecord[currentDisplayIndex][2];
    ticks = lapRecord[currentDisplayIndex][3];
}

// Update LEDs and states based on mode
void switchState() {
    switch (currentState) {
        case MODE_START:
            PC->DOUT |= ((1 << 12)|(1 << 14));
            PC->DOUT &= ~(1 << 13);
            startCounter();
            return;
            break;

        case MODE_PAUSE:
            PC->DOUT &= ~(1 << 14);
            PC->DOUT |= ((1 << 12)|(1 << 13));
            pauseCounter();
            return;
            break;

        case MODE_DISPLAY:
            PC->DOUT |= ((1 << 14)|(1 << 13)|(1 << 12));
            pauseCounter();
            return;
            break;

        default:
            PC->DOUT |= ((1 << 14)|(1 << 13));
            PC->DOUT &= ~(1 << 12);
            pauseCounter();
            return;
            break;
    }
}

void EINT1_IRQHandler(void) {
    rotateDisplay();
    CLK_SysTickDelay(75);
    PB->ISRC |= (1 << 15);
}

void TMR0_IRQHandler(void) {
    TIMER0->TISR |= (1 << 0);
    ++ticks; // Increment ticks (1/10 second)
	  PC->DOUT ^= (1 << 9);
	
		if (minutes == 10) {
        minutes = 0;
    }
		if (tensOfSecond == 6) {
        ++minutes;
        tensOfSecond = 0;
    }
		if (unitSecond == 10) {
        ++tensOfSecond;
        unitSecond = 0;
    }
		if (ticks == 10) { 
        ++unitSecond;
        ticks = 0;
    }
}

int main(void) {
    SYS_UnlockReg();
    setupKeyMatrix(); 
    enableClockSource();
    enableTimer0();
    setupGPIO();
    SYS_LockReg();

    while (1) {  // Infinite loop for stopwatch operation
        PA->DOUT &= ~(1 << 3);
        PA->DOUT &= ~(1 << 4);
        PA->DOUT &= ~(1 << 5);

        switchState();
				
				// Handle button presses
        if (isK1Pressed) {
            startPauseMode();
            while (isK1Pressed);
        }
        if (isK9Pressed) {
            pauseResetMode();
            while (isK9Pressed);
        }
        if (isK5Pressed) {
            displayMode();
            while (isK5Pressed);
        }
				
				// Display time if not in Idle mode
        if (currentState != MODE_IDLE) {
            showNumbers(minutes, tensOfSecond, unitSecond, ticks);
        }
    }
}