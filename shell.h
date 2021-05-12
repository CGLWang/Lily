#pragma once

//Li_List valcues_of_fields(Li_List list);

float shell_calculate_expression(char *rx);

int shell_cal_exp_cmd(int n, char **arg);

//int shell_cal_assitant(int n, char** arg);

int shell_do_cmd(char *rx);

int shell_do_fun(char *rx);

int shell_do_cal(char *rx);

//int shell_do_fields(char *rx);

int shell_do_notFound(char *rx);
// int assign_field_from_field(Field dst, Field source, int off);
// int assign_field_from_string(Field dst, char *val, int off);

// void field_to_string(char *tx, Field fed, int off);

int assign_var_from_var(Var dst, Var src, int at_dst, int at_src);

int assign_var_from_string(Var dst, char *val, int at);

int assign_string_var(Var var, const char *val);

void var_to_string(char *tx, Var var, int at);

Li_List valcues_of_vars(Li_List list);

float value_from_string_or_var(char *name);
int ebrace_value_from_string(char* name);
int shell_do_var(char *rx);

// int assign_string_field(Field fed, const char *val);

//float value_from_string_or_field(char *name);
//Li_List valcues_of_fields(Li_List list);
