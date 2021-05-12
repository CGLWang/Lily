#ifdef in_PC
#include "Lily_help.h"
extern int new_count;

#else
#include "Lily_help.h"
#include <string.h>
#endif // in_PC

//like: for i = 1:10
// j=i+1
//end
//or
//like: for i = 1:2:10
// j=i+1
//end

typedef char *str;
typedef struct
{
	Var iter_field;		   //迭代变量指针
	Li_List for_cmd_lines; //循环执行的指令列表
	float end_val, step_val;
	char now_at;			//执行指令计数
	char var_at;			//迭代变量 偏置
	char need_delete_field; //end时是否需要删除变量
	char be_greater;		//迭代方向，增加/减小

} ForItem_def;

typedef struct
{
	Li_List cmd_lines;		 //循环执行的指令列表
	char loop_count, now_at; //执行指令计数
} LoopItem_def;

#define for_stack_len 4
#define loop_stack_len 4
ForItem_def for_area_stack[for_stack_len];
char for_stack_count = 0;
LoopItem_def loop_area_stack[loop_stack_len];
char loop_stack_count = 0;

Li_List for_cmd_lines = NULL;
float *end_val, *step_val;
char *now_at;
char *be_greater;
char *need_delete_field;
Var iter_field = NULL;
char var_at = 0;
Li_String scopy = NULL;
char wait_for_end_count = 0;
char in_filling = 0;

Li_List loop_cmd_lines = NULL;
char *loop_count = 0, *loop_now_at = 0;
char loop_in_filling = 0;

int end_for();
int do_for_loop();
char *get_cmd_line_at(Li_List, int);
int hijackor_for_end(int n, char **args);
void update_for_area();
void release_all();
int hijackor_loop_end(int n, char *args[]);
int do_loop_loop();
void end_loop();
void update_loop_area();

// new:
// items, iter_field_index, for_cmd_lines,iter_field_name
int cmd_for_start(int n, char *args[]) // call by lily_do
{
	if (n < 2)
		return -1;
	joint_args(n - 1, args + 1);							  // i=1:0.1:3
	Li_List items = str_split_by_str(args[1], (char *)" =:"); // new [ i 1 0.1 3]

	args = (char **)(items->content);
	float start, step, endv;
	char be_greater_;
	char need_delete_field_;

	if (items->count < 3)
	{
		delete_list(items);
		li_error("bad arguments", -65);
	}
	Lily_List numbers_a;
	numbers_a.content = (char *)(args + 1);
	numbers_a.count = items->count - 1;
	Li_List numbers = valcues_of_vars(&numbers_a); // new
	if (numbers == NULL)
	{
		delete_list(items);
		li_error("bad arguments", -66);
	}
	float *nums = list_content(numbers, float);

	if (numbers->count == 3)
	{
		step = nums[1];
		endv = nums[2];
	}
	else if (numbers->count == 2)
	{
		step = 1.0f;
		endv = nums[1];
	}
	else
	{
		delete_list(numbers);
		delete_list(items);
		li_error("bad arguments", -67);
	}
	start = nums[0];
	delete_list(numbers);
	if (start < endv && step > 0.0f)
	{
		be_greater_ = 1;
	}
	else if (start > endv && step < 0)
	{
		be_greater_ = 0;
	}
	else
	{
		delete_list(items);
		li_error("bad step", -2);
	}

	int iter_field_index = search_var_in_Lily_ui(args[0]);
	need_delete_field_ = 0;
	if (iter_field_index < 0)
	{
		iter_field_index = public_a_new_var(args[0], 'f', to_voidf(start));
		need_delete_field_ = 1;
		if (iter_field_index < 0) // not create new field
		{
			delete_list(items);
			//release_all();
			fun_exit();
			lily_out("\aerror -12:build new field failed\n");
			li_error("apply filed", -12);
		}
		iter_field = li_vars + iter_field_index;
	}
	else
	{
		iter_field = li_vars + iter_field_index;
		var_at = ebrace_value_from_string(args[0]);
		if (var_at < 0)
		{
			var_at = 0;
		}

		switch (iter_field->type)
		{
		case 'f':
			set_value_of_var(iter_field, var_at, to_voidf(start));
			break;
		case 'd':
		case 'c':
			set_value_of_var(iter_field, var_at, to_void((int)start));
			break;
		default:
			delete_list(items);
			// li_error("bad var", -5);
			break;
		}
	}

	delete_list(items);
	add_hijack(hijackor_for_end);
	in_filling = 1;
	for_cmd_lines = new_list(sizeof(str), 4);
	// create successful
	//push it to stack

	ForItem_def *area = for_area_stack + for_stack_count;
	area->iter_field = iter_field;
	area->var_at = var_at;
	area->for_cmd_lines = for_cmd_lines;
	// area->start_val = start;
	area->end_val = endv;
	area->step_val = step;
	area->now_at = 0;
	area->need_delete_field = need_delete_field_;
	area->be_greater = be_greater_;

	for_stack_count++;
	update_for_area();

	wait_for_end_count = 1;
#ifdef for_show_detial
	lily_out(":");
#endif // for_show_detial
	return 0;
}
// new si = for_cmd_lines.contnet[i]
int hijackor_for_end(int n, char **args)
{
	char *s = args[0];
	if (str_equal(args[0], "for") || str_equal(args[0], "loop"))
	{
		wait_for_end_count++;
	}
	else if (strcmp(s, "end") == 0)
	{
		wait_for_end_count--;
		if (wait_for_end_count == 0)
		{
			in_filling = 0;
			add_listener(do_for_loop);
			*now_at = 0;
			str s = get_cmd_line_at(for_cmd_lines, *now_at);
			addTaskArg((TasksArg_def)lily_do, s);
#ifdef for_show_detial
			lily_out(s);
			lily_out(">");
#endif // for_show_detial

			return Lily_cmd_do_not_notify; // free hijack, not notify
		}
	}
	//save the cmd lines
	char *si = new_string_by(s);
	if (si == NULL || li_add(for_cmd_lines, si) < 0)
	{
		lily_out("loop error:mry\n");
		fun_exit();
		return -4;
	}
#ifdef for_show_detial
	lily_out(":");
#endif // for_show_detial

	return KeepHijack; //notify
}

int do_for_loop()
{
	if (in_filling)
	{
		for_stack_count--; //switch last area
		update_for_area();
	}

	if (++*now_at >= for_cmd_lines->count)
	{
		float start_val;
		int vi;
		*now_at = 0;
		// *start_val += *step_val;
		switch (iter_field->type)
		{
		case 'f':
			assign_f_from_void(start_val, get_value_of_var(iter_field, var_at));
			start_val += *step_val;
			set_value_of_var(iter_field, var_at, to_voidf(start_val));
			// *(float *)(iter_field->ref) += *step_val;
			// start_val = *(float *)(iter_field->ref);
			break;
		case 'd':
		case 'c':
			vi = (int)get_value_of_var(iter_field, var_at);
			vi += *step_val; // abs(step_val)>1
			set_value_of_var(iter_field, var_at, to_void(vi));

			// *(int *)(iter_field->ref) += *step_val;
			start_val = vi; //*(int *)(iter_field->ref);
			break;

		default:
			break;
		}

		if ((*be_greater && start_val > *end_val) || (!*be_greater && start_val < *end_val)) //iteration end
		{
			end_for();
			return 0;
		}
	}

	str s = get_cmd_line_at(for_cmd_lines, *now_at);
	if (in_filling)
	{
		for_stack_count++;
		update_for_area(); //switch back
	}
	addTaskArg((TasksArg_def)lily_do, s);

#ifdef for_show_detial
	lily_out(s);
	lily_out(">");
#endif // for_show_detial

	return 0;
}
//delete
//for_cmd_lines->content,for_cmd_lines,scopy,iter_field

int end_for()
{
	int i, n;
	n = for_cmd_lines->count;
	str *ss = (str *)(for_cmd_lines->content);
	for (i = 0; i < n; i++)
	{
		free(ss[i]);
#ifdef in_PC
		new_count--;
		cout << "#" << new_count << "#";
#endif // in_PC
	}
	delete_list(for_cmd_lines);
	for_cmd_lines = NULL;
	if (for_stack_count + loop_stack_count == 1)
	{
		if (scopy)
			delete_li_string(scopy);
		scopy = NULL;
	}

	if (*need_delete_field)
	{
		delete_field(1, (char **)(&(iter_field->name)));
		iter_field = NULL;
		need_delete_field = NULL;
	}
	//lily_hooks_cmd_done = NULL;
	for_stack_count--; //pop area
	update_for_area();
	invoke_listener;
	if (have_listener)
		addTask_(current_listener); //Notify the listener that the for loop is done
#ifdef for_show_detial
	lily_out("<\n");
#endif // for_show_detial

	return 0;
}
// new scopy
char *get_cmd_line_at(Li_List for_cmd_lines, int at)
{
	if (for_cmd_lines->count == 0)
	{
		return (char *)"";
	}
	str *ss = (str *)(for_cmd_lines->content);
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

void update_for_area()
{
	if (for_stack_count >= for_stack_len)
	{
		lily_out("error for_stack_count<0\n");
		return;
	}
	ForItem_def *area = for_area_stack + for_stack_count - 1;
	iter_field = area->iter_field;
	var_at = area->var_at;
	for_cmd_lines = area->for_cmd_lines;
	// start_val = &area->start_val;
	end_val = &area->end_val;
	step_val = &area->step_val;
	now_at = &area->now_at;
	be_greater = &area->be_greater;
	need_delete_field = &area->need_delete_field;
}

void release_all()
{
	for (; for_stack_count > 0; for_stack_count--)
	{
		update_for_area();
		end_for();
	}
	cmd_done_listener_stack_top = 0; //remove all listener
	stack_top = 0;					 // remove all hijack
	lily_ui.hijacked = 0;
}

int cmd_loop_start(int n, char *args[]) //loop 3
{
	if (n < 2)
		return -1;
	//get loop count
	int loop_count_;
	float v = value_from_string_or_var(args[1]);
	if (!isnan(v))
	{
		loop_count_ = v;
	}
	else
	{
		li_error("bad arg", -2);
	}
	
	loop_cmd_lines = new_list(sizeof(str), 4);
	if (loop_cmd_lines == NULL)
	{
		li_error("memory out", -4);
	}
	loop_in_filling = 1;
	add_hijack(hijackor_loop_end);
	// create successfully
	//push it to stack
	LoopItem_def *area = loop_area_stack + loop_stack_count;
	area->cmd_lines = loop_cmd_lines;
	area->loop_count = loop_count_;
	loop_stack_count++;
	update_loop_area();

	wait_for_end_count = 1;
#ifdef for_show_detial
	lily_out(":");
#endif // for_show_detial
	return 0;
}

int hijackor_loop_end(int n, char *args[])
{
	char *s = args[0];
	if (str_equal(args[0], "for") || str_equal(args[0], "loop"))
	{
		wait_for_end_count++;
	}
	else if (strcmp(s, "end") == 0)
	{
		wait_for_end_count--;
		if (wait_for_end_count == 0)
		{
			loop_in_filling = 0;
			add_listener(do_loop_loop);
			*loop_now_at = 0;
			str s = get_cmd_line_at(loop_cmd_lines, *loop_now_at);
			addTaskArg((TasksArg_def)lily_do, s);
#ifdef for_show_detial
			lily_out(s);
			lily_out(">");
#endif // for_show_detial

			return Lily_cmd_do_not_notify; // free hijack, not notify
		}
	}
	//save the cmd lines
	char *si = new_string_by(s);
	if (si == NULL || li_add(loop_cmd_lines, si) < 0)
	{
		lily_out("loop error:mry\n");
		fun_exit();
		return -4;
	}
#ifdef for_show_detial
	lily_out(":");
#endif // for_show_detial

	return KeepHijack; //notify
}

int do_loop_loop()
{
	if (loop_in_filling)
	{
		loop_stack_count--; //switch last area
		update_loop_area();
	}

	if (++*loop_now_at >= loop_cmd_lines->count)
	{
		*loop_now_at = 0;
		(*loop_count)--;
		if (*loop_count == 0)
		{
			end_loop();
			return 0;
		}
	}

	str s = get_cmd_line_at(loop_cmd_lines, *loop_now_at);
	if (loop_in_filling)
	{
		loop_stack_count++;
		update_loop_area(); //switch back
	}
	addTaskArg((TasksArg_def)lily_do, s);

#ifdef for_show_detial
	lily_out(s);
	lily_out(">");
#endif // for_show_detial

	return 0;
}
void end_loop()
{
	int i, n;
	n = loop_cmd_lines->count;
	str *ss = (str *)(loop_cmd_lines->content);
	for (i = 0; i < n; i++)
	{
		free(ss[i]);
#ifdef in_PC
		new_count--;
		cout << "#" << new_count << "#";
#endif // in_PC
	}
	delete_list(loop_cmd_lines);
	loop_cmd_lines = NULL;
	if (loop_stack_count + for_stack_count == 1)
	{
		if (scopy)
			delete_li_string(scopy);
		scopy = NULL;
	}

	//lily_hooks_cmd_done = NULL;
	loop_stack_count--; //pop area
	update_loop_area();
	invoke_listener;
	if (have_listener)
		addTask_(current_listener); //Notify the listener that the for loop is done
#ifdef for_show_detial
	lily_out("<\n");
#endif // for_show_detial
}
void update_loop_area()
{
	LoopItem_def *area = loop_area_stack + loop_stack_count - 1;
	loop_count = &area->loop_count;
	loop_cmd_lines = area->cmd_lines;
	loop_now_at = &(area->now_at);
}

void release_all_loops()
{
	for (; loop_stack_count > 0; loop_stack_count--)
	{
		update_loop_area();
		end_loop();
	}
	cmd_done_listener_stack_top = 0; //remove all listener
	stack_top = 0;					 // remove all hijack
	lily_ui.hijacked = 0;
}

void fun_exit()
{
	release_all();
	release_all_loops();
}
