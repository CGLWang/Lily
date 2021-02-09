#include "Lily_ui.h"
#include "Lily_boardcast.h"
char rx[256];
unsigned char ri, hi;
char tx[64];
unsigned char ti;

char clip_board[128];

Lily_cmds_def lily_ui;//a value is copied, so a cmd or field can be add in a stack call

static Arg_Tasks_def ui_stack[4];
static char stack_top = 0;
void (*deal_byte_stream)(char*) = NULL;
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
		lily_cout("cmd too long");
	if (hi+1 != ri)//read done
		lily_cout("cmd queue error");
#endif // in_debug

	cmd[i] = '\0';
	hi++; // jump to the next cmd
	return cmd;
#undef len
}

char assign_send_s(char code,char**send_s, bool* send)
{
	if (code &( Cmd_done|Cmd_false))
	{
		*send = true;
		if (code & Cmd_done)
			*send_s = (char*)"done";
		else
			*send_s = (char*)"false input";
	}
	if (code & Cmd_send)
		*send = true;
	return code & Cmd_end;

}
int shell_do()
{
	char i;
	int hit = 0;
	char* rx = get_cmd_from_rx();
	//if (*rx == frame_head_0)
	//{
	//	if (deal_byte_stream != NULL)
	//		deal_byte_stream(rx);
	//	/*return end;*/
	//}
	char* send_s = tx;
	static Cmd_para_set_def para_set;
	int num_length = 0;
	float* nums = NULL;
	bool need_send = false;
	char return_code;

	while (*rx==' ')
	{
		rx++;
	}
	if (stack_top)
	{
		hit = 1;
		
		Li_List list = str_split(rx, ' ');
		return_code = ui_stack[stack_top](list->count,(char**)(list->content));
		//if (assign_send_s(return_code, &send_s, &need_send))
		//	stack_top--;
		if (return_code==0)
			stack_top--;

		if (!stack_top)
		{
			lily_ui.hijacked = false;
		}
	}

	if (!hit)
	{
		for (i = 0; i < lily_ui.cmds->count; i++)//cmds
		{
			Cmd_def* cmd = (Cmd_def*)list_index(lily_ui.cmds, i);
			hit = str_equal(rx, cmd->name);
			if (hit)
			{
				/*void* arg = &para_set;
				para_set.cmd_id = cmd->id;
				switch (cmd->type)
				{
				case with_bytes:
					arg = rx+hit;
					break;
				case with_numbers:
					para_set.numbers = get_nums_from_rx(rx+hit, &(para_set.numbers_length));
					hijacked = true;
					break;
				case with_paras:
					para_set.paras = get_paras_from_rx(rx+hit, &(para_set.paras_length));
					break;
				case with_both:
					para_set.numbers = get_nums_from_rx(rx+hit, &(para_set.numbers_length));
					hijacked = true;
					para_set.paras = get_paras_from_rx(rx+hit, &(para_set.paras_length));
					break;
				default:
					break;
				}*/
				Li_List list = str_split(rx, ' ');
				return_code = (cmd->todo)(list->count, (char**)(list->content));
				if (return_code < 0)
					lily_cout("error\n");
				/*switch (return_code)
				{
				case Cmd_send:
					need_send = true;
					break;
				case Cmd_false_input:
					send_s = (char*)"false input!";
					need_send = true;
				case Cmd_send_done:case Cmd_send_done_and_end:
					send_s = (char*)"done";
					need_send = true;
				default:
					break;
				}*/
				break;
			}
		}
	}

	if (!hit)
	{
		hit = shell_do_fields(rx) >= 0;
		
		//for (i = 0; i < lily_ui.fields->count; i++)
		//{
		//	Field_def* field = (Field_def*)list_index(lily_ui.fields, i);
		//	hit = str_equal(rx, field->name);
		//	if (hit)
		//	{
		//		
		//		//nums = get_nums_from_rx(rx + hit, &num_length);
		//		//if (num_length == 1)
		//		//	*((float*)field->ref) = nums[0];
		//		//strcpy(rx, field->name);
		//		//switch (field->type)
		//		//{
		//		//case float_type:
		//		//	strcat(rx, "=%f");
		//		//	sprintf(tx, rx, *(field->ref));
		//		//	break;
		//		//case int_type:
		//		//	strcat(rx, "=%d");
		//		//	sprintf(tx, rx, *((int*)(field->ref)));
		//		//	break;
		//		//default:
		//		//	break;
		//		//}
		//		//need_send = True;
		//		break;
		//	}
		//}
	}
	
	if (!hit)
	{
		strcpy(tx, "no cmd:\"");
		strcat(tx, rx);
		strcat(tx, "\"");
		send_s = tx;
		need_send = 1;
	}
	if (need_send)
	{
		if (send_s != NULL)
		{
			/*if (!hijacked)*/
				lily_cout(">>");
				/*else
					lily_cout(">");*/
			lily_cout(send_s);
			lily_cout("\n");
		}
#ifdef in_debug
		else
			lily_cout("NULL ptr exception!\n");
#endif // in_debug
	}
	return end;
}


//char public_cmd(Cmd_def new_cmd)
//{
//	/*if (lily_ui.cmds_len == lily_ui.cmds_cap)
//	{
//		Cmd_def* new_p;
//		new_p = (Cmd_def*)realloc(lily_ui.cmds, (lily_ui.cmds_cap + 5) * sizeof(Cmd_def));
//		if (new_p == NULL)
//		{
//			lily_sendmsg("error 1");
//			return 1;
//		}
//		lily_ui.cmds = new_p;
//		lily_ui.cmds_cap += 5;
//	}
//	lily_ui.cmds[lily_ui.cmds_len++] = new_cmd;*/
//
//	return list_add(lily_ui.cmds, &new_cmd);
//}
//publish a field in float type, 
// so that you can carry out write/read operation on it by cmd
//char public_field(Field_def new_field)
//{
//	//if (lily_ui.fields_len == lily_ui.fields_cap)
//	//{
//	//	Field_def* new_p;
//	//	new_p = (Field_def*)realloc(lily_ui.fields, (lily_ui.fields_cap + 5) * sizeof(Field_def));
//	//	if (new_p == NULL)
//	//	{
//	//		lily_sendmsg("error 2");
//	//		return 1;
//	//	}
//	//	lily_ui.fields = new_p;
//	//	lily_ui.fields_cap += 5;
//	//}
//	//lily_ui.fields[lily_ui.fields_len++] = new_field;
//	return   list_add(lily_ui.fields, &new_field);
//}

// add a hijack in a cmd execution to override the cmd response
// in a hijack, the <call_back> will receive the handle of the cmd line,
// and do whatever you like with it
// <call_back> return 0 means end this hijack, otherwise the White Rooks will keep it on afterward
void add_hijack(Arg_Tasks_def call_back)
{
	ui_stack[++stack_top] = call_back;
	lily_ui.hijacked = true;
	if (stack_top >= stack_len)
	{
		lily_cout(">>ui stack overflow!");
		stack_top = 0;
		lily_ui.hijacked = false;
	}
}
//this function split the cmd line into several sub strings(except the cmd characteristic word) called para by the White Rooks,
// each para is isolating with others in the original cmd lines by a non-alphabet letter including blank,number, comma etc.
// using this function to get additional operation words in your hijack.

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
int cmd_help(int nargin, char** arg)
{
	lily_cout(">>help:\n");
	Cmd_def* p = (Cmd_def*)(lily_ui.cmds->content);
	bool hit = false;
	for(int now = 1;now<nargin;now++)
	{
		hit = false;
		char* item = NULL;
		item = arg[now];
		int n = lily_ui.cmds->count;
		for (int i = 0; i < n; i++)
		{
			if (str_equal(item, p[i].name))
			{
				sprintf(tx, ">>%s: id:%d, %s\n", p[i].name,p[i].id, p[i].annotation);
				lily_cout(tx);
				hit = true;
				break;
			}
		}
		if(hit)continue;
		Field_def* q = (Field_def*)(lily_ui.fields->content);
		n = lily_ui.fields->count;
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
		}
		if(hit)continue;
		lily_cout(">>not found help of:");
		lily_cout(item);
		lily_cout("\n");
		return -1;
	}

	if (nargin>1)
		return 0;
	lily_cout(">cmds:\n");
	for (int i = 0; i < lily_ui.cmds->count; i++)
	{
		sprintf(tx, "%d- %s:\n", i, p[i].name);
		lily_cout(tx);
	}
	Field_def* q = (Field_def*)(lily_ui.fields->content);
	lily_cout(">fields:\n");
	for (int i = 0; i < lily_ui.fields->count; i++)
	{
		sprintf(tx, "%d- %s\n", i, q[i].name);
		lily_cout(tx);
	}
	return 0;
}

int pass_cmd(int n, char** arg)
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
			lily_cout(arg[1]);
			lily_cout(" not found\n");
			return -1;
		}
		Field_def* q = (Field_def*)(lily_ui.fields->content);
		if (q[index].annotation[0] != '_')
			free(q[index].name);
		if (q[index].type == 's')
			delete_li_string((Li_String)(q[index].ref));
		list_remove_at(lily_ui.fields, index);
	}
	lily_cout("done\n");
	return 0;
}

//creat and public a cmd with defaults
//just provide a name and fun ptr of this cmd
void creat_public_a_cmd(char* name, Arg_Tasks_def link)
{	
	Cmd_def cmd;
	cmd.name = name;
	cmd.annotation = (char*)"none";
	cmd.todo = link;
	cmd.id = lily_ui.cmds->count;
	//cmd.type = with_bytes;
	public_cmd(cmd);
}
//creat and public a cmd with defaults
void creat_public_a_field(char* name, void*link)
{
	Field_def fed;
	fed.name = name;
	fed.annotation =(char*) "_none";
	fed.ref = link;
	fed.type = 'f';
	public_field(fed);
}

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
int shell_do_fields(char* cmd)
{
	//joint_args(n, arg);
	int index = str_index(cmd, '=');
	str_replace(cmd, '=', ' ');
	Li_List li = str_split(cmd, ' ');
	int n = li->count;
	char** args = (char**)li->content;
	char* val=NULL;
	if (index > 0)
	{
		/*cmd[index] = '\0'*/;
		val = args[1];
	}
	index = search_field_in_Lily_ui(args[0]);
	if (index < 0&&val==NULL)return -1;
	Field field = &(((Field)(lily_ui.fields->content))[index]);
	char pattern[] = "=%f\n";
	pattern[2] = field->type;

	if (val != NULL && index >= 0)
	{

		if (field->type == 's')
		{
			if (assign_li_string((Li_String)(field->ref), val) < 0)
			{
				lily_cout("error, too long\n");
			}
			else
			{
				lily_ui.para_updated = true;
			}
		}
		else
		{
			int num_length = 0;
			float* nums = NULL;

			nums = get_nums_from_rx(val, &num_length);
			if (num_length == 1)
			{
				switch (field->type)
				{
				case 'f':
					*(float*)(field->ref) = nums[0];
					break;
				case 'd':
					*(int*)(field->ref) = (int)(nums[0]);
					break;
				case 's':
					break;
				default:
					break;
				}
				lily_ui.para_updated = true;
			}
		}	
	}
	else if(val!=NULL) // index <0 and val is not Null
	{
		int num_length = 0;
		float* nums = NULL;

		nums = get_nums_from_rx(val, &num_length);
		if (num_length > 0)
		{
			num_length = public_a_new_field(cmd, 'f', nums[0]);
		}
		else
			num_length = public_a_new_string_field(cmd, val);
		if (num_length < 0)return -1;
		lily_cout("done\n");
		return 0;
	}
		
	//show
	lily_cout(field->name);
	switch (field->type)
	{
	case 'f':
		sprintf(tx, pattern, *(float*)(field->ref));
		break;
	case 'd':
		sprintf(tx, pattern, *(int*)(field->ref));
		break;
	case 's':
		sprintf(tx, pattern, *(char**)(field->ref));
		break;
	default:
		lily_cout("type error");
		break;
	}
	lily_cout(tx);

	return index;
}

int public_a_new_string_field(char*name, char* s)
{
	Field_def fed;
	fed.annotation =(char*) "none";
	
	fed.name =(char*) malloc(strlen(name) + 1);
	if (fed.name == NULL)return -1;
	strcpy(fed.name, name);
	Li_String li = new_li_string_by(s);
	if (li == NULL)return -1;
	fed.ref = li;
	fed.type = 's';
	public_field(fed);
	return 0;
}
int public_a_new_field(char* name, char type, float val)
{
	Field_def fed;
	fed.annotation = (char*)"none";
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
	public_field(fed);
	return 0;
}