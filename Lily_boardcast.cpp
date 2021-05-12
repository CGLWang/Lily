#ifdef in_PC
#include"Lily_help.h"
#else
#include"Lily_help.h"
#endif // in_PC


void boardcast_from(Boardcast *b)
{
	int i;
	for (i = 0; i < b->audience_number; i++)
		addTask(b->recievers[i]);
}
void regist_to_boardcast(Boardcast *b, Tasks_def f)
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
void deregist_from_boardcast(Boardcast *b, Tasks_def f)
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
		if (b->recievers[i] == f)
			break;

	if (i == b->audience_number)
		return;
	for (j = i; j < b->audience_number; j++)
		b->recievers[j] = b->recievers[j + 1];
	(b->audience_number)--;
}
Lily_timers lily_timers;
#define quick_timer_len 4
Tasks_def quick_timer[quick_timer_len];
int quick_timer_count[quick_timer_len];
char quick_timer_size = 0;
void public_a_timer(Tasks_def task, unsigned int period)
{

	list_add(lily_timers.peroids, &period);
	list_add(lily_timers.ticks, &period);
	list_add(lily_timers.timer_works, &task);
}
int remove_timer(Tasks_def task)
{
	int index;
	index = list_find(lily_timers.timer_works, &task);
	if (index < 0)
		return List_not_found;
	list_remove_at(lily_timers.timer_works, index);
	list_remove_at(lily_timers.peroids, index);
	list_remove_at(lily_timers.ticks, index);
	return List_normal_code;
}
int find_timer(Tasks_def task)
{
	return list_find(lily_timers.timer_works, &task);
}

int quick_timer_tick();

void lily_tick()
{
	int i, n;
	n = lily_timers.timer_works->count;
	unsigned int *ticks = (unsigned int *)lily_timers.ticks->content;
	for (i = 0; i < n; i++)
	{
		ticks[i]--;
		if (ticks[i] == 0)
		{
			ticks[i] = *(unsigned int *)list_index(lily_timers.peroids, i);
			addTask_(*(Tasks_def *)list_index(lily_timers.timer_works, i));
		}
	}
	// quick_timer_tick();
	for (i = 0; i < quick_timer_size; i++)
	{
		if (--quick_timer_count[i] <= 0)
		{
			//swap
			addTask_(quick_timer[i]);
			quick_timer[i] = quick_timer[quick_timer_size - 1];
			quick_timer_count[i--] = quick_timer_count[quick_timer_size - 1];

			//remove
			quick_timer_size--;
			// i--;
		}
	}
}

int quick_timer_tick()
{
	int i;
	for (i = 0; i < quick_timer_size; i++)
	{
		if (--quick_timer_count[i] <= 0)
		{
			//swap
			quick_timer_count[i--] = quick_timer_count[quick_timer_size - 1];
			//remove
			quick_timer_size--;
			// i--;
		}
	}
	return 0;
}
int add_timer_once(Tasks_def task, int count)
{
	quick_timer[quick_timer_size] = task;
	quick_timer_count[quick_timer_size++] = count;
	if (quick_timer_size >= quick_timer_len)
	{
		quick_timer_size = 0;
		lily_out("\aquick timer overflow\n");
		return -1;
	}
	return 0;
}
int change_quick_timer_count(Tasks_def timer, int newCount)
{
	int i;
	for (i = 0; i < quick_timer_size; i++)
	{
		if (quick_timer[i] == timer)
		{
			quick_timer_count[i] = newCount;
			return 0;
		}
	}
	return -1;
}
int create_or_change_quick_timer_count(Tasks_def timer, int newCount)
{
	int i;
	for (i = 0; i < quick_timer_size; i++)
	{
		if (quick_timer[i] == timer)
		{
			quick_timer_count[i] = newCount;
			return 0;
		}
	}
	return add_timer_once(timer, newCount);
}
