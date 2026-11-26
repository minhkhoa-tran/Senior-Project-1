char segments[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tmr0.h"
#include "keypad.h"

#define READ_ADC  1
#define CALCULATE_HEART_BEAT 2
#define SHOW_HEART_BEAT 3
#define IDLE 0
#define DEFAULT -1

volatile int rate[10];
volatile unsigned long samplecounter = 0;
volatile unsigned long lastbeattime = 0;
volatile int peak = 512;
volatile int trough = 512;
volatile int thresh = 530;
volatile int amp = 0;
volatile bool firstbeat = true;
volatile bool secondbeat = false;

volatile int bpm;
volatile int signal;
volatile int IBI = 600;
volatile bool pulse = false;
volatile bool qs = false;

int main_state = -1;
int adc_value = 0;

int N, i;
uint16_t runningtotal = 0;

char input_threshold[3] = {'n', 'n', 'n'};
int threshold_digit = 0;
int temp_threshold = 0;
int threshold = 0;
char kp;
bool threshold_set = false;

void displaydigit(unsigned short digit, unsigned short position) {
    PORTC = 0x00;
    PORTD = segments[digit];

    switch(position) {
        case 0: RC1_bit = 1; break;
        case 1: RC2_bit = 1; break;
        case 2: RC3_bit = 1; break;
    }

    Delay_ms(3);
    PORTC = 0x00;
}

void displaybpm(unsigned int value) {
    unsigned short h = value / 100;
    unsigned short t = (value / 10) % 10;
    unsigned short u = value % 10;

    displaydigit(h, 0);
    displaydigit(t, 1);
    displaydigit(u, 2);
}

void calculate(int adc_value){
     signal = adc_value;
     samplecounter +=2;
     N = samplecounter - lastbeattime;
     if (signal < thresh && N > (IBI / 5)*3){
        if (signal < trough){
           trough = signal;
        }
     }
     if (signal > thresh && signal > peak){
        peak = signal;
     }
     if (N > 250) {
        if ((signal > thresh) && (pulse == false) && (N > (IBI / 5)*3)){
           pulse = true;
           IBI = samplecounter - lastbeattime;
           lastbeattime = samplecounter;
           if (secondbeat){
              secondbeat = false;
              for (i = 0; i <= 9; i++){
                  rate[i] = IBI;
              }
           }
           if (firstbeat){
              firstbeat = false;
              secondbeat = true;
              return;
           }

           for (i=0; i<=8; i++){
               rate[i] = rate[i + 1];
               runningtotal += rate[i];
           }
           rate[9] = IBI;
           runningtotal += rate[9];
           runningtotal /= 10;
           bpm = 60000 / runningtotal;
           qs = true;
        }
     }
     if (signal < thresh && pulse == true){
        pulse = false;
        amp = peak - trough;
        thresh = amp / 2 + trough;
        peak = thresh;
        trough = thresh;
     }
     if (N > 1000){
        thresh = 530;
        peak = 512;
        trough = 512;
        lastbeattime = samplecounter;
        firstbeat = true;
        secondbeat = false;
     }
}

void system_init(void){
    TRISA = 0xFF; //Port A as inuput
    TRISD = 0x00; //Port D as output
    TRISC = 0x00;
    TMR0_Initialize();
    TMR0_StartTimer();
    INTCON.GIE = 1;
    INTCON.PEIE = 1;
    ADC_Init();
    InitKeypad();
    switch_press_scan();
}

char key;
void main (){
system_init();                           // Initialize system
main_state = READ_ADC;
while (1){
      if(!threshold_set) {
            char key = switch_press_scan();
            if(key >= '0' && key <= '9' && threshold_digit < 3) {
                input_threshold[threshold_digit] = key;
                threshold_digit++;
            }

            else if(key == '*') { 
                if(threshold_digit > 0) {
                    threshold_digit--;
                    input_threshold[threshold_digit] = 'n';
                }
            }

            else if(key == '#' && threshold_digit == 3) {
                threshold = (input_threshold[0] - '0') * 100 +
                            (input_threshold[1] - '0') * 10 +
                            (input_threshold[2] - '0');
                            threshold_set = true;
            }
            
            temp_threshold = 0;
            if(threshold_digit > 0) temp_threshold += (input_threshold[0] - '0') * 100;
            if(threshold_digit > 1) temp_threshold += (input_threshold[1] - '0') * 10;
            if(threshold_digit > 2) temp_threshold += (input_threshold[2] - '0');

            displaybpm(temp_threshold);
        }
        else{
             switch (main_state)
             {
              case READ_ADC:
              {
              adc_value = ADC_Read(0);
              main_state = CALCULATE_HEART_BEAT;
              break;
              }
              case CALCULATE_HEART_BEAT:
              {
              calculate(adc_value);
              main_state = SHOW_HEART_BEAT;
              break;
              }
              case SHOW_HEART_BEAT:
              {
              if (qs == true){
              qs = false;
              bpm = (bpm * 0.9) - 100;
              }
              displaybpm(bpm);
              if(bpm >= threshold){
              RC0_bit = 1;
              }else{RC0_bit = 0;}
              }

              main_state = IDLE;
              break;

              case IDLE:
              {
              break;
              }
              default:
              {
              }
              }
              }
}
}

void timer_isr() {
    main_state = READ_ADC;
}

void interrupt(void) {
    if (INTCON.TMR0IE == 1 && INTCON.TMR0IF == 1) {
        TMR0_ISR();
    }
}