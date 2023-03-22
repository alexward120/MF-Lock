#ifndef __STM32L476R_NUCLEO_MOTOR_H
#define __STM32L476R_NUCLEO_MOTOR_H

#include "stm32l476xx.h"

void Motor_GPIO_Init(void);
void Motor_delay(void);
void Power_North(void);
void Power_South(void);
void Power_East(void);
void Power_West(void);
void Depower_Horizontal(void);
void Depower_Vertical(void);
void Half_Stepping_Clockwise(void);
void Half_Stepping_CounterClockwise(void);

#endif
