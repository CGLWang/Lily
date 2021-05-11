#define _CRT_SECURE_NO_WARNINGS

//#include<stdio.h>
//#include<stdlib.h>
//#include<ctype.h>
#include "Lily_basic.h"
#include "Lily_ui.h"
#include <math.h>

//#include <iostream>
#include "shell.h"
#ifdef in_PC
#include <iostream>
using namespace std;
extern int new_count;
#endif // in_PC
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
typedef char *string_;
// list is in string type
// return values in float type
// return NULL if in error
// note: delete list int time
Li_List valcues_of_fields(Li_List list)
{
	typedef char *string_;
	string_ *item_strs = list_content(list, string_);
	int n = list->count;
	Li_List values = new_li_cap(float, n);
	for (int i = 0; i < n; i++)
	{
		string_ name = item_strs[i];
		float val = value_from_string_or_field(name);
		if (isnan(val))
		{
			delete_list(values);
			return NULL;
		}
		li_add(values, val);

		// if (str_is_name(name))
		// {
		// 	int index = search_field_in_Lily_ui(name);
		// 	if (index < 0)
		// 	{
		// 		delete_list(values);
		// 		return NULL;
		// 	}
		// 	Field fields = li_fields;
		// 	char type = fields[index].type;
		// 	float val;
		// 	if (type == 'f')
		// 	{
		// 		val = *((float *)(fields[index].ref));
		// 	}
		// 	else if (type == 'd')
		// 	{
		// 		val = *((int *)(fields[index].ref));
		// 	}
		// 	else
		// 	{
		// 		delete_list(values);
		// 		return NULL;
		// 	}
		// 	li_add(values, val);
		// }
		// else if (str_is_numeric(name))
		// {
		// 	float val = atof(name);
		// 	li_add(values, val);
		// }
		// else
		// {
		// 	delete_list(values);
		// 	return NULL;
		// }
	}
	return values;
}

//get value from a string
//support like:
//a
//123.4
//a[1], field type 'f,d,s'
float value_from_string_or_field(string_ name)
{
	float val;
	if (str_is_name(name))
	{
		int index = search_field_in_Lily_ui(name);
		if (index < 0)
		{
			return NAN;
		}
		int idx1 = str_index(name, '[');
		if (idx1 < 0)
		{
			return NAN;
		}
		int idx2 = str_index(name, ']');
		if (idx2 < 0)
		{
			return NAN;
		}
		name[idx2] = '\0';
		if (!str_is_numeric(name + idx1 + 1))
			return NAN;
		int offs = atoi(name + idx1 + 1);

		Field fields = li_fields;
		char type = fields[index].type;
		if (offs != 0 && isLower(type))
			return NAN;
		if (isUpper(type))
			type += 32; //to lower
		if (type == 'f')
		{
			val = ((float *)(fields[index].ref))[offs];
		}
		else if (type == 'd')
		{
			val = ((int *)(fields[index].ref))[offs];
		}
		else if (type == 's')
		{
			val = ((char *)(fields[index].ref))[offs];
		}
		else
		{
			return NAN;
		}
	}
	else if (str_is_numeric(name))
	{
		val = atof(name);
	}
	else
	{
		return NAN;
	}
	return val;
}
float shell_calculate_expression(char *rx)
{
	//step1: get objects attending caculation
	//step2: calculate one by one
	static char operaions[] = "*/+- ";
	// 2+a*3
	//2,3
	//+,*

	//get indexs of operations
	operaions[4] = '\0';
	Li_List opes_index = str_find(rx, operaions);
	int *indexs = list_content(opes_index, int);
	int n = opes_index->count;
	// get operations
	char *opes = (char *)malloc(n + 1);
#ifdef in_PC
	new_count++;
	cout << "#" << new_count << "#";
#endif // in_PC
	if (opes == NULL)
	{
		delete_list(opes_index);
		lily_error_msg = "malloc error";
		free(opes);
#ifdef in_PC
		new_count--;
		cout << "#" << new_count << "#";
#endif // in_PC
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
		lily_error_msg = "number not match";
		free(opes);
#ifdef in_PC
		new_count--;
		cout << "#" << new_count << "#";
#endif // in_PC
		return NAN;
	}
	Li_List values_ = valcues_of_fields(list);
	delete_list(list);
	if (values_ == NULL)
	{
		lily_error_msg = "field missing or not numeric";
		free(opes);
#ifdef in_PC
		new_count--;
		cout << "#" << new_count << "#";
#endif // in_PC
		return NAN;
	}
	float *values = list_content(values_, float);

	// start calculate on by on
	// e.g.:opes: ++*+/
	for (i = 0; operaions[i] != ' ';)
	{
		n = str_index(opes, operaions[i]);
		if (n < 0)
		{
			i++; // to next operation[* / + -]
			continue;
		}
		// operations[i], at index n in opes,
		// get nums at n and n+1, using operations[i] to calculate
		switch (i)
		{
		case 0: //*
			values[n] = values[n] * values[n + 1];
			break;
		case 1: // /
			if (values[n + 1] == 0.0)
			{
				delete_list(values_);
				free(opes);
#ifdef in_PC
				new_count--;
				cout << "#" << new_count << "#";
#endif // in_PC
				return NAN;
			}
			values[n] = values[n] / values[n + 1];
			break;
		case 2: // +
			values[n] = values[n] + values[n + 1];
			break;
		case 3: // -
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
	free(opes);
#ifdef in_PC
	new_count--;
	cout << "#" << new_count << "#";
#endif // in_PC
	return result;
}

int shell_cal_exp_cmd(int n, char **arg)
{
	float result;
	if (n == 2)
	{
		result = shell_calculate_expression(arg[1]);
		if (isnan(result))
		{
			return -1;
		}

		//if (result == NAN)return - 1;
		sprintf(tx, "%f\n", result);
		lily_out(tx);
	}
	else if (n == 3)
	{
		result = shell_calculate_expression(arg[2]);
		if (isnan(result))
			return -3;

		int index = search_field_in_Lily_ui(arg[1]);
		if (index < 0)
		{
			index = public_a_new_field(arg[1], 'f', 0.0f);
		}
		Field fields = li_fields;
		Field field = &fields[index];

		if (field->type == 'f')
		{
			*((float *)(field->ref)) = result;
		}
		else if (field->type == 'd')
		{
			*((int *)(field->ref)) = (int)result;
		}
		else
		{
			lily_error_msg = "field is not numeric";
			return -4;
		}

		lily_out(arg[1]);
		sprintf(tx, "=%f\n", result);
		lily_out(tx);
	}
	return 0;
}
// help cmds
// cmd1
int shell_do_cmd(char *rx)
{
	int hit = 0;
	int return_code;
	hit = search_cmd_in_Lily_ui(rx);
	if (hit < 0)
		return 0; //not hit, to next proess

	Cmd_def *cmd = (Cmd_def *)list_index(lily_ui.cmds, hit);
	Li_List list = str_split(rx, ' ');
	hit = list->count;
	string_ *para = (char **)(list->content);
	for (int i = 0; i < hit; i++)
	{
		if (para[i][0] != '$')
			continue;
		//replace para[i] to the field content
		int index = search_field_in_Lily_ui(para[i] + 1);
		if (index < 0)
		{
			delete_list(list);
			lily_error_msg = "\'$\' false used, field not exists";
			return -1; // index out
		}
		Field fields = li_fields;
		Field field = fields + index;
		//if (field->type != 's')
		//{
		//	delete_list(list);
		//	return -2;//type error
		//}
		//para[i] = ((Li_String)field->ref)->str;
		field_to_string(tx, field, 0);
		para[i] = tx; // (char*)field->ref;
	}
	return_code = (cmd->todo)(list->count, (char **)(list->content));
	delete_list(list);
	//if (return_code < 0)
	//{
	//	return return_code;
	//}
	//return 1;
	return return_code | shell_dos_done;
}
int shell_do_fun(char *rx)
{
	int hit1;
	char *ro = rx;
	hit1 = str_contains(rx, '=');
	Field assign_field = NULL;
	if (hit1)
	{
		rx[hit1 - 1] = '\0';
		int n = search_field_in_Lily_ui(rx);
		if (n < 0)
		{
			// rx[hit1 - 1] = '=';
			return 0;
		}
		assign_field = li_fields + n;
		rx += hit1;
	}
	int hit = search_fun_in_Lily_ui(rx);
	if (hit < 0)
	{
		// if (hit1)
		// {
		// 	*(rx - 1) = '=';
		// }
		return 0; //not hit, to next proess
	}
	Fun fun = &li_funs[hit];
	void *f = fun->ref;
	typedef float (*f0)();
	typedef float (*f1)(float);
	typedef float (*f2)(float, float);
	typedef float (*f3)(float, float, float);
	typedef float (*f4)(float, float, float, float);
	typedef float (*fi1)(int);
	typedef float (*fi2)(int, int);
	typedef float (*fi3)(int, int, int);
	typedef float (*fi4)(int, int, int, int);
	float result;
	if (fun->narg == 0)
	{
		result = ((f0)f)();
		if (assign_field != NULL)
		{
			switch (assign_field->type)
			{
			case 'f':
				*(float *)(assign_field->ref) = result;
				break;
			case 'd':
				*(int *)(assign_field->ref) = result;
				break;
			default:
				lily_error_msg = "field type dismatch";
				return -33;
			}
		}
		return 1;
	}
	str_replace_by_str(rx, "(),", ' ');
	Li_List list = str_split(rx, ' ');
	hit = list->count;
	string_ *para = (char **)(list->content);
	float *nums = (float *)(list->content);
	int error = 0;
	for (int i = 1; i < hit; i++) //skip the first one
	{
		if (str_is_numeric(para[i]))
		{
			float num = str_to_float(para[i]);
			nums[i] = num;
			continue;
		}
		//replace para[i] to the field content
		int index = search_field_in_Lily_ui(para[i]);
		if (index < 0)
		{
			error = -1;
			lily_error_msg = "field not exists";
			break;
		}
		Field field = li_fields + index;
		if (field->type == 'f')
		{
			nums[i] = *((float *)field->ref);
		}
		else if (field->type == 'd')
		{
			nums[i] = *((int *)field->ref);
		}
		else
		{
			error = -3;
			lily_error_msg = "field is not numeric";
			break;
		}
	}
	if (error != 0)
	{
		delete_list(list);
		return error;
	}

	int narg = list->count - 1;
	nums += 1;
	if (~(fun->narg) != 0) //
	{
		narg = fun->narg; // need narg paras
		if (list->count - 1 < narg)
		{
			static float nums_n[4];
			int i;
			for (i = 0; i < list->count - 1; i++)
				nums_n[i] = nums[i];
			if (i == 0) //no para provided
			{
				nums_n[i++] = 0.0f;
			}
			for (; i < narg; i++)
				nums_n[i] = nums_n[i - 1]; // fill with the last number
			nums = nums_n;
		}
	}

	if (fun->type == 'd') //int
	{
		switch (narg)
		{
		case 0:
			result = ((f0)f)();
			break;
		case 1:
			result = ((fi1)f)(nums[0]);
			break;
		case 2:
			result = ((fi2)f)(nums[0], nums[1]);
			break;
		case 3:
			result = ((fi3)f)(nums[0], nums[1], nums[2]);
			break;
		case 4:
			result = ((fi4)f)(nums[0], nums[1], nums[2], nums[3]);
			break;
		default:
			error = -4;
			lily_error_msg = "too many paras(>5)";
			break;
		}
	}
	else //float
	{
		switch (narg)
		{
		case 0:
			result = ((f0)f)();
			break;
		case 1:
			result = ((f1)f)(nums[0]);
			break;
		case 2:
			result = ((f2)f)(nums[0], nums[1]);
			break;
		case 3:
			result = ((f3)f)(nums[0], nums[1], nums[2]);
			break;
		case 4:
			result = ((f4)f)(nums[0], nums[1], nums[2], nums[3]);
			break;
		default:
			error = -4;
			lily_error_msg = "too many paras(>5)";
			break;
		}
	}

	if (assign_field != NULL)
	{
		switch (assign_field->type)
		{
		case 'f':
			*(float *)(assign_field->ref) = result;
			break;
		case 'd':
			*(int *)(assign_field->ref) = result;
			break;
		default:
			error = -23;
			lily_error_msg = "field type dismatch";
			break;
		}
	}
	delete_list(list);
	if (error < 0)
	{
		return error;
	}
	return 1;
}
// a = 1+1
// a+1
// 1+1
int shell_do_cal(char *rx)
{
	if (!str_contains_by_str(rx, (char *)"+-*/"))
		return 0;
	int ind_equ = str_index(rx, '=');
	if (ind_equ >= 0)
	{
		ind_equ++;
		while (rx[ind_equ] == ' ')
			ind_equ++;

		if (rx[ind_equ] == '-') // a = -12,like this
		{
			return 0;
		}
	}
	Li_List li = str_split(rx, '=');
	if (li->count > 2)
	{
		delete_list(li);
		return -1;
	}
	string_ *strs = list_content(li, string_);
	string_ args[3];
	args[0] = (char *)"cal";
	args[1] = strs[0];
	if (li->count == 2)
		args[2] = strs[1];
	int code = shell_cal_exp_cmd(li->count + 1, args);
	delete_list(li);
	if (code < 0)
		return code;
	return 1;
}
// a = 1
// a = b
// a, if a is a field
// a = b[i]
//a[d] = b[i]
int shell_do_fields(char *rx)
{
	//strcpy(tx, rx);
	char *cmd = rx;
	int index = str_index(cmd, '=');
	str_replace(cmd, '=', ' ');
	str_replace_by_str(cmd, "[]", ' ');
	// split maxmmun two string: a = b
	Li_List li = str_split(cmd, ' ');
	int n = li->count;
	if (n > 3) // contains too many '='
	{
		lily_error_msg = "too many \'=\'";
		delete_list(li);
		return -3;
	}

	char **args = (char **)li->content;
	char *val = NULL;

	int fed_index = 0;
	if (n == 3) //k[2]=a -> k 2  a
	{
		if (!str_is_numeric(args[1]))
		{
			lily_error_msg = "arg[1] is bad";
			delete_list(li);
			return -31;
		}
		fed_index = atoi(args[1]);
		list_remove_at(li, 1);
		n -= 1;
	}

	if (index > 0) // contsins '=', assign operation
	{
		val = args[1];
	}
	index = search_field_in_Lily_ui(args[0]);

	if (index < 0 && val == NULL) // hit miss
	{
		delete_list(li);
		return 0;
	}

	Field fields = (Field)(lily_ui.fields->content);
	Field first_field = NULL, sencond_field = NULL;
	if (index >= 0)
		first_field = &(fields[index]);

	//char pattern[] = "=%f\n";
	int code = 0;
	if (val != NULL) // '='
	{
		index = search_field_in_Lily_ui(val);
		if (index >= 0)
		{
			sencond_field = &fields[index];
		}
	}

	// creat new Field
	if (first_field == NULL && val != NULL) // e.g.: newField = a
	{
		if (sencond_field != NULL) // creat a new field and overwrite,e.g.: newField = oldFiled
		{
			switch (sencond_field->type)
			{
			case 's':
				code = public_a_new_string_field(cmd, (char *)(sencond_field->ref));
				break;
			case 'f':
				code = public_a_new_field(cmd, sencond_field->type, *((float *)(sencond_field->ref)));
				break;
			case 'd':
				code = public_a_new_field(cmd, sencond_field->type, *((int *)(sencond_field->ref)));
				break;
			default:
				break;
			}
		}
		else //e.g.: newField = 123
		{
			if (str_is_numeric(val))
			{
				float num = atof(val);
				code = public_a_new_field(cmd, 'f', num);
			}
			else
			{
				code = public_a_new_string_field(cmd, val);
			}
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
	else if (first_field != NULL && val != NULL) //reassign a field
	{
		if (sencond_field != NULL) // e.g.: f1 = f2
		{
			code = assign_field_from_field(first_field, sencond_field, fed_index);
		}
		else // e.g.: f1 = 123
		{
			code = assign_field_from_string(first_field, val, fed_index);
		}
		if (code < 0)
		{
			delete_list(li);
			return code;
		}
	}

	//show
	//pattern[2] = first_field->type;
	lily_out(first_field->name);
	if (fed_index)
	{
		sprintf(tx, "[%d]", fed_index);
		lily_out(tx);
	}
	tx[0] = '=';
	field_to_string(tx + 1, first_field, fed_index);
	lily_out(tx);
	lily_out("\n");
	delete_list(li);
	return 1;
}
int shell_do_notFound(char *rx)
{
	lily_out("\ano cmd:\"");
	lily_out(rx);
	lily_out("\"\n");
	return 1;
}

int assign_field_from_field(Field dst, Field source, int dst_offsets)
{
	if (dst->type == 's') //dst type is string
	{
		if (source->type == 's') // s->s
		{
			//return assign_li_string((Li_String)(dst->ref), ((Li_String)(source->ref))->str);
			return assign_string_field(dst, (char *)(source->ref));
		}
		// n-> s
		//delete_li_string((Li_String)(dst->ref));
		free(dst->ref);
#ifdef in_PC
		new_count--;
		cout << "#" << new_count << "#";
#endif // in_PC
		dst->type = source->type;
		dst->ref = malloc(sizeof(float)); // note 4 byte[float and int] is ok there
#ifdef in_PC
		new_count++;
		cout << "#" << new_count << "#";
#endif // in_PC
		memcpy(dst->ref, source->ref, sizeof(float));
		return 0;
	}
	// dst type is numeric
	//s -> n
	if (source->type == 's')
	{
		if ((dst->annotation)[0] == '_')
		{
			return -1; //protected
		}
		void *p = new_string_by((char *)(source->ref));
		if (p == NULL)
			return -2;
		free(dst->ref);
#ifdef in_PC
		new_count--;
		cout << "#" << new_count << "#";
#endif // in_PC \
	//dst->ref = new_li_string_by(((Li_String)(source->ref))->str);
		dst->ref = p;
		dst->type = source->type;
		return 0;
	}
	//n -> n
	if ((dst->annotation)[0] == '_' && dst->type != source->type)
	{
		li_error("protected,type not match", -1);
	}

	dst->type = source->type;
	memcpy(dst->ref, source->ref, sizeof(float));
	memcpy((void *)((char *)(dst->ref) + dst_offsets * 4), source->ref, sizeof(float));
	return 0;
}
int assign_field_from_string(Field dst, char *val, int dst_offset)
{
	float val_f;
	int val_i;
	switch (dst->type)
	{
	case 's':
		return assign_string_field(dst, val);
		break;
	case 'S':
		return assign_li_string((Li_String)(dst->ref), val);
		break;
	case 'd':
	case 'f':
		if (str_is_numeric(val))
		{
			if (dst->type == 'd')
			{
				val_i = atoi(val);
				((int *)(dst->ref))[dst_offset] = val_i;
			}
			else
			{
				val_f = atof(val);
				((float *)(dst->ref))[dst_offset] = val_f;
			}
		}
		else // s -> n
		{
			if ((dst->annotation)[0] == '_')
			{
				return -1;
			}
			free(dst->ref);
#ifdef in_PC
			new_count--;
			cout << "#" << new_count << "#";
#endif // in_PC \
	//dst->ref = new_li_string_by(val);
			dst->ref = new_string_by(val);
			dst->type = 's';
		}
		break;
	default:
		break;
	}
	return 0;
}

// use relloc to reapply memory
int assign_string_field(Field fed, const char *val)
{
	if (fed->annotation[0] == '_')
		return -2; //protected cannot write
	if (fed->type != 's')
		return -3; //type error
	int n = strlen(val);
	void *p;
	if (fed->ref != NULL)
		p = realloc(fed->ref, n + 1);
	else
	{
		p = malloc(n + 1); // init
#ifdef in_PC
		new_count++;
		cout << "#" << new_count << "#";
#endif // in_PC
	}

	if (p == NULL)
		return -1;
	fed->ref = p;
	strcpy((char *)(p), val);
	return 0;
}

void field_to_string(char *tx, Field fed, int off)
{
	switch (fed->type)
	{
	case 's':
		if (fed->ref != NULL)
			strcpy(tx, (char *)(fed->ref));
		else
			tx[0] = '\0';
		break;
	case 'f':
		sprintf(tx, "%f", ((float *)(fed->ref))[off]);

		break;
	case 'd':
		_itoa(((int *)(fed->ref))[off], tx, 10);
		// sprintf(tx, "%d", ((int *)(fed->ref))[off]);
		break;
	case 'S':
		strcpy(tx, ((Li_String)(fed->ref))->str);
		break;
	default:
		break;
	}
}

//a[n]=b[m]
// numericly assignation
int assign_var_from_var(Var dst, Var src, int at_dst, int at_src)
{

	// 	if (NorFedType(dst) == 'c') //dst type is string
	// 	{
	// 		if (source->type == 'C') // s->s
	// 		{
	// 			return assign_string_field(dst, (char *)(source->ref));
	// 		}
	// 		// n-> s
	// 		free(dst->ref);
	// #ifdef in_PC
	// 		new_count--;
	// 		cout << "#" << new_count << "#";
	// #endif // in_PC
	// 		dst->type = source->type;
	// 		dst->ref = malloc(sizeof(float)); // note 4 byte[float and int] is ok there
	// #ifdef in_PC
	// 		new_count++;
	// 		cout << "#" << new_count << "#";
	// #endif // in_PC
	// 		memcpy(dst->ref, source->ref, sizeof(float));
	// 		return 0;
	// 	}
	// 	//n -> n
	// 	if ((dst->annotation)[0] == '_' && NorFedType(dst) != NorFedType(source))
	// 	{
	// 		li_error("protected,type not match", -1);
	// 	}
	// 	if (source->type == 'c' && dst->type != 'c')
	// 	{
	// 		li_error("string type not supported", -2);
	// 	}
	// #define toType(d, t) ((t *)(d->ref))

	float vs;
	void *vp;
	vp = get_value_of_var(src, at_src);

	switch (src->type)
	{
	case 'f':
		assign_f_from_void(vs, vp);
		break;
	case 'd':
	case 'c':
		vs = void_to_int(vp);
		break;
	default:
		li_error("undifined type", -3);
		break;
	}

	switch (dst->type)
	{
	case 'f':
		set_value_of_var(dst, at_dst, to_voidf(vs));
		break;
	case 'd':
		set_value_of_var(dst, at_dst, to_void(((int)vs)));
		break;
	case 'c':
		set_value_of_var(dst, at_dst, to_void(((char)vs)));
		break;
	default:
		li_error("undifined type", -4);
		break;
	}

	return 0;
}
// val: a string like '123.4' for numeric var
// or any string for a char* var
int assign_var_from_string(Var dst, char *val, int at)
{
	float val_f;
	int val_i;
	int erc = 0;
	switch (dst->type)
	{
	case 'c':
		if (at)
		{
			li_error("type error", -34);
		}
		erc = assign_string_var(dst, val);
		break;
	case 'd':
	case 'f':
		if (str_is_numeric(val))
		{
			if (dst->type == 'd')
			{
				val_i = atoi(val);
				erc = set_value_of_var(dst, at, to_void(val_i));
			}
			else
			{
				val_f = atof(val);
				erc = set_value_of_var(dst, at, to_voidf(val_f));
			}
		}
		else // s -> n
		{
			li_error("bad assign", -14);
			// 			if (dst->isb)
			// 			{
			// 				return -1;
			// 			}
			// 			free(dst->ref);
			// #ifdef in_PC
			// 			new_count--;
			// 			cout << "#" << new_count << "#";
			// #endif // in_PC
			// 			dst->ref = new_string_by(val);
			// 			dst->type = 's';
		}
		break;
	default:
		break;
	}
	return erc;
}
int assign_string_var(Var var, const char *val)
{
	if (var->isb)
		return -2; //protected cannot write
	if (var->type != 'c' || !var->isa)
		return -3; //type error
	int n = strlen(val);
	void *p;
	if (var->ref != NULL)
		p = realloc(var->ref, n + 1);
	else
	{
		p = malloc(n + 1); // init
#ifdef in_PC
		new_count++;
		cout << "#" << new_count << "#";
#endif // in_PC
	}

	if (p == NULL)
		return -1;
	var->ref = p;
	strcpy((char *)(p), val);
	var->len = n + 1;
	return 0;
}
// return value of var[at] in string type
// note: if var is in type 'c' and is a array,
//   it will return whole string, not a number in string
void var_to_string(char *tx, Var var, int at)
{
	if (at & !var->isa)
	{
		*tx = '\0'; //index error
		return;
	}
	if (var->type == 'c' && var->isa)
	{
		if (at >= var->len)
		{
			*tx = '\0'; //index out
			return;
		}
		char *p = cast(var->type, char *);
		strcpy(tx, p + at);
		return;
	}
	void *val = get_value_of_var(var, at);
	float vf;
	switch (var->type)
	{
	case 'f':
		assign_f_from_void(vf, val);
		sprintf(tx, "%f", vf);
		break;
	case 'd':
	case 'c':
		_itoa(void_to_int(val), tx, 10);
		// sprintf(tx, "%d", ((int *)(fed->ref))[off]);
		break;
	default:
		break;
	}
}
