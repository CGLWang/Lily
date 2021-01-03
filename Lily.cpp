

#include "Lily.h"
#include "Lily_basic.h"
#include "Lily_tasks.h"
#include "Lily_ui.h"
#include "Lily_boardcast.h"
typedef unsigned char byte;

byte sum_check = 0;
typedef struct 
{
	byte head[2];
	byte length;
}BaseFrame;
typedef struct {
	BaseFrame head;
	unsigned short distance, strength, temperature;
	byte sum_check;
};
int r_count = 0;
void lily_cin(char c)
{
	static byte head[2] = { frame_head_0,frame_head_1 };
	static byte step = 0, length = 0;
	static bool in_frame = false;
	
	r_count++;
	if (r_count == 147)
	{
		cout << hasTask << " " << hasTask_ << head << " " << rear << endl;
	}
	switch (step)
	{
	case 0:
		if (c == head[0])
		{
			step++;
		}
		break;

	case 1:
		if (c == head[1])
			step ++;
		else step = 0;
		break;
	case 2:
		in_frame = true;
		length = c;
		sum_check = head[0] + head[1] + c;
		step++;
		break;
	case 3:
		length--;
		if (length)
		{
			sum_check = head[0] + head[1] + c;
		}
		else//end
		{
			if (c != sum_check)
			{
				lily_cout("sum check failed\n");
			}
			rx[ri++] = c;
			step = 0;
			in_frame = false;
			c = '\0';
		}
	default:
		break;
	}
	if (!in_frame)
	{
		if (c == ';' || c == '\n')
			c = '\0';
		if (c == '\0')
			addTask_(excute_command);
	}
	
	rx[ri++] = c;
#ifdef in_debug
	if (ri == 255)
	{
		lily_cout("255 end");
	}
	if (ri == hi)
	{
		lily_cout("cmd queue over flow");
	}
#endif
}


float test_fields=12;

void lily_init()
{
	tasks[0] = task_mointor;
	lily_ui.cmds = new_list(sizeof(Cmd_def), 10);
	lily_ui.fields = new_list(sizeof(Field_def), 10);
	lily_ui.hijacks = new_list(sizeof(hijack), 4);
	lily_ui.para_updated = false;
	if (lily_ui.cmds == NULL || lily_ui.fields == NULL|| lily_ui.hijacks==NULL)
	{
		lily_cout("lily init failed!");
		return;
	}
	Cmd_def help;
	help.cmd = (char*)"help";
	help.annotation = (char*)"to see all cmd and fields";
	help.type = with_paras;
	help.todo = cmd_help;
	help.id = 0;
	public_cmd(help);
	public_a_field("test_field", &test_fields);
	Field_def fed = { (char*)"rcount",(char*)"received key count",(float*)&r_count,int_type };
	public_field(fed);
	//timer
	lily_timers.timer_works = new_list(sizeof(Tasks_def), 4);
	lily_timers.peroids = new_list(sizeof(unsigned int), 4);
	lily_timers.ticks = new_list(sizeof(unsigned int), 4);
	if (lily_timers.timer_works == NULL || lily_timers.peroids == NULL || lily_timers.ticks == NULL)
	{
		lily_cout("lily init failed!");
		return;
	}
}



