#define _XTAL_FREQ 20000000 //Crystal Frequency, used in delay

#define TMR0_INTERRUPT_TICKER_FACTOR    1

void TMR0_Initialize(void);

void TMR0_StartTimer(void);

void TMR0_StopTimer(void);

void TMR0_ISR(void);

void timer_isr(void);

void interrupt (void);