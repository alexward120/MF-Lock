// Name(s): Alexander Ward, Diego Jerez
#include "keypad.h"


//ROW = {c3, c2, ph1, ph0} input
//COLUMN = {c0, c1, b0, a4} output

void keypad_GPIO_Init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOHEN;
	
	//resetting all pins to input mode
	GPIOA->MODER &= ~GPIO_MODER_MODE4;
	GPIOB->MODER &= ~(GPIO_MODER_MODE0);
	GPIOC->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1
					  | GPIO_MODER_MODE3 | GPIO_MODER_MODE2); 
	GPIOH->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1);
	
	//setting column pins to output mode
	GPIOA->MODER |= GPIO_MODER_MODE4_0;
	GPIOB->MODER |= GPIO_MODER_MODE0_0;
	GPIOC->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0);
	
	//resetting all pins to no pull up no pull down
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD4);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD0);
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1
					 | GPIO_PUPDR_PUPD3 | GPIO_PUPDR_PUPD2);
	GPIOH->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1);
	
	//setting row pins to pull up
	GPIOC->PUPDR |= (GPIO_PUPDR_PUPD3_0 | GPIO_PUPDR_PUPD2_0);
	GPIOH->PUPDR |= (GPIO_PUPDR_PUPD0_0 | GPIO_PUPDR_PUPD1_0);
	
	//setting column pins to push pull
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT4;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT0;
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1);
	
	//setting column pins to have low speed
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED4;
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED0;
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0 | GPIO_OSPEEDR_OSPEED1);
}

//sets all column pins high
void set_col_high(void) {
	GPIOC->ODR |= GPIO_ODR_OD0;
	GPIOC->ODR |= GPIO_ODR_OD1;
	GPIOB->ODR |= GPIO_ODR_OD0;
	GPIOA->ODR |= GPIO_ODR_OD4;
}

//returns key selected on pinpad
char keypad_select(void) {
	/* Make COL 1 LOW and all other COLs HIGH */
	set_col_high();
	GPIOC->ODR &= ~GPIO_ODR_OD0;
	
	if (!((GPIOC->IDR) & GPIO_IDR_ID3)) { //if ROW 1 is low
			while(!((GPIOC->IDR) & GPIO_IDR_ID3));
		return '1';
    }
	if (!((GPIOC->IDR) & GPIO_IDR_ID2)) { //if ROW 2 is low
			while(!((GPIOC->IDR) & GPIO_IDR_ID2));
		return '4';
    }
	if (!((GPIOH->IDR) & GPIO_IDR_ID1)) { //if ROW 3 is low
			while(!((GPIOH->IDR) & GPIO_IDR_ID1));
		return '7';
    }
	if (!((GPIOH->IDR) & GPIO_IDR_ID0)) { //if ROW 4 is low
			while(!((GPIOH->IDR) & GPIO_IDR_ID0));
		return '*';
    }
	
	/* Make COL 2 LOW and all other COLs HIGH */
	set_col_high();
	GPIOC->ODR &= ~GPIO_ODR_OD1;
	
	if (!((GPIOC->IDR) & GPIO_IDR_ID3)) { //if ROW 1 is low
			while(!((GPIOC->IDR) & GPIO_IDR_ID3));
		return '2';
    }
	if (!((GPIOC->IDR) & GPIO_IDR_ID2)) { //if ROW 2 is low
			while(!((GPIOC->IDR) & GPIO_IDR_ID2));
		return '5';
    }
	if (!((GPIOH->IDR) & GPIO_IDR_ID1)) { //if ROW 3 is low
			while(!((GPIOH->IDR) & GPIO_IDR_ID1));
		return '8';
    }
	if (!((GPIOH->IDR) & GPIO_IDR_ID0)) { //if ROW 4 is low
			while(!((GPIOH->IDR) & GPIO_IDR_ID0));
		return '0';
    }
	
	/* Make COL 3 LOW and all other COLs HIGH */
	set_col_high();
	GPIOB->ODR &= ~GPIO_ODR_OD0;
	
	if (!((GPIOC->IDR) & GPIO_IDR_ID3)) { //if ROW 1 is low
			while(!((GPIOC->IDR) & GPIO_IDR_ID3));
		return '3';
    }
	if (!((GPIOC->IDR) & GPIO_IDR_ID2)) { //if ROW 2 is low
			while(!((GPIOC->IDR) & GPIO_IDR_ID2));
		return '6';
    }
	if (!((GPIOH->IDR) & GPIO_IDR_ID1)) { //if ROW 3 is low
			while(!((GPIOH->IDR) & GPIO_IDR_ID1));
		return '9';
    }
	if (!((GPIOH->IDR) & GPIO_IDR_ID0)) { //if ROW 4 is low
			while(!((GPIOH->IDR) & GPIO_IDR_ID0));
		return '#';
    }
	
	/* Make COL 4 LOW and all other COLs HIGH */
	set_col_high();
	GPIOA->ODR &= ~GPIO_ODR_OD4;
	
	if (!((GPIOC->IDR) & GPIO_IDR_ID3)) { //if ROW 1 is low
			while(!((GPIOC->IDR) & GPIO_IDR_ID3));
		return 'A';
    }
	if (!((GPIOC->IDR) & GPIO_IDR_ID2)) { //if ROW 2 is low
			while(!((GPIOC->IDR) & GPIO_IDR_ID2));
		return 'B';
    }
	if (!((GPIOH->IDR) & GPIO_IDR_ID1)) { //if ROW 3 is low
			while(!((GPIOH->IDR) & GPIO_IDR_ID1));
		return 'C';
    }
	if (!((GPIOH->IDR) & GPIO_IDR_ID0)) { //if ROW 4 is low
			while(!((GPIOH->IDR) & GPIO_IDR_ID0));
		return 'D';
    }
	return '\0';
}
