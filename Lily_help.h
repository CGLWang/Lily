#pragma once

//[options]
#define in_debug

#ifdef in_PC
#define Tasks_using_pool
#define  Tasks_using_queue

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <iostream>
using namespace std;

#include "Lily.h"// in, out
#include "Lily_basic.h"
#include "Lily_tasks.h"//task_arg defination, tasks pool
#include "Lily_ui.h"// tx, rx, cmd, field
#include "Lily_boardcast.h" //timer and boardcast
#include "shell.h"
#include"Lily_for.h"
#else
#define  Tasks_using_queue
#include <stdlib.h>
#include "Lily.h"// in, out
#include "basic.h"// List, str deal like
#include "Lily_tasks.h"//task_arg defination, tasks pool
#include "ui.h"// tx, rx, cmd, field
#include "Lily_boardcast.h" //timer and boardcast
#include "shell.h"
#include"Lily_for.h"
#endif // in_PC





// ui ->Lily_tasks(task_arg), -> basic(list)
//shell ->ui(cmd, field)
//Lily_boardcast -> Lily_tasks(task_arg),->basic(List)

/* missing delete_list(li);here
int shell_do_cal(char* rx)
{
	if (!str_contains_by_str(rx, (char*)"+-"))
		return 0;
	//int ind_equ = str_index(str, '=');
	Li_List li = str_split(rx, '=');
	if (li->count > 2)
	{
		delete_list(li);
		return -1;
	}
*/