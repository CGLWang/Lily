#pragma once
#include "Lily.h"
#include "Lily_basic.h"

//typedef struct
//{
//	float* numbers;// numbers contained in a command string
//	char** paras;// option substrings in command
//	int numbers_length;//length of numbers
//	int paras_length;//length of options
//	char cmd_id;// this fields indicates which cmd was hit
//}Cmd_para_set_def;
/*
* a cmd hit will receive parameters set with this type,
* 
*/

//enum Cmd_type
//{
//	plain_text, // receive with plain text following by cmd key words in a command
//	with_numbers, // with numbers
//	with_paras, // with options substring
//	with_both, // both with numbers and options
//	with_bytes //receive with byte stream in the command
//};
//

/*Cmd_def
	char* cmd;
	char* annotation;
	Arg_Tasks_def todo;
	Cmd_type type;
	char id;
;*/
typedef struct
{
	//string, cmd key word, which will used to distinct from each others 
	char* name;
	// string, explain the function of this cmd
	char* annotation;
	// function pointer, with prototype: char(*)(void* paras_sets, &string msg_back)
	// where *paras_sets is in type of Cmd_para_set_def or byte depending the type of this cmd
	// cmd in type of with_bytes will receive a byte stream, 
	//while other types will receive a struct containing numbers and options
	// msg_back is a pointer to a string buffer, return message with it
	Arg_Tasks_def todo;
	//Cmd_type type;
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
	char* name;
	char* annotation;
	void* ref;
	char type;//'f','d','s'
}Field_def;

typedef struct
{
	Lily_List* cmds, * fields, * hijacks;
	bool para_updated, hijacked;
}Lily_cmds_def;
// cmds and fields is copied since from adding.

typedef Field_def* Field;
typedef Cmd_def* Cmd;

#define stack_len 4
typedef char (*hijack)(char*);

extern char rx[];
extern unsigned char ri, hi;

extern char tx[];
extern unsigned char ti;
extern Lily_cmds_def lily_ui;
extern void (*deal_byte_stream)(char*);


int excute_cmd();
int shell_do_dep(char*);
//************************************
// Method:    add_hijack
// FullName:  add_hijack
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: hijack call_back char(*hijack)(char*) return 0 for end
//************************************
void add_hijack(Arg_Tasks_def call_back);
int search_cmd_in_Lily_ui(char* item);
int search_field_in_Lily_ui(char* item);
int cmd_help(int a, char** b);

int pass_cmd(int n, char** arg);

int delete_field(int n, char** arg);

int whos(int n, char** arg);

int system(int n, char** arg);

void public_a_cmd_ref(char* name, Arg_Tasks_def link);
void public_a_float_field_ref(char* name, void* link);
int joint_args(int n, char** args);
int assign_field_from_field(Field dst, Field source);
int assign_field_from_string(Field dst, char* val);
int shell_do_fields_dep(char* cmd);
int public_a_new_string_field(char* name, char* s);
int public_a_new_field(char* name, char type, float val);
#define public_a_cmd_link(name,link) public_a_cmd_ref((char*) name, (Arg_Tasks_def) link)
//provide a name and a ref
#define public_a_field_ref(name,ref) public_a_float_field_ref((char*) name, ref)

//this will copy a backup
#define public_cmd(new_cmd) list_add(lily_ui.cmds, &new_cmd)
//new_field: type of Field_def, not ref
#define public_field(new_field) list_add(lily_ui.fields, &new_field)

//e.g.: Field fields = li_fields;
#define li_fields ((Field)(lily_ui.fields->content));
#define li_cmds (Cmd)(lily_ui.cmds->content);