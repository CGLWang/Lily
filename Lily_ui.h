#pragma once
#include "Lily.h"
#include "Lily_basic.h"

typedef struct
{
	//string, cmd key word, which will used to distinct from each others 
	char* name;
	// string, explain the function of this cmd
	const char* annotation;
	// function pointer, 
	Arg_Tasks_def todo;
	// a unique id for each cmd
	char id;
}Cmd_def;
//enum Fields_type
//{
//	float_type,
//	int_type,
//	string_type
//};

/*
* field def
* char* field;
	char* annotation;
	float* ref;
	Fields_type type;
*/
typedef struct
{
	char* name;// may from a memory
	const char* annotation;
	void* ref;
	char type;//'f','d','s'
}Field_def;

typedef struct
{
	char* name;
	void* ref;
	char narg;
}Fun_def;

typedef struct
{
	Lily_List* cmds, * fields,* funs;// , * hijacks;
	bool para_updated, hijacked;
}Lily_cmds_def;
// cmds and fields is copied since from adding.

typedef Field_def* Field;
typedef Cmd_def* Cmd;
typedef Fun_def* Fun;
#define stack_len 4

extern char rx[];
extern unsigned char ri, hi;

extern char tx[];
extern unsigned char ti;
extern Lily_cmds_def lily_ui;
extern Tasks_def lily_hooks_cmd_done;
extern char* lily_hooks_cmd_hook;

int excute_cmd();
void add_hijack(Arg_Tasks_def call_back);

int search_cmd_in_Lily_ui(char* item);
int search_field_in_Lily_ui(char* item);
int search_fun_in_Lily_ui(char* item);

int help(int a, char** b);

int pass(int n, char** arg);

int delete_field(int n, char** arg);

int whos(int n, char** arg);

int system(int n, char** arg);

int delay_cmd(int n, char** arg);

void public_a_cmd_link(const char* name, Arg_Tasks_def link);
void public_a_field_ref(const char* name, void* link);
void public_a_fun_link(const char* name, void* link);
void public_a_fun_link_n(const char* name, void* link, char n);
int joint_args(int n, char** args);
int public_a_new_string_field(char* name, char* s);
int public_a_new_field(char* name, char type, float val);

int cmd_for_start(int n, char* args[]);
int hijackor_wait_key(int n, char** s);

//this will copy a backup
#define public_cmd(new_cmd) list_add(lily_ui.cmds, &new_cmd)
//new_field: type of Field_def, not ref
#define public_field(new_field) list_add(lily_ui.fields, &new_field)
#define public_fun(new_fun) list_add(lily_ui.funs, &new_fun)

//e.g.: Field fields = li_fields;
#define li_fields ((Field)(lily_ui.fields->content))
#define li_cmds ((Cmd)(lily_ui.cmds->content))
#define li_funs ((Fun)(lily_ui.funs->content))