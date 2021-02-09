// C_Lily.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

#include "Lily.h"
#include "Lily_tasks.h"
#include "Lily_ui.h"
#include "Lily_boardcast.h"
using namespace std;
unsigned int lily_millis()
{
    unsigned int a = GetTickCount64();
    return a / 1000;
}
int cmd_test0(int arg,char**);
int cmd_test1(int arg, char**);

int timer0();

int timer1();

void task6(char* s);

int key_input_task();
int sleep_task();
int task4();

int main()
{
    Tasks_def task_p;
    lily_init();
    addTask(key_input_task);
    addTask(sleep_task);
    char s[] = "help s";
    char* str = s;
    cout << "ready";
    Li_List list = str_split(str,' ');
    for (int i = 0; i < list->count; i++)
    {
        char* *s2 = (char**)list_index(list, i);
        cout << i << ":" << *s2 << endl;
    }
    //addTask_(task4);
    Cmd_def cmd1;
    cmd1.annotation = (char*)"call exit";
    cmd1.name =(char*) "exit";
    cmd1.todo = (Arg_Tasks_def)exit;
    
    public_cmd(cmd1);

	/*Cmd_def help;
	help.cmd = (char*)"text";
	help.annotation = (char*)"set fields";
	help.type = with_bytes;
	help.todo = task5;
	public_cmd(help);*/

    public_a_cmd("text", cmd_test1);
    public_a_timer(timer0, 30);
    public_a_timer(timer1, 10);
    deal_byte_stream = task6;
    
    char buff[128];
	/* char* p = (char*)"##9123456789nop\n";
	 strcpy(buff,p);
	 p = buff;
	 p[2] = 9;

	 while (*p != '\0')
	 {
		 lily_cin(*p);
		 p++;
	 }
	 p = (char*)"help;guess\n";
	 while (*p != '\0')
	 {
		 lily_cin(*p);
		 p++;
	 }*/
    run_tasks();
    return 0;


}

void task6(char* s)
{
    cout << "-->task6\n";
    char length = s[2];
    if (s[length+3] != '\0')
    {
        s[length+3] = '\0';
        cout << "!";
    }
    cout << s;

}
int key_input_task()
{
	static int c = 0;
	c++;
	if (c >= 3)
		c = 0;
         
    char cv;
    if (_kbhit())
    {
        cv = _getch();
        cout << cv;
        if (cv == '\r')
            cv = '\n';
        lily_cin(cv);
    }    

    return 1;
}
int sleep_task()
{
    //static int c = 0;
	//cout << "-->task2:sleep" << endl;
    Sleep(100);
    lily_tick();
   /* c++;
    if (c == 10)
    {
        c = 0;
        
    }*/
    //cout << "-->task2:wake" << endl;

    return 1;
}
int cmd_test0(int arg,char**send_s)
{

 //   Cmd_para_set_def* sets = (Cmd_para_set_def*)arg;
 //   int i;
 //   cout << "\ncmd\nnumbers:" << endl;
 //   for (i = 0; i < sets->numbers_length; i++)
 //       cout << sets->numbers[i]<<", ";
 //   cout << endl;
	//cout << "paras:" << endl;
	//for (i = 0; i < sets->paras_length; i++)
	//	cout << sets->paras[i] << ", ";
	//cout << endl;

 //   char** s = (char**)send_s;
 //   *(char**)send_s =(char*) "done----";
    
    return Cmd_send;
}
int task4()
{
    //cout << "-->task4:done\n";
    return 0;
}
int cmd_test1(int arg, char** send_s)
{
	//static int step = 0;
	//char* str = (char*)arg;
	//char** send = (char**)send_s;

	//if (step)
	//{

	//	if (str_index(str, 'y') >= 0)
	//	{
	//		step = 0;
	//		*send = (char*)"yes";
	//		return Cmd_send_done_and_end;
	//	}
	//	else
	//	{
	//		*send = (char*)"no";
	//		return Cmd_send;
	//	}
	//}
	//step = 1;
	//cout << "frame cmd:" << str << endl;
	//add_hijack(cmd_test1);
	return Cmd_send_done;
}

int timer0()
{
    //cout << "->timer0 tick 10cycles\n";
    return end;
}
int timer1()
{
    static int c = 0;
    c++;
    if (c == 2)
        remove_timer(timer1);
	cout << "->timer1 tick 5cycles\n";
	return end;
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

