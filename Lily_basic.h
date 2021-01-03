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
}List;

#define list_index(list,i) (list->content+i*list->type_size)
#define List_normal_code 0
#define List_no_space 1
#define List_null_ptr 2
#define List_index_overflow 3
#define List_not_found 4

char init_list(List* list, unsigned int unit_size2, unsigned int init_cap);
List* new_list(unsigned int type, unsigned int init_cap);
char list_recap(List* list, unsigned int new_cap);
char list_add(List* list, void* c);
char list_remove_at(List* list, unsigned int index);
bool list_item_equal(void* item1, void* item2, unsigned int type);
char list_remove(List* list, void* item);
int list_find(List* list, void* item);
char delete_list(List* list);
