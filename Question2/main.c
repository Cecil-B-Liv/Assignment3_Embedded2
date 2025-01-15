#include <stdio.h>

#include "NUC100Series.h"
#include "SYS_init.h"
#include "clockState.h"
#include "displayDigit.h"
#include "enableClockAndTimer.h"
#include "setupGPIOandKeyMatrix.h"

#define isK1Pressed (!(PA->PIN & (1 << 2)))  // key1
#define isK2Pressed (!(PA->PIN & (1 << 1)))  // key5
#define isK3Pressed (!(PA->PIN & (1 << 0)))  // col9
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

volatile int historyIndex = 0;
volatile int displayIndex = 0;

// Array to store 5 lap records
int lapRecord[LAP_LIMIT][4] = {0};

// enum of modes for timewatch transistion
enum clock_mode { MODE_START, MODE_PAUSE, MODE_IDLE, MODE_DISPLAY };
enum clock_mode currentState = MODE_IDLE;

// Declare functions
void save(int, int, int, int);
void reset(void);
void reset(void);
void saveCurrentTime();
void reloadCurrentTime();
void displayRecord();

static void startPauseMode() {  // key 1 press
    PA->DOUT &= ~(1 << 3);
    PA->DOUT |= (1 << 4);
    PA->DOUT |= (1 << 5);

    if (isK1Pressed) {
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

static void displayMode() {  // key 5 press
    PA->DOUT |= (1 << 3);
    PA->DOUT &= ~(1 << 4);
    PA->DOUT |= (1 << 5);

    if (isK2Pressed) {
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

static void pauseResetMode() {  // col3
    PA->DOUT |= (1 << 3);
    PA->DOUT |= (1 << 4);
    PA->DOUT &= ~(1 << 5);

    if (isK3Pressed) {
        if (currentState == MODE_PAUSE) {
            reset();
            currentState = MODE_IDLE;
        } else if (currentState == MODE_START) {
            save(minutes, tensOfSecond, unitSecond, ticks);
        }
    }
}

static void rotateDisplay() {  // pb15
    if (currentState == MODE_DISPLAY) {
        if (displayIndex < 4) {
            displayIndex++;
        } else {
            displayIndex = 0;
        }
        displayRecord();
    }
}

void save(int minutes, int tensOfSecond, int unitSecond, int ticks) {
    lapRecord[historyIndex][3] = ticks;
    lapRecord[historyIndex][2] = unitSecond;
    lapRecord[historyIndex][1] = tensOfSecond;
    lapRecord[historyIndex][0] = minutes;
    historyIndex = (historyIndex + 1) % LAP_LIMIT;
}

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

void saveCurrentTime() {
    tempMinutes = minutes;
    tempTensOfSecond = tensOfSecond;
    tempUnitSecond = unitSecond;
    tempTicks = ticks;
}

void reloadCurrentTime() {
    minutes = tempMinutes;
    tensOfSecond = tempTensOfSecond;
    unitSecond = tempUnitSecond;
    ticks = tempTicks;
    displayIndex = 0;
}

void displayRecord() {
    minutes = displayIndex + 1;
    tensOfSecond = lapRecord[displayIndex][1];
    unitSecond = lapRecord[displayIndex][2];
    ticks = lapRecord[displayIndex][3];
}

void switchState() {
    switch (currentState) {
        case MODE_START:
            PC->DOUT |= (1 << 14);
            PC->DOUT &= ~(1 << 13);
            PC->DOUT |= (1 << 12);
            startCounter();
            return;
            break;

        case MODE_PAUSE:
            PC->DOUT &= ~(1 << 14);
            PC->DOUT |= (1 << 13);
            PC->DOUT |= (1 << 12);
            pauseCounter();
            return;
            break;

        case MODE_DISPLAY:
            PC->DOUT |= (1 << 14);
            PC->DOUT |= (1 << 13);
            PC->DOUT |= (1 << 12);
            pauseCounter();
            return;
            break;

        default:
            PC->DOUT |= (1 << 14);
            PC->DOUT |= (1 << 13);
            PC->DOUT &= ~(1 << 12);
            pauseCounter();
            return;
            break;
    }
}

int main(void) {
    SYS_UnlockReg();
    setupKeyMatrix();
    enableClockSource();
    enableTimer0();
    setupGPIO();
    SYS_LockReg();

    while (1) {
        PA->DOUT &= ~(1 << 3);
        PA->DOUT &= ~(1 << 4);
        PA->DOUT &= ~(1 << 5);

        switchState();

        if (currentState != MODE_IDLE) {
            showNumbers(minutes, tensOfSecond, unitSecond, ticks);
        }

        if (isK1Pressed) {
            startPauseMode();
            while (isK1Pressed);
        }
        if (isK3Pressed) {
            pauseResetMode();
            while (isK3Pressed);
        }
        if (isK2Pressed) {
            displayMode();
            while (isK2Pressed);
        }
    }
}

void TMR0_IRQHandler(void) {
    TIMER0->TISR |= (1 << 0);
    ++ticks;
    if (ticks == 10) {
        ++unitSecond;
        ticks = 0;
    }
    if (unitSecond == 10) {
        ++tensOfSecond;
        unitSecond = 0;
    }
    if (tensOfSecond == 6) {
        ++minutes;
        tensOfSecond = 0;
    }
    if (minutes == 10) {
        minutes = 0;
    }
    PC->DOUT ^= (1 << 8);
}

void EINT1_IRQHandler(void) {
    rotateDisplay();
    CLK_SysTickDelay(75);
    PB->ISRC |= (1 << 15);
}