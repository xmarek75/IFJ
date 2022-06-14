/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: param_stack.h
* Popis souboru: rozhrani pro implementaci zasobniku pro tokeny
* Autori: Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/
#ifndef PARAM_STACK_H
#define PARAM_STACK_H

#include "symtable.h"
#include "scanner.h"

#include <stdbool.h>

typedef struct param_stack
{
    Token token; // hodnota tokenu
    struct param_stack *nextPtr; // ukazatel na dalsi polozku
} param_stack_item;

typedef struct
{
    param_stack_item *topPtr; //ukazatel na vrchol zasobniku
} param_stack;

/*
* funknce na inicializaci zasobniku
*
* @param ukazatel na zasobnik
*/
void param_stack_init(param_stack *s);
/*
* funkce ulozi hodnotu na vrchol zasobniku
*
* @param stack - ukazatel na zasobnik
* @param token - token ktery se ma ulozit
* @param data_type - typ tokenu
* @return True pokud byl program uspesny jinak False
*/
bool param_stack_push(param_stack *s, Token token);
/*
*
*
*
*/
Token param_stack_pop(param_stack *s);
/*
*
*
*
*/
bool param_stack_is_empty(param_stack *s);



#endif
