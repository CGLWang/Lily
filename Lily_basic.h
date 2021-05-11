#pragma once
#define isUpper(c) ('A' <= c && c <= 'Z')
#define isLower(c) ('a' <= c && c <= 'z')
#define toUpper(c) (c&~0x20)
#define toLower(c) (c|0x20)
#define reLU(c) (c^0x20)

#define isA(c) (isLower(c)||isUpper(c))
#define isD(c) ('0'<=c&&c<='9')


char str_startwith(char* s, char* cmd);
int str_contains(char* s, char c);
int str_contains_by_str(char* s, char* c);
char str_equal(const char* s,const char* cmd);
int str_index(char* s, char c);
char str_lower(char* str);
char str_replace(char* s, char from, char to);
char str_replace_by_str(char* s,const char* from, char to);
int str_is_numeric(char* name);
int str_is_name(char* name);
char int_to_string(int n, char* s);

float str_to_float(char* s);

float* get_nums_from_rx(char* rs, int* length_back);
char** get_paras_from_rx(char* rx, int* len);

void limit_to(float* x, float max);
void limit_from_to(float* x, float lower, float upper);



typedef struct
{
	char* content;
	unsigned int count, cap, type_size;
}Lily_List;
typedef Lily_List* Li_List;

typedef struct {
	char* str;
	int length;
}Li_String_def;
typedef Li_String_def* Li_String;

Li_List str_split(char* str, char split_char);

Li_List str_split_by_str(char* str, char* split_char);

Li_List str_find(char* s, char* find);

Li_List str_find_sort(char* s, char* find);

#define list_index(list,i) (list->content+i*list->type_size)
//e.g.: Li_list list = new_li(int);
#define new_li(type) new_list(sizeof(type),4)
//e.g.: Li_list list = new_li_cap(int,8);
#define new_li_cap(type,cap) new_list(sizeof(type),cap)
//e.g.: int index = li_add(list,item);
#define li_add(list,obj) (list_add(list,&(obj)))
//e.g.: string* strs = list_content(list, string);
#define list_content(list,type) (type*)(list->content)


#define List_normal_code 0
#define List_no_space 1
#define List_null_ptr 2
#define List_index_overflow 3
#define List_not_found 4

int init_list(Lily_List* list, unsigned int unit_size2, unsigned int init_cap);
Lily_List* new_list(unsigned int type, unsigned int init_cap);
int list_recap(Lily_List* list, unsigned int new_cap);
int list_add(Lily_List* list, void* c);
int list_remove_at(Lily_List* list, unsigned int index);
bool list_item_equal(void* item1, void* item2, unsigned int type);
int list_remove(Lily_List* list, void* item);
int list_find(Lily_List* list, void* item);
int delete_list(Lily_List* list);

Li_String new_li_string_by(char* str);

char* new_string_by(char* str);

void delete_li_string(Li_String li);

int assign_li_string(Li_String li, char* source);
