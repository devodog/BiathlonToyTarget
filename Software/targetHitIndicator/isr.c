/*
 * isr.c
 *
 * Created: 21.04.2021 21:00:00
 * Author: Dag
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdbool.h>

#include "serial.h"

volatile uint8_t bytesReceived;

// NEW: Holds the input characters received from the terminal other that carriage return & line feed
extern char termInputBuffer[80];
extern bool cmdComplete;
extern bool setpMotorHalt;
extern bool hit;
extern uint8_t targetId0;

extern uint8_t cmdLength;

/*
ISR(INT0_vect)
{
   // ** Micro switch indicate that the door is about to OPEN.
   // IF IMMEDIATE NOTIFICATION WANTED ISSUE A ITN (Incident Triggered Notification).
   // ... by setting a ITN FLAG.
   //
   // MAKE SURE THAT ALL THE REGISTERES ARE SET CORRECTLY...
   //
   // Setting the I-bit in the SREG register is usually done by calling sei();
   // The MCUCR can be left untouched.
   // GIMSK's INT0 bit needs to be set to '1'. (I would assume that this is also set by sei()).
   // ...this all... so maybe nothing needs to be done to arm this interrupt.
   printf("INT0\n");
   // Should mask away this interrupt so that it will be kept silent if triggered for a long period.(?)
}
*/

/************************************************************************
USART Receive interrupt service routine                              

Will handle command-line input from the serial communication terminal
reading byte by byte and waiting for a cr-lf that will complete the 
input

Every input character will be echoed back to the sending terminal.
*************************************************************************/
ISR(USART0_RX_vect) // will execute for every received  byte from the GSM module.
{	
   int n = bytesReceived; // global byte counter...
   char rxChar;
   rxChar = USART_ReceiveByte();
   if (rxChar == 13) {
      cmdComplete = true;
      cmdLength = n;
      bytesReceived = 0;
      return;
   }
   else if (rxChar == 3) {
      cmdComplete = true;
      bytesReceived = 0;
      return;
   }
   else {
      termInputBuffer[n] = rxChar;
      // echo input char
      usart_putchar(rxChar);
   }
   n++;
   bytesReceived = n;
}

ISR(PCINT0_vect) {
   // read value on the relevant pins and store these in a global variable ...
   // for now we'll use a single boolean variable. 
   if (hit == true)
   return;
   hit = true;
   // Read the Pin Change Int flag register 0...
   targetId0 = PINA & 0x1f;
   
   //printf("\nPCINT0_vect\n");
}
