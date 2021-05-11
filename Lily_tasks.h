#pragma once
#include "Lily.h"
#ifdef Tasks_using_pool
#define Tasks_LEN 32
extern Tasks_def tasks[Tasks_LEN];
extern int hasTask;

void addTask(Tasks_def task);
void endTaskAt(char index);
void endTask(Tasks_def task);
char hadTask(Tasks_def task);
#endif // Tasks_using_pool

#ifdef Tasks_using_queue 
extern unsigned int front, rear;
#ifndef Tasks_LEN
#define Tasks_LEN 11
#endif

#ifdef Tasks_using_pool
#define tasks_ tasks2
#define hasTask_ hasTask2
#define addTask_ addTask2
#define endTask_ endTask2
#define endTaskAt_ endTaskAt2
#define hadTask_ hadTask2
#else
#define Tasks_def Tasks2_def
#define tasks_ tasks
#define hasTask_ hasTask
#define addTask_ addTask
#define endTask_ endTask
#define endTaskAt_ endTaskAt

#define  hadTask_ hadTask
#endif // Tasks_using_pool
extern Tasks_def tasks_[Tasks_LEN];
extern unsigned int hasTask_;

void addTask_(Tasks_def task);
void endTask_(Tasks_def task);
char hadTask_(Tasks_def task);
void endTaskAt_(char index);
#endif // Tasks_using_queue

void run_tasks();
int task_mointor();

void addTaskArg(TasksArg_def f, void* arg);
