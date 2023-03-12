// Name(s): Alexander Ward, Diego Jerez

#include "UART.h"

void UART1_Init(void) {
	RCC->APB2ENR = RCC_APB2ENR_USART1EN; //enable the uart2 clock
	RCC->CFGR &= ~RCC_CFGR_MCOSEL;
	RCC->CFGR |= RCC_CFGR_MCOSEL_0;  //setting the uart2 port clock to be system clock
}

//pins pa9, pa10 for usart1
void UART1_GPIO_Init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; //turning on clock for gpioA
	
	// Set both GPIO pins to alternative function mode (7)
	GPIOA->MODER &= ~(GPIO_MODER_MODE9 | GPIO_MODER_MODE10);
	GPIOA->MODER |= (GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1);
	
	GPIOA->AFR[1] |= GPIO_AFRH_AFSEL9; //setting pin9(channel 7 which has usart2_tx)
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL9_3;
	GPIOA->AFR[1] |= GPIO_AFRH_AFSEL10; //setting pin10(channel 7 which has usart2_rx)
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL10_3;
	
	//Setting for very high output speed
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED9 | GPIO_OSPEEDR_OSPEED10);
	
	//Set to push-pull output type
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT9 | GPIO_OTYPER_OT10);
	
	//Set to pull up
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD9 | GPIO_PUPDR_PUPD10);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPD9_0 | GPIO_PUPDR_PUPD10_0);
}

void USART_Init(USART_TypeDef* USARTx) {
	USARTx->CR1 &= ~USART_CR1_UE; //disabling usart to modify registers
	USARTx->CR1 &= ~USART_CR1_M; //setting word length to 8 bits
	USARTx->CR1 &= ~USART_CR1_OVER8; //enable oversampling by 16 bit
	USARTx->CR1 &= ~USART_CR2_STOP; //setting stop bit amount to 1
	
	USARTx->BRR = 80000000 / 19200; //setting baud rate to 9600 (CLK / baud rate)

	USARTx->CR1 |= USART_CR1_TE; //enable transmitter
	USARTx->CR1 |= USART_CR1_RE; //enable receiver

	USARTx->CR1 |= USART_CR1_UE; //enabling usart
}

void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes) {
	int i;
	// TXE is cleared by a write to the USART_DR register.
	// TXE is set by hardware when the content of the TDR 
	// register has been transferred into the shift register.
	for (i = 0; i < nBytes; i++) {
		while (!(USARTx->ISR & USART_ISR_TXE));   	// wait until TXE (TX empty) bit is set
		// Writing USART_DR automatically clears the TXE flag 	
		USARTx->TDR = buffer[i] & 0xFF;
		USART_Delay(300);
	}
	while (!(USARTx->ISR & USART_ISR_TC));   		  // wait until TC bit is set
	USARTx->ISR &= ~USART_ISR_TC;
}

uint8_t USART_Read(USART_TypeDef * USARTx) {
	// SR_RXNE (Read data register not empty) bit is set by hardware
	while (!(USARTx->ISR & USART_ISR_RXNE));  // Wait until RXNE (RX not empty) bit is set
	// USART resets the RXNE flag automatically after reading DR
	return ((uint8_t)(USARTx->RDR & 0xFF));
	// Reading USART_DR automatically clears the RXNE flag 
}

void USART_Delay(uint32_t us) {
	uint32_t time = 100*us/7;    
	while(--time);   
}

void USART_IRQHandler(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t * pRx_counter) {
	if(USARTx->ISR & USART_ISR_RXNE) {						// Received data                         
		buffer[*pRx_counter] = USARTx->RDR;         // Reading USART_DR automatically clears the RXNE flag 
		(*pRx_counter)++;  
		if((*pRx_counter) >= BufferSize )  {
			(*pRx_counter) = 0;
		}   
	} else if(USARTx->ISR & USART_ISR_TXE) {
 		//USARTx->ISR &= ~USART_ISR_TXE;            // clear interrupt 
		//Tx1_Counter++;
	} else if(USARTx->ISR & USART_ISR_ORE) {			// Overrun Error
		while(1);
	} else if(USARTx->ISR & USART_ISR_PE) {				// Parity Error
		while(1);
	} else if(USARTx->ISR & USART_ISR_PE) {				// USART_ISR_FE	
		while(1);
	} else if (USARTx->ISR & USART_ISR_NE){ 			// Noise Error Flag
		while(1);     
	}	
}
