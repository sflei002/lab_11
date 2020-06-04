#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

unsigned long int findGCD(unsigned long int a, unsigned long int b){
	unsigned long int c;
	while(1){
		c = a % b;
		if(c == 0) { return b; }
		a = b;
		b = c;
	}
	return 0;
}

typedef struct task{
	signed char state;
	unsigned long period;
	unsigned long elapsedTime;
	int (*TickFct)(int);
} task;

#endif
