/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: symtable.c
* Popis souboru: implementace tabulky symbolu
* Autori: Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/
#include "symtable.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>


#define HANDLE_DS_ERR(command) if (command == DS_ERROR) return NULL;

unsigned int get_hash(char *id)
{ // djb2 hash function
    unsigned int hash = 5381;
    char c;
    while ((c = *id++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash % HT_SIZE;
}


// ********************** Function symbol table *****************************
void STF_init(ST_funcs *table)
{
    for (int i = 0; i < HT_SIZE; i++)
    { // for each item in array
        (*table)[i] = NULL; // set pointer to null
    }
}

SYM_function *STF_search(ST_funcs *table, char *id)
{
    SYM_function *item = (*table)[get_hash(id)]; // pointer to first item of synonymes with certain hash
    while (item && dstr_cmp_str(&(item->id), id) != 0)
    { // do until item doesnt exist or item has the right id
        item = item->next; // go to next synonym
    }
    return item;
}

SYM_function *STF_add_symbol(ST_funcs *table, char *id)
{
    unsigned int hash = get_hash(id); // pointer to first item of synonymes with certain hash

    SYM_function *new_item = (SYM_function *) malloc(sizeof(SYM_function)); // allocate space for new item
    if (!new_item)
    { // malloc faileds
        return NULL;
    }
    HANDLE_DS_ERR(dstr_init(&(new_item->id)));
    HANDLE_DS_ERR(dstr_add_str(&(new_item->id), id));
    HANDLE_DS_ERR(dstr_init(&(new_item->param_types)));
    HANDLE_DS_ERR(dstr_init(&(new_item->ret_types)));
    new_item->defined = false;
    new_item->next = NULL;

    new_item->next = (*table)[hash]; // set inserted items descendant to prior first synonym pointer
    (*table)[hash] = new_item; // set pointer to first synonym to inserted item
    return new_item;
}


#define FREE_DSTR(dstr);    if (dstr.alloc_size > 0)    \
                            {                           \
                                free(dstr.string);      \
                            }

void STF_delete_all(ST_funcs *table)
{
    SYM_function *item;
    SYM_function *del_item;
    for (int i = 0; i < HT_SIZE; i++)
    { // for each item in array
        item = (*table)[i];
        while (item)
        { // do until no elements in list of synonyms
            del_item = item;
            item = item->next;
            FREE_DSTR(del_item->id);
            FREE_DSTR(del_item->param_types);
            FREE_DSTR(del_item->ret_types);
            free(del_item);
        }
        (*table)[i] = NULL; // set first of synonyms pointer to null
    }
}
// ********************** End of function symbol table *****************************


// ********************** Variable symbol table *****************************
void STV_init(ST_vars *table)
{
    for (int i = 0; i < HT_SIZE; i++)
    { // for each item in array
        (*table)[i] = NULL; // set pointer to null
    }
}

void STV_dispose(ST_vars *table)
{
    SYM_variable *item;
    SYM_variable *del_item;
    for (int i = 0; i < HT_SIZE; i++)
    { // for each item in array
        item = (*table)[i];
        while (item)
        { // do until no elements in list of synonyms
            del_item = item;
            item = item->next;
            FREE_DSTR(del_item->id);
            free(del_item);
        }
        (*table)[i] = NULL; // set first of synonyms pointer to null
    }
}

SYM_variable *STV_add_symbol(ST_vars *table, char *id, int level)
{
    unsigned int hash = get_hash(id); // pointer to first item of synonymes with certain hash

    SYM_variable *new_item = (SYM_variable *) malloc(sizeof(SYM_variable)); // allocate space for new item
    if (!new_item)
    { // malloc failed
        return NULL;
    }
    HANDLE_DS_ERR(dstr_init(&(new_item->id)));
    HANDLE_DS_ERR(dstr_add_str(&(new_item->id), id));
    new_item->data_type = DT_UNDEF;
    new_item->level = level;

    new_item->next = (*table)[hash]; // set inserted items descendant to prior first synonym pointer
    (*table)[hash] = new_item; // set pointer to first synonym to inserted item
    return new_item;
}
// ********************** End of variable symbol table *****************************

// ********************** Variable symbol table list *****************************
int STV_L_init(ST_L_vars *list)
{
    list->upper = NULL;
    list->sym_table = (ST_vars*) malloc(sizeof(ST_vars));
    if (!list->sym_table)
    {
        return ERR_INTERNAL;
    }
    list->level = 0;
    STV_init(list->sym_table);
    return ERR_OK;
}

int STV_L_deeper(ST_L_vars **list)
{
    ST_L_vars *deepest = malloc(sizeof(ST_L_vars));
    if (!deepest)
    {
        return ERR_INTERNAL;
    }
    STV_L_init(deepest);
    deepest->level = (*list)->level + 1;
    deepest->upper = *list;
    *list = deepest;
    return ERR_OK;
}

void STV_L_upper(ST_L_vars **list)
{
    ST_L_vars *deepest = *list;
    *list = (*list)->upper;

    STV_dispose(deepest->sym_table);
    free(deepest->sym_table);
    free(deepest);
}

int STV_L_get_current_level(ST_L_vars *list)
{
   return list->level;
}

SYM_variable *STV_L_add_symbol(ST_L_vars *list, char *id)
{
   return STV_add_symbol(list->sym_table, id, list->level);
}

SYM_variable *STV_L_search(ST_L_vars *list, char *id)
{
    for ( ; list; list = list->upper) // go to upper variable symtable)
    {
        if (list->sym_table == NULL)
        {
            continue;
        }
        SYM_variable *item = (*(list->sym_table))[get_hash(id)]; // pointer to first item of synonymes with certain hash
        while (item)
        { // do until item doesnt exist or item has the right id
            if(dstr_cmp_str(&(item->id), id) == 0)
            {
                return item;
            }
            item = item->next; // go to next synonym
        }
    }
    return NULL;
}

void STV_L_dispose(ST_L_vars *list)
{
    while (list->upper)
    {
        STV_dispose(list->sym_table);
        free(list->sym_table);
        ST_L_vars *upper = list->upper;
        free(list);
        list = upper;
    }
    STV_dispose(list->sym_table);
    free(list->sym_table);
}
// ********************** end of variable symbol table list *****************************
// int main()
// {
//     ST_funcs func_ST_h;
//     ST_L_vars vars_ST_h;
//     ST_funcs *func_ST = &func_ST_h;
//     ST_L_vars *vars_ST = &vars_ST_h;

//     STF_init(func_ST);
//     STF_add_symbol(func_ST, "modulo");
//     STF_add_symbol(func_ST, "test");
//     STF_search(func_ST, "modulo");
//     STF_search(func_ST, "test");
//     STF_delete_all(func_ST);

//     STV_L_init(vars_ST);
//     STV_L_add_symbol(vars_ST, "promena");
//     STV_L_deeper(&vars_ST);
//     STV_L_add_symbol(vars_ST, "promena");
//     STV_L_search(vars_ST, "promena");
//     STV_L_upper(&vars_ST);
//     STV_L_search(vars_ST, "promena");
//     STV_L_dispose(vars_ST);

//     return 0;
// }




















