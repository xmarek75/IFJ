/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: stack.h
* Popis souboru: rozhrani pro implemenmtaci zasobniku
* Autor: Daniel Ponizil (xponiz01)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/

#ifndef IFJ_SYNTAX_ANALYSIS_STACK
#define IFJ_SYNTAX_ANALYSIS_STACK

#include "expression.h"
#include "symtable.h"

#include <stdbool.h>

typedef struct stack
{
    int nonterm_index;
    expr_table_tokens data; // hodnota tokenu
    DATA_TYPE type;    // datovy typ
    struct stack *nextPtr; // ukazatel na dalsi polozku
} token_stack_item;

typedef struct
{
    token_stack_item *topPtr; //ukazatel na vrchol zasobniku
} token_stack;

// inicializace zasobniku
void token_stack_init(token_stack *s);

// pushne (vlozi) na zasobnik token
bool token_stack_push(token_stack *s, expr_table_tokens data,DATA_TYPE t, int nonterm_index);

// vymaze jeden prvek ze zasobniku
bool token_stack_pop(token_stack *s);

// vrati true/false podle toho, jestli je zasobnik prazdny
bool token_stack_is_empty(token_stack *s);

// vola funkci pop, dokud funkce pop vraci true
// vymaze postupne vsechny prvky ze zasobniku
void token_stack_free(token_stack *s);

// vrati hodnotu prvku na vrcholu zasobniku
token_stack_item* token_on_top(token_stack *s);

// vrati terminal na vrcholu zasobniku, pokud je mensi jak STOP
// hodnoty musi byt nad STOP v expr_table_tokens (expression.h)
token_stack_item* token_stack_top_term(token_stack *s);

// Za posledni token se vlozi STOP
// Ten token nesmi byt neterminal
bool token_stack_insert_after_top_terminal(token_stack *s, expr_table_tokens stop, DATA_TYPE t);

#endif
