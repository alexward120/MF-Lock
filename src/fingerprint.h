#include "stm32l476xx.h"
#include "UART.h"
#include <string.h>


#define TRUE  1
#define FALSE 0

// Basic response message definition
#define ACK_SUCCESS       	0x00
#define ACK_FAIL         		0x01
#define ACK_FULL	  				0x04
#define ACK_NO_USER	  			0x05
#define ACK_TIMEOUT       	0x08
#define ACK_GO_OUT	  			0x0F		// The center of the fingerprint is out of alignment with sensor

//User information definition
#define ACK_ALL_USER       	0x00
#define ACK_GUEST_USER 	  	0x01
#define ACK_NORMAL_USER 	  0x02
#define ACK_MASTER_USER    	0x03

#define USER_MAX_CNT	   	  500	// Maximum fingerprint number

// Command definition
#define CMD_HEAD	   				0xF5
#define CMD_TAIL	 	 				0xF5
#define CMD_ADD_1    				0x01
#define CMD_ADD_2    				0x02
#define CMD_ADD_3	   				0x03
#define CMD_MATCH	   				0x0C
#define CMD_DEL		   				0x04
#define CMD_DEL_ALL	 				0x05
#define CMD_USER_CNT 				0x09
#define CMD_COM_LEV	 				0x28
#define CMD_LP_MODE  				0x2C
#define CMD_TIMEOUT  				0x2E

#define CMD_FINGER_DETECTED 0x14

//wake pa3 
//rst pa2
void GPIO_Init_Fingerprint(void);

void TxByte(uint8_t temp);
uint8_t TxAndRxCmd(uint8_t Scnt, uint8_t Rcnt, uint16_t Delay_ms);
uint8_t GetUserCount(void);
uint8_t GetcompareLevel(void);
uint8_t SetcompareLevel(uint8_t temp);	// The default value is 5, can be set to 0-9, the bigger, the stricter
uint8_t AddUser(void);
uint8_t ClearAllUser(void);
uint8_t VerifyUser(void);
uint8_t GetTimeOut(void);	
void Analysis_PC_Command(void);
void Auto_Verify_Finger(void);
