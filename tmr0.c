#include <stdint.h>
#include "tmr0.h"

const uint8_t tmr0_mask = 0xFF;

void TMR0_Initialize(void) {
    // Set TMR0 to the options selected in the User Interface

    // T0PS 1:256; T0SE Increment_hi_lo; T0CS FOSC/4; PSA assigned;
    //OPTION_REG = 0x04;
    OPTION_REG.T0CS = 0;
    OPTION_REG.T0SE = 1;
    OPTION_REG.PSA = 0;             // prescale use for only Timer0 not for WatchDog_timer
    OPTION_REG = (OPTION_REG & 0xF8) | 0x07;
    TMR0 = (uint8_t)(tmr0_mask & (256-(((2 *_XTAL_FREQ)/(256*4))/1000)));     // delay for 2ms
                                                                 // TimerCountMax - (((delay(ms) * Focs(hz)) / (PreScale_Val * 4)) / 1000)

    // Clear Interrupt flag before enabling the interrupt
    INTCON.TMR0IF = 0;
}


void TMR0_StartTimer(void) {
    // Start the Timer by writing to TMR0ON bit
    //T0CONbits.TMR0ON = 1;
    INTCON.TMR0IE = 1;
}

void TMR0_StopTimer(void) {
    // Stop the Timer by writing to TMR0ON bit
    //T0CONbits.TMR0ON = 0;
    INTCON.TMR0IE = 0;
}

void TMR0_ISR(void) {
    static volatile uint16_t CountCallBack = 0;
    // disabling TMR0 interrupt.
    INTCON.TMR0IE = 0;
    // clear the TMR0 interrupt flag
    INTCON.TMR0IF = 0;

    // reload TMR0
    // Write to the Timer0 register
    TMR0 = (uint8_t)(tmr0_mask & (256-(((2 *_XTAL_FREQ)/(256*4))/1000)));     // delay for 2ms

                                                                // TimerCountMax - (((delay(ms) * Focs(hz)) / (PreScale_Val * 4)) / 1000)
    if (++CountCallBack >= TMR0_INTERRUPT_TICKER_FACTOR)
    {
        // ticker function call
        timer_isr();

        // reset ticker counter
        CountCallBack = 0;
    }

    // Enabling TMR0 interrupt.
    INTCON.TMR0IE = 1;
}