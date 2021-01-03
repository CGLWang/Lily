#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <iostream>
using namespace std;

#define True true
#define False false
#define not !

#ifndef using_Lily
#define using_Lily
#endif // !using_Lily

//[options]
#define Tasks_using_pool
#define  Tasks_using_queue
#define in_debug
#define end 0
#define again 1
typedef char(*Tasks_def)();//return 0 meas task ended
typedef char (*Arg_Tasks_def)(void*, void*);


//[hardware interface]
#define lily_Delay(ms) osDelay(ms)
#define lily_cout(msg) cout<<msg
extern unsigned int lily_millis();
void lily_cin(char UCA1RXBUF);
void lily_tick();


void lily_init();

enum ActState {
	onScerrn,
	background,
	closed
};
typedef struct{
	Tasks_def tick;
	char (*key_pressed)(char c);
	void(*on_creat)();
	void(*start)();
	void(*go_background)();
	void(*close)();
	char* ico,*name;
	ActState state;
	int id;
}BaseActivity;
