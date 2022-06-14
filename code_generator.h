/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: code_generator.h
* Popis souboru: rozhrani knihovny pro code_generator.c
* Autor: Stepan Bilek (xbilek25), Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/




#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "expression.h"

#ifndef CODE_GEN_H
#define CODE_GEN_H

void generate_code_start();

void generate_label(char *name);

void generate_return_value(int index);

void generate_function_end();

void generate_function_params(int index, Token token, int level);

void generate_write_param(Token token, int level);

void generate_write_count(int count);

void generate_createframe();

void generate_call_function(char * function_name);

void generate_var_def(char *name, int level);

void generate_var_def_assign_retval(char *name, int level);

void generate_var_def_assign_expr(char *name, int level, int nonterm_index, int expression_index);

void generate_def_function_params(int index, Token token, int level);

char* convert_STR(D_str str);

void generate_expr_int_nonterm(int value, int nonterm_index, int expression_index);

void generate_expr_num_nonterm(double value, int nonterm_index, int expression_index);

void generate_expr_str_nonterm(D_str string, int nonterm_index, int expression_index);

void generate_expr_id_nonterm(char *name, int level, int nonterm_index, int expression_index);

void generate_expr_nil_nonterm(int nonterm_index, int expression_index);

void generate_expr_mul(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index);

void generate_expr_add(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index);

void generate_expr_div(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index);

void generate_expr_int_div(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index);

void generate_expr_sub(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index);

void generate_expr_concat(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index);


void generate_expr_less(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index);

void generate_expr_less_eq(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index);

void generate_expr_more(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index);

void generate_expr_more_eq(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index);

void generate_expr_eq(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index);

void generate_expr_not_eq(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index);

void generate_expr_hash(D_str string, int nonterm_index, int expression_index);


void generate_fnc_return(int ret_index, int nonterm_index, int expression_index);

void generate_if_start(int if_index, int expression_index, int nonterm_index);

void generate_else_label(int if_index);

void generate_end_label(int if_index);


void generate_while_start(int while_index, int expression_index, int nonterm_index);

void generate_while_end_label(int while_index);


void generate_builtin();

void generate_expr(Token token);

#endif
