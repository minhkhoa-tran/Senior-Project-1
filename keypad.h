#ifndef __KEYPAD_H
#define __KEYPAD_H

#define _XTAL_FREQ 20000000 //Crystal Frequency, used in delay

// Define pins
#define R1  RB7_bit
#define R2  RB6_bit
#define R3  RB5_bit
#define R4  RB4_bit
#define C1  RB3_bit
#define C2  RB2_bit
#define C3  RB1_bit
#define C4  RB0_bit

#define Keypad_PORT                        PORTB
#define Keypad_PORT_Direction                TRISB


// Function declarations
void InitKeypad(void);
char switch_press_scan(void);

#endif