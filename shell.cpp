#ifdef in_PC
#include"Lily_help.h"
#include <math.h>
extern int new_count;

#else
#include"Lily_help.h"
#include <math.h>
#include <stdio.h>

#endif // in_PC


typedef char *string_;

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
	Li_List values_ = valcues_of_vars(list);
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
			if (values[n + 1] == 0.0f)
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

		int index = search_var_in_Lily_ui(arg[1]);
		
		int at;
		if (index < 0)
		{
			index = public_a_new_var(arg[1], 'f', 0);
		}
		else
		{
			at = ebrace_value_from_string(arg[1]);
			if (at < 0)
			{
				li_error("index error", -53);
			}
		}
		Var var = li_vars + index;
		int rtc = 0;
		if (var->type == 'f')
		{
			rtc = set_value_of_var(var, at, to_voidf(result));
		}
		else if (var->type == 'd')
		{
			rtc = set_value_of_var(var, at, to_void((int)result));
		}
		else
		{
			lily_error_msg = "field is not numeric";
			return -4;
		}
		if (rtc < 0)
		{
			li_error("assign error", rtc);
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
	char*tx1 = tx;
	for (int i = 0; i < hit; i++)
	{
		if (para[i][0] != '$')
			continue;
		//replace para[i] to the field content
		int index = search_var_in_Lily_ui(para[i] + 1);
		if (index < 0)
		{
			delete_list(list);
			lily_error_msg = "\'$\' false used, field not exists";
			return -1; // index out
		}
		int at = ebrace_value_from_string(para[i]);
		var_to_string(tx1,li_vars+index,at);
		
		para[i] = tx1; // (char*)field->ref;
		tx1 += strlen(tx1)+1;
	}
	return_code = (cmd->todo)(list->count, (char **)(list->content));
	delete_list(list);
	return return_code | shell_dos_done;
}
int shell_do_fun(char *rx)
{
	int hit1;
	hit1 = str_contains(rx, '=');
	Var assign_field = NULL;
	if (hit1)
	{
		rx[hit1 - 1] = '\0';
		int n = search_var_in_Lily_ui(rx);
		if (n < 0)
		{
			return 0;
		}
		assign_field = li_vars + n;
		rx += hit1;
	}
	int hit = search_fun_in_Lily_ui(rx);
	if (hit < 0)
	{
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
		nums[i] = value_from_string_or_var(para[i]);
		if(isnan(nums[i]))
		{
			error = -1;
			lily_error_msg = "bad value";
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


int shell_do_notFound(char *rx)
{
	lily_out("\ano cmd:\"");
	lily_out(rx);
	lily_out("\"\n");
	return 1;
}


//a[n]=b[m]
// numericly assignation
int assign_var_from_var(Var dst, Var src, int at_dst, int at_src)
{
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
		if (at) // must start from head
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
	if (at && !var->isa)
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
		char *p = cast(var->ref, char *);
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

Li_List valcues_of_vars(Li_List list)
{
	typedef char *string_;
	string_ *item_strs = list_content(list, string_);
	int n = list->count;
	Li_List values = new_li_cap(float, n);
	for (int i = 0; i < n; i++)
	{
		string_ name = item_strs[i];
		float val = value_from_string_or_var(name);
		if (isnan(val))
		{
			delete_list(values);
			return NULL;
		}
		li_add(values, val);
	}
	return values;
}

/*get value from a string,support array index, only the value not string
support like:
a
123.4
a[1], field type 'f,d,c'
a[b[n]]
no \w or space in name
return NAN if in error*/
float value_from_string_or_var(string_ name)
{
	float val;
	int idx1 = str_index(name, '[');
	int idx2 = str_index_right(name, ']');
	if (idx1 > 0 && idx2 > 0) //a[i]
	{
		name[idx1] = '\0';
		name[idx2] = '\0';
	}
	if (str_is_name(name))
	{
		int index = search_var_in_Lily_ui(name);

		if (index < 0)
		{
			return NAN;
		}

		int at = 0;
		float vid;
		if (idx1 > 0 && idx2 > 0) //a[i]
		{
			float vid = value_from_string_or_var(name + idx1 + 1);
			if (isnan(vid))
				return NAN;
			if (vid - cast(vid, int) != 0.0f) //index must be a intergal
				return NAN;
			at = vid;
		}
		else if (!(idx1 < 0 && idx2 < 0))
			return NAN;

		Var var = li_vars + index;

		void *p = get_value_of_var(var, at);

		switch (var->type)
		{
		case 'f':
			assign_f_from_void(vid, p);
			break;
		case 'd':
		case 'c':
			vid = void_to_int(p);
			break;
		default:
			break;
		}
		return vid;
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

//return the index in "[]",
//like 'a[1]', a[b[n]]
// return -1 for error
// return -2 if index is not a intergal
// !note:if no [], 0 will be returned
int ebrace_value_from_string(string_ name)
{
	int at=0;
	int idx1 = str_index(name, '[');
	int idx2 = str_index_right(name, ']');

	if (idx1 > 0 && idx2 > 0) //a[i]
	{
		name[idx1] = '\0';
		name[idx2] = '\0';
		float vid = value_from_string_or_var(name + idx1 + 1);
		if (isnan(vid))
			return -1;
		if (vid - cast(vid, int) != 0.0f) //index must be a intergal
			return -2;
		at = vid;
	}
	return at;
}

// assign, create, and show vars
// >>a=1, a[2]=b, a[b[c]]=d, a=hello, a[4]
// type conversion
// >>a=123 $C, a $f,a $F, b $C

int shell_do_var(char *rx)
{
	char *cmd = rx;
	str_wrap(rx, ' '); //remove white spaces

	int equ_idx = str_index(cmd, '=');
	int typ_idx = str_index(cmd, '$');
	char *rx1 = rx, *rx2 = NULL; // rx='a=123', rx1='a', rx2='123'

	if (typ_idx > 0)
	{
		cmd[typ_idx] = '\0';
		typ_idx = cmd[typ_idx + 1]; // Type
	}

	if (equ_idx > 0)
	{
		cmd[equ_idx] = '\0';
		rx2 = cmd + equ_idx + 1;
	}

	int var1_idx = search_var_in_Lily_ui(rx1);

	if (var1_idx < 0 && rx2 == NULL) // hit miss, >>adf
	{
		return 0; //do not dealing
	}

	Var var1 = NULL;

	if (var1_idx >= 0)
		var1 = li_vars + var1_idx;

	int code = 0;
	float rx2_value = NAN;
	if (rx2 != NULL)
		rx2_value = value_from_string_or_var(rx2);

	int b1, b2;
	b1 = str_index(rx1, '[');
	b2 = str_index_right(rx1, ']');
	float brace_val = NAN;
	if (b1 > 0 && b2 > 0) //array
	{
		rx1[b2] = '\0';
		rx1[b1] = '\0';
		brace_val = value_from_string_or_var(rx1 + b1 + 1);
		if (isnan(brace_val) || brace_val - (int)brace_val != 0.0f)
		{
			li_error("index must be a intergal", -32);
		}
	}

	// creat new Field
	if (var1 == NULL && rx2 != NULL) // e.g.: newField = a
	{
		//determine the type of var1
		rx2_value = value_from_string_or_var(rx2);
		char type;
		if (typ_idx > 0)
			type = typ_idx;
		else if (isnan(rx2_value))
			type = 'C'; //string type
		else
			type = 'f';

		if (type != 'C' && isnan(rx2_value))
			return 12; // error or not match

		if (type == 'C')
		{
			code = public_new_vars_array(rx1, type, rx2, strlen(rx2) + 1);
		}
		else //determine to create a array or var
		{
			//aessert rx2_value is not NAN
			if (!isnan(brace_val)) //array
			{
				if (toLower(type) == 'f')
					code = public_new_vars_array(rx1, type, to_voidf(rx2_value), brace_val);
				else
					code = public_new_vars_array(rx1, type, to_void((int)rx2_value), brace_val);
				brace_val = 0.0f;
			} //single
			else
			{
				if (type == 'f')
					code = public_a_new_var(rx1, type, to_voidf(rx2_value));
				else if (type == 'd' || type == 'c')
					code = public_a_new_var(rx1, type, to_void((int)rx2_value));
				else
				{
					li_error("type error", -53);
				}
			}
		}

		if (code < 0)
		{
			return code;
		}
		else
		{
			var1 = li_vars + code;
		}
	}
	else if (var1 != NULL && rx2 != NULL) //reassign a field
	{
		// a
		if (isUpper(typ_idx) && !var1->isa) //assign as a array
		{
			//but var is not an array
			li_error("bad type cover", -17);
		}

		if (typ_idx > 0)
		{
			var1->type = toLower(typ_idx);
		}

		if (isnan(rx2_value))
		{
			if (var1->type == 'c' && var1->isa)
				assign_string_var(var1, rx2);
			else
				code = -33; //assign error
		}
		else // assign number
		{
			if (isnan(brace_val))
				brace_val = 0.0f;
			if (var1->type == 'f')
				code = set_value_of_var(var1, brace_val, to_voidf(rx2_value));
			else if (var1->type == 'd' || var1->type == 'c')
				code = set_value_of_var(var1, brace_val, to_void((int)rx2_value));
			else
			{
				li_error("type error", -63);
			}
		}
		if (code < 0)
		{
			return code;
		}
	}

	//show
	lily_out(var1->name);
	if (b1 > 0 && b2 > 0)
	{
		sprintf(tx, "[%d]", cast(brace_val, int));
		lily_out(tx);
	}
	tx[0] = '=';
	if (isnan(brace_val))
		var_to_string(tx + 1, var1, 0);
	else
		var_to_string(tx + 1, var1, brace_val);
	lily_out(tx);
	lily_out("\n");
	return 1;
}
