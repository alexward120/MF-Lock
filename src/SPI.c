// Name(s): Alexander Ward, Diego Jerez

#include "SPI.h"
#include "SysTimer.h"
#include "stm32l476xx.h"


extern uint8_t Rx1_Counter;
extern uint8_t Rx2_Counter;

//pins pb3, pb4, pb5 for lcd SPI1   //pins pb13, pb14, pb15 for touch SPI2

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

void SPI1_GPIO_Init(void) {  //mosi=pb5, sck=pb3, miso=pb4
	// Enable the GPIO Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	
	// Set Alternative Functions for LCD
	GPIOB->MODER &= ~(GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5);
	GPIOB->MODER |= GPIO_MODER_MODE3_1 | GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1;
	
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL3 | GPIO_AFRL_AFSEL4 | GPIO_AFRL_AFSEL5);
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL3_2 | GPIO_AFRL_AFSEL3_0 |
	                 GPIO_AFRL_AFSEL4_2 | GPIO_AFRL_AFSEL4_0 |
	                 GPIO_AFRL_AFSEL5_2 | GPIO_AFRL_AFSEL5_0; 
	
	// Set GPIO Pins to: Very High Output speed, Output Type Push-Pull, and No Pull-Up/Down
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED3 | GPIO_OSPEEDR_OSPEED4 | GPIO_OSPEEDR_OSPEED5;
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT3 | GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD3 | GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5);
}

void SPI2_GPIO_Init(void) {  //used for touch screen
	// Enable the GPIO Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	// Set Alternative Functions for Touch
	GPIOB->MODER &= ~(GPIO_MODER_MODE13 | GPIO_MODER_MODE14 | GPIO_MODER_MODE15);
	GPIOB->MODER |= GPIO_MODER_MODE13_1 | GPIO_MODER_MODE14_1 | GPIO_MODER_MODE15_1;
	
	GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL13 | GPIO_AFRH_AFSEL14 | GPIO_AFRH_AFSEL15);
	GPIOB->AFR[1] |= 
					 GPIO_AFRH_AFSEL13_2 | GPIO_AFRH_AFSEL13_0 |
	                 GPIO_AFRH_AFSEL14_2 | GPIO_AFRH_AFSEL14_0 |
	                 GPIO_AFRH_AFSEL15_2 | GPIO_AFRH_AFSEL15_0;
	
	// Set GPIO Pins to: Very High Output speed, Output Type Push-Pull, and No Pull-Up/Down
	GPIOB->OSPEEDR |=  GPIO_OSPEEDR_OSPEED13 | GPIO_OSPEEDR_OSPEED14 | GPIO_OSPEEDR_OSPEED15;
	GPIOB->OTYPER &= ~( GPIO_OTYPER_OT13 | GPIO_OTYPER_OT14 | GPIO_OTYPER_OT15);
	GPIOB->PUPDR &= ~( GPIO_PUPDR_PUPD13 | GPIO_PUPDR_PUPD14 | GPIO_PUPDR_PUPD15);
}

//SPI1 Init for ILI9341 on Nucleo board
void SPI1_Init(void){
	// Enable SPI clock and Reset SPI
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;  
	RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
	
	// Disable SPI
	SPI1->CR1 &= ~SPI_CR1_SPE; 
	
	// Configure for Full Duplex Communication
	SPI1->CR1 &= ~SPI_CR1_RXONLY; 
	
	// Configure for 2-line Unidirectional Data Mode
	SPI1->CR1 &= ~SPI_CR1_BIDIMODE;
	
	// Disable Output in Bidirectional Mode
	SPI1->CR1 &= ~SPI_CR1_BIDIOE;
	
	// Set Frame Format
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;
	
	// Set Data Length to 8 bits 
	SPI1->CR2 &= ~SPI_CR2_DS;     
	SPI1->CR2 |= SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;  // 0111 -> 8-bit
	SPI1->CR2 &= ~SPI_CR2_FRF;
	
	// Configure Clock
	SPI1->CR1 &= ~SPI_CR1_CPOL; 
	SPI1->CR1 &= ~SPI_CR1_CPHA;
	
	// Set Baud Rate Prescaler. (Setting to 16)
	SPI1->CR1 &= ~SPI_CR1_BR;
	//SPI1->CR1 |= SPI_CR1_BR_2;
	
	// Disable Hardware CRC Calculation
	SPI1->CR1 &= ~SPI_CR1_CRCEN;
	
	// Set as Master and Enable Software Slave Management and NSS Pulse Management
	SPI1->CR1 |= SPI_CR1_MSTR;
	SPI1->CR1 |= SPI_CR1_SSM;
	SPI1->CR2 |= SPI_CR2_NSSP;
	
	// Manage NSS using Software
	SPI1->CR1 |= SPI_CR1_SSI;
	
	// Set FIFO Reception Threshold
	SPI1->CR2 |= SPI_CR2_FRXTH;
	
	// Enable SPI
	SPI1->CR1 |= SPI_CR1_SPE; 
}

//SPI2 Init for Touchscreen on Nucleo board
void SPI2_Init(void) {
	// Enable SPI clock and Reset SPI
	RCC->APB1ENR1 |= RCC_APB1ENR1_SPI2EN;     
	RCC->APB1RSTR1 |= RCC_APB1RSTR1_SPI2RST;  
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_SPI2RST;
	
	// Disable SPI
	SPI2->CR1 &= ~SPI_CR1_SPE; 
	
	// Configure for Full Duplex Communication
	SPI2->CR1 &= ~SPI_CR1_RXONLY;
	
	// Configure for 2-line Unidirectional Data Mode
	SPI2->CR1 &= ~SPI_CR1_BIDIMODE;
	
	// Disable Output in Bidirectional Mode
	SPI2->CR1 &= ~SPI_CR1_BIDIOE;
	
	// Set Frame Format
	SPI2->CR1 &= ~SPI_CR1_LSBFIRST;
	
	// Set Data Length to 8 bits 
	SPI2->CR2 &= ~SPI_CR2_DS;     
	SPI2->CR2 |= SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
	SPI2->CR2 &= ~SPI_CR2_FRF;
	
	// Configure Clock
	SPI2->CR1 &= ~SPI_CR1_CPOL;
	SPI2->CR1 &= ~SPI_CR1_CPHA;
	
	// Set Baud Rate Prescaler. (Setting to 32)
	SPI2->CR1 |= SPI_CR1_BR;
	//SPI2->CR1 |= SPI_CR1_BR_2;
	
	// Disable Hardware CRC Calculation
	SPI2->CR1 &= ~SPI_CR1_CRCEN;
	
	// Set as Master and Disable Software Slave Management and NSS Pulse Management
	SPI2->CR1 |= SPI_CR1_MSTR; 
	SPI2->CR1 |= SPI_CR1_SSM;
	SPI2->CR2 |= SPI_CR2_SSOE;
	SPI2->CR2 |= SPI_CR2_NSSP;
	
	// Manage NSS using Hardware
	SPI2->CR1 &= ~SPI_CR1_SSI;
	
	// Set FIFO Reception Threshold
	SPI2->CR2 |= SPI_CR2_FRXTH;
	
	// Enable SPI
	SPI2->CR1 |= SPI_CR1_SPE;
} 

void SPI_Write(SPI_TypeDef * SPIx, uint8_t *txBuffer, int size) {
	uint8_t rxBuffer = 0;
	int i = 0;
	for (i = 0; i < size; i++) {
		while(!(SPIx->SR & SPI_SR_TXE ));  // Wait for TXE (Transmit buffer empty)
		*((volatile uint8_t*)&SPIx->DR) = *txBuffer;
		while(SPIx->SR & SPI_SR_BSY); // Wait for BSY flag cleared
		txBuffer++;
		/*
		if ((SPIx->SR & SPI_SR_RXNE ) == SPI_SR_RXNE) {
			
		rxBuffer = *((volatile uint8_t*)&SPIx->DR);
		rxBuffer++;
		}*/
	}
}

void SPI_Read(SPI_TypeDef * SPIx, uint8_t *rxBuffer, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		while((SPIx->SR & SPI_SR_RXNE ) != SPI_SR_RXNE); 
		*rxBuffer = *((volatile uint8_t*)&SPIx->DR);
		rxBuffer++;
	}
}

//Incorporate delay function (same as delay() in previous labs but with us)
void SPI_Delay(uint32_t us) {
	uint32_t i, j;
	for (i = 0; i < us; i++) {
		for (j = 0; j < 18; j++) // This is an experimental value.
			(void)i;
	}
}
