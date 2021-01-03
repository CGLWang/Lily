#pragma once
#include "Lily.h"
#include "Lily_basic.h"

typedef struct
{
	float* numbers;// numbers contained in a command string
	char** paras;// option substrings in command
	int numbers_length;//length of numbers
	int paras_length;//length of options
	char cmd_id;// this fields indicates which cmd was hit
}Cmd_para_set_def;
/*
* a cmd hit will receive parameters set with this type,
* 
*/

enum Cmd_type
{
	plain_text, // receive with plain text following by cmd key words in a command
	with_numbers, // with numbers
	with_paras, // with options substring
	with_both, // both with numbers and options
	with_bytes //receive with byte stream in the command
};


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
	char* cmd;
	// string, explain the function of this cmd
	char* annotation;
	// function pointer, with prototype: char(*)(void* paras_sets, &string msg_back)
	// where *paras_sets is in type of Cmd_para_set_def or byte depending the type of this cmd
	// cmd in type of with_bytes will receive a byte stream, 
	//while other types will receive a struct containing numbers and options
	// msg_back is a pointer to a string buffer, return message with it
	Arg_Tasks_def todo;
	Cmd_type type;
	// a unique id for each cmd
	char id;
}Cmd_def;
enum Fields_type
{
	float_type,
	int_type
};

/*
* field def
* char* field;
	char* annotation;
	float* ref;
	Fields_type type;
*/
typedef struct
{
	char* field;
	char* annotation;
	float* ref;
	Fields_type type;
}Field_def;

typedef struct
{
	/*Cmd_def* cmds;
	Field_def* fields;

	unsigned int cmds_len, fields_len;
	unsigned int cmds_cap, fields_cap;*/
	List* cmds, * fields, * hijacks;
	bool para_updated;
}Lily_cmds_def;

#define stack_len 4
typedef char (*hijack)(char*);

#define Cmd_send 0x02
#define Cmd_done 0x04
#define Cmd_end 0x08
#define Cmd_false 0x20

//#define Cmd_send 1
#define Cmd_not_send 0
#define Cmd_false_input 0x20
#define Cmd_send_done  0x04
#define Cmd_send_done_and_end  0x04|0x08
#define Cmd_send_and_end 0x02|0x08
extern char rx[];
extern unsigned char ri, hi;

extern char tx[];
extern unsigned char ti;
extern Lily_cmds_def lily_ui;
extern void (*deal_byte_stream)(char*);

#define frame_head_0 '#'
#define frame_head_1 '#'

char excute_command();

//************************************
// Method:    add_hijack
// FullName:  add_hijack
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: hijack call_back char(*hijack)(char*) return 0 for end
//************************************
void add_hijack(Arg_Tasks_def call_back);
char cmd_help(void* a, void* b);

void creat_public_a_cmd(char* name, Arg_Tasks_def link);
void creat_public_a_field(char* name, void* link);
#define public_a_cmd(name,link) creat_public_a_cmd((char*) name, (Arg_Tasks_def) link)
//provide a name and a ref
#define public_a_field(name,ref) creat_public_a_field((char*) name, ref)

//this will copy a backup
#define public_cmd(new_cmd) list_add(lily_ui.cmds, &new_cmd)
//new_field: type of Field_def, not ref
#define public_field(new_field) list_add(lily_ui.fields, &new_field)

