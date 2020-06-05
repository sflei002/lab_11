/*	Author: sflei002
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #2
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
unsigned char mess[] = "CS120B is Legend... wait for it DARY!";
unsigned char ms[32] = "";
unsigned char scroll = 0;
//--------------------------------------------------
void P_string(unsigned char string[], unsigned char index){
	for(unsigned char i = 0; i < 32; ++i){
		ms[i] = ' ';
	}
	if(index < 15){
		for(unsigned char i = 0; i < index; ++i){
                ms[16 - index + i] = string[i];
        	}
	}
	else{
		for(unsigned char i = 0; i < 16; ++i){
                ms[15 - i] = string[index - i];
        	}
	}
}
//--------------------------------------------------
enum S_States {S_1};
int Tick_S(int state){
	switch(state){
		case S_1:
			if(scroll < sizeof(mess)){
				scroll++;
			}
			else{
				scroll = 0;
			}
			break;
		default:
			state = S_1;
			break;
	}
	switch(state){
                case S_1:
			P_string(mess, scroll);
			LCD_DisplayString(1, ms);
			break;
		default: break;
	}
	return state;
}
//--------------------------------------------------

int main(void) {
    DDRB = 0xFF;
    PORTB = 0x00;
    DDRD = 0xFF;
    PORTD = 0x00;

    static task task1;
    task *tasks[] = {&task1};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    const char start = -1;
    task1.state = -1;
    task1.period = 100;
    task1.elapsedTime = task1.period;
    task1.TickFct = &Tick_S;

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
