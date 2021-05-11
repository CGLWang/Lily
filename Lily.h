#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <iostream>
using namespace std;

#define True true
#define False false

#ifndef using_Lily
#define using_Lily
#endif // !using_Lily

//[options]
#define Tasks_using_pool
#define  Tasks_using_queue
#define in_debug
#define in_PC
#define end 0
#define again 1
// #define var auto
extern void (*lily_Delay)(unsigned int ms);
extern void (*lily_out)(const char*msg);
extern unsigned int (*lily_millis)();

typedef int(*Tasks_def)();//return 0 meas task ended
typedef int(*TasksArg_def)(void*);
//typedef int (*Arg_Tasks_def)(void* argc, void* argv);
typedef int (*Arg_Tasks_def)(int argc, char* argv[]);


//[hardware interface]
// #define lily_Delay(ms) osDelay(ms)
// #define lily_out(msg) cout<<msg

// extern unsigned int lily_millis();
extern const char* lily_error_msg;
#define li_error(msg,code) lily_error_msg=msg;return code
void lily_in(char UCA1RXBUF);
// void lily_tick();

void lily_init();

void lily_delay(unsigned int ms);
void lily_out_queue(const char*msg);