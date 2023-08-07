/*
 * motor.h
 *
 * Created: 24.04.2021 14:03:24
 *  Author: Dag
 */ 

#ifndef MOTOR_H_
#define MOTOR_H_

#define MICRO_STEP 200
#define SEQUENCE_STEP 50

#define IDLE_INDICATION 60
#define SPEED_DIVIDER 20
#define MAX_SPEED 80

//#define DYNAMIC_SPEED_CONTROL 1
#ifndef DYNAMIC_SPEED_CONTROL
#define MINIMUM_SIGNAL_DURATION 2350   // microseconds - without dynamic speed
// adjustment...
#else
#define MINIMUM_SIGNAL_DURATION 100
// Sampling the adc takes more than one millisecond so the dynamic speed
// adjustment will not give max speed to the motor.
#endif

#define MAX_SIGNAL_DURATION ((1024/SPEED_DIVIDER)+1)
#define UNIT_STATUS_LED_ON() PORTB |= (1<<PORTB3)
#define UNIT_STATUS_LED_OFF() PORTB &= ~(1<<PORTB3)

#define UNIT_STATUS_LED1_ON() PORTC |= (1<<PORTC0)
#define UNIT_STATUS_LED1_OFF() PORTC &= ~(1<<PORTC0)

#define UNIT_STATUS_LED2_ON() PORTC |= (1<<PORTC2)
#define UNIT_STATUS_LED2_OFF() PORTC &= ~(1<<PORTC2)

// Step motor operation configuration
typedef struct {
   bool man;            // Manual operation...
   uint8_t dir;         // the direction of the rotation
   uint16_t speed;		//
   uint16_t turn;       // number of rotations to execute
   uint16_t dur;        // duration of continue motor operation
   uint16_t delay;      // delay before executing the motor operation
   uint16_t refpos;     // reference position
   uint16_t steps;      // current or next position relative to the reference position (get/set)
   uint16_t degree;     // current or next position in degrees relative to the reference position
   uint8_t error;       // last error number
} STEP_MOTOR_CONFIG;

void promt();
void initStepMotorConfig(STEP_MOTOR_CONFIG* cfg);
uint8_t setMotorParams(char *arg, char *value);
uint8_t executeCmd(char *termInput, int cmdLength);
void noOperation();
int setStepMotorSpeed();
bool isStepMotorHalted();
void run(int dir, int speed, int numOfSteps);
void moveServo(char targetState);

#endif /* MOTOR_H_ */