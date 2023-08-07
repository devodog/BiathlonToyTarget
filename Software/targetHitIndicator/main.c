/*
 * main.c
 *
 * Created: 13.01.2022 14:22:30
 * Author : Dag
 *
 * Source code for the tinyWatcher board based on the ATtiny1634 MCU from Atmel
 * (now Microchip).
 *
 * ****************************************************************************
 * CODE TO HANDEL PIN CHANGE INTERRUPT WHICH WILL PRODUCE PWM TO A SERVO MOTOR 
 * TO ROTATE THE SERVO MOTOR'S SHAFT MORE THAN 100 DEG. 
 * ****************************************************************************
 *
 * GPIO Port B
 * PB2
 * PB3(output): D100 (BLUE LED)
 *
 * GPIO Port C
 * PC0(output): 
 * PC2(output): LED2
 *
 *
 */ 
#ifndef F_CPU
// define cpu clock speed if not defined
#define F_CPU 12000000UL
#endif

#include <avr/io.h>

#include <string.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdbool.h>

#include "appver.h"
#include "serial.h"

#define UNIT_STATUS_LED_ON() PORTB |= (1<<PORTB3)
#define UNIT_STATUS_LED_OFF() PORTB &= ~(1<<PORTB3)

#define UNIT_STATUS_LED1_ON() PORTC |= (1<<PORTC0)
#define UNIT_STATUS_LED1_OFF() PORTC &= ~(1<<PORTC0)

#define UNIT_STATUS_LED2_ON() PORTC |= (1<<PORTC2)
#define UNIT_STATUS_LED2_OFF() PORTC &= ~(1<<PORTC2)

char termInputBuffer[80];
bool cmdComplete;

bool hit;
uint8_t cmdLength = 0;
volatile uint8_t targetId0 = 0; // 

uint8_t targetState[6];

void ledBlink()
{
   UNIT_STATUS_LED_ON();
   _delay_ms(150);
   UNIT_STATUS_LED_OFF();
   _delay_ms(150);
}

void led1Blink()
{
   UNIT_STATUS_LED1_ON();
   _delay_ms(150);
   UNIT_STATUS_LED1_OFF();
   _delay_ms(150);
}

void led2Blink()
{
   UNIT_STATUS_LED2_ON();
   _delay_ms(150);
   UNIT_STATUS_LED2_OFF();
   _delay_ms(150);
}

char *cmdList[] = {"servo", "info"};
// servo   : 1 arg 'h' Hit indication or 'r' reset operation

enum commands {
   SERVO,
   INFO
} COMMANDS;


void promt() {
   printf("\nbtt> ");
}


   
// _delay_us(2200); max counterclockwise (165 degrees)
// _delay_us(1300); middle? (90 degrees)
void moveServo(char targetEvent, uint8_t targetCover) {
   printf("\nstate=%x,,,target=%d\n", (uint8_t)targetState, targetCover);
   
   for (int i=0; i<20; i++) {
      switch(targetCover) {
         case 1:
            PORTA |= 1<<PORTA5;
            break;
         case 2:
            PORTA |= 1<<PORTA6;
            break;
         case 3:
            PORTB |= 1<<PORTB2;
            break;
         case 4:
            PORTB |= 1<<PORTB1;
            break;
         case 5:
            PORTC |= 1<<PORTC0;
            break;
         default:
            return;
      }
      
      if (targetEvent == 'r') {
         _delay_us(2000);
      }
      else {
         _delay_us(1000);
      }
      
      switch(targetCover) {
         case 1:
			 PORTA &= ~(1<<PORTA5);
			 break;
         case 2:
			 PORTA &= ~(1<<PORTA6);
			 break;
         case 3:
			 PORTB &= ~(1<<PORTB2);
			 break;
         case 4:
			 PORTB &= ~(1<<PORTB1);
			 break;
         case 5:
			 PORTC &= ~(1<<PORTC0);
			 break;
         default:
         return;
      }
       _delay_ms(20); 
   }
   if (targetEvent == 'r') {
      targetState[targetCover] = 0;
   }
   else {
      targetState[targetCover] = 1;
   }
 }

uint8_t executeCmd(char *termInput, int cmdLength) {
   int i = 0;
   for (; i < sizeof(cmdList); i++) {
      if (strncmp(cmdList[i], termInput, strlen(cmdList[i])) == 0) {
         break;
      }
   }
   
   if (i >= sizeof(cmdList)) {
      printf("\n%s is not recognized", termInput);
      promt();
   }
   else {
      switch (i) {
         case SERVO:
		   // If reset all...
            if ((termInput[6] == 'r')&&(termInput[8] == 'a')) {
			      for (int i = 1; i < 6; i++) {
				      if(targetState[i]==1) {
					      moveServo('r', i);
					      targetState[i] = 0;
					      printf("\nMoving servo...");
				      }
			      }
		      }
		      // If set all as hit...
		      else if ((termInput[6] == 'h')&&(termInput[8] == 'a')) {
			      for (int i = 1; i < 6; i++) {
				      if(targetState[i]==0) {
					      moveServo('h', i);
					      targetState[i] = 1;
					      printf("\nMoving servo...");
				      }
			      }
		      }
		      // set or reset specific...
		      else {
			      moveServo(termInput[6], termInput[8]-48);
		      }
            promt();
         break;
         
         case INFO:
            printf("\nBiathlonToyTarget ver. %d.%d build: %d\n", MAJOR_VERSION, MINOR_VERSION, BUILD);
            for (int i=1; i<6; i++) {
               printf("Target[%d] state: %d\n", i, targetState[i]);
            }
            // Info about the current target status?
            promt();
         break;
         
         default:
            printf("\nNOP[i=%d]", i);
            promt();
         break;
      }
   }
   return 0;
}

void init() {
   // Initialize PORT PA5 & PA6 as output (PWM to servos).
   DDRA |= (1<<PORTA5)|(1<<PORTA6);
   
   //DDRA &= ~(1<<PORTA0); // Pin Change Input interrupt
     
   DDRC |= (1<<PORTC0)|(1<<PORTC2); // PC0 for PWM to servo. C2 for LED.
   
   DDRB |= (1<<PORTB1)|(1<<PORTB2)|(1<<PORTB3);  // PB1 & PB2 for PWM to servos. PB3 for LED.
   
   hit = false; // Target status
   
   
   // Assign that address for my stdout function...
   stdout = &mystdout;
   USART_Init();
   
   // Reset all target covers.
   moveServo('r', 'a');
   
   // Initialize the pin-change interrupts...
   // We'll start with one pin which is PA6 = PCI6
   
   // MCUCR – MCU Control Register
   MCUCR |= 0x1; // Any logical change on INT0 generates an interrupt request
   // GIMSK – General Interrupt Mask Register
   GIMSK |= 0x8; // Pin Change Interrupt Enable 0 - for all pins of port A.
   PCMSK0 |= 0x1f; // Pin Change Enable Mask on PORTA => 0b0001 1111 = (PA0 & PA1 & PA2 & PA3 & PA4)
   
   sei();
   
   // Boot printout...
   printf("Version.%d.%d-b:%d\n", MAJOR_VERSION, MINOR_VERSION, BUILD);
   for(int8_t i=5; i>0; i--)
   {
      if (i==1) {
        printf("\n");
      }
      else {
        printf(".");
      }
      ledBlink();
   }
   promt();
}

int main(void)
{
   init();
   uint8_t target = 0;
   // Received a command from the serial com terminal 
   while (1) {
      if(cmdComplete) {
         cmdComplete = false;
         if (cmdLength == 0) {
            promt();
            continue;
         }
         executeCmd(&termInputBuffer[0], cmdLength);
         memset(termInputBuffer, 0, 80);
      }
      
      if (hit) {
         // Pin Change interrupt for PORT A only, using pin PA0 to PA4 as hit indicators. 
         printf("\nHIT on target %x", targetId0);
         for (int i = 0; i < 5; i++) {
            if ((targetId0 >> i) == 1)
               target = i+1;
         }
		 
		 if ((target < 6) && (target > 0)) {
			moveServo('h', target);
		 }
       promt();
		 hit = false;
      }
    }
}