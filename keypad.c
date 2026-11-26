#include "keypad.h"

// Function name: InitKeypad
void InitKeypad(void)
{
        Keypad_PORT            = 0x00;        // Set Keypad port pin values zero
        Keypad_PORT_Direction = 0b00001111;       // Last 4 pins input, First 4 pins output

        OPTION_REG &= 0x7F;
}

char keypad_scanner()
{
        R1 = 0; R2 = 1; R3 = 1; R4 = 1;

        if (C1 == 0) { Delay_ms(100); while (C1==0); return '1'; }
        if (C2 == 0) { Delay_ms(100); while (C2==0); return '2'; }
        if (C3 == 0) { Delay_ms(100); while (C3==0); return '3'; }
        if (C4 == 0) { Delay_ms(100); while (C4==0); return 'A'; }

        R1 = 1; R2 = 0; R3 = 1; R4 = 1;

        if (C1 == 0) { Delay_ms(100); while (C1==0); return '4'; }
        if (C2 == 0) { Delay_ms(100); while (C2==0); return '5'; }
        if (C3 == 0) { Delay_ms(100); while (C3==0); return '6'; }
        if (C4 == 0) { Delay_ms(100); while (C4==0); return 'B'; }

        R1 = 1; R2 = 1; R3 = 0; R4 = 1;

        if (C1 == 0) { Delay_ms(100); while (C1==0); return '7'; }
        if (C2 == 0) { Delay_ms(100); while (C2==0); return '8'; }
        if (C3 == 0) { Delay_ms(100); while (C3==0); return '9'; }
        if (C4 == 0) { Delay_ms(100); while (C4==0); return 'C'; }

        R1 = 1; R2 = 1; R3 = 1; R4 = 0;

        if (C1 == 0) { Delay_ms(100); while (C1==0); return '*'; }
        if (C2 == 0) { Delay_ms(100); while (C2==0); return '0'; }
        if (C3 == 0) { Delay_ms(100); while (C3==0); return '#'; }
        if (C4 == 0) { Delay_ms(100); while (C4==0); return 'D'; }


//        return 'n';
}


// Function name: GetKey
// Read pressed key value from keypad and return its value
char switch_press_scan()                    // Get key from user
{
        char key = 'n';              // Assume no key pressed

        while(key=='n')              // Wait untill a key is pressed
                key = keypad_scanner();   // Scan the keys again and again

        return key;                  //when key pressed then return its value
}