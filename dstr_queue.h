/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: dstr_queue.h
* Popis souboru: rozhrani souboru implementujiciho frontu
* Autor: Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/

#ifndef QUEUE_H
#define QUEUE_H

#include "dynamic_string.h"

#include <stdbool.h>

typedef struct queue_itemT
{
    D_str str; // hodnota tokenu
    struct queue_itemT *next; // ukazatel na dalsi polozku
} queue_item;

typedef struct
{    queue_item *first; //ukazatel na vrchol zasobniku
} dstr_queue;


void dstr_queue_init(dstr_queue *q);

bool dstr_queue_insert(dstr_queue *q, D_str dstr);

D_str dstr_queue_pop(dstr_queue *q);

bool dstr_queue_empty(dstr_queue *q);


#endif
