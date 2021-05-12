#include "Lily_ui.h"
#include "Lily_boardcast.h"
#include "Lily_tasks.h"
#include "shell.h"
#ifdef in_PC
#include <iostream>
using namespace std;
extern int new_count;
#endif // in_PC
char rx[256];
unsigned char ri, hi;
char tx[64];
unsigned char ti;

Lily_cmds_def lily_ui; //a value is copied, so a cmd or field can be add in a stack call

#define lily_do_stack_len 4
//about the return code of a hijackor:
//the lower byte: keep hijack
// higher byte: notify

static Arg_Tasks_def ui_stack[lily_do_stack_len];
char stack_top = 0;
//notify stack
//about this hook
// a cmd return 0 will defaultly notify
// hijackor return 0 will not notify defaultly, and release hijack
Tasks_def cmd_done_listener[lily_do_stack_len];
char cmd_done_listener_stack_top = 0;

// this fun also change the hi index
// assert that the cmd is less than 63b in length
int get_cmd_from_rx()
{
#define len 64
	static char cmd[len];
	char i = 0, c;
	c = rx[hi];
	for (; rx[hi] != '\0' && i < len; i++, hi++)
	{
		c = rx[hi];

		if (c >= 'A' && c <= 'Z')
			c += 0x20;
		cmd[i] = c;
	}
#ifdef in_debug
	if (i == len)
		lily_out("cmd too long");
	if (hi + 1 != ri) //read done
		lily_out("cmd queue error");
#endif // in_debug

	cmd[i] = '\0';
	hi++; // jump to the next cmd
	//addTaskArg((TasksArg_def)lily_do, cmd);
	lily_do(cmd);
	return 0;

#undef len
}

float open_time = 0;
//int excute_cmd2()
//{
//	float t = lily_millis();
//	if (t < open_time)
//		return 1; // go again
//
//	char* rx;
//	if (lily_hooks_cmd_hook == NULL)
//		rx = get_cmd_from_rx();
//	else
//	{
//		rx = lily_hooks_cmd_hook;
//		lily_hooks_cmd_hook = NULL;
//	}
//
//
//	// haijcked ?
//	if (stack_top)
//	{
//		//Li_List list = str_split(rx, ' ');
//		//char return_code = ui_stack[stack_top](list->count, (char**)(list->content));
//		char return_code = ui_stack[stack_top](1, &rx);
//		if (return_code == 0)// 0:done, -1:error
//			stack_top--;
//		if (!stack_top)
//		{
//			lily_ui.hijacked = false;
//		}
//		//delete_list(list);
//		return 0;
//	}
//
//	while (*rx == ' ')
//		rx++;
//	if (*rx == '\0')
//	{
//		if (lily_hooks_cmd_done != NULL)
//			addTask_(lily_hooks_cmd_done);
//		return -1;
//	}
//
//	int code;
//	typedef int(*shell_do_def)(char*);
//	static shell_do_def shell_do[] = { shell_do_cmd,shell_do_fun,shell_do_cal,shell_do_fields,shell_do_notFound };
//	for (int i = 0; i < 5; i++)
//	{
//		code = (shell_do[i])(rx);
//		if (code == 0)continue;
//		break;
//	}
//	if (code < 0)
//	{
//		sprintf(tx, "error(%d)\n", code);
//		lily_out(tx);
//	}
//	if (lily_hooks_cmd_done != NULL)
//		addTask_(lily_hooks_cmd_done);
//	return 0;
//}

int lily_do(char *rx)
{
	float t = lily_millis();
	if (t < open_time)
		return 1; // go again
#ifdef in_debug
	if (rx == NULL)
	{
		lily_out("shell do error");
		li_error("null ptr", -44);
	}
#endif // in_debug

	// if in haijcked, let hijackor to deal with the message
	if (stack_top)
	{
		//the lower byte: keep hijack
		// higher byte: notify
		int return_code = ui_stack[stack_top](1, &rx);
		if (!(return_code & KeepHijack))
		{
			if (stack_top)
				stack_top--;
		}
		if (!(return_code & Lily_cmd_do_not_notify))
		{
			if (have_listener)
				addTask_(current_listener);
		}
		if (!stack_top)
		{
			lily_ui.hijacked = false;
		}
		return 0;
	}

	while (*rx == ' ')
		rx++;
	void (*lily_out_back)(const char *) = NULL;
	if (*rx == '|')
	{
		lily_out_back = lily_out;
		lily_out = lily_out_queue;
		rx++;
	}
	if (*rx == '\0') // work done, nothing did
	{
		//if (lily_hooks_cmd_done != NULL)
		//	addTask_(lily_hooks_cmd_done);
		if (have_listener)
			addTask_(current_listener);

		if (lily_out_back)
			lily_out = lily_out_back;
		return -1;
	}

	int code;
	typedef int (*shell_do_def)(char *);
	static shell_do_def shell_do[] = {shell_do_cmd, shell_do_fun, shell_do_cal, shell_do_var, shell_do_notFound};
	char rx_back[32];
	for (int i = 0; i < 5; i++)
	{
		strcpy(rx_back, rx);
		code = (shell_do[i])(rx_back);
		if (code == 0)
			continue;
		break;
	}
	//code!=0
	// code > 0 and code&Lily_cmd_do_not_notify: not notify
	// code <0 : error
	if (code < 0)
	{
		lily_out(rx);
		sprintf(tx, "\a\nerror(%d):", code);
		lily_out(tx);
		if(lily_error_msg!=NULL)
			lily_out((char *)lily_error_msg);
		lily_out("\n");
	}
	//if (lily_hooks_cmd_done != NULL)
	//	addTask_(lily_hooks_cmd_done);
	if (have_listener)
	{
		if (code < 0 || !(code & Lily_cmd_do_not_notify))
		{
			if (current_listener == NULL)
			{
				lily_out("error null ptr");
			}
			addTask_(current_listener);
		}
	}
	if (lily_out_back)
		lily_out = lily_out_back;
	return 0;
}

//添加劫持程序,call_back返回0释放劫持
void add_hijack(Arg_Tasks_def call_back)
{
	ui_stack[++stack_top] = call_back;
	lily_ui.hijacked = true;
	if (stack_top >= lily_do_stack_len)
	{
		lily_out(">>ui stack overflow!");
		stack_top = 0;
		lily_ui.hijacked = false;
	}
}

void add_listener(Tasks_def f)
{
	cmd_done_listener[cmd_done_listener_stack_top++] = f;
	if (cmd_done_listener_stack_top >= lily_do_stack_len)
	{
		lily_out(">>listener stack overflow!");
		cmd_done_listener_stack_top = 0;
	}
	//cmd_done_listener_stack_top %= lily_do_stack_len;
}
int search_cmd_in_Lily_ui(char *item)
{
	int i, n;
	n = lily_ui.cmds->count;
	Cmd cmds = (Cmd)(lily_ui.cmds->content);
	for (i = 0; i < n; i++)
	{
		if (!str_equal(item, cmds[i].name))
			continue;
		return i;
	}
	return -1;
}
// int search_field_in_Lily_ui(char *item)
// {
// 	int i, n;
// 	n = lily_ui.fields->count;
// 	Field fields = (Field)(lily_ui.fields->content);
// 	for (i = 0; i < n; i++)
// 	{
// 		if (!str_equal(item, fields[i].name))
// 			continue;
// 		return i;
// 	}
// 	return -1;
// }
int search_fun_in_Lily_ui(char *item)
{
	int i, n;
	n = lily_ui.funs->count;
	Fun funs = li_funs;
	for (i = 0; i < n; i++)
	{
		if (str_equal(item, funs[i].name))
			return i;
	}
	return -1;
}

int help(int nargin, char **arg)
{
	//lily_cout(">>help:\n");
	Cmd p = li_cmds;
	for (int now = 1; now < nargin; now++)
	{
		char *item = NULL;
		item = arg[now];
		int index = search_cmd_in_Lily_ui(item);
		if (index >= 0)
		{
			sprintf(tx, ">>%s: id:%d, %s\n", p[index].name, p[index].id, p[index].annotation);
			lily_out(tx);
			continue;
		}
		index = search_fun_in_Lily_ui(item);
		if (index >= 0)
		{
			Fun r = li_funs;
			sprintf(tx, ">>%s:%d\n", r[index].name, r[index].narg);
			lily_out(tx);
			continue;
		}
		// Field q = li_fields; // (Field_def*)(lily_ui.fields->content);
		// Var q = li_vars;
		// index = search_field_in_Lily_ui(item);
		// if (index >= 0)
		// {
		// 	char s[] = "f";
		// 	sprintf(tx, ">>%s:%s, type:", q[index].name, q[index].annotation);
		// 	lily_out(tx);
		// 	s[0] = q[index].type;
		// 	lily_out(s);
		// 	lily_out("\n");
		// 	//sprintf(tx, ">>%s: id:%d, %s\n", p[index].name, p[index].id, p[index].annotation);
		// 	//lily_cout(tx);
		// 	continue;
		// }
		lily_out(">>not found help of:");
		lily_out(item);
		lily_out("\n");
		return -1;
	}

	if (nargin > 1)
		return 0;
	lily_out(">cmds:\n");
	for (int i = 0; i < lily_ui.cmds->count; i++)
	{
		sprintf(tx, "%s\n", p[i].name);
		lily_out(tx);
	}
	Fun r = li_funs;
	lily_out(">funs:\n");
	for (int i = 0; i < lily_ui.funs->count; i++)
	{
		sprintf(tx, "%s\n", r[i].name);
		lily_out(tx);
	}

	// Field_def *q = (Field_def *)(lily_ui.fields->content);
	// lily_out(">fields:\n");
	// for (int i = 0; i < lily_ui.fields->count; i++)
	// {
	// 	sprintf(tx, "%s\n", q[i].name);
	// 	lily_out(tx);
	// }
	return 0;
}

int pass(int n, char **arg)
{
	int nn = 1;
	if (n > 1)
	{
		nn = atoi(arg[1]);
	}

	for (int i = 0; i < nn; i++)
	{
		while (rx[hi] != '\0')
		{
			if (hi == ri)
				break;
			hi++;
		}
		if (hi == ri)
			break;
		hi++;
	}
	return 0;
}

int delete_field(int n, char **arg)
{
	char user_do = 1;
	int i = 1;
	if (n == 1)
	{
		i = 0;
		user_do = 0;
	}
	// if (n < 2 )return -1;
	for (; i < n; i++)
	{
		// int index = search_field_in_Lily_ui(arg[i]);
		int index = search_var_in_Lily_ui(arg[i]);
		if (index < 0)
		{
			lily_out(arg[1]);
			lily_out(" not found\n");
			return -1;
		}
		Var var = li_vars + index;
		if (var->isb)
			return -3;
		// Field_def *q = (Field_def *)(lily_ui.fields->content);
		// if (q[index].annotation[0] == '_')
		// 	return -3; //protected
		// free(q[index].name);
		free(cast(var->name, void *));
#ifdef in_PC
		new_count--;
		cout << "#" << new_count << "#";
#endif // in_PC
		if (var->isa)
		{
			free(var->ref);
#ifdef in_PC
			new_count--;
			cout << "#" << new_count << "#";
#endif // in_PC
		}
		// free(q[index].ref);
// #ifdef in_PC
// 		new_count--;
// 		cout << "#" << new_count << "#";
// #endif // in_PC                          \
	//if (q[index].annotation[0] != '_') \
	//	free(q[index].name);              \
	//if (q[index].type == 's')          \
	//delete_li_string((Li_String)(q[index].ref));

		list_remove_at(lily_ui.vars, index);
	}
	if (user_do)
		lily_out("done\n");
	return 0;
}

int whos(int nargin, char **arg)
{
	bool hit = false;
	for (int now = 1; now < nargin; now++)
	{
		hit = false;
		char *item = NULL;
		item = arg[now];

		//Field_def *q = (Field_def *)(lily_ui.fields->content);
		Var q = li_vars;
		int n = lily_ui.vars->count;
		for (int i = 0; i < n; i++)
		{
			if (str_equal(item, q[i].name))
			{
				//char s[] = "f";
				if (q[i].isa)
					sprintf(tx, ">>%s type:%c%c*%d\n", q[i].name, q[i].isb ? '_' : ' ', q[i].type, q[i].len);
				else
					sprintf(tx, ">>%s type:%c%c\n", q[i].name, q[i].isb ? '_' : ' ', q[i].type);
				lily_out(tx);
				/*s[0] = q[i].type;
				lily_out(s);
				lily_out("\n");*/
				hit = true;
				break;
			}
		}
		if (hit)
			continue;
		lily_out(">>not found help of:");
		lily_out(item);
		lily_out("\n");
		return -1;
	}

	if (nargin > 1)
		return 0;

	Var q = li_vars;
	int n = lily_ui.vars->count;

	for (int i = 0; i < n; i++)
	{
		sprintf(tx, "%s:", q[i].name);
		lily_out(tx);
		var_to_string(tx, q + i, 0);

		/*switch (q[i].type)
		{
		case 's':
			sprintf(tx, "%s\n", ((char *)(q[i].ref)));
			break;
		case 'S':
			sprintf(tx, "%s\n", ((Li_String)q[i].ref)->str);
			break;
		case 'f':
			sprintf(tx, "%f\n", *((float *)q[i].ref));
			break;
		case 'd':
			sprintf(tx, "%d\n", *((int *)q[i].ref));
			break;
		default:
			break;
		}*/
		lily_out(tx);
		lily_out("\n");
	}
	return 0;
}

int system(int n, char **arg)
{
	//task
	lily_out("tasks:");
	int_to_string(hasTask, tx);
	lily_out(tx);
	lily_out("-");
	int_to_string((rear + Tasks_LEN - front) % Tasks_LEN, tx);
	lily_out(tx);
	lily_out("\n");
	return 0;
}

int delay_cmd(int n, char **arg)
{
	if (n == 1)
	{
		open_time = lily_millis() + 1000.0f;
		return 1;
	}
	else if (n == 2)
	{
		float t = atof(arg[1]);
		if (t <= 0.0f)
			return -1;
		open_time = lily_millis() + t;
		return 1;
	}
	return -2;
}
//creat and public a cmd with defaults
//just provide a name and fun ptr of this cmd
int public_a_cmd_link(const char *name, Arg_Tasks_def link)
{
	Cmd_def cmd;
	cmd.name = (char *)name;
	cmd.annotation = (char *)"none";
	cmd.todo = link;
	cmd.id = lily_ui.cmds->count;
	return public_cmd(cmd);
}
//creat and public a cmd with defaults
// note:
// no extra memory, save nothing, used in built in var
//e.g.: public_a_field_ref("test_field", &test_fields);
//return index
// int public_a_field_ref(const char *name, void *link)
// {
// 	Field_def fed;
// 	fed.name = (char *)name;
// 	fed.annotation = (char *)"_builtIn"; // strats with '_' means this field is protected
// 	fed.ref = link;
// 	fed.type = 'f';
// 	return public_field(fed);
// }
// int public_a_field_ref_type(const char *name, void *link, char type)
// {
// 	Field_def fed;
// 	fed.name = (char *)name;
// 	fed.annotation = "_builtIn"; // strats with '_' means this field is protected
// 	fed.ref = link;
// 	fed.type = type;
// 	return public_field(fed);
// }
int public_a_fun_link(const char *name, void *link)
{
	Fun_def fun = {(char *)name, link, 255, 'f'};
	return public_fun(fun);
}
int public_a_fun_link_n(const char *name, void *link, char n)
{
	Fun_def fun = {(char *)name, link, n, 'f'};
	return public_fun(fun);
}
int public_a_fun_link_int(const char *name, void *link, char n)
{
	Fun_def fun = {(char *)name, link, n, 'd'};
	return public_fun(fun);
}
//拼接由str_split分割的字符串
int joint_args(int n, char **args)
{
	int i;
	char *p;
	n = n - 1;
	int len = 0;
	for (i = 0; i < n; i++)
	{
		int nn = strlen(args[i]);
		p = args[i] + nn;
		len += nn;
		*p = ' ';
	}
	return len;
}

// creat a new li_string to save [s]
// int public_a_new_string_field(char *name, char *s)
// {
// 	Field_def fed;
// 	fed.annotation = (char *)"UserField";
// 	fed.name = (char *)malloc(strlen(name) + 1);
// #ifdef in_PC
// 	new_count++;
// 	cout << "#" << new_count << "#";
// #endif // in_PC
// 	if (fed.name == NULL)
// 		return -1;
// 	strcpy(fed.name, name);
// 	//Li_String li = new_li_string_by(s);
// 	//if (li == NULL)return -1;
// 	void *p = new_string_by(s);
// 	if (p == NULL)
// 		return -2;
// 	fed.ref = p;
// 	fed.type = 's';
// 	return public_field(fed);
// }
// when use this function to public a field
// note:
// it will apply a new memory to save [name] and [val], so when delete a field, free that first
// this publication is only support a int and float field, for a string field, using [public_a_new_string_field]
// return the index of new field, if it<0, error occured
// int public_a_new_field(char *name, char type, float val)
// {
// 	Field_def fed;
// 	fed.annotation = (char *)"UserField";
// 	fed.name = (char *)malloc(strlen(name) + 1);
// #ifdef in_PC
// 	new_count++;
// 	cout << "#" << new_count << "#";
// #endif // in_PC
// 	if (fed.name == NULL)
// 		return -1;
// 	strcpy(fed.name, name);
// 	void *li = NULL;
// 	switch (type)
// 	{
// 	case 'f':
// 		li = malloc(sizeof(float));
// #ifdef in_PC
// 		new_count++;
// 		cout << "#" << new_count << "#";
// #endif // in_PC
// 		if (li == NULL)
// 			return -1;
// 		*((float *)li) = val;
// 		break;
// 	case 'd':
// 		li = malloc(sizeof(int));
// #ifdef in_PC
// 		new_count++;
// 		cout << "#" << new_count << "#";
// #endif // in_PC
// 		if (li == NULL)
// 			return -1;
// 		*((int *)li) = (int)val;
// 		break;
// 	default:
// 		break;
// 	}
// 	fed.ref = li;
// 	fed.type = type;
// 	return public_field(fed);
// }

// wait key for n times
int hijackor_wait_key(int n, char **s)
{
	static int in_hijack = 0;
	// static Tasks_def hooks_copy = NULL;
	if (in_hijack == 0)
	{
		if (n == 2)
		{
			in_hijack = atoi(s[1]);
		}
		if (in_hijack < 1)
			in_hijack = 1;
		add_hijack(hijackor_wait_key);
		lily_out("K>");
		return Lily_cmd_do_not_notify; // first enter, not notify listener
	}
	else
	{
		in_hijack--;
		lily_out(s[0]);
		if (in_hijack)
		{
			lily_out("\nK>");
			return KeepHijack | Lily_cmd_do_not_notify; //still keep hijack,but not notify
		}
		lily_out("<\n");
		// lily_hooks_cmd_done = hooks_copy;
		// if (hooks_copy != NULL)
		// 	addTask_(hooks_copy);

		in_hijack = 0;
		return 0; //free hijack, work done, notify
	}
}

int cmd_echo(int n, char **arg)
{
	if (n < 1)
	{
		li_error("bad arg", -1);
	}

	int now = 1;
	if (strcmp("echo", arg[0]) != 0)
	{
		now = 0;
	}
	int index;
	//	Field fed = li_fields;
	for (; now < n; now++)
	{
		// index = search_field_in_Lily_ui(arg[now]);
		// if (index < 0)
		// {
		lily_out(arg[now]);
		// }
		// else
		// {
		// 	field_to_string(tx, li_fields + index, 0);
		// 	lily_out(tx);
		// }

		if (now < n - 1)
			lily_out(", ");
		else
			lily_out("\n");
	}

	return 0;
}

int sleep_timer()
{
	// remove_timer(sleep_timer);
	if (stack_top > 0)
		stack_top--;
	if (have_listener)
	{
		addTask_(current_listener); //notify listener
	}
	return 0;
}
int hijack_sleep(int n, char **arg)
{
	return KeepHijack | Lily_cmd_do_not_notify;
}
int cmd_hijack_sleep(int n, char **arg)
{
	float sleep_time = 1.0f;
	if (n < 1)
	{
		li_error("bad arg", -1);
	}
	if (n >= 2)
	{
		sleep_time = value_from_string_or_var(arg[1]);
		if (isnan(sleep_time))
		{
			li_error("field miss", -2);
		}

		// if (str_is_numeric(arg[1]))
		// {
		// 	sleep_time = atoi(arg[1]);
		// }
		// else
		// {
		// 	// int index = search_field_in_Lily_ui(arg[1]);
		// 	int index = search_var_in_Lily_ui(arg[1]);
		// 	if (index < 0)
		// 	{
		// 		li_error("field miss", -2);
		// 	}
		// 	Field fed = li_fields + index;
		// 	switch (fed->type)
		// 	{
		// 	case 'f':
		// 		sleep_time = *(float *)(fed->ref);
		// 		break;
		// 	case 'd':
		// 		sleep_time = *(int *)(fed->ref);
		// 		break;
		// 	default:
		// 		li_error("field type not match", -3);
		// 		//				break;
		// 	}
		// }
	}

	if (sleep_time < 0.026f)
		sleep_time = 0.026f;
	add_hijack(hijack_sleep);
	// public_a_timer(sleep_timer,Second(sleep_time));
	create_or_change_quick_timer_count(sleep_timer, Second(sleep_time));
	return Lily_cmd_do_not_notify;
}

//Var operations

// add a static var
// name: a pointer to a static mamory
// link: a pointer to a variable
// type: 'fdc' for a single var, 'FDC' for a array
int public_a_var_ref(const char *name, void *link, char type)
{
	Var_def var;
	var.name = name;
	var.ref = link;
	var.len = 0;
	var.type = toLower(type);
	var.isa = isUpper(type);
	var.isb = 1;
	var.isp = 1;
	return public_var(var);
}

// name: using malloc to apply a new memory
// type: 'fdc'
// value: a word of value
int public_a_new_var(char *name, char type, void *val)
{
	Var_def var;
	var.len = 0;
	var.isb = 0; //UserField
	var.isa = 0; //sigle value
	var.isp = 0; // not pointer

	var.name = (char *)malloc(strlen(name) + 1);
#ifdef in_PC
	new_count++;
	cout << "#" << new_count << "#";
#endif // in_PC
	if (var.name == NULL)
		return -1;
	strcpy((char *)(var.name), name);

	//value
	if (!isLower(type))
		return -3;
	var.ref = val;
	var.type = type;
	return public_var(var);
}
// type: 'FDC'
// val: a string if in type 'c', or a initial value to the array
// !note: when in type 'C', len = strlen(val)+1
// len: length to the new array
int public_new_vars_array(char *name, char type, void *val, int len)
{
	Var_def var;
	var.isb = 0; //UserField
	var.isa = 1; //sigle value
	var.isp = 1; // not pointer

	var.name = (char *)malloc(strlen(name) + 1);
#ifdef in_PC
	new_count++;
	cout << "#" << new_count << "#";
#endif // in_PC
	if (var.name == NULL)
		return -1;
	strcpy((char *)(var.name), name);

	//value
	void *li = NULL;
	if (islower(type))
		type = toUpper(type);
	if (type == 'C')
	{
		li = malloc(len);
#ifdef in_PC
		new_count++;
		cout << "#" << new_count << "#";
#endif // in_PC
	}
	else if (type == 'F' || type == 'D')
	{
		li = malloc(len * 4);
#ifdef in_PC
		new_count++;
		cout << "#" << new_count << "#";
#endif // in_PC
	}
	else
		return -23;
	if (li == NULL)
		return -33; // malloc error

	// initial values
	if (type == 'C')
	{
		strcpy((char *)li, (char *)val);
	}
	else if (type == 'F' || type == 'D')
	{
		int i;
		for (i = len - 1; i; i--)
		{
			((int *)li)[i] = (int)val;
		}
		*((int *)li) = (int)val;
	}
	else
		return -43;

	var.ref = li;
	var.type = toLower(type);
	var.len = len;
	return public_var(var);
}
// at: the index of value, only valid when var.isa=1
// return a word of value
void *get_value_of_var(Var var, int at)
{
	void *val;
	switch (var->type)
	{
	case 'f':
	case 'd':
		if (var->isa) // a array, assert var.isp==1
		{
			val = (void *)(((int *)(var->ref))[at]);
		}
		else // not a array, a sigle value, [at] is invalid
		{
			val = var->isp ? (void *)(*((int *)(var->ref))) : var->ref;
			// if (var->isp)
			// {
			// 	val = (void *)(*((int *)(var->ref)));
			// }
			// else
			// 	val = var->ref;
		}
		break;
	case 'c':
		if (var->isa) // a array, assert var.isp==1
		{
			val = (void *)(((char *)(var->ref))[at]);
		}
		else // not a array, a sigle value, [at] is invalid
		{
			val = var->isp ? (void *)(*((char *)(var->ref))) : var->ref;
			// if (var->isp)
			// {
			// 	val = (void *)(*((int *)(var->ref)));
			// }
			// else
			// 	val = var->ref;
		}
		break;

	default:
		return NULL;
	}
	return val;
}
//return -1 or others if in error
int set_value_of_var(Var var, int at, void *val)
{
	switch (var->type)
	{
	case 'f':
	case 'd':
		if (var->isa) // a array, assert var.isp==1
		{
			if (!var->isb && at >= var->len)
				return -4; //index out
			(((int *)(var->ref))[at]) = void_to_int(val);
		}
		else // not a array, a sigle value, [at] is invalid
		{
			if (at)
				return -2;
			if (var->isp)
			{
				*((int *)(var->ref)) = void_to_int(val);
			}
			else
				var->ref = val;
		}
		break;
	case 'c':
		if (var->isa) // a array, assert var.isp==1
		{
			if (!var->isb && at >= var->len)
				return -4; //index out
			(((char *)(var->ref))[at]) = void_to_int(val);
		}
		else // not a array, a sigle value, [at] is invalid
		{
			if (at)
				return -3;
			if (var->isp)
			{
				*((char *)(var->ref)) = void_to_int(val);
			}
			else
				var->ref = val;
		}
		break;

	default:
		return -1;
	}
	return 0;
}
//free the memory of a var
// nothing to do if var is builtin
// after delete a var, remove it from lily_ui.vars
void delete_a_var(Var var)
{
	if (var->isb)
		return;
	free((void *)(var->name));
#ifdef in_PC
	new_count--;
	cout << "#" << new_count << "#";
#endif // in_PC
	if (var->isa)
	{
		free(var->ref);
#ifdef in_PC
		new_count--;
		cout << "#" << new_count << "#";
#endif // in_PC
	}
}

int search_var_in_Lily_ui(char *item)
{
	int i, n;
	n = lily_ui.vars->count;
	Var fields = li_vars;
	for (i = 0; i < n; i++)
	{
		if (!str_equal(item, fields[i].name))
			continue;
		return i;
	}
	return -1;
}

// type conversion:
// 1.when assign an array var with a non-array var
//   the first element will be changed, while others stay constant
// 2. when assign a non-array var with an array var
//	  only the first element of the array will used
// 3. others conversion is just like numeric conversion
