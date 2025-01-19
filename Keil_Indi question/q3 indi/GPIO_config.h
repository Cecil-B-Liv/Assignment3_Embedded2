#include <stdio.h>
#include "NUC100Series.h"


void GPIO_Config(void);

void GPIO_Config(void) {
    // Configure GPIO for 7-segment
    // Set mode for PC4 to PC7 - output push-pull
    GPIO_SetMode(PC, BIT4, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT5, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT6, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT7, GPIO_MODE_OUTPUT);
    // Set mode for PE0 to PE7 - output push-pull
    GPIO_SetMode(PE, BIT0, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PE, BIT1, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PE, BIT2, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PE, BIT3, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PE, BIT4, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PE, BIT5, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PE, BIT6, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PE, BIT7, GPIO_MODE_OUTPUT);

		//setup leds
		GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
		GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
		GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
		GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
}
