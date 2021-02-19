#include "Lily_boardcast.h"
#include "Lily_tasks.h"
#include "Lily_basic.h"


void boardcast_from(Boardcast* b)
{
	int i;
	for (i = 0; i < b->audience_number; i++)
		addTask(b->recievers[i]);
}
void regist_to_boardcast(Boardcast* b, Tasks_def f)
{
#ifdef in_debug
	if (b == NULL || f == NULL)
	{
		lily_out(">>boardcast exception NULL\n");
		return;
	}
#endif


	b->recievers[b->audience_number] = f;
	(*b).audience_number++;

	if (b->audience_number == boardcast_recievers_number)
	{
#ifdef in_debug
		lily_out(">>boardcast add reciever overflow\n");
#endif
		b->audience_number = 0;
	}
}
void deregist_from_boardcast(Boardcast* b, Tasks_def f)
{
#ifdef in_debug
	if (b == NULL || f == NULL)
	{
		lily_out(">>boardcast exception NULL\n");
		return;
	}
#endif

	int i, j;
	for (i = 0; i < b->audience_number; i++)
		if (b->recievers[i] == f)break;

	if (i == b->audience_number)return;
	for (j = i; j < b->audience_number; j++)
		b->recievers[j] = b->recievers[j + 1];
	(b->audience_number)--;
}
Lily_timers lily_timers;

void public_a_timer(Tasks_def task, unsigned int period)
{
	
	list_add(lily_timers.peroids, &period);
	list_add(lily_timers.ticks, &period);
	list_add(lily_timers.timer_works, &task);
}
int remove_timer(Tasks_def task)
{
	int index;
	index = list_find(lily_timers.timer_works,&task);
	if (index < 0)
		return List_not_found;
	list_remove_at(lily_timers.timer_works, index);
	list_remove_at(lily_timers.peroids, index);
	list_remove_at(lily_timers.ticks, index);
	return List_normal_code;
}

void lily_tick()
{
	int i, n;
	n = lily_timers.timer_works->count;
	unsigned int* ticks = (unsigned int*)lily_timers.ticks->content;
	for (i = 0; i<n; i++)
	{
		ticks[i]--;
		if (ticks[i] == 0)
		{
			ticks[i] = *(unsigned int*)list_index(lily_timers.peroids, i);
			addTask_(*(Tasks_def*)list_index(lily_timers.timer_works, i));
		}
	}

}