#include "Lily_ui.h"
#include "Lily_boardcast.h"
char rx[256];
unsigned char ri, hi;
char tx[64];
unsigned char ti;

Lily_cmds_def lily_ui;//a value is copied, so a cmd or field can be add in a stack call

static Arg_Tasks_def ui_stack[4];
static char stack_top = 0;
void (*deal_byte_stream)(char*) = NULL;
// this fun also change the hi index
// assert that the cmd is less than 31b in length
char* get_cmd_from_rx()
{
	static char cmd[64];
	char i=0, c;
	c = rx[hi];
	if (c == frame_head_0)//a frame
	{
		cmd[i++] = c;
		hi++;
		cmd[i++] = rx[hi++];
		char length = rx[hi++];
		cmd[i++] = length;
		length++;
		while (length)
		{
			cmd[i++] = rx[hi++];
			length--;
		}
		return cmd;
	}
	for (; rx[hi] != '\0'; i++, hi++)
	{
		c = rx[hi];

		if (c >= 'A' && c <= 'Z')
			c += 0x20;

		cmd[i] = c;


	}
#ifdef in_debug
	if (hi+1 != ri)//read done
		lily_cout("cmd queue error");
#endif // in_debug

	cmd[i] = '\0';
	hi++; // jump to the next cmd
	return cmd;
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
char excute_command()
{
	char i;
	int hit = 0;
	char* rx = get_cmd_from_rx();
	if (*rx == frame_head_0)
	{
		if (deal_byte_stream != NULL)
			deal_byte_stream(rx);
		/*return end;*/
	}
	char* send_s = tx;
	static Cmd_para_set_def para_set;

	int num_length = 0;
	float* nums = NULL;
	bool need_send = false;
	bool hijacked = false;
	char return_code;

	while (*rx==' ')
	{
		rx++;
	}
	if (stack_top)
	{
		hit = 1;
		hijacked = true;
		return_code = ui_stack[stack_top](rx,&send_s);
		if (assign_send_s(return_code, &send_s, &need_send))
			stack_top--;
		
		//statue = ((hijack*)(lily_ui.hijacks->content))[lily_ui.hijacks->count - 1](rx);
	}

	if (not hit)
	{
		for (i = 0; i < lily_ui.cmds->count; i++)//cmds
		{
			Cmd_def* cmd = (Cmd_def*)list_index(lily_ui.cmds, i);
			hit = str_equal(rx, cmd->cmd);
			if (hit)
			{
				void* arg = &para_set;
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
				}
				return_code = (cmd->todo)(arg, &send_s);
				assign_send_s(return_code, &send_s, &need_send);
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
		for (i = 0; i < lily_ui.fields->count; i++)
		{
			Field_def* field = (Field_def*)list_index(lily_ui.fields, i);
			hit = str_equal(rx, field->field);

			if (hit)
			{
				nums = get_nums_from_rx(rx + hit, &num_length);

				if (num_length == 1)
					*((float*)field->ref) = nums[0];
				strcpy(rx, field->field);
				switch (field->type)
				{
				case float_type:
					strcat(rx, "=%f");
					sprintf(tx, rx, *(field->ref));
					break;
				case int_type:
					strcat(rx, "=%d");
					sprintf(tx, rx, *((int*)(field->ref)));
				default:
					break;
				}
				need_send = True;
				break;
			}
		}
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
			if (!hijacked)
				lily_cout(">>");
			else
				lily_cout(">");
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
	if (stack_top >= stack_len)
	{
		lily_cout(">>ui stack overflow!");
		stack_top = 0;
	}
}
//this function split the cmd line into several sub strings(except the cmd characteristic word) called para by the White Rooks,
// each para is isolating with others in the original cmd lines by a non-alphabet letter including blank,number, comma etc.
// using this function to get additional operation words in your hijack.

int search_in_Lily_ui(char* item)
{
	return -1;
}
char cmd_help(void* a, void* b)
{
	Cmd_para_set_def* pas = (Cmd_para_set_def*)a;
	lily_cout(">>help:\n");
	Cmd_def* p = (Cmd_def*)(lily_ui.cmds->content);
	if (pas->paras_length == 1)
	{
		char* item = NULL;
		item = pas->paras[0];
		int n = lily_ui.cmds->count;
		for (int i = 0; i < n; i++)
		{
			if (str_equal(item, p[i].cmd))
			{
				const char* s[] = { "plain text","numbers","options","numbers and options","bytes" };
				sprintf(tx, ">>%s:%s, ", p[i].cmd, p[i].annotation);
				lily_cout(tx);
				lily_cout("type:");
				lily_cout(s[p[i].type]);
				lily_cout("\n");
				return Cmd_not_send;
			}
		}
		Field_def* q = (Field_def*)(lily_ui.fields->content);
		n = lily_ui.fields->count;
		for (int i = 0; i < n; i++)
		{
			if (str_equal(item, q[i].field))
			{
				const char* s[] = { "float","int"};
				sprintf(tx, ">>%s:%s, type", q[i].field, q[i].annotation);
				lily_cout(tx);
				lily_cout(s[q[i].type]);
				lily_cout("\n");
				return Cmd_not_send;
			}
		}
		lily_cout(">>not found help of:");
		lily_cout(item);
		lily_cout("\n");
		return Cmd_not_send;
	}

	lily_cout(">cmds:\n");
	for (int i = 0; i < lily_ui.cmds->count; i++)
	{
		sprintf(tx, "%d- %s:%s\n", i, p[i].cmd, p[i].annotation);
		lily_cout(tx);
	}
	Field_def* q = (Field_def*)(lily_ui.fields->content);
	lily_cout(">fields:\n");
	for (int i = 0; i < lily_ui.fields->count; i++)
	{
		sprintf(tx, "%d- %s:%s\n", i, q[i].field, q[i].annotation);
		lily_cout(tx);
	}
	return Cmd_not_send;
}

//creat and public a cmd with defaults
//just provide a name and fun ptr of this cmd
void creat_public_a_cmd(char* name, Arg_Tasks_def link)
{	
	Cmd_def cmd;
	cmd.cmd = name;
	cmd.annotation = (char*)"none";
	cmd.todo = link;
	cmd.id = lily_ui.cmds->count;
	cmd.type = with_bytes;
	public_cmd(cmd);
}
//creat and public a cmd with defaults
void creat_public_a_field(char* name, void*link)
{
	Field_def fed;
	fed.field = name;
	fed.annotation =(char*) "none";
	fed.ref = (float*)link;
	fed.type = float_type;
	public_field(fed);
}