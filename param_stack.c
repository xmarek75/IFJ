/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: param_stack.c
* Popis souboru: implementace zasobniku pro tokeny
* Autori: Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/

#include <stdlib.h>

#include "param_stack.h"
#include "scanner.h"



void param_stack_init(param_stack *s)
{
    s->topPtr = NULL;
}
bool param_stack_push(param_stack *s, Token token)
{
    param_stack_item *newStackPtr = (param_stack_item*)malloc(sizeof(param_stack_item));

    if (newStackPtr == NULL)
    {
        return false; //malloc selhal
    }
    newStackPtr->token = token;
    newStackPtr->nextPtr = s->topPtr;
    s->topPtr = newStackPtr;

    return true;
}
Token param_stack_pop(param_stack *s)
{
    Token poped_token = s->topPtr->token;
    param_stack_item *to_free = s->topPtr;
    s->topPtr = s->topPtr->nextPtr;
    free(to_free);
    return poped_token;
}

bool param_stack_is_empty(param_stack *s)
{
    return (s->topPtr == NULL);
}

