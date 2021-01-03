#include <string.h>
#include "Lily.h"
#include "Lily_basic.h"
char str_startwith(char* s, char* cmd)
{
	int i, n;
	n = strlen(cmd);

	if (strlen(s) < n)return 0;

	for (i = 0; i < n; i++)
		if (s[i] != cmd[i])return 0;

	return 1;
}

/*
s :rx content, setv(100,0);
cmd: cmd str, setv
consider ':' as a end of string as well
*/
char str_equal(char* s, char* cmd)
{
	if (s[0] == '\0') return 0;

	int i = 0;
	char a, b;

	do
	{
		a = s[i];
		b = cmd[i];
		i++;

		if (a != b)return 0;
	} while (s[i] != '\0' && cmd[i] != '\0' && cmd[i] != ':'); // the next char is not \0

	a = s[i];
	b = cmd[i];

	if (a == '\0' && b != '\0')return 0;

	if ('a' <= a && a <= 'z')return 0;

	return i;
}
int str_index(char* s, char c) //return 0 or 1
{
	int i;
	int n = strlen(s);

	for (i = 0; i < n; i++)
		if (s[i] == c)return i;

	return -1;
}
char str_lower(char* str)
{
	if (str == NULL)
		return 0;

	char* p = str;

	while (*p != '\0')
	{
		if (*p >= 'A' && *p <= 'Z')
			*p = (*p) + 0x20;

		p++;
	}

	return p - str;
}

char str_replace(char* s, char from, char to)
{
	char times = 0;
	int i;

	for (i = 0; s[i] != '\0'; i++)
	{
		if (s[i] == from)
		{
			s[i] = to;
			times++;
		}
	}

	return times;
}
// add the num to a string
// return: length of num
char int_to_string(int n, char* s)
{
	if (n == 0)
	{
		s[0] = '0';
		return 1;
	}
	char add = 0;

	if (n < 0)
	{
		n *= -1;
		s[0] = '-';
		s++;
		add = 1;
	}
	int i;
	for (i = 0; n > 0; i++, n /= 10)
	{
		s[i] = n % 10 + '0';
	}
	int j;
	char a;
	for (j = 0; j < i / 2; j++)
	{
		a = s[j];
		s[j] = s[i - j - 1];
		s[i - j - 1] = a;
	}
	return i + add;
}

float* get_nums_from_rx(char* rs, int* length_back)
{
	static float nums[10];
	int i, k=0;
	float j = 10.0f;
	bool negative=false;
	j = 10.0f;
	bool has_decimal=false;
	float integer = 0.0f, decimal = 0.0f;
	char c;
	bool last_is_numeric=false;
	for (i = 0; rs[i] != '\0'; i++)
	{
		c = rs[i];
		if ('0' <= c && c <= '9') //number
		{
			if (!last_is_numeric)//at the begin
			{
				integer = (c - '0');
				decimal = 0;
				has_decimal = false;
				j = 10;
			}
			else if (has_decimal)//go on
			{
				decimal += (float)(c - '0') / j;
				j *= 10.0f;
			}
			else
			{
				integer *= 10.0f;
				integer += (c - '0');
			}
			last_is_numeric = True;
			continue;
		}
		// not numeric
		else if (c == '.' && last_is_numeric)//xxx.
		{
			has_decimal = True;
			last_is_numeric = true;
			continue;
		}
		else if (c == '-')// -
		{
			negative = !negative;//clear in end
		}
		else if (last_is_numeric)//end
		{
			nums[k] = decimal + integer;
			if (negative)
			{
				nums[k] *= -1;
				negative = false;
			}
			k++;

		}
		last_is_numeric = False;
	}

	if (last_is_numeric) //end
	{
		nums[k] = decimal + integer;
		if (negative)
			nums[k] *= -1;
		k++;
	}

	if (k == 0)
	{
		*length_back = 0;
		return nums;
	}

	*length_back = k;
	return nums;
}

// assert a word starts with a letter a-z
char** get_paras_from_rx(char* rx, int* len)
{
	static char s[5][8];//5 rows, 7 chars a string maximum
	static char* p[5];
	int i, j;
	bool is_last_a_letter;
	for (i = 0; i < 5; i++)
		p[i] = s[i];
	i = 0;
	//while (*rx != '\0')//skip the cmd
	//{
	//	if (*rx < 'a' || *rx>'z')//not a letter
	//		break;
	//	rx++;
	//}
	while (*rx != '\0')//skip the cmd
	{
		if ('a' <= *rx && *rx <= 'z')// a letter
			break;
		rx++;
	}

	if (*rx == '\0')
	{
		*len = 0;
		return p;
	}
	i = 0;
	j = 0;
	is_last_a_letter = false;
	while (*rx != '\0')
	{
		if ('a' <= *rx && *rx <= 'z' ||*rx=='_')// a letter
		{
			if (!is_last_a_letter)//begin
			{
				j = 0;
			}
			//go on
			s[i][j++] = *rx;
			is_last_a_letter = true;
			rx++;
			continue;
		}
		else if (is_last_a_letter)//end
		{
			s[i][j] = '\0';
			i++;
		}
		rx++;
		is_last_a_letter = false;
	}
	
	s[i][j] = '\0';
	if (is_last_a_letter)//end
	{
		i++;
	}
	*len = i;
	return p;
}

void limit_to(float* x, float max)
{
	if (*x > max)
		*x = max;
	else if (*x < -max)
		*x = -max;
}
void limit_from_to(float* x, float lower, float upper)
{
	if (*x < lower)
		*x = lower;
	else if (*x > upper)
		*x = upper;
}


char init_list(List* list, unsigned int unit_size2, unsigned int init_cap)
{
	list->type_size = unit_size2;
	list->cap = 0;
	list->content = (char*)calloc(init_cap, unit_size2);
	if (list->content == NULL)return 1;
	list->cap = init_cap;
	list->count = 0;
	return 0;
}
List* new_list(unsigned int type, unsigned int init_cap)
{
	List* list = (List*)malloc(sizeof(List));
	if (list == NULL)return list;
	list->count = 0;
	list->type_size = type;
	list->content = (char*)calloc(init_cap, type);
	if (list->content == NULL)
		list->cap = 0;
	else
		list->cap = init_cap;

	return list;
}
char list_recap(List* list, unsigned int new_cap)
{
	void* p;
	p = realloc(list->content, new_cap * list->type_size);
	if (p == NULL)
	{
		return List_no_space;
	}
	list->content = (char*)p;
	list->cap += 5;
	return List_normal_code;
}
char list_add(List* list, void* c)
{
	if (list->count >= list->cap)
		if (list_recap(list, list->cap + 5))return List_no_space;
	memcpy((char*)(list->content) + list->count * list->type_size, c, list->type_size);
	list->count++;
	return List_normal_code;
}
char list_remove_at(List* list, unsigned int index)
{
	if (index >= list->count)return List_index_overflow;
	char* i, * n, * p;
	list->count--;
	n = list->content + list->count * list->type_size;
	i = list->content + index * list->type_size;
	p = i + list->type_size;
	for (; i < n; i++, p++)*i = *p;
	//while (i < n) *(i++) = *(p++);
	return List_normal_code;
}
bool list_item_equal(void* item_a, void* item_b, unsigned int type)
{
	int i;
	char* item1 = (char*)item_a;
	char* item2 = (char*)item_b;
	for (i = 0; i < type; i++)
	{
		if (item1[i] != item2[i])return false;
	}
	return true;
}
char list_remove(List* list, void* item)
{
	int i;
	char* p = list->content;
	for (i = 0; i < list->count; i++, p += list->type_size)
		if (list_item_equal(item, p, list->type_size))break;
	if (i == list->count)return List_not_found;
	list_remove_at(list, i);
	return List_normal_code;
}
int list_find(List* list, void* item)
{
	int i;
	char* p = list->content;
	for (i = 0; i < list->count; i++, p += list->type_size)
		if (list_item_equal(item, p, list->type_size))return i;
	return -1;
}
char delete_list(List* list)
{
	if (list == NULL)return List_null_ptr;
	if (list->content != NULL)
		free(list->content);
	free(list);
	return List_normal_code;
}