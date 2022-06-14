/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: dynamic_string.c
* Popis souboru: implementace knihovny pro praci s nekonecne dlouhymi retezci
* Autor: Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/


#include <stdlib.h>
#include <string.h>
#include "dynamic_string.h"

int dstr_init(D_str *ds)
{
    ds->string = (char *) malloc(DEF_SIZE);
    if (!ds->string)
    { // alokace selhala
        return DS_ERROR;
    }

    ds->string[0] = '\0';
    ds->alloc_size = DEF_SIZE;
    ds->lenght = 0;
    return DS_SUCCES;
}

void dstr_free(D_str *ds)
{
    free(ds->string);
}

void dstr_clear(D_str *ds)
{
    ds->string[0] = '\0';
    ds->lenght = 0;
}

int dstr_add_char(D_str *ds, char c)
{
    if (ds->lenght + 1 >= ds->alloc_size)
    { // ds overflow -> need to allocate more space
        ds->string = (char *) realloc(ds->string, ds->alloc_size * 2);
        if (!ds->string)
        {
            return DS_ERROR;
        }
        ds->alloc_size *= 2;
    }
    ds->string[ds->lenght] = c;
    (ds->lenght)++;
    ds->string[ds->lenght] = '\0';
    return DS_SUCCES;
}

int dstr_add_str(D_str *ds, char *s)
{
    unsigned int s_len = strlen(s);
    if (ds->lenght + s_len >= ds->alloc_size)
    { // ds overflow -> need to allocate more space
        ds->string = (char *) realloc(ds->string, ds->lenght + s_len + DEF_SIZE);
        if (!ds->string)
        {
            return DS_ERROR;
        }
        ds->alloc_size = ds->lenght + s_len + DEF_SIZE;
    }
    strcat(ds->string, s);
    ds->lenght += s_len;
    return DS_SUCCES;
}

int dstr_cmp_dstr(D_str *ds1, D_str *ds2)
{
    return strcmp(ds1->string, ds2->string);
}

int dstr_cmp_str(D_str *ds, char *s)
{
    return strcmp(ds->string, s);
}

int dstr_copy_dstr(D_str *src, D_str *dest)
{

    if (src->lenght > dest->lenght)
    {
        dest->string = (char *) realloc(dest->string, src->lenght + 1);
        if (!dest->string)
        {
            return DS_ERROR;
        }
        dest->alloc_size = src->lenght + 1;
    }
    strcpy(dest->string, src->string);
    dest->lenght = src->lenght;
    return DS_SUCCES;
}
