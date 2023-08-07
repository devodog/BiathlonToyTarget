/*
 * serial.c
 *
 * Created: 28.09.2015 09:28:05
 *  Author: Dag
 *
 *
 * AtMega32U4 UART functions.
 *
 */ 
#include "serial.h"

void USART_Init(void)
{
   //Set baud rate
   UBRR0L=(uint8_t)UBRRVAL;   // low byte
   UBRR0H=(UBRRVAL>>8);			// high byte
   
   // ** Enable Transmitter and Receiver
   UCSR0B = ((1<<RXEN0)|(1<<TXEN0));
   UCSR0B |= (1<<RXCIE0); //RX Complete Interrupt Enable
   // UCSZ02 must be '0' for 8 bit operation - this is the initial value of this bit in the UCSRnB register.
   
   // Don't touch the UCSR0C register - the initial values will do.
   
   // enable global interrupts
   // - done in the Main_Init function...
}

uint8_t USART_ReceiveByte(void)
{
   // Wait until a byte has been received
   while ((UCSR0A & (1<<RXC0)) == 0);
   // Return received data
   return UDR0;
}

void USART_Flush( void )
{
   unsigned char dummy;
   while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
}

void USART_Transmit( unsigned char data )
{
   /* Wait for empty transmit buffer */
   while ( !( UCSR0A & (1<<UDRE0)) )
   ;
   /* Put data into buffer, sends the data */
   UDR0 = data;
}

void usart_putchar(char data) 
{
   // Wait for empty transmit buffer
   //while ( !(UCSR0A & (_BV(UDRE0))) );
   while ( !( UCSR0A & (1<<UDRE0)) )
   ;
   
   // Start transmission
   UDR0 = data;
}
/*
char usart_getchar(void) {
   // Wait for incoming data
   while ( !(UCSR0A & (_BV(RXC0))) );
   // Return the data
   return UDR0;
}
*/
unsigned char usart_kbhit(void) {
   //return nonzero if char waiting polled version
   unsigned char b;
   b=0;
   if(UCSR0A & (1<<RXC0)) b=1;
   return b;
}

void usart_pstr(char *s) {
   // loop through entire string
   while (*s) {
      usart_putchar(*s);
      s++;
   }
}

// this function is called by printf as a stream handler
int usart_putchar_printf(char var, FILE *stream) {
   // translate \n to \r for br@y++ terminal
   if (var == '\n')
   usart_putchar('\r');
   
   usart_putchar(var);
   return 0;
}
