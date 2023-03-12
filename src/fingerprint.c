#include "fingerprint.h"

//wake pa3 
//rst pa2

uint8_t finger_TxBuf[9];			

uint8_t     Finger_SleepFlag;

void GPIO_Init_Fingerprint(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
	GPIOA->MODER |= (GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0); //set pins to output mode
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_2 | GPIO_OTYPER_OT_3);
	GPIOA->PUPDR &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
}

void TxByte(uint8_t temp) {
	USART_Write(USART1, &temp, 1);
}


uint8_t TxAndRxCmd(uint8_t Scnt, uint8_t Rcnt, uint16_t Delay_ms) {
	uint8_t  i, j, CheckSum;
	uint32_t before_tick;        
	uint32_t after_tick;
	uint8_t   overflow_Flag = 0;
	TxByte(CMD_HEAD);
	CheckSum = 0;
	for (i=0; i < Scnt; i++) {
		TxByte(finger_TxBuf[i]);
		CheckSum ^= finger_TxBuf[i];
	}
	TxByte(CheckSum);
	TxByte(CMD_TAIL);
}