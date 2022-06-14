/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: symtable.h
* Popis souboru: rozhrani pro implementaci tabulky symbolu
* Autori: Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/
#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>
#include "dynamic_string.h"

#define HT_SIZE 1999

// Vycet datovych typu
typedef enum
{
    DT_INT = 'I',
    DT_NUM = 'N',
    DT_STR = 'S',
    DT_NIL = 'Z',
    DT_UNDEF = 'U'
} DATA_TYPE;

// Struktura symbolu v tabulce symbolu pro funkce
typedef struct sym_function{
  D_str id;
  D_str param_types;
  D_str ret_types;
  bool defined;
  struct sym_function *next;
} SYM_function;

// Tabulka symbolu pro funkce
typedef SYM_function *ST_funcs[HT_SIZE];

// Funkce pro ziskani indexu v hashovaci tabulce
unsigned int get_hash(char *id);

// Funkce pro inicializaci tabulky symbolu funkci
void STF_init(ST_funcs *table);

// Funkce pro vyhledani prvku v tabulce symbolu funkci
// vrací ukazatel na prvek nebo NULL
SYM_function *STF_search(ST_funcs *table, char *id);

// Funkce pro pridani symbolu do tabulky symbolu funkci
SYM_function *STF_add_symbol(ST_funcs *table, char *id);

// Funkce pro odstraneni symbolu z tabulky symbolu funkci
void STF_delete(ST_funcs *table, char *id);

// Funkce pro smazani cele tabulky symbolu funkci a uvolneni vsech alokovanych zdroju
void STF_delete_all(ST_funcs *table);

// Struktura symbolu v tabulce symbolu promennych
typedef struct sym_variable{
  D_str id;
  DATA_TYPE data_type;
  struct sym_variable *next;
  int level;
} SYM_variable;

// Tabulka symbolu pro promenne
typedef SYM_variable *ST_vars[HT_SIZE];

// Seznam tabulek symbolu promennych
typedef struct st_l_vars{
  ST_vars *sym_table;
  struct st_l_vars *upper;
  int level;
}ST_L_vars;

// Inicializace dilci tabulky symbolu promennych
void STV_init(ST_vars *table);

// Funkce pro smazani cele dilci tabulky promennych a uvolneni alokovanych zdroju
void STV_dispose(ST_vars *table);

// Funkce pro pridani symbolu do dilci tabulky symbolu promennych
SYM_variable *STV_add_symbol(ST_vars *table, char *id, int level);


// Funkce pro inicializaci seznamu tabulek symbolu promennych
int STV_L_init(ST_L_vars *table);

// Funkce pro pridanni tabulky symbolu promennych na konec seznamu
int STV_L_deeper(ST_L_vars **list);

// Funkce pro odstranění posledni (nejhloubejsi) tabulky symbolu ze seznamu
void STV_L_upper(ST_L_vars **table);

// Funkce ktera vraci momentalni uroven zanoreni
int STV_L_get_current_level(ST_L_vars *list);

// Funkce pro pridani symbolu do posledni (nejhloubejsi) tabulky symbolu ze seznamu
SYM_variable *STV_L_add_symbol(ST_L_vars *list, char *id);

// Funkce pro vyhledani symbolu v listu tabulek symbolu promennych
SYM_variable *STV_L_search(ST_L_vars *list, char *id);

// Funkce pro odstranění posledni (nejhloubejsi) tabulky symbolu ze seznamu
void STV_L_dispose(ST_L_vars *list);


#endif
