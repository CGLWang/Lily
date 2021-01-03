#pragma once
#include "Lily.h"
#include "Lily_basic.h"
#define boardcast_recievers_number 5
typedef struct
{
	Tasks_def recievers[boardcast_recievers_number];
	//char (*recievers[boardcast_recievers_number])();
	char audience_number;
}Boardcast;
void boardcast_from(Boardcast* b);
void regist_to_boardcast(Boardcast* b, Tasks_def f);
void deregist_from_boardcast(Boardcast* b, Tasks_def f);

void public_a_timer(Tasks_def task, unsigned int period);

int remove_timer(Tasks_def task);


typedef struct
{
	List *timer_works, *peroids,*ticks;
}Lily_timers;

extern Lily_timers lily_timers;
