#pragma once
char str_startwith(char* s, char* cmd);
char str_equal(char* s, char* cmd);
int str_index(char* s, char c);
char str_lower(char* str);
char str_replace(char* s, char from, char to);
char int_to_string(int n, char* s);

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

#define list_index(list,i) (list->content+i*list->type_size)
#define li_add(list,obj) (list_add(list,&obj))

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

void delete_li_string(Li_String li);

int assign_li_string(Li_String li, char* source);
