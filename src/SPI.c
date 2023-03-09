// Name(s): Alexander Ward, Diego Jerez

#include "SPI.h"
#include "SysTimer.h"
#include "stm32l476xx.h"


extern uint8_t Rx1_Counter;
extern uint8_t Rx2_Counter;

//pins pb3, pb4, pb5 for lcd SPI1   //pins pb13, pb14, pb15 for touch SPI2
void SPI_GPIO_Init(void) {
	// Enable the GPIO Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	// Set Alternative Functions for LCD
	GPIOB->MODER &= ~(GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5);
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL3_2 | GPIO_AFRL_AFSEL3_0 |
	                 GPIO_AFRL_AFSEL4_2 | GPIO_AFRL_AFSEL4_0 |
	                 GPIO_AFRL_AFSEL5_2 | GPIO_AFRL_AFSEL5_0; 
	
	// Set Alternative Functions for Touch
	GPIOB->MODER &= ~(GPIO_MODER_MODE13 | GPIO_MODER_MODE14 | GPIO_MODER_MODE15);
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL13_2 | GPIO_AFRH_AFSEL13_0 |
	                 GPIO_AFRH_AFSEL14_2 | GPIO_AFRH_AFSEL14_0 |
	                 GPIO_AFRH_AFSEL15_2 | GPIO_AFRH_AFSEL15_0;
	
	// Set GPIO Pins to: Very High Output speed, Output Type Push-Pull, and No Pull-Up/Down
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED3 | GPIO_OSPEEDR_OSPEED4 | GPIO_OSPEEDR_OSPEED5 |
					GPIO_OSPEEDR_OSPEED13 | GPIO_OSPEEDR_OSPEED14 | GPIO_OSPEEDR_OSPEED15;
	
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT3 | GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 |
					   GPIO_OTYPER_OT13 | GPIO_OTYPER_OT14 | GPIO_OTYPER_OT15);
	
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD3 | GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 |
					  GPIO_PUPDR_PUPD13 | GPIO_PUPDR_PUPD14 | GPIO_PUPDR_PUPD15);
}

//SPI Init for ILI9341 on Nucleo board
void SPI_Init(void){
	// Enable SPI clock and Reset SPI
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;  
	RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_SPI2EN;     
	RCC->APB1RSTR1 |= RCC_APB1RSTR1_SPI2RST;  
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_SPI2RST;
	
	// Disable SPI
	SPI1->CR1 &= ~SPI_CR1_SPE;
	SPI2->CR1 &= ~SPI_CR1_SPE;
	
	// Configure for Full Duplex Communication
	SPI1->CR1 &= ~SPI_CR1_RXONLY;
	SPI2->CR1 |= SPI_CR1_RXONLY;
	
	// Configure for 2-line Unidirectional Data Mode
	SPI1->CR1 &= ~SPI_CR1_BIDIMODE;
	SPI2->CR1 &= ~SPI_CR1_BIDIMODE;
	
	// Set Frame Format
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;  // 0 -> MSB First
	SPI2->CR1 &= ~SPI_CR1_LSBFIRST;
	
	// Set Data Length to 8 bits ASK ABOUT THIS and if // Disable Output in Bidirectional Mode is needed
	SPI1->CR2 &= ~SPI_CR2_DS;     
	SPI1->CR2 |= SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;  // 0111 -> 8-bit
	SPI1->CR2 &= ~SPI_CR2_FRF;
	
	SPI2->CR2 &= ~SPI_CR2_DS;     
	SPI2->CR2 |= SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
	SPI2->CR2 &= ~SPI_CR2_FRF;
	
	// Configure Clock
	SPI1->CR1 &= ~SPI_CR1_CPOL; 
	SPI1->CR1 &= ~SPI_CR1_CPHA;
	
	SPI2->CR1 &= ~SPI_CR1_CPOL;
	SPI2->CR1 &= ~SPI_CR1_CPHA;
	
	// Set Baud Rate Prescaler. (Setting to 16)
	SPI1->CR1 &= ~SPI_CR1_BR;
	SPI1->CR1 |= SPI_CR1_BR_1 | SPI_CR1_BR_0;
	
	SPI2->CR1 &= ~SPI_CR1_BR;
	SPI2->CR1 |= SPI_CR1_BR_1 | SPI_CR1_BR_0;
	
	// Disable Hardware CRC Calculation
	SPI1->CR1 &= ~SPI_CR1_CRCEN;
	SPI2->CR1 &= ~SPI_CR1_CRCEN;
	
	// Set as Master and Enable Software Slave Management and NSS Pulse Management
	SPI1->CR1 |= SPI_CR1_MSTR;
	SPI1->CR1 |= SPI_CR1_SSM;
	SPI1->CR2 |= SPI_CR2_NSSP;
	
	SPI2->CR1 &= ~SPI_CR1_MSTR; 
	SPI2->CR1 |= SPI_CR1_SSM;
	SPI2->CR2 |= SPI_CR2_NSSP;
	
	// Manage NSS using Software
	SPI1->CR1 |= SPI_CR1_SSI;
	SPI2->CR1 &= ~SPI_CR1_SSI;
	
	// Set FIFO Reception Threshold
	SPI1->CR2 |= SPI_CR2_FRXTH;
	SPI2->CR2 |= SPI_CR2_FRXTH;
	
	// Enable SPI
	SPI1->CR1 |= SPI_CR1_SPE;
	SPI2->CR1 |= SPI_CR1_SPE;
}


void SPI_Write(SPI_TypeDef * SPIx, uint8_t *txBuffer, uint8_t * rxBuffer, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		while( (SPIx->SR & SPI_SR_TXE ) != SPI_SR_TXE );  // Wait for TXE (Transmit buffer empty)
		*((volatile uint8_t*)&SPIx->DR) = txBuffer[i];
		while((SPIx->SR & SPI_SR_RXNE ) != SPI_SR_RXNE); // Wait for RXNE (Receive buffer not empty)
		rxBuffer[i] = *((__IO uint8_t*)&SPIx->DR);
	}
	while( (SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY ); // Wait for BSY flag cleared
}

void SPI_Read(SPI_TypeDef * SPIx, uint8_t *rxBuffer, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		while( (SPIx->SR & SPI_SR_TXE ) != SPI_SR_TXE ); // Wait for TXE (Transmit buffer empty)
		*((volatile uint8_t*)&SPIx->DR) = rxBuffer[i];	
		// The clock is controlled by master. Thus the master has to send a byte
		// data to the slave to start the clock. 
		while((SPIx->SR & SPI_SR_RXNE ) != SPI_SR_RXNE); 
		rxBuffer[i] = *((__IO uint8_t*)&SPIx->DR);
	}
	while( (SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY ); // Wait for BSY flag cleared
}

//Incorporate delay function (same as delay() in previous labs but with us)
void SPI_Delay(uint32_t us) {
	uint32_t i, j;
	for (i = 0; i < us; i++) {
		for (j = 0; j < 18; j++) // This is an experimental value.
			(void)i;
	}
}
