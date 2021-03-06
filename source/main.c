/*	Author: sflei002
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
//#include <avr/interrupt.h>
#include "keypad.h"
#include "scheduler.h"
#include "timer.h"

//--------------------------------------------------
enum KP_States{KP_init, KP_1};
int Tick_KP(int state){
	unsigned char x = GetKeypadKey();
	switch(state){
		case KP_init:
			state = KP_1;
			break;
		case KP_1:
			break;
		default:
			state = KP_init;
			break;
	}
	switch(state){
		case KP_init:
			state = KP_1;
                        break;
                case KP_1:
			switch(x){
			case '\0' : PORTB = 0x1F; break;
			case '0': PORTB = 0x00; break;
			case '1': PORTB = 0x01; break;
			case '2': PORTB = 0x02; break;
			case '3': PORTB = 0x03; break;
			case '4': PORTB = 0x04; break;
			case '5': PORTB = 0x05; break;
			case '6': PORTB = 0x06; break;
			case '7': PORTB = 0x07; break;
			case '8': PORTB = 0x08; break;
			case '9': PORTB = 0x09; break;
			case '*': PORTB = 0x0E; break;
			case '#': PORTB = 0x0F; break;
			case 'A': PORTB = 0x0A; break;
			case 'B': PORTB = 0x0B; break;
			case 'C': PORTB = 0x0C; break;
			case 'D': PORTB = 0x0D; break;
			default: PORTB = 0x1B; break;
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

    static task task1;
    task *tasks[] = {&task1};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    const char start = -1;
    task1.state = start;
    task1.period = 50;
    task1.elapsedTime = task1.period;
    task1.TickFct = &Tick_KP;

    unsigned short i;
    unsigned long GCD = tasks[0]->period;
    for(i = 1; i < numTasks; i++){
	GCD = findGCD(GCD, tasks[i]->period);
    }

    TimerSet(GCD);
    TimerOn();
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
