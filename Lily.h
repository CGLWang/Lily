
#ifndef LILY_H
#define LILY_H

#ifdef in_PC

#else
// #include <stdlib.h>
#include "core.h"

#endif



extern void (*lily_Delay)(unsigned int ms);
extern void (*lily_out)(const char*msg);
extern unsigned int (*lily_millis)();
extern const char* lily_error_msg;
#define li_error(msg,code) lily_error_msg=msg;return code
void lily_in(char UCA1RXBUF);

void lily_init();

void lily_delay(unsigned int ms);
void lily_out_queue(const char*msg);

#endif