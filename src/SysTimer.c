#include "SysTimer.h"

volatile uint32_t msTicks;


//******************************************************************************************
// Initialize SysTick	
//******************************************************************************************	
void SysTick_Init(void){
		SysTick->CTRL = 0; // Disable SysTick IRQ and SysTick Counter
    
    // SysTick Reload Value Register
    SysTick->LOAD = 80000000/1000 - 1; // Enter the correct LOAD value that will give us a 1 ms period
    
    // SysTick Current Value Register
    SysTick->VAL = 0;
    
    // Configure and Enable SysTick interrupt in NVIC
		NVIC_SetPriority(SysTick_IRQn, 1); // Set Priority to 1
    NVIC_EnableIRQ(SysTick_IRQn);
    
    
    // Enables SysTick exception request
    // 1 = counting down to zero asserts the SysTick exception request
    // 0 = counting down to zero does not assert the SysTick exception request
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    
    // Select clock source
    // If CLKSOURCE = 0, the external clock is used. The frequency of SysTick clock is the frequency of the AHB clock divided by 8.
    // If CLKSOURCE = 1, the processor clock is used.
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;        
    
    // Enable SysTick IRQ and SysTick Timer
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  
}

//******************************************************************************************
// SysTick Interrupt Handler
//******************************************************************************************
void SysTick_Handler(void){
	msTicks++;
}
	
//******************************************************************************************
// Delay in ms
//******************************************************************************************
void delay (uint32_t T){
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < T);
	
  msTicks = 0;
}
