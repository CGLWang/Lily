

#include "Lily.h"
#include "Lily_basic.h"
#include "Lily_tasks.h"
#include "Lily_ui.h"
#include "Lily_boardcast.h"
#include "shell.h"

const char* lily_error_msg=NULL;
//typedef struct 
//{
//	byte head[2];
//	byte length;
//}BaseFrame;
//typedef struct {
//	BaseFrame head;
//	unsigned short distance, strength, temperature;
//	byte sum_check;
//};
int r_count = 0;
void lily_in(char c)
{
	r_count++;
	if (c == ';' || c == '\n')
		c = '\0';
	if (c == '\0')
	{
		addTask_(excute_cmd);
	}
		
	if (c == '\b')
	{
		ri--;
	}
	else 
	rx[ri++] = c;
#ifdef in_debug
	if (ri == 255)
	{
		lily_out("255 end");
	}
	if (ri == hi)
	{
		lily_out("cmd queue over flow");
	}
#endif
}


float test_fields=12;
int time_cmd(int n, char** a)
{
	float t = lily_millis();
	sprintf(tx, "%f\n", t);
	lily_out(tx);
	return 0;
}

void echo_error()
{
	lily_out(lily_error_msg);
	lily_out("\n");
}
void lily_init()
{
	tasks[0] = task_mointor;
	lily_ui.cmds = new_list(sizeof(Cmd_def), 10);
	lily_ui.fields = new_list(sizeof(Field_def), 10);
	lily_ui.funs = new_list(sizeof(Fun_def), 4);
	//lily_ui.hijacks = new_list(sizeof(hijack), 4);
	lily_ui.para_updated = false;
	lily_ui.hijacked = false;
	if (lily_ui.cmds == NULL || lily_ui.fields == NULL|| lily_ui.funs ==NULL)
	{
		lily_out("lily init failed!");
		return;
	}
	Cmd_def new_cmd;
	new_cmd.name = (char*)"help";
	new_cmd.annotation = (char*)"help [cmd1]:info of cmd and fields";
	new_cmd.todo = help;
	new_cmd.id = 0;
	public_cmd(new_cmd);

	//Cmd_def new_cmd;
	new_cmd.name = (char*)"pass";
	new_cmd.annotation = (char*)"pass [n]:skip cmds";
	new_cmd.todo = pass;
	new_cmd.id = 1;
	public_cmd(new_cmd);

	new_cmd.name = (char*)"delete";
	new_cmd.annotation = (char*)"delete field";
	new_cmd.todo = delete_field;
	new_cmd.id = 2;
	public_cmd(new_cmd);

	new_cmd.name = (char*)"whos";
	new_cmd.annotation = (char*)"[fields]:list fields";
	new_cmd.todo = whos;
	new_cmd.id = 3;
	public_cmd(new_cmd);
	
	new_cmd.name = (char*)"sys";
	new_cmd.annotation = (char*)":sysInfo";
	new_cmd.todo = system;
	new_cmd.id = 4;
	public_cmd(new_cmd);

	new_cmd.name = (char*)"cal";
	new_cmd.annotation = (char*)"caculateExpression";
	new_cmd.todo = shell_cal_exp_cmd;
	new_cmd.id = 5;
	public_cmd(new_cmd);

	new_cmd.name = (char*)"delay";
	new_cmd.annotation = (char*)"delayMs";
	new_cmd.todo = delay_cmd;
	new_cmd.id = 6;
	public_cmd(new_cmd);

	new_cmd.name = (char*)"for";
	new_cmd.annotation = (char*)"forLoops";
	new_cmd.todo = cmd_for_start;
	new_cmd.id = 7;
	public_cmd(new_cmd);
	//hijackor_wait_key
	new_cmd.name = (char*)"wk";
	new_cmd.annotation = (char*)"waitKey";
	new_cmd.todo = hijackor_wait_key;
	new_cmd.id = 8;
	public_cmd(new_cmd);

	public_a_cmd_link("time", time_cmd);
	public_a_fun_link_n("er", echo_error, 0);
	public_a_field_ref("test_field", &test_fields);
	Field_def fed = { (char*)"rcount",(char*)"_rxCount",(float*)&r_count,'d' };
	public_field(fed);
	fed.name = (char*)"tx";
	fed.annotation = "_builtIn";
	fed.type = 's';
	fed.ref = tx;
	public_field(fed);
	//public_field(fed);
	//public_a_new_string_field((char*)"test",(char*) "test");
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



