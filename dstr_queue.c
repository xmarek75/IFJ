/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: dstr_queue.c
* Popis souboru: zdrojovy soubor s implementaci fronty
* Autor: Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/

#include <stdlib.h>

#include "dstr_queue.h"

void dstr_queue_init(dstr_queue *q)
{
    q->first = NULL;
}
/*
* funkce ulozi hodnotu na vrchol zasobniku
*
* @param stack - ukazatel na zasobnik
* @param token - token ktery se ma ulozit
* @param data_type - typ tokenu
* @return True pokud byl program uspesny jinak False
*/
bool dstr_queue_insert(dstr_queue *q, D_str dstr)
{
    queue_item *new_item = (queue_item*)malloc(sizeof(queue_item));
    if (new_item == NULL)
    {
        return false; //malloc selhal
    }
    dstr_init(&(new_item->str));
    dstr_copy_dstr(&dstr, &(new_item->str));
    new_item->next = NULL;
    queue_item *tmp = q->first;
    if (tmp == NULL)
    {
        q->first = new_item;
    }
    else
    {
        while(tmp->next != NULL)
        {
            tmp = tmp->next;
        }
        tmp->next = new_item;
    }

    return true;
}

D_str dstr_queue_pop(dstr_queue *q)
{
    D_str popped_str = q->first->str;
    queue_item *to_free = q->first;
    q->first = q->first->next;
    free(to_free);
    return popped_str;
}

bool dstr_queue_empty(dstr_queue *q)
{
    if(q->first == NULL)
    {
        return true;
    }
    else
    {
        return false;
    }

}
