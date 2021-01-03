
#include "Lily.h"
#include "Lily_basic.h"
#include "Lily_tasks.h"
#include "Lily_ui.h"

#ifdef Tasks_using_pool
Tasks_def tasks[Tasks_LEN];
int hasTask;

void addTask(Tasks_def task)
{
	hasTask++;
	if (hasTask >=Tasks_LEN)hasTask = 1;
	tasks[hasTask] = task;
	//taskpool.hasTask++;
	//taskpool.tasks[taskpool.hasTask] = task;
}
void endTaskAt(char index)
{
	int i;
	if (index > hasTask)
	{
#ifdef in_debug
		lily_cout("error invalid index");
		return;
#endif // in_debug

	}
	for (i = index; i < hasTask; i++)
		tasks[i] = tasks[i + 1];
	hasTask--;
}
//end a task
//if no such task, nothing will be done
void endTask(Tasks_def task)
{
	if (hasTask == 1 && tasks[1] == task)
	{
		hasTask--;
		return;
	}

	int i;
	for (i = 1; i <= hasTask; i++)
	{
		if (tasks[i] == task)break;
	}

	if (i <= hasTask)
	{
		endTaskAt(i);
	}
}
//whether has a task in pool
//return the index of a task, if any
char hadTask(Tasks_def task)
{
	char b = 0;
	char i;
	for (i = 0; i <= hasTask; i++)
	{
		if (tasks[i] == task)
		{
			b = i;
			break;
		}
	}
	return b;
}

#endif

#ifdef Tasks_using_queue
unsigned int front, rear;

Tasks_def tasks_[Tasks_LEN];
unsigned int hasTask_;
void addTask_(Tasks_def task)
{
	tasks_[rear++] = task;
	if (rear >= Tasks_LEN)
		rear = 0;
}

void endTask_(Tasks_def task)//trash
{
	int i;
	for (i = front; i != rear;)
	{
		if (tasks_[i] == task)break;
		i++;
		if (i == Tasks_LEN)
			i = 0;
	}
	if (i == rear)return;
	int j = i, next;
	while (j != rear)
	{
		next = j + 1;
		if (next == Tasks_LEN)
			next = 0;
		tasks_[j] = tasks_[next];
		j = next;
	}
}
void endTaskAt_(char index)
{
	int j = index, next;
	while (j != rear)
	{
		next = j + 1;
		if (next == Tasks_LEN)
			next = 0;
		tasks_[j] = tasks_[next];
		j = next;
	}
}
//whether has a task in pool
//return the index of a task, if any
char hadTask_(Tasks_def task)
{
	char b = 0;
	char i;

	for (i = front; i != rear; )
	{
		if (tasks_[i] == task)
		{
			b = i;
			break;
		}

		if (++i == Tasks_LEN)
			i = 0;
	}
	return b;
}
//
//#undef tasks_
//#undef hasTask_
//#undef addTask_
//#undef endTask_
//#undef hadTask_
//#undef  endTaskAt_
#endif // Tasks_using_queue

void run_tasks()
{
	int i;
	while (1)
	{
#if defined(Tasks_using_pool) && defined(Tasks_using_queue)
		for (i = 0; i <= hasTask; i++)
		{
#ifdef in_debug
			if (tasks[i] == NULL)
			{
				lily_cout("null ptr");
				break;
			}
				
#endif // in_debug
			if (!tasks[i]())
			{
				endTaskAt(i);
				i--;
			}
		}
		
		i = rear;
		while (front != i)
		{
			if (tasks_[front]())
				addTask_(tasks_[front]);
			if (++front >= Tasks_LEN)
				front = 0;
		}
#else
#ifdef Tasks_using_pool
		for (i = 0; i <= hasTask; i++)
			tasks[i]();
#endif 
#ifdef Tasks_using_queue
		i = rear;

		while (front != i)
		{

			if (tasks[front]())addTask(tasks[front]);
			if (++front == Tasks_LEN)
				front = 0;
		}

		lily_Delay(1);
#endif
#endif

	}
}

//monitor 
char task_mointor()
{
	static int c = 100;
	static int last_t = 0;
	c--;
	if (c)return 1;
	c = 100;
	int t = lily_millis();
	int dt = t - last_t;
	last_t = t;
	sprintf(tx, "using %d ms\n", dt);
	lily_cout(tx);
	return 1;
}