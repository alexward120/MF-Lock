// Name(s): Alexander Ward, Diego Jerez

#include "stm32l476xx.h"
#include "motor.h"
#include "SPI.h"
#include "MY_ILI9341.h"
#include "TSC2046.h"
#include "UART.h"
#include "SysTimer.h"
#include "SysClock.h"

void draw_keypad(void);
//pins pb3, pb4, pb5 for lcd SPI1   //pins pb13, pb14, pb15 for touch SPI2

/*Pinout: 
	
1 VCC LCD power supply is positive (3.3V~5V)
2 GND LCD Power ground

3 CS LCD selection control signal						pb2
4 RESET LCD reset control signal						pb11
5 DC/RS LCD register / data selection control signal	pb12

6 SDI(MOSI) LCD SPI bus write data signal:  			pb5
7 SCK LCD SPI bus clock signal: 						pb3
8 LED LCD backlight control signal: 					3.3v
9 SDO(MISO) LCD SPI bus read data signal :  			pb4

Touch:
10 T_CLK Touch screen SPI bus clock pin(SDK): 			pb13
11 T_CS Touch screen chip select control pin:     		pb8
12 T_DIN Touch screen SPI bus write data pin(MOSI): 	pb14
13 T_DO Touch screen SPI bus read data pin(MISO):		pb15
14 T_IRQ Touch screen interrupt detection:				pb9
*/

static enum{
	lock,
	lock_enter_code,
	lock_finger_sensor,
	unlock,
} state = lock;



int main(void){
	System_Clock_Init(); //80 mhz clock
	//Motor_GPIO_Init(); //pins pc5, pc6, pc8, pc9
	
	SPI1_GPIO_Init(); //pins pb3, pb4, pb5 for lcd SPI1   
	//SPI2_GPIO_Init(); //pins pb13, pb14, pb15 for touch SPI2
	SPI1_Init();
	SPI2_Init();
	
	ILI9341_Init(SPI1, GPIOB, 2, GPIOB, 12, GPIOB, 11);
	ILI9341_setRotation(3);
	//240 by 320 
	ILI9341_Fill(COLOR_BLACK);
	draw_keypad();
	
	//TSC2046_Begin(SPI2, GPIOB, 8);
	//TSC2046_Calibrate();
	//UART1_Init();
	//UART1_GPIO_Init(); //pins pa9, pa10 for usart1
	
	//set motor to lock position to start
	
	
	while(1) {
		switch (state) {
			case lock :
				//set motor to lock position
				//move to enter code state
				break;
			
			case lock_enter_code : 
				//show touchpad on screen
				//get input from user
				//if code correct go to fingerprint state, otherwise stay in this state
				break;
			
			case lock_finger_sensor :
				//prompt the user on screen to grab fingerprint
				//max 3 tries before state set to lock enter code
				//if succesfull move on to unlock state
				break;
			
			case unlock :
				//display on screen that the lock is unlocked
				//set motor to unlock position
				//state in this state until user presses button to lock door or screen is tapped
				break;
		}	
	}
}

void draw_keypad(void) {
	char k= '1';
	for (int16_t i=40; i <= 280; i+=80) {
		for (int16_t j=40; j <= 200; j+=80) {
			ILI9341_fillCircle(j, i, 35, COLOR_LGRAY);  
		}
	}
	
	for (int16_t i=40; i <= 280; i+=80) {
		for (int16_t j=40; j <= 200; j+=80, k++) {
			if (k == 58) {
				ILI9341_drawChar(j-13, i-17, 'X', COLOR_DGRAY, COLOR_LGRAY, 5);
			}
			else if (k == 59) {
				ILI9341_drawChar(j-13, i-17, '0', COLOR_DGRAY, COLOR_LGRAY, 5);
			}
			else if (k == 60) {
				ILI9341_drawChar(j-13, i-17, 'E', COLOR_DGRAY, COLOR_LGRAY, 5);
			}
			else {
				ILI9341_drawChar(j-13, i-17, k, COLOR_DGRAY, COLOR_LGRAY, 5);
			}
		}
	}
}
