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

typedef struct
{
	char* name;// may from a memory
	const char* annotation;
	void* ref;
	char type;//'f','d','c',
	char length;
}Field_def;

#define to_voidf(x) (void *)(*(int *)&x)
#define to_void(x) ((void *)x)
#define void_to_int(p) ((int)p)
#define assign_f_from_void(f, p) (*((int *)&f) = (int)p)
#define cast(x,type) ((type)(x))
typedef struct
{
	const char* name;// name id
	void* ref; // pointer to a memory, or a value
	char type; // type symbol, 'f':float, 'd':int, 'c':char
	char len;  // length of this array, only when isp==1
	char isb :1; // is built in variable? that means this var cannot be changed in type
	char isp : 1;  // is ref a pointer or a value?
	char isa : 1;  // is ref a array?
}Var_def;


typedef struct
{
	char* name;
	void* ref;
	char narg,type;
}Fun_def;

typedef struct
{
	Lily_List* cmds, * fields,* funs, *vars;// , * hijacks;
	bool para_updated, hijacked;
}Lily_cmds_def;
// cmds and fields is copied since from adding.

typedef Field_def* Field;
typedef Var_def* Var;
typedef Cmd_def* Cmd;
typedef Fun_def* Fun;
//#define stack_len 4

#define NorFedType(fed) (fed->type&0x20)
#define KeepHijack 1
//#define DoNotNotifyListener 0x0100
#define Lily_cmd_do_not_notify 0x0100// return code from shell_do

#define shell_dos_done 0x10 //return from shell_do_xxx showing it deal the work

//#define add_listener(f) cmd_done_listener[cmd_done_listener_stack_top++]=f;cmd_done_listener_stack_top%=lily_do_stack_len
#define invoke_listener cmd_done_listener_stack_top--
#define current_listener cmd_done_listener[cmd_done_listener_stack_top-1]
#define have_listener cmd_done_listener_stack_top
extern char stack_top;
extern Tasks_def cmd_done_listener[];
extern char cmd_done_listener_stack_top;


extern char rx[];
extern unsigned char ri, hi;

extern char tx[];
extern unsigned char ti;
extern Lily_cmds_def lily_ui;
// extern Tasks_def lily_hooks_cmd_done;
// extern char* lily_hooks_cmd_hook;

int get_cmd_from_rx();
int lily_do(char* rx);
void add_hijack(Arg_Tasks_def call_back);
void add_listener(Tasks_def f);

int search_cmd_in_Lily_ui(char* item);
int search_field_in_Lily_ui(char* item);
int search_fun_in_Lily_ui(char* item);

int help(int a, char** b);

int pass(int n, char** arg);

int delete_field(int n, char** arg);

int whos(int n, char** arg);

int system(int n, char** arg);
int delay_cmd(int n, char** arg);

int public_a_cmd_link(const char* name, Arg_Tasks_def link);
int public_a_field_ref(const char* name, void* link);
int public_a_field_ref_type(const char* name, void*link,char type);
int public_a_fun_link(const char* name, void* link);
int public_a_fun_link_n(const char* name, void* link, char n);
int public_a_fun_link_int(const char* name, void* link, char n);

int joint_args(int n, char** args);
int public_a_new_string_field(char* name, char* s);
int public_a_new_field(char* name, char type, float val);


int hijackor_wait_key(int n, char** s);
int cmd_echo(int n, char **arg);
int cmd_hijack_sleep(int n, char **arg);

//this will copy a backup
#define public_cmd(new_cmd) list_add(lily_ui.cmds, &new_cmd)
//new_field: type of Field_def, not ref
#define public_field(new_field) list_add(lily_ui.fields, &new_field)
#define public_fun(new_fun) list_add(lily_ui.funs, &new_fun)

//e.g.: Field fields = li_fields;
#define li_fields ((Field)(lily_ui.fields->content))
#define li_cmds ((Cmd)(lily_ui.cmds->content))
#define li_funs ((Fun)(lily_ui.funs->content))

#define public_var(new_field) list_add(lily_ui.vars, &new_field)

int public_a_var_ref(const char* name, void* link, char type);
int public_a_new_var(char* name, char type, void* val);
int public_new_vars_array(char* name, char type, void* val, int len);
void* get_value_of_var(Var var, int at);
int set_value_of_var(Var var, int at, void* val);
void delete_a_var(Var var);
