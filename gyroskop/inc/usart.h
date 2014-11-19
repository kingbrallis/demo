/*
 * usart.h
 *
 *  Created on: Nov 26, 2012
 *      Author: andrei
 */

#ifndef USART_H_
#define USART_H_

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

extern PUTCHAR_PROTOTYPE;
extern void USART2_Init(uint32_t speed);


#endif /* USART_H_ */
