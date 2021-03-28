#include "Lily_ui.h"
#include "Lily_boardcast.h"
#include "Lily_tasks.h"
#include "shell.h"
char rx[256];
unsigned char ri, hi;
char tx[64];
unsigned char ti;


Lily_cmds_def lily_ui;//a value is copied, so a cmd or field can be add in a stack call

static Arg_Tasks_def ui_stack[4];
static char stack_top = 0;

Tasks_def lily_hooks_cmd_done = NULL;
char* lily_hooks_cmd_hook=NULL;

// this fun also change the hi index
// assert that the cmd is less than 31b in length
char* get_cmd_from_rx()
{
#define len 64
	static char cmd[len];
	char i=0, c;
	c = rx[hi];
	//if (c == frame_head_0)//a frame
	//{
	//	cmd[i++] = c;
	//	hi++;
	//	cmd[i++] = rx[hi++];
	//	char length = rx[hi++];
	//	cmd[i++] = length;
	//	length++;
	//	while (length)
	//	{
	//		cmd[i++] = rx[hi++];
	//		length--;
	//	}
	//	return cmd;
	//}
	for (; rx[hi] != '\0'&&i<len; i++, hi++)
	{
		c = rx[hi];

		if (c >= 'A' && c <= 'Z')
			c += 0x20;
		cmd[i] = c;
	}
#ifdef in_debug
	if (i == len)
		lily_out("cmd too long");
	if (hi+1 != ri)//read done
		lily_out("cmd queue error");
#endif // in_debug

	cmd[i] = '\0';
	hi++; // jump to the next cmd
	return cmd;
#undef len
}

//char assign_send_s(char code,char**send_s, bool* send)
//{
//	if (code &( Cmd_done|Cmd_false))
//	{
//		*send = true;
//		if (code & Cmd_done)
//			*send_s = (char*)"done";
//		else
//			*send_s = (char*)"false input";
//	}
//	if (code & Cmd_send)
//		*send = true;
//	return code & Cmd_end;
//
//}

float open_time = 0;
int excute_cmd()
{
	float t = lily_millis();
	if (t < open_time)
		return 1; // go again

	char* rx;
	if (lily_hooks_cmd_hook == NULL)
		rx = get_cmd_from_rx();
	else
	{
		rx = lily_hooks_cmd_hook;
		lily_hooks_cmd_hook = NULL;
	}

	
	// haijcked ?
	if (stack_top)
	{
		//Li_List list = str_split(rx, ' ');
		//char return_code = ui_stack[stack_top](list->count, (char**)(list->content));
		char return_code = ui_stack[stack_top](1, &rx);
		if (return_code == 0)// 0:done, -1:error
			stack_top--;
		if (!stack_top)
		{
			lily_ui.hijacked = false;
		}
		//delete_list(list);
		return 0;
	}

	while (*rx == ' ')rx++;
	if (*rx == '\0')
	{
		if (lily_hooks_cmd_done != NULL)
			addTask_(lily_hooks_cmd_done);
		return -1;
	}

	int code;
	typedef int(*shell_do_def)(char*);
	static shell_do_def shell_do[] = { shell_do_cmd,shell_do_fun,shell_do_cal,shell_do_fields,shell_do_notFound };
	for (int i = 0; i < 5; i++)
	{
		code = (shell_do[i])(rx);
		if (code == 0)continue;
		break;
	}
	if (code < 0)
	{
		sprintf(tx, "error(%d)\n", code);
		lily_out(tx);
	}
	if (lily_hooks_cmd_done != NULL)
		addTask_(lily_hooks_cmd_done);
	return 0;
}
//添加劫持程序,call_back返回0释放劫持
void add_hijack(Arg_Tasks_def call_back)
{
	ui_stack[++stack_top] = call_back;
	lily_ui.hijacked = true;
	if (stack_top >= stack_len)
	{
		lily_out(">>ui stack overflow!");
		stack_top = 0;
		lily_ui.hijacked = false;
	}
}
int search_cmd_in_Lily_ui(char* item)
{
	int i, n;
	n = lily_ui.cmds->count;
	Cmd cmds = (Cmd)(lily_ui.cmds->content);
	for (i = 0; i < n; i++)
	{
		if (!str_equal(item, cmds[i].name))continue;
		return i;
	}
	return -1;

}
int search_field_in_Lily_ui(char* item)
{
	int i,n;
	n = lily_ui.fields->count;
	Field fields = (Field)(lily_ui.fields->content);
	for (i = 0; i < n; i++)
	{
		if (!str_equal(item, fields[i].name))continue;
		return i;
	}
	return -1;
}
int search_fun_in_Lily_ui(char* item)
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

int help(int nargin, char** arg)
{
	//lily_cout(">>help:\n");
	Cmd p = li_cmds;
	bool hit = false;
	for(int now = 1;now<nargin;now++)
	{
		hit = false;
		char* item = NULL;
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

		//int n = lily_ui.cmds->count;
		//for (int i = 0; i < n; i++)
		//{
		//	if (str_equal(item, p[i].name))
		//	{
		//		sprintf(tx, ">>%s: id:%d, %s\n", p[i].name,p[i].id, p[i].annotation);
		//		lily_cout(tx);
		//		hit = true;
		//		break;
		//	}
		//}
		//if(hit)continue;
		Field q = li_fields;// (Field_def*)(lily_ui.fields->content);
		index = search_field_in_Lily_ui(item);
		if (index >= 0)
		{
			char s[] = "f";
			sprintf(tx, ">>%s:%s, type:", q[index].name, q[index].annotation);
			lily_out(tx);
			s[0] = q[index].type;
			lily_out(s);
			lily_out("\n");
			//sprintf(tx, ">>%s: id:%d, %s\n", p[index].name, p[index].id, p[index].annotation);
			//lily_cout(tx);
			continue;
		}
		/*n = lily_ui.fields->count;
		for (int i = 0; i < n; i++)
		{
			if (str_equal(item, q[i].name))
			{
				char s[] = "f";
				sprintf(tx, ">>%s:%s, type:", q[i].name, q[i].annotation);
				lily_cout(tx);
				s[0] = q[i].type;
				lily_cout(s);
				lily_cout("\n");
				hit = true;
				break;
			}
		}*/
		//if(hit)continue;
		lily_out(">>not found help of:");
		lily_out(item);
		lily_out("\n");
		return -1;
	}

	if (nargin>1)
		return 0;
	lily_out(">cmds:\n");
	for (int i = 0; i < lily_ui.cmds->count; i++)
	{
		sprintf(tx, "%d- %s\n", i, p[i].name);
		lily_out(tx);
	}
	Fun r = li_funs;
	lily_out(">funs:\n");
	for (int i = 0; i < lily_ui.funs->count; i++)
	{
		sprintf(tx, "%d- %s\n", i, r[i].name);
		lily_out(tx);
	}

	Field_def* q = (Field_def*)(lily_ui.fields->content);
	lily_out(">fields:\n");
	for (int i = 0; i < lily_ui.fields->count; i++)
	{
		sprintf(tx, "%d- %s\n", i, q[i].name);
		lily_out(tx);
	}
	return 0;
}

int pass(int n, char** arg)
{
	int nn=1;
	if (n > 1)
	{
		nn = atoi(arg[1]);
	}

	for (int i = 0; i < nn; i++)
	{
		while (rx[hi] != '\0')
		{
			if (hi == ri)break; 0;
			hi++;
		}
		if(hi==ri)break;
		hi++;
	}
	return 0;
}

int delete_field(int n, char** arg)
{
	
	if (n < 2 )return -1;
	for (int i = 1; i < n; i++)
	{
		int index = search_field_in_Lily_ui(arg[i]);
		if (index < 0)
		{
			lily_out(arg[1]);
			lily_out(" not found\n");
			return -1;
		}
		Field_def* q = (Field_def*)(lily_ui.fields->content);
		if (q[index].annotation[0] == '_')
			return -3;//protected
		free(q[index].name);
		free(q[index].ref);
		//if (q[index].annotation[0] != '_')
		//	free(q[index].name);
		//if (q[index].type == 's')
			//delete_li_string((Li_String)(q[index].ref));

		list_remove_at(lily_ui.fields, index);
	}
	lily_out("done\n");
	return 0;
}

int whos(int nargin, char** arg)
{
	bool hit = false;
	for (int now = 1; now < nargin; now++)
	{
		hit = false;
		char* item = NULL;
		item = arg[now];
		
		Field_def* q = (Field_def*)(lily_ui.fields->content);
		int n = lily_ui.fields->count;
		for (int i = 0; i < n; i++)
		{
			if (str_equal(item, q[i].name))
			{
				char s[] = "f";
				sprintf(tx, ">>%s:%s, type:", q[i].name, q[i].annotation);
				lily_out(tx);
				s[0] = q[i].type;
				lily_out(s);
				lily_out("\n");
				hit = true;
				break;
			}
		}
		if (hit)continue;
		lily_out(">>not found help of:");
		lily_out(item);
		lily_out("\n");
		return -1;
	}

	if (nargin > 1)
		return 0;
	
	Field_def* q = (Field_def*)(lily_ui.fields->content);
	for (int i = 0; i < lily_ui.fields->count; i++)
	{
		sprintf(tx, "%d- %s:", i, q[i].name);
		lily_out(tx);
		switch (q[i].type)
		{
		case 's':
			sprintf(tx, "%s\n", ((char*)(q[i].ref)));
			break;
		case 'S':
			sprintf(tx, "%s\n", ((Li_String)q[i].ref)->str);
			break;
		case 'f':
			sprintf(tx, "%f\n", *((float*)q[i].ref));
			break;
		case 'd':
			sprintf(tx, "%d\n", *((int*)q[i].ref));
			break;
		default:
			break;
		}
		lily_out(tx);
	}
	return 0;
}

int system(int n, char** arg)
{
	//task
	lily_out("tasks:");
	int_to_string(hasTask,tx);
	lily_out(tx);
	lily_out("-");
	int_to_string((rear + Tasks_LEN - front ) % Tasks_LEN, tx);
	lily_out(tx);
	lily_out("\n");
	return 0;
}

int delay_cmd(int n, char** arg)
{
	if (n == 1)
	{
		open_time = lily_millis() + 1000.0f;
		return 1;
	}
	else if (n == 2)
	{
		float t = atof(arg[1]);
		if (t <= 0.0f)return -1;
		open_time = lily_millis() + t;
		return 1;
	}
	return -2;
}
//creat and public a cmd with defaults
//just provide a name and fun ptr of this cmd
void public_a_cmd_link(const char* name, Arg_Tasks_def link)
{	
	Cmd_def cmd;
	cmd.name = (char*)name;
	cmd.annotation = (char*)"none";
	cmd.todo = link;
	cmd.id = lily_ui.cmds->count;
	public_cmd(cmd);
}
//creat and public a cmd with defaults
// note:
// no extra memory, save nothing, used in built in var
//e.g.: public_a_field_ref("test_field", &test_fields);
void public_a_field_ref(const char* name, void*link)
{
	Field_def fed;
	fed.name = (char*)name;
	fed.annotation =(char*) "_builtIn";// strats with '_' means this field is protected
	fed.ref = link;
	fed.type = 'f';
	public_field(fed);
}

void public_a_fun_link(const char* name, void* link)
{
	Fun_def fun = {(char*)name,link,255};
	public_fun(fun);
}
void public_a_fun_link_n(const char* name, void* link, char n)
{
	Fun_def fun = { (char*)name,link,n };
	public_fun(fun);
}
//拼接由str_split分割的字符串
int joint_args(int n,char** args)
{
	int i;
	char* p;
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
int public_a_new_string_field(char*name, char* s)
{
	Field_def fed;
	fed.annotation =(char*) "UserField";
	fed.name =(char*) malloc(strlen(name) + 1);
	if (fed.name == NULL)return -1;
	strcpy(fed.name, name);
	//Li_String li = new_li_string_by(s);
	//if (li == NULL)return -1;
	void* p = new_string_by(s);
	if (p == NULL)return -2;
	fed.ref = p;
	fed.type = 's';
	return public_field(fed);
}
// when use this function to public a field
// note:
// it will apply a new memory to save [name] and [val], so when delete a field, free that first
// this publication is only support a int and float field, for a string field, using [public_a_new_string_field]
// return the index of new field, if it<0, error occured
int public_a_new_field(char* name, char type, float val)
{
	Field_def fed;
	fed.annotation = (char*)"UserField";
	fed.name = (char*)malloc(strlen(name) + 1);
	if (fed.name == NULL)return -1;
	strcpy(fed.name, name);
	void* li = NULL;
	switch (type)
	{
	case 'f':
		li = malloc(sizeof(float));
		if (li == NULL)return -1;
		*((float*)li) = val;
		break;
	case 'd':
		li = malloc(sizeof(int));
		if (li == NULL)return -1;
		*((int*)li) = (int)val;
		break;
	default:
		break;
	}
	fed.ref = li;
	fed.type = type;
	return public_field(fed);
}

//like: for i = 1:10
// j=i+1
//end
//or 
//like: for i = 1:2:10
// j=i+1
//end
typedef char* str;
Li_List for_cmd_lines = NULL;
float start_val, end_val, step_val = 1.0f;
int now_at = 0;
int iter_field_index;
char need_delete_field = 0;
Field iter_field = NULL;
Li_String scopy = NULL;

int end_for();
int do_for_loop();
char* get_cmd_line_at(int at);



int hijackor_for_end(int n, char** args);

int cmd_for_start(int n, char* args[])
{
	if (n < 2)return -1;
	joint_args(n - 1, args + 1);
	auto items = str_split_by_str(args[1], (char*)" =:");
	
	args = (char**)(items->content);

	start_val = atof(args[1]);
	iter_field_index = search_field_in_Lily_ui(args[0]);
	
	need_delete_field = 0;
	if (iter_field_index < 0)
	{
		iter_field_index = public_a_new_field(args[0], 'f', start_val);
		need_delete_field = 1;
	}
	
	iter_field = li_fields + iter_field_index;

	if (items->count == 4)
	{
		step_val = atof(args[2]);
		end_val = atof(args[3]);
	}
	else if (items->count == 3)
	{
		step_val = 1.0f;
		end_val = atof(args[2]);
	}
	else
	{
		li_error("bad arguments", -1);
	}

	add_hijack(hijackor_for_end);
	for_cmd_lines = new_list(sizeof(str), 4);
	return 0;
}

int hijackor_for_end(int n, char** args)
{
	char* s = args[0];
	if (strcmp(s, "end") == 0)
	{
		lily_hooks_cmd_done = do_for_loop;
		now_at = 0;
		lily_hooks_cmd_hook = get_cmd_line_at(now_at);

		addTask_(excute_cmd);
		return 0;// free hijack
	}
	//save the cmd lines
	
	char* si = new_string_by(s);
	if (li_add(for_cmd_lines, si) < 0)
	{
		lily_out("error");
	}
	return 1;
}

int end_for()
{
	int i, n;
	n = for_cmd_lines->count;
	str* ss = (str*)(for_cmd_lines->content);
	for (i = 0; i < n; i++)
	{
		free(ss[i]);
	}
	free(for_cmd_lines);
	for_cmd_lines = NULL;
	delete_li_string(scopy);
	scopy = NULL;
	if (need_delete_field)
	{
		free(iter_field->name);
		free(iter_field->ref);
		iter_field = NULL;
		list_remove_at(lily_ui.fields, iter_field_index);
	}
	return 0;
}

int do_for_loop()
{
	now_at++;
	if (now_at >= for_cmd_lines->count)
	{
		now_at = 0;
		start_val += step_val;
		*(float*)(iter_field->ref) = start_val;
		if (start_val > end_val)//iteration end
		{
			end_for();
			lily_hooks_cmd_done = NULL;
			return 0;
		}
	}
	lily_hooks_cmd_hook = get_cmd_line_at(now_at);
	addTask_(excute_cmd);
	lily_hooks_cmd_done = do_for_loop;
	return 0;
}

char* get_cmd_line_at(int at)
{
	if (for_cmd_lines->count == 0)
	{
		return (char*)"";
	}
	str* ss = (str*)(for_cmd_lines->content);
	str s = ss[at];
	if (scopy == NULL)
	{
		scopy = new_li_string_by(s);
	}
	else
	{
		assign_li_string(scopy, s);
	}
	return scopy->str;
	
}

int hijackor_wait_key(int n, char** s)
{
	static char in_hijack = 0;
	static Tasks_def hooks_copy = NULL;
	if (in_hijack==0)
	{
		if (n == 2)
		{
			in_hijack = atoi(s[1]);
		}
		if(in_hijack<1)
			in_hijack = 1;
		hooks_copy = lily_hooks_cmd_done;
		lily_hooks_cmd_done = NULL;
		add_hijack(hijackor_wait_key);
		lily_out("H>");
		return 0;
	}
	else
	{
		in_hijack--;
		if (in_hijack)
		{
			lily_out("H>");
		}
		if (hooks_copy != NULL)
			addTask_(hooks_copy);

		in_hijack = 0;
		return 0;//free
	}
}