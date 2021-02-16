#define _CRT_SECURE_NO_WARNINGS

//#include<stdio.h>
//#include<stdlib.h>
//#include<ctype.h> 
#include"Lily_basic.h"
#include"Lily_ui.h"
#include<math.h>

//#include <iostream>
#include "shell_cal.h"
//#include <string>
//using namespace std;

//typedef char* string_;
//int main()
//{
//	lily_init();
//	char s[] = "123+4*6/5          ";
//	
//	char ss[] = "*/+- ";
//	
//	while (1)
//	{
//		char s2[30];
//		//cin >> s2;
//		//scanf("%s", &s2);
//		cin.getline(s2, 30);
//		strcpy(s, s2);
//		Li_List list = str_split_by_str(s2, ss);
//		string_* strs = list_content(list, string_);
//		//cout << s << endl;
//		for (int i = 0; i < list->count; i++)
//		{
//			char* s2 = strs[i];
//			cout << i << ":" << s2 << endl;
//		}
//
//		float result = shell_calculate_expression(s);
//		cout <<"="<< result<<endl;
//	}
//	return 0;
//}
//
//Li_List new_stack()
//{
//	return new_list(sizeof(float), 8);
//}
//void delete_stack(Li_List stack)
//{
//	delete_list(stack);
//}
//void stack_push(Li_List stack, float val)
//{
//	list_add(stack, &val);
//}
//float stack_peak(Li_List stack)
//{
//	float* vs = (float*)(stack->content);
//	return vs[stack->count];
//}
//float stack_pop(Li_List stack)
//{
//	if (stack->count < 1)
//		return NAN;
//	float* vs = (float*)(stack->content);
//	float v =  vs[stack->count];
//	stack->count--;
//	return v;
//}
typedef char* string_;
// list in string type
// return NULL if in error
// note: delete list int time
Li_List valcues_of_fields(Li_List list)
{
	typedef char* string_;
	string_* item_strs = list_content(list, string_);
	int n = list->count;
	Li_List values = new_li_cap(float,n);
	for (int i = 0; i < n; i++)
	{
		string_ name = item_strs[i];
		if (str_is_name(name))
		{
			int index = search_field_in_Lily_ui(name);
			if (index < 0)
			{
				delete_list(values);
				return NULL;
			}
			Field fields = li_fields;
			char type = fields[index].type;
			float val;
			if (type == 'f')
			{
				val = *((float*)(fields[index].ref));
			}
			else if (type == 'd')
			{
				val = *((int*)(fields[index].ref));
			}
			else
			{
				delete_list(values);
				return NULL;
			}
			li_add(values, val);
		}
		else if (str_is_numeric(name))
		{
			float val = atof(name);
			li_add(values, val);
		}
		else
		{
			delete_list(values);
			return NULL;
		}
	}
	return values;
}
float shell_calculate_expression(char* rx)
{
	//step1: get objects attending caculation
	//step2: calculate one by one
	char operaions[] = "*/+- ";
	// 2+a*3
	//2,3
	//+,*

	//get indexs of operations
	operaions[4] = '\0';
	Li_List opes_index = str_find(rx, operaions);
	int* indexs = list_content(opes_index, int);
	int n = opes_index->count;
	// get operations
	char* opes = (char*)malloc(n + 1);
	if (opes == NULL)
	{
		delete_list(opes_index);
		return NAN;
	}
		
	int i;
	for (i = 0; i < n; i++)
		opes[i] = rx[indexs[i]];
	opes[i] = '\0';
	delete_list(opes_index);

	// get numbers/fields, in form of string
	operaions[4] = ' ';
	Li_List list = str_split_by_str(rx, operaions);
	if (list->count - n != 1)
	{
		delete_list(list);
		return NAN;
	}
	Li_List values_ = valcues_of_fields(list);
	delete_list(list);
	if (values_ == NULL)
	{
		return NAN;
	}
	float* values = list_content(values_, float);
	
	// start calculate on by on
	// e.g.:opes: ++*+/
	for (i = 0; operaions[i] != ' '; )
	{
		n = str_index(opes, operaions[i]);
		if (n < 0)
		{
			i++;// to next operation[* / + -]
			continue;
		}
		// operations[i], at index n in opes, 
		// get nums at n and n+1, using operations[i] to calculate
		switch (i)
		{
		case 0://*
			values[n] = values[n] * values[n + 1];
			break;
		case 1:// /
			if (values[n + 1] == 0.0)
			{
				delete_list(values_);
				return NAN;
			}
			values[n] = values[n] / values[n + 1];
			break;
		case 2:// +
			values[n] = values[n] + values[n + 1];
			break;
		case 3:// -
			values[n] = values[n] - values[n + 1];
			break;
		default:
			break;
		}
		// remove 
		list_remove_at(values_, n + 1);
		for (int j = n; opes[j] != '\0'; j++)
			opes[j] = opes[j + 1];
	}
	float result = values[0];
	delete_list(values_);
	return result;
}

int shell_cal_exp_cmd(int n, char** arg)
{
	float result;
	if (n == 2)
	{
		result = shell_calculate_expression(arg[1]);
		if(isnan(result))return -1;
		//if (result == NAN)return - 1;
		sprintf(tx, "%f\n", result);
		lily_cout(tx);
	}
	else if (n == 3)
	{
		int index = search_field_in_Lily_ui(arg[1]);
		if (index < 0)
		{
			index = public_a_new_field(arg[1],'f', 0.0f);
			//return -2;
		}
		Field fields = li_fields;
		Field field = &fields[index];
		result = shell_calculate_expression(arg[2]);
		if (isnan(result))return -3;

		if (field->type == 'f')
		{
			*((float*)(field->ref)) = result;
		}
		else if (field->type == 'd')
		{
			*((int*)(field->ref)) = (int)result;
		}
		else
			return -4;
		lily_cout(arg[1]);
		sprintf(tx, "=%f\n", result);
		lily_cout(tx);

	}
	return 0;
}

//deperacted
//int shell_cal_assitant(int n, char** arg)
//{
//	// cal a=a+1 *2
//	//skip the first
//	char* str;
//	if (strcmp("cal", arg[0]) == 0)
//	{
//		joint_args(n - 1, arg + 1);
//		str = arg[1];
//	}
//	else
//	{
//		joint_args(n, arg);
//		str = arg[0];
//	}
//	
//	//int ind_equ = str_index(str, '=');
//	Li_List li = str_split(str, '=');
//	int code = shell_cal_exp_cmd(li->count, (char**)(li->content));
//	delete_list(li);
//	return code;
//}

// help cmds
// cmd1
int shell_do_cmd(char* rx)
{
	int hit = 0;
	char return_code;
	hit = search_cmd_in_Lily_ui(rx);
	if (hit < 0)return 0;//not hit, to next proess

	Cmd_def* cmd = (Cmd_def*)list_index(lily_ui.cmds, hit);
	Li_List list = str_split(rx, ' ');
	return_code = (cmd->todo)(list->count, (char**)(list->content));
	delete_list(list);
	if (return_code < 0)
	{
		//sprintf(tx, "error(%d)\n", return_code);
		//lily_cout(tx);
		return return_code;
	}
	return 1;
}
// a = 1+1
// a+1
// 1+1
int shell_do_cal(char* rx)
{
	if (!str_contains_by_str(rx, (char*)"+-*/"))
		return 0;
	//int ind_equ = str_index(str, '=');
	Li_List li = str_split(rx, '=');
	if (li->count > 2)return -1;
	string_* strs = list_content(li, string_);
	string_ args[3];
	args[0] = (char*)"cal";
	args[1] = strs[0];
	if(li->count==2)
		args[2] = strs[1];
	int code = shell_cal_exp_cmd(li->count+1, args);
	delete_list(li);
	if(code<0)
		return code;
	return 1;
}
// a = 1
// a = b
// a, if a is a field
int shell_do_fields(char* rx)
{
	strcpy(tx, rx);
	char* cmd = tx;
	int index = str_index(cmd, '=');
	str_replace(cmd, '=', ' ');
	Li_List li = str_split(cmd, ' ');
	int n = li->count;
	char** args = (char**)li->content;
	char* val = NULL;
	if (index > 0)
	{
		val = args[1];
	}
	index = search_field_in_Lily_ui(args[0]);

	if (index < 0 && val == NULL)
	{
		delete_list(li);
		return 0;
	}

	Field fields = (Field)(lily_ui.fields->content);
	Field first_field = NULL, sencond_field = NULL;
	if (index >= 0)
		first_field = &(fields[index]);

	char pattern[] = "=%f\n";
	int code = 0;
	if (val != NULL)
	{
		index = search_field_in_Lily_ui(val);
		if (index >= 0)
		{
			sencond_field = &fields[index];
		}
	}


	if (first_field == NULL && val != NULL)
	{
		if (sencond_field != NULL) // creat a new field and overwrite
		{
			switch (sencond_field->type)
			{
			case 's':
				code = public_a_new_string_field(cmd, ((Li_String)sencond_field->ref)->str);
				break;
			case 'f':
				code = public_a_new_field(cmd, sencond_field->type, *((float*)(sencond_field->ref)));
				break;
			case 'd':
				code = public_a_new_field(cmd, sencond_field->type, *((int*)(sencond_field->ref)));
				break;
			default:
				break;
			}
		}
		else
		{
			int num_length = 0;
			float* nums = NULL;

			nums = get_nums_from_rx(val, &num_length);
			if (num_length > 0)
			{
				code = public_a_new_field(cmd, 'f', nums[0]);
			}
			else
				code = public_a_new_string_field(cmd, val);
		}

		if (code < 0)
		{
			delete_list(li);
			return code;
		}
		else
		{
			first_field = &fields[code];
		}
	}
	else if (first_field != NULL && val != NULL)  //reassign a field
	{
		if (sencond_field != NULL)
		{
			code = assign_field_from_field(first_field, sencond_field);
		}
		else
		{
			code = assign_field_from_string(first_field, val);
		}
		if (code < 0)
		{
			return code;
		}
	}
	
	//show
	pattern[2] = first_field->type;
	lily_cout(first_field->name);
	switch (first_field->type)
	{
	case 'f':
		sprintf(tx, pattern, *(float*)(first_field->ref));
		break;
	case 'd':
		sprintf(tx, pattern, *(int*)(first_field->ref));
		break;
	case 's':
		sprintf(tx, pattern, *(char**)(first_field->ref));
		break;
	default:
		lily_cout("type error");
		break;
	}
	lily_cout(tx);
	delete_list(li);
	return 1;
}
int shell_do_notFound(char* rx)
{
	lily_cout("no cmd:\"");
	lily_cout(rx);
	lily_cout("\"\n");
	return 1;
}

