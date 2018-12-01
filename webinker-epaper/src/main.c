#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "gde043a2.h"
#include "sram.h"
#include "usart.h"
#include "imgdec.h"
#include "config.h"
#include "delay.h"

// Add images
#include "images/ap_setup.h"
//#include "images/lost_connection.h"
//#include "images/low_battery.h"
//#include "images/sleep.h"
//#include "images/dhcp_error.h"
//#include "images/dns_error.h"
//#include "images/connection_error.h"
#include "images/invalid_image.h"

#include <stm32f10x.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_pwr.h>
#include <system_stm32f10x.h>


typedef enum CMD_IMAGE_EXT_RLE
{
	CMD_IMAGE_UNCOMPRESSED = 0,
	CMD_IMAGE_COMPRESSED = 1

} CMD_IMAGE_EXT_RLE;

// Bit   7 Defines if the data is run length encoded
// Bit   6 Tells whether to show a predefined picture (0) or to load a picture (1)
//         If the bit is 1, it will be followed by 120000 bytes with the picture content
// Bit 5,0 defines which pre-loaded picture to show (from the 4 in-ROM available)
typedef union
{
	uint8_t command;
	struct
	{
		unsigned IMAGE_PRELOAD_NUM : 6;
		unsigned IMAGE_EXT     : 1;
		CMD_IMAGE_EXT_RLE IMAGE_EXT_RLE : 1;
	} u;
} epaper_cmd_t;

// Variables
const void * image_table[2] =
{
	ap_setup,
	//lost_connection,
	//low_battery,
	//sleep_mode,
	//dhcp_error,
	//dns_error,
	//connection_error,
	invalid_image
};

unsigned char scratch[(800*600*2)/8] __attribute__((section(".extdata"), used));
epaper_cmd_t cmd;

// Prototypes
extern void initialise_monitor_handles(void);

void initHW() {
	// Init basic stuff
	SystemInit();

	/* Debug support for low power modes: */
	DBGMCU_Config(DBGMCU_SLEEP, ENABLE);
	DBGMCU_Config(DBGMCU_STOP, ENABLE);
	DBGMCU_Config(DBGMCU_STANDBY, ENABLE);

	// Enable clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	// Disable the fucking JTAG!
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);

	// Initialize SRAM
	FSMC_SRAM_Init();
}

void syncBlink(uint16_t time, uint8_t blinks) {
	// Some blink
	for(int i = 0; i < blinks; i++)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_3);
		_delay_ms(time / 2);
		GPIO_ResetBits(GPIOB, GPIO_Pin_3);
		_delay_ms(time / 2);
	}
}

void GPIO_Initialize()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

	// Configure PB3 as LED output
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Configure PA9 / PA10 as UART1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinRemapConfig(GPIO_Remap_USART1, DISABLE);
}

int USART_Receive_Task()
{
	uint32_t timeout = 0;

	// Check if there is data available in the UART, return otherwise:
	if(TM_USART_Available(USART1) < 3)
		return -1;

	// Wait for sync bytes
	if (TM_USART_Getc(USART1) != 0xAA)
		return -1;

	if (TM_USART_Getc(USART1) != 0x55)
		return -1;

	// Get the current command from UART (First byte)
	cmd.command = TM_USART_Getc(USART1);

	if (cmd.u.IMAGE_EXT) {
		// Keep reading for the external image!
		unsigned int spointer = 0;
		if (cmd.u.IMAGE_EXT_RLE)
		{
			while(1) {
				while(TM_USART_BufferEmpty(USART1) == SET) {
					if (timeout++ == 50000) {
						syncBlink(500, 2);
						return CMD_IMAGE_COMPRESSED;
					}
				}
				scratch[spointer++] = TM_USART_Getc(USART1);
			}
		}
		else
		{
			while (spointer < sizeof(scratch)) {
				// Read buffer to scratch!
				while(TM_USART_BufferEmpty(USART1) == SET) {
					if (timeout++ == 5000000) {
						syncBlink(500, 3);
						return -1;
					}
				}
				scratch[spointer++] = TM_USART_Getc(USART1);
			}
		}
	}
	else {
		// Copy the internal compressed image
		memcpy(scratch, image_table[cmd.u.IMAGE_PRELOAD_NUM], sizeof(scratch));
		syncBlink(200, 2);
		return CMD_IMAGE_COMPRESSED;
	}

	syncBlink(200, 2);
	return CMD_IMAGE_UNCOMPRESSED;
}

int main() {
	// Init HW for the microcontroller
	initHW();

	// Enable semihosting if enabled
#ifdef DEBUG
	initialise_monitor_handles();
#endif

	debug_print("Hallo\n");

	GPIO_Initialize();
	delay_init();

	// Initialize USART1 on default pins
	TM_USART_Init(USART1, TM_USART_PinsPack_1, 1000000);

	// Blink to indicate the chip has booted
	syncBlink(500, 2);

	// Clear the screenbuffer
	memset(scratch, 0xFF, sizeof(scratch));

	// Initialize tables (according to direction)
	einkd_init(0);

	while (1) {
		int status = USART_Receive_Task();
		if (status == CMD_IMAGE_UNCOMPRESSED)
		{
			einkd_PowerOn();
			einkd_refresh(scratch);
			einkd_PowerOff();
		}
		else if (status == CMD_IMAGE_COMPRESSED)
		{
			einkd_PowerOn();
			einkd_refresh_compressed(scratch);
			einkd_PowerOff();
		}
	}
	return 0;
}


