/*	Author: sflei002
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <stdio.h>
#include "io.h"

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include <avr/interrupt.h>
#include "bit.h"
#include "scheduler.h"
#include "keypad.h"
//#include "timer.h"
//--------------------------------------------------

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
        TCCR1B = 0x0B;

        OCR1A = 125;

        TIMSK1 = 0x02;

        TCNT1 = 0;

        _avr_timer_cntcurr = _avr_timer_M;

        SREG |= 0x80;
}

void TimerOff(){
        TCCR1B = 0x00;
}

void TimerISR(){
        TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
        _avr_timer_cntcurr--;
        if(_avr_timer_cntcurr == 0){
                TimerISR();
                _avr_timer_cntcurr = _avr_timer_M;
        }
}

void TimerSet(unsigned long M){
        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;
}


//--------------------------------------------------
unsigned char x;
unsigned char B = 0x00;
enum KP_States {KP_1};
int Tick_KP(int state){
	x = GetKeypadKey();
        switch(state){
                case KP_1:
                        break;
                default:
                        state = KP_1;
                        break;
        }
        switch(state){
                case KP_1:
                        switch(x){
                        case '\0' : B = 0x1F; break;
                        case '0': B = 0x00; LCD_Cursor(1); LCD_WriteData(B + '0');  break;
                        case '1': B = 0x01; LCD_Cursor(1); LCD_WriteData(B + '0');  break;
                        case '2': B = 0x02; LCD_Cursor(1); LCD_WriteData(B + '0');  break;
                        case '3': B = 0x03; LCD_Cursor(1); LCD_WriteData(B + '0');  break;
                        case '4': B = 0x04; LCD_Cursor(1); LCD_WriteData(B + '0');  break;
                        case '5': B = 0x05; LCD_Cursor(1); LCD_WriteData(B + '0');  break;
                        case '6': B = 0x06; LCD_Cursor(1); LCD_WriteData(B + '0');  break;
                        case '7': B = 0x07; LCD_Cursor(1); LCD_WriteData(B + '0');  break;
                        case '8': B = 0x08; LCD_Cursor(1); LCD_WriteData(B + '0');  break;
                        case '9': B = 0x09; LCD_Cursor(1); LCD_WriteData(B + '0');  break;
                        case '*': B = 0x0E; LCD_Cursor(1); LCD_WriteData(0x2A); break;
                        case '#': B = 0x0F; LCD_Cursor(1); LCD_WriteData(0x23); break;
                        case 'A': B = 0x0A; LCD_Cursor(1); LCD_WriteData(0x41); break;
                        case 'B': B = 0x0B; LCD_Cursor(1); LCD_WriteData(0x42); break;
                        case 'C': B = 0x0C; LCD_Cursor(1); LCD_WriteData(0x43); break;
                        case 'D': B = 0x0D; LCD_Cursor(1); LCD_WriteData(0x44); break;
                        default: B = 0x1B; break;
                        }
                break;
                default: break;
        }
        return state;
}
//--------------------------------------------------

int main(void) {
    DDRB = 0xFF;
    PORTB = 0x00;
    DDRC = 0xF0;
    PORTC = 0x0F;
    DDRD = 0xFF;
    PORTD = 0x00;

    static task task1;
    task *tasks[] = {&task1};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    const char start = -1;
    task1.state = start;
    task1.period = 100;
    task1.elapsedTime = task1.period;
    task1.TickFct = &Tick_KP;

    unsigned short i;
    unsigned long GCD = tasks[0]->period;
    for(i = 1; i < numTasks; i++){
	GCD = findGCD(GCD, tasks[i]->period);
    }

    TimerSet(GCD);
    TimerOn();
    LCD_init();
    LCD_ClearScreen();
    unsigned char j;
    while (1) {
	for(j = 0; j < numTasks; j++){
		if(tasks[j]->elapsedTime == tasks[j]->period){
			tasks[j]->state = tasks[j]->TickFct(tasks[j]->state);
			tasks[j]->elapsedTime = 0;
		}
		tasks[j]->elapsedTime += GCD;
	}
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 0;
}
