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
void EXTI_Init(void);
void EXTI15_10_IRQHandler(void);

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
static int interuppt_flag = 0;

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
	USART_Init(USART2); //set to USART 1 for bluetooth and USART2 for wired mode
	
	ILI9341_Init(SPI1, GPIOB, 2, GPIOB, 8, GPIOB, 11);
	ILI9341_setRotation(2);
	
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
				//display enter code
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("ENTER CODE", 45, 80, COLOR_RED, COLOR_BLACK, 4);
				
				//grab user input from keypad
				getPasscode(ans);
				
				//resets interuppt flag if interuppt is called and makes sure to skip rest of this state
				if (interuppt_flag) {
					interuppt_flag = 0;
					break;
				}
				//compare user input to actual passcode
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
				//move to next state if passcode is correct
				if (!flag) {
					state = lock_multifactor;
				}
				break;
			}
			
			case lock_multifactor : {
				//display USE MFA
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("USE MFA", 70, 80, COLOR_RED, COLOR_BLACK, 4);
				
				//prompt user on device to enter code
				printf("Enter 4 digit code\n");
				
				//read user input
				scanf("%s", mfa_ans); 
				
				//compare user input to mfa password
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
				//move to next state if MFA passes
				if (!flag) {
					state = unlocking;
				}
				break;
			}
			
			case unlocking : {
				//display the lock is unlocking
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("UNLOCKING", 55, 80, COLOR_GREEN, COLOR_BLACK, 4);
				
				//move the lock to unlock position
				Half_Stepping_Clockwise();
				Depower_Horizontal();
				Depower_Vertical();
				
				//change state to unlocked state
				state = unlock;
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("UNLOCKED", 65, 80, COLOR_GREEN, COLOR_BLACK, 4);
				break;
			}
			
			case unlock : {
				//stay in this state unless '*' or '#' is pressed
				char val;
				val = keypad_select();
				
				//lock door externally
				if (val == '*') {
					state = locking;
				}
				//attempt to change the passcode
				if (val == '#') {
					state = newpasswordattempt;
				}
				break;
			}
			
			case newpasswordattempt : {
				//display USE MFA
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("USE MFA", 70, 80, COLOR_GREEN, COLOR_BLACK, 4);
				
				//prompt user device to enter code
				printf("Enter 4 digit code\n");
				
				//grab user input
				scanf("%s", mfa_ans);
				
				//compare MFA to user input
				flag = 0;
				for (int i=0; i < 4; i++) {
					//if values don't match go back to unlocked state
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
				//move on to inputting new password if MFA passes
				if (!flag) {
					state = newpassword;
				}
				break;
			}
			
			case newpassword : {
				//prompt user to enter new password
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("ENTER NEW", 75, 40, COLOR_GREEN, COLOR_BLACK, 3);
				ILI9341_printText("PASSWORD", 83, 80, COLOR_GREEN, COLOR_BLACK, 3);
				
				//grab user input
				char newpass[4];
				getPasscode(newpass);
				
				//prompt user to confirm password
				ILI9341_Fill(COLOR_BLACK);
				ILI9341_printText("CONFIRM PASSWORD", 20, 80, COLOR_GREEN, COLOR_BLACK, 3);
				
				//grab user input
				char confirmpass[4];
				getPasscode(confirmpass);
				flag = 0;
				
				//compare both user inputs to make sure they match
				for (int i=0; i < 4; i++) {
					//if they don't match prompt user that passwords don't match and try again
					if (newpass[i] != confirmpass[i]) {
						ILI9341_Fill(COLOR_BLACK);
						ILI9341_printText("PASSWORDS", 83, 70, COLOR_RED, COLOR_BLACK, 3);
						ILI9341_printText("DON'T MATCH", 65, 110, COLOR_RED, COLOR_BLACK, 3);
						flag = 1;
						delay(1000);
						break;
					}
				}
				//if passwords match prompt user that the password is set and go back to unlocked state
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
//grabs user input for 4 presses from keypad and stores in input array
void getPasscode(char* arr) {
	char val= 'E';
	int i=0;
	while(i < 4) {
		//if button is pressed break out of function
		if (interuppt_flag) {
			return;
		}
		val = keypad_select();
		if ((val >= '0' && val <= '9') || (val >= 'A' && val <= 'D') || val == '*' || val == '#') {
			ILI9341_drawChar(85 + (40*i), 120, '*', COLOR_WHITE, COLOR_BLACK, 5);
			arr[i] = val;
			i++;
		}
	}
}

//interuppt for user button which acts like an internal lock
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


//interuppt handler for internal lock
void EXTI15_10_IRQHandler(void) {
	// Clear interrupt pending bit
	EXTI->PR1 |= EXTI_PR1_PIF13;
	
	//Set the state to locking so lock will lock
	if (state == unlock) {
		state = locking;
	}
	//Set the state to unlocking if door is currently locked in enter code state
	else if (state == lock_enter_code) {
		interuppt_flag = 1;
		state = unlocking;
	}
}
	

