/*
 * Serial.h
 *
 * Created: 28.09.2015 09:30:45
 *  Author: Dag
 */ 
#ifndef SERIAL_H_
#define SERIAL_H_

#ifndef F_CPU
// define cpu clock speed if not defined
#define F_CPU 12000000UL
#endif

#include <avr/io.h>
#include <stdio.h>

// ** Set desired baud rate
#define BAUDRATE 9600

// ** Calculate UBRR value
#define UBRRVAL ((F_CPU/(BAUDRATE*16UL))-1)

void USART_Init(void);
void USART_Flush( void );
uint8_t USART_ReceiveByte(void);
void usart_init(uint16_t ubrr);
char usart_getchar( void );
void usart_putchar( char data );
void usart_pstr(char *s);
unsigned char kbhit(void);
int usart_putchar_printf(char var, FILE *stream);

/********************************************************************************
Global Variables
********************************************************************************/
static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

#endif /* SERIAL_H_ */
