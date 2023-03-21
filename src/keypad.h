#ifndef __STM32L476G_DISCOVERY_KEYPAD_H
#define __STM32L476G_DISCOVERY_KEYPAD_H

#include "stm32l476xx.h"

void keypad_GPIO_Init(void);
void set_col_high(void);
char keypad_select(void);

#endif
