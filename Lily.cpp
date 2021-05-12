#ifdef in_PC
#include"Lily_help.h"

#else

#include <stdio.h>
#include "core.h"
#include"Lily_help.h"

#endif

unsigned int lily_built_in_millis();

void (*lily_Delay)(unsigned int ms) = lily_delay;
void (*lily_out)(const char *msg) = lily_out_queue;
unsigned int (*lily_millis)() = lily_built_in_millis;

const char *lily_error_msg = NULL;
int r_count = 0;
void lily_in(char c)
{
	r_count++;
	if (c == ';' || c == '\n')
		c = '\0';
	if (c == '\0')
	{
		addTask_(get_cmd_from_rx);
	}

	if (c == '\b')
	{
		ri--;
	}
	else
		rx[ri++] = c;
#ifdef in_debug
	if (ri == hi)
	{
		lily_out("cmd queue over flow");
	}
#endif
}

float time_cmd()
{
	int t = lily_millis();
	sprintf(tx, "%d\n", t);
	lily_out(tx);
	return (float)t;
}

void echo_error()
{
	if (lily_error_msg == NULL)
		return;
	lily_out((char *)lily_error_msg);
	lily_out("\n");
}
void lily_init()
{
#ifdef in_PC
	tasks[0] = task_mointor;
#endif
	lily_ui.cmds = new_list(sizeof(Cmd_def), 10);
	lily_ui.vars = new_list(sizeof(Var_def), 10);
	lily_ui.funs = new_list(sizeof(Fun_def), 4);
	lily_ui.para_updated = false;
	lily_ui.hijacked = false;
	if (lily_ui.cmds == NULL  || lily_ui.vars == NULL || lily_ui.funs == NULL)
	{
		lily_out("lily init failed!");
		return;
	}
#ifdef in_debug
	Cmd_def new_cmd;
	new_cmd.name = (char *)"help";
	new_cmd.annotation = (char *)"helpDoc";
	new_cmd.todo = help;
	new_cmd.id = 0;
	public_cmd(new_cmd);

	//Cmd_def new_cmd;
	// new_cmd.name = (char*)"pass";
	// new_cmd.annotation = (char*)"pass [n]:skip cmds";
	// new_cmd.todo = pass;
	// new_cmd.id = 1;
	// public_cmd(new_cmd);

	new_cmd.name = (char *)"delete";
	new_cmd.annotation = (char *)"deleteField";
	new_cmd.todo = delete_field;
	new_cmd.id = 2;
	public_cmd(new_cmd);

	new_cmd.name = (char *)"whos";
	new_cmd.annotation = (char *)"listFields";
	new_cmd.todo = whos;
	new_cmd.id = 3;
	public_cmd(new_cmd);

	// new_cmd.name = (char*)"sys";
	// new_cmd.annotation = (char*)":system info";
	// new_cmd.todo = system_cmd;
	// new_cmd.id = 4;
	// public_cmd(new_cmd);

	// new_cmd.name = (char*)"cal";
	// new_cmd.annotation = (char*)"[exp]:caculate expression";
	// new_cmd.todo = shell_cal_exp_cmd;
	// new_cmd.id = 5;
	// public_cmd(new_cmd);

	new_cmd.name = (char *)"delay";
	new_cmd.annotation = (char *)"[ms]:delayCmd";
	new_cmd.todo = delay_cmd;
	new_cmd.id = 6;
	public_cmd(new_cmd);

	new_cmd.name = (char *)"for";
	new_cmd.annotation = (char *)"forLoops";
	new_cmd.todo = cmd_for_start;
	new_cmd.id = 7;
	public_cmd(new_cmd);

	new_cmd.name = (char *)"loop";
	new_cmd.annotation = (char *)"LoopN";
	new_cmd.todo = cmd_loop_start;
	new_cmd.id = 8;
	public_cmd(new_cmd);

	//hijackor_wait_key
	new_cmd.name = (char *)"wk";
	new_cmd.annotation = (char *)"waitKey";
	new_cmd.todo = hijackor_wait_key;
	new_cmd.id = 8;
	public_cmd(new_cmd);

	new_cmd.name = (char *)"sleep";
	new_cmd.annotation = (char *)"sleepSec";
	new_cmd.todo = cmd_hijack_sleep;
	new_cmd.id = 9;
	public_cmd(new_cmd);

	new_cmd.name = (char *)"echo";
	new_cmd.annotation = (char *)"echoField";
	new_cmd.todo = cmd_echo;
	new_cmd.id = 10;
	public_cmd(new_cmd);

	public_a_fun_link_n("time", time_cmd, 0);
	public_a_fun_link_n("er", echo_error, 0);
	public_a_var_ref("rcount", &r_count, 'd');
#endif
	//timer
	lily_timers.timer_works = new_list(sizeof(Tasks_def), 4);
	lily_timers.peroids = new_list(sizeof(unsigned int), 4);
	lily_timers.ticks = new_list(sizeof(unsigned int), 4);
	if (lily_timers.timer_works == NULL || lily_timers.peroids == NULL || lily_timers.ticks == NULL)
	{
		lily_out("lily init failed!");
		return;
	}
}

void lily_delay(unsigned int ms)
{
	for (unsigned int i = 0; i < ms; i++)
		for (int j = 0; j < 10000; j++)
			;
}
void lily_out_queue(const char *msg)
{
}
unsigned int lily_built_in_millis()
{
	return 0;
}
