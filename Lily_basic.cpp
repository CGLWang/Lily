#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "Lily.h"
#include "Lily_basic.h"
#ifdef in_PC
#include<iostream>
using namespace std;
int new_count = 0;
#endif // in_PC

char str_startwith(char* s, char* cmd)
{
	int i, n;
	n = strlen(cmd);

	if (strlen(s) < n)return 0;

	for (i = 0; i < n; i++)
		if (s[i] != cmd[i])return 0;

	return 1;
}

int str_contains(char* s, char c)
{
	for (int i = 0; s[i] != '\0'; i++)
		if (s[i] == c)return i+1;
	return 0;
}
int str_contains_by_str(char* s, char* c)
{
	for (int i = 0; s[i] != '\0'; i++)
		if(str_contains(c,s[i]))return 1;
		//if (s[i] == c)return 1;
	return 0;
}

char str_equal(const char* s, const char* cmd)
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
	if ('0' <= a && a <= '9')return 0;
	return i;
}
// return -1 if not found
int str_index(char* s, char c) 
{
	int i;
	for (i = 0; s[i] !='\0'; i++)
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
char str_replace_by_str(char* s,const char* from, char to)
{
	char times = 0;
	int i;

	for (i = 0; s[i] != '\0'; i++)
	{
		const char* p = from;
		while (*p!='\0')
		{
			if (*p == s[i])
			{
				s[i] = to;
				times++;
				break;
			}
			p++;
		}
	}

	return times;
}
// '-123.45', '+13.890'
int str_is_numeric(char* name)
{
	if (!isD(*name) && *name != '+'&& *name != '-')return 0;
	if (!isD(*name))
	{
		name++;
		if (*name == '\0')return 0;
	}
	else
		name++;

	char lastIsDecimal = 0;
	for(;*name != '\0';name++)
	{
		if (isD(*name))
		{
			lastIsDecimal = 0;
			continue;
		}
		else if(*name=='.')
		{
			if (lastIsDecimal)return 0;
			lastIsDecimal = 1;
			continue;
		}

		return 0;
	}
	return 1;
}
// a, _ah_haha_, _label_text3
int str_is_name(char* name)
{
	if (!isA(*name) && *name != '_')return 0;
	if (*name == '_')
	{
		name++;
		if (*name == '\0')return 0;
	}else
		name++;
	

	while (*name!='\0')
	{
		if (!isA(*name) && !isD(*name) && *name != '_')return 0;
		name++;
	}
	return 1;
}
//in place operation, 
// note: delete list returned in time
Li_List str_split(char* str, char split_char)
{
	Li_List list = new_list(sizeof(char*), 4);
	if (*str == '\0')
		return list;
	char* last;// , * next;
	char* now = str;
	if (*now != split_char)
	{
		list_add(list, &str);
	}
	last = now;
	now++;
	// next = now;
	while (*now != '\0')
	{
		if (*last == split_char && *now != split_char)
		{
			li_add(list, now);
			/**last = '\0';*/
		}
		if (*last != split_char && *now == split_char)
		{
			*now = '\0';
			last = &split_char;
			now++;// = next;
			//next++;
			continue;
		}
		
		last = now;
		now++; //= next;
		//next++;
	}
	return list;
}
//in place operation, 
// note: delete list returned in time
Li_List str_split_by_str(char* str, char* split_char)
{
	Li_List list = new_list(sizeof(char*), 4);
	if (*str == '\0')
		return list;
	char* last;//, * next;
	char* now = str;
	if(!str_contains(split_char, *now))
	//if (*now != split_char)
	{
		list_add(list, &str);
	}
	last = now;
	now++;
	// next = now;
	while (*now != '\0')
	{
		if (str_contains(split_char, *last) && !str_contains(split_char, *now))
		//if (*last == split_char && *now != split_char)
		{
			li_add(list, now);
			/**last = '\0';*/
		}
		if (!str_contains(split_char, *last) && str_contains(split_char, *now))
		//if (*last != split_char && *now == split_char)
		{
			*now = '\0';
			last = split_char;
			now++;// = next;
			//next++;
			continue;
		}

		last = now;
		now++; //= next;
		//next++;
	}
	return list;
}
// note: delete list returned in time
// find indexs of chars in find in string s
//e.g. str_find("1+2*3/4", "+-*/")
// return [1,3,5]
Li_List str_find(char* s, char* find)
{
	Li_List list = new_list(sizeof(int), 4);
	if (list == NULL)return list;
	for(int i = 0; s[i]!='\0';i++)
	{
		if (str_contains(find, s[i]))
			li_add(list, i);
	}
	return list;
}
Li_List str_find_sort(char* s, char* find)
{
	Li_List list = new_list(sizeof(int), 4);
	if (list == NULL)return list;
	for (int i = 0; find[i] != '\0'; i++)
	{
		for(int j=0;s[j]!='\0';j++)
			if(s[j]==find[i])
				li_add(list, j);
	}
	return list;
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
	s[i + add] = '\0';
	return i + add;
}

float str_to_float(char* s)
{
	int integer = 0;
	float decimal = 0.0f;
	float kd = 0.1;
	int in_decimal = 0;
	char is_neg = 0;
	if (*s == '-')
	{
		is_neg = 1;
		s++;
	}
	else if (*s == '+')
	{
		s++;
	}
	for (; *s != '\0'; s++)
	{
		if (*s == '.')
		{
			in_decimal = 1;
			continue;
		}
		if (!isD(*s))return 0.0f;
		if (in_decimal)
		{
			float d = *s - '0';
			
			decimal += d * kd;
			kd /= 10.0f;
		}
		else
		{
			int d = *s - '0';
			integer *= 10;
			integer += d;
		}
	}
	if (is_neg)
	{
		return -(integer + decimal);
	}
	else
		return (integer + decimal);
}
float* get_nums_from_rx(char* rs, int* length_back)
{
	static float nums[10];
	int i, k = 0;
	float j = 10.0f;
	bool negative = false;
	j = 10.0f;
	bool has_decimal = false;
	float integer = 0.0f, decimal = 0.0f;
	char c;
	bool last_is_numeric = false;
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
		if ('a' <= *rx && *rx <= 'z' || *rx == '_')// a letter
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

//int li_count = 0;
int init_list(Lily_List* list, unsigned int unit_size2, unsigned int init_cap)
{
	list->type_size = unit_size2;
	list->cap = 0;
	list->content = (char*)calloc(init_cap, unit_size2);
#ifdef in_PC
	new_count++;
	cout << "#" << new_count << "#";
#endif // in_PC
	if (list->content == NULL)return 1;
	list->cap = init_cap;
	list->count = 0;
	return 0;
}
Lily_List* new_list(unsigned int type, unsigned int init_cap)
{
	//li_count++;
	//static char tx[30];
	//sprintf(tx,"+>%d\n",li_count);
	//lily_out(tx);

	Lily_List* list = (Lily_List*)malloc(sizeof(Lily_List));
#ifdef in_PC
	new_count++;
	cout << "#" << new_count << "#";
#endif // in_PC
	if (list == NULL)return list;
	list->count = 0;
	list->type_size = type;
	list->content = (char*)calloc(init_cap, type);
#ifdef in_PC
	new_count++;
	cout << "#" << new_count << "#";
#endif // in_PC
	if (list->content == NULL)
		list->cap = 0;
	else
		list->cap = init_cap;

	return list;
}
int list_recap(Lily_List* list, unsigned int new_cap)
{
	void* p;
	p = realloc(list->content, new_cap * list->type_size);
	if (p == NULL)
	{
		return -1;
	}
	list->content = (char*)p;
	list->cap = new_cap;
	return 0;
}
int list_add(Lily_List* list, void* c)
{
	if (list->count >= list->cap)
		if (list_recap(list, list->cap + 5))return -1;
	memcpy((char*)(list->content) + list->count * list->type_size, c, list->type_size);
	list->count++;
	return list->count - 1;
}
int list_remove_at(Lily_List* list, unsigned int index)
{
	if (index >= list->count)return -1;
	char* i, * n, * p;
	list->count--;
	n = list->content + list->count * list->type_size;
	i = list->content + index * list->type_size;
	p = i + list->type_size;
	for (; i < n; i++, p++)*i = *p;
	//while (i < n) *(i++) = *(p++);
	return 0;
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
int list_remove(Lily_List* list, void* item)
{
	int i;
	char* p = list->content;
	for (i = 0; i < list->count; i++, p += list->type_size)
		if (list_item_equal(item, p, list->type_size))break;
	if (i == list->count)return -1;
	list_remove_at(list, i);
	return 0;
}
int list_find(Lily_List* list, void* item)
{
	int i;
	char* p = list->content;
	for (i = 0; i < list->count; i++, p += list->type_size)
		if (list_item_equal(item, p, list->type_size))return i;
	return -1;
}
int delete_list(Lily_List* list)
{
	if (list == NULL)return -1;
	if (list->content != NULL)
		free(list->content);
	free(list);
#ifdef in_PC
	new_count -= 2;
	cout << "#" << new_count << "#";
#endif // in_PC
	return 0;
}

//depracted 
// use new_string_by to creat sample string field
Li_String new_li_string_by(char* str)
{
	Li_String li_string = (Li_String)malloc(sizeof(Li_String_def));
	if (li_string == NULL)return NULL;
	int n = strlen(str);
	li_string->str =(char*) malloc(n+1);
#ifdef in_PC
	new_count += 2;
	cout << "#" << new_count << "#";
#endif // in_PC
	if (li_string->str == NULL)
	{
		free(li_string);
#ifdef in_PC
		new_count--;
		cout << "#" << new_count << "#";
#endif // in_PC
		return NULL;
	}
	strcpy(li_string->str, str);
	li_string->length = n;
	return li_string;
}
char* new_string_by(char* str)
{
	int n = strlen(str) + 1;
	void *p = malloc(n);
#ifdef in_PC//start malloc(.*);\r\n#if
	new_count++;
	cout << "#" << new_count << "#";
#endif // in_PC
#ifdef in_debug
	if (p == NULL)lily_out("new_string_by failed");
#endif // in_debug

	if (p == NULL)return NULL;
	strcpy((char*)p, str);
	return (char*)p;
}

void delete_li_string(Li_String li)
{
	free(li->str);
	free(li);
#ifdef in_PC
	new_count -= 2;
	cout << "#" << new_count << "#";
#endif // in_PC
}
int assign_li_string(Li_String li, char* source)
{
	int n = strlen(source);
	if (n > li->length)
	{
		void* p = realloc(li->str, n+1);
		if (p == NULL)
		{
			return -1;
		}
		li->str = (char*)p;
	}
	strcpy(li->str, source);
	li->length = n;
	return n;
}

