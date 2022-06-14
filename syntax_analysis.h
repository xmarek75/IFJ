/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: syntax_analysis.h
* Popis souboru: rozhrani pro syntaktickou a semantickou analyzu
* Autori: Pavel Marek(xmarek75), Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/
#ifndef _ANALYSIS_H
#define _ANALYSIS_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "scanner.h"
#include "symtable.h"
#include "expression.h"
//#include "symtable"

/*
* @brief token_eq_kw funkce na porovnani tokenu s keywords
*
* @param token vstupni token
* @param keyword klicove slovo ktere porovnavame s tokenem
*/
bool token_eq_kw(Data *data, Keyword kw);
bool token_eq_type(Data *data, Type type);
/*
* @brief analyze funkce na inicializaci analyzy
*
*/
int analyze();
/*
* @brief req funkce na prvni pravidlo req
*
* @brief Token
*/
int req(Data *data);
int prog(Data *data);
int types(Data *data);
int par_types(Data *data);
int type(Data *data);
int next_type(Data *data);
int params(Data *data);
int next_param(Data *data);
int args(Data *data);
int next_term(Data *data);
int term(Data *data);
int body(Data *data);
int expr_or_fnc(Data *data);
int expressions(Data *data);
int next_expression(Data *data);
int assign(Data *data);
int next_id(Data *data);
int exprs_or_fnc(Data *data);
int init(Data *data);


//semantic functions
int set_type(Data *data);

bool id_in_glob_symt(Data *data);
bool cmp_params_in_function(Data *data);
bool cmp_ret_in_function(Data *data);
int define_function(Data *data);
int declare_function(Data *data);
bool set_param_to_function(Data *data);
bool set_ret_to_function(Data *data);

//special function
void unget_token(Data *data);
char KW_to_DT(Token *token);        //prevod keyword do data_type
bool is_expr_end(Data *data);

//funkce na porovnani poctu parametru a navratovych hodnot
bool check_param_count(Data *data);
bool check_ret_count(Data *data);
int check_types_count(Data *data);

bool var_check_identic_id(Data *data);
#endif
