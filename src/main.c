// Name(s): Alexander Ward, Diego Jerez

#include "stm32l476xx.h"
#include "motor.h"
#include "SPI.h"
#include "MY_ILI9341.h"
#include "UART.h"
#include "SysTimer.h"
#include "SysClock.h"
#include "keypad.h"

void getPasscode(char * arr);
//pins pb3, pb4, pb5 for lcd SPI1   //pins pb13, pb14, pb15 for touch SPI2

void EXTI_Init(void);
void EXTI15_10_IRQHandler(void);



/*Pinout: 
	
1 VCC LCD power supply is positive (3.3V~5V)
2 GND LCD Power ground

3 CS LCD selection control signal						pb2
4 RESET LCD reset control signal						pb11
5 DC/RS LCD register / data selection control signal	pb8

6 SDI(MOSI) LCD SPI bus write data signal:  			pb5
7 SCK LCD SPI bus clock signal: 						pb3
8 LED LCD backlight control signal: 					3.3v
9 SDO(MISO) LCD SPI bus read data signal :  			pb4
*/

static enum{
	locking,
	lock_enter_code,
	lock_multifactor,
	unlocking,
	unlock,
	newpasswordattempt,
	newpassword,
} state = lock_enter_code;

static char password[4] = {'1', '2', '3', '4'};
static char mfa_password[4] = {'4', '3', '2', '1'};
static char ans[4];
static char mfa_ans[4];

int main(void){
	System_Clock_Init(); //80 mhz clock
	SysTick_Init();
	Motor_GPIO_Init(); //pins pc5, pc6, pc8, pc9
	EXTI_Init();
	SPI1_GPIO_Init(); //pins pb3, pb4, pb5 for lcd SPI1   
	SPI1_Init();
	keypad_GPIO_Init();
	UART2_Init();
	UART2_GPIO_Init();
	USART_Init(USART2);
	
	ILI9341_Init(SPI1, GPIOB, 2, GPIOB, 8, GPIOB, 11);
	ILI9341_setRotation(2); //uses 4
	ILI9341_Fill(COLOR_BLACK);
	ILI9341_printText("LOCKING", 70, 80, COLOR_RED, COLOR_BLACK, 4);
	//set motor to lock position to start
	Half_Stepping_CounterClockwise();
	Depower_Horizontal();
	Depower_Vertical();
	int flag;
	while(1) {
		
		switch (state) {
			case locking : {
				//display locking
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("LOCKING", 70, 80, COLOR_RED, COLOR_BLACK, 4);
				//turn motor to lock position
				Half_Stepping_CounterClockwise();
				Depower_Horizontal();
				Depower_Vertical();
				//move to lock_enter_code case
				state = lock_enter_code;
				break;
			}
			
			case lock_enter_code : {
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("ENTER CODE", 45, 80, COLOR_RED, COLOR_BLACK, 4);
				
				getPasscode(ans);
				flag = 0;
				for (int i=0; i < 4; i++) {
					if (ans[i] != password[i]) {
						ILI9341_Fill(COLOR_BLACK);
						ILI9341_printText("WRONG CODE", 45, 80, COLOR_RED, COLOR_BLACK, 4);
						flag = 1;
						delay(1000);
						break;
					}
				}
				if (!flag) {
					state = lock_multifactor;
				}
				break;
			}
			
			case lock_multifactor : {
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("USE MFA", 70, 80, COLOR_RED, COLOR_BLACK, 4);
				printf("Enter 4 digit code\n");
				scanf("%s", mfa_ans); 
				//if succesfull move on to unlock state
				flag = 0;
				for (int i=0; i < 4; i++) {
					if (mfa_ans[i] != mfa_password[i]) {
						ILI9341_Fill(COLOR_BLACK);
						ILI9341_printText("WRONG CODE", 45, 80, COLOR_RED, COLOR_BLACK, 4);
						flag = 1;
						delay(1000);
						state = lock_enter_code;
						break;
					}
				}
				if (!flag) {
					state = unlocking;
				}
				break;
			}
			case unlocking : {
				//set motor to unlocked case
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("UNLOCKING", 55, 80, COLOR_GREEN, COLOR_BLACK, 4);
				Half_Stepping_Clockwise();
				Depower_Horizontal();
				Depower_Vertical();
				state = unlock;
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("UNLOCKED", 65, 80, COLOR_GREEN, COLOR_BLACK, 4);
				break;
			}
			
			case unlock : {
				//state in this state until user presses button to lock door
				char val;
				val = keypad_select();
				if (val == '*') {
					state = locking;
				}
				if (val == '#') {
					state = newpasswordattempt;
				}
				break;
			}
			
			case newpasswordattempt : {
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("USE MFA", 70, 80, COLOR_GREEN, COLOR_BLACK, 4);
				printf("Enter 4 digit code\n");
				scanf("%s", mfa_ans);
				flag = 0;
				for (int i=0; i < 4; i++) {
					if (mfa_ans[i] != mfa_password[i]) {
						ILI9341_Fill(COLOR_BLACK);
						ILI9341_printText("WRONG CODE", 45, 80, COLOR_RED, COLOR_BLACK, 4);
						flag = 1;
						delay(1000);
						ILI9341_printText("UNLOCKED", 65, 80, COLOR_GREEN, COLOR_BLACK, 4);
						state = unlock;
						break;
					}
				}
				if (!flag) {
					state = newpassword;
				}
				break;
			}
			
			case newpassword : {
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("ENTER NEW", 75, 40, COLOR_GREEN, COLOR_BLACK, 3);
				ILI9341_printText("PASSWORD", 83, 80, COLOR_GREEN, COLOR_BLACK, 3);
				char newpass[4];
				getPasscode(newpass);
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("CONFIRM PASSWORD", 20, 80, COLOR_GREEN, COLOR_BLACK, 3);
				char confirmpass[4];
				getPasscode(confirmpass);
				flag = 0;
				for (int i=0; i < 4; i++) {
					if (newpass[i] != confirmpass[i]) {
						ILI9341_Fill(COLOR_BLACK);
						ILI9341_printText("PASSWORDS", 83, 70, COLOR_RED, COLOR_BLACK, 3);
						ILI9341_printText("DON'T MATCH", 65, 110, COLOR_RED, COLOR_BLACK, 3);
						flag = 1;
						delay(1000);
						break;
					}
				}
				if (!flag) {
					delay(200);
					ILI9341_Fill(COLOR_BLACK);
					ILI9341_printText("PASSWORD SET", 50, 80, COLOR_GREEN, COLOR_BLACK, 3);
					for (int i=0; i < 4; i++) {
						password[i] = newpass[i];
					}
					delay(1000);
					ILI9341_Fill(COLOR_BLACK);
					ILI9341_printText("UNLOCKED", 65, 80, COLOR_GREEN, COLOR_BLACK, 4);
					state = unlock;
				}
			}
		}
	}
}

void getPasscode(char* arr) {
	char val= 'E';
	int i=0;
	while(i < 4) {
		val = keypad_select();
		if ((val >= '0' && val <= '9') || (val >= 'A' && val <= 'D') || val == '*' || val == '#') {
			ILI9341_drawChar(85 + (40*i), 120, '*', COLOR_WHITE, COLOR_BLACK, 5);
			arr[i] = val;
			i++;
		}
	}
}


void EXTI_Init(void) {
	// Initialize User Button
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	GPIOC->MODER &= ~GPIO_MODER_MODE13;
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13;
	
	// Configure SYSCFG EXTI
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;
	
	// Configure EXTI Trigger
	EXTI->FTSR1 |= EXTI_FTSR1_FT13;
	
	// Enable EXTI
	EXTI->IMR1 |= EXTI_IMR1_IM13;
	
	// Configure and Enable in NVIC
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_SetPriority(EXTI15_10_IRQn, 0);
}



void EXTI15_10_IRQHandler(void) {
	// Clear interrupt pending bit
	EXTI->PR1 |= EXTI_PR1_PIF13;
	// Define behavior that occurs when interrupt occurs
	state = locking;
}
	

