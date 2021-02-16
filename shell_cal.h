#pragma once

//Li_List valcues_of_fields(Li_List list);

float shell_calculate_expression(char* rx);

int shell_cal_exp_cmd(int n, char** arg);

//int shell_cal_assitant(int n, char** arg);

int shell_do_cmd(char* rx);

int shell_do_cal(char* rx);

int shell_do_fields(char* rx);

int shell_do_notFound(char* rx);
