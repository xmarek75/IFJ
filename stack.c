/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: stack.c
* Popis souboru: implemenmtace zasobniku
* Autor: Daniel Ponizil (xponiz01)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/

#include <stdlib.h>

#include "stack.h"

// inicializace zasobniku
void token_stack_init(token_stack *s)
{
    s->topPtr = NULL;
}

// pushne (vlozi) na zasobnik token
bool token_stack_push(token_stack *s, expr_table_tokens data, DATA_TYPE t, int nonterm_index)
{
    token_stack_item *newStackPtr = (token_stack_item*)malloc(sizeof(token_stack_item));

    if (newStackPtr == NULL)
    {
        return false; //malloc selhal
    }
    newStackPtr->data = data;
    newStackPtr->type = t;
    newStackPtr->nextPtr = s->topPtr;
    newStackPtr->nonterm_index = nonterm_index;

    s->topPtr = newStackPtr;

    return true;
}

// vymaze jeden prvek ze zasobniku
bool token_stack_pop(token_stack *s)
{
    token_stack_item *stackPtr;
    if (s->topPtr == NULL)
    {
        return false;
    }
    else
    {
        stackPtr = s->topPtr;
        s->topPtr = stackPtr->nextPtr;
        free(stackPtr);
        return true;
    }
}

// vrati true/false podle toho, jestli je zasobnik prazdny
bool token_stack_is_empty(token_stack *s)
{
    return (s->topPtr == NULL);
}

// vola funkci pop, dokud funkce pop vraci true
// vymaze postupne vsechny prvky ze zasobniku
void token_stack_free(token_stack *s)
{
    while (token_stack_pop(s))
        ;
}

// vrati hodnotu prvku na vrcholu zasobniku
token_stack_item* token_on_top(token_stack *s)
{
    return s->topPtr;
}

// vrati terminal na vrcholu zasobniku, pokud je mensi jak STOP
// hodnoty musi byt nad STOP v expr_table_tokens (expression.h)
token_stack_item *token_stack_top_term(token_stack *s)
{
    token_stack_item *tmp;
    tmp = s->topPtr;
    while (tmp != NULL)
    {
        if (tmp->data < STOP)
            return tmp;
        tmp = tmp->nextPtr;
    }
    return NULL;
}

// Za posledni token se vlozi STOP
// Ten token nesmi byt neterminal
bool token_stack_insert_after_top_terminal(token_stack *s, expr_table_tokens stop, DATA_TYPE t)
{
	token_stack_item* old = NULL;

	for (token_stack_item* tmp = s->topPtr; tmp != NULL; tmp = tmp->nextPtr)
	{
		if (tmp->data < STOP)
		{
			token_stack_item* new = (token_stack_item*)malloc(sizeof(token_stack_item));

			if (new == NULL)
            {
				return false; //malloc selhal
            }
			new->data = stop;
			new->type = t;

			if (old != NULL)
            {
				new->nextPtr = old->nextPtr;
				old->nextPtr = new;
			}
			else
			{
				new->nextPtr = s->topPtr;
				s->topPtr = new;
			}
			return true;
		}
		old = tmp;
	}
	return false;
}
