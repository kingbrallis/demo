/*
 * usart.c
 *
 *  Created on: Nov 26, 2012
 *      Author: andrei
 */

#include <stdint.h>
#include "stm32f30x_conf.h"
#include "usart.h"

static USART_InitTypeDef USART_InitStructure;
static GPIO_InitTypeDef GPIO_InitStructure;


void USART2_Init(uint32_t speed)
{
	/*!< At this stage the microcontroller clock setting is already configured,
			   this is done through SystemInit() function which is called from startup
			   file (startup_stm32f37x.s) before to branch to application main.
			   To reconfigure the default setting of SystemInit() function, refer to
			   system_stm32f37x.c file
	 */
	/* USARTx configured as follow:
				- BaudRate = 115200 baud
				- Word Length = 8 Bits
				- One Stop Bit
				- No parity
				- Hardware flow control disabled (RTS and CTS signals)
				- Receive and transmit enabled
	 */
	USART_InitStructure.USART_BaudRate = speed;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Enable USART clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Connect PXx to USARTx_Tx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);

	/* Connect PXx to USARTx_Rx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART configuration */
	USART_Init(USART2, &USART_InitStructure);

	/* Enable USART */
	USART_Cmd(USART2, ENABLE);
}

/**
 * @brief  Retargets the C library printf function to the USART.
 * 			Overwrite __io_putchar function used by printf
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
	/* Put character on the serial line */
	USART2->TDR = (ch & (uint16_t)0x01FF);

	/* Loop until transmit data register is empty */
	while ((USART2->ISR & USART_FLAG_TXE) == (uint16_t) RESET);

	return ch;
}
