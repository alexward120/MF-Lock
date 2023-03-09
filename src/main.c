// Name(s): Alexander Ward, Diego Jerez

#include "stm32l476xx.h"
#include "motor.h"
#include "SPI.h"
#include "MY_ILI9341.h"
#include "UART.h"
#include "SysTimer.h"
#include "SysClock.h"


int main(void){
	System_Clock_Init(); //80 mhz clock
	Motor_GPIO_Init(); //pins pc5, pc6, pc8, pc9
	
	SPI_GPIO_Init(); //pins pb3, pb4, pb5 for lcd SPI1   //pins pb13, pb14, pb15 for touch SPI2
	SPI_Init();
	
	UART1_Init();
	UART1_GPIO_Init(); //pins pa9, pa10 for usart1
	
	
	while(1) {
		//TODO 
	}
}
