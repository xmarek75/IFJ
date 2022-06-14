/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: dynamic_string.h
* Popis souboru: rozhrani knihovny pro praci s nekonecne dlouhymi retezci
* Autor: Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/

#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

#define DS_SUCCES 0 // operace probehla uspesne
#define DS_ERROR 1  // operace selhala

#define DEF_SIZE 16 // vychozi velikost retezce

/**
 * @struct Struktura reprezentujici dynamicky retezec
 */
typedef struct
{
    char* string; // ukazatel na retezec
    unsigned int lenght; // delka retezce
    unsigned int alloc_size; // velikost alokovane pameti
} D_str;

/**
 * Funkce pro inicializaci dynamickeho retezce
 * @param ds Ukazatel na dynamicky retezec
 * @return Vraci 0 pokud byla inicializace uspesna, jinak 1
 */
int dstr_init(D_str *ds);

/**
 * Procedura pro uvolneni alokovane pameti dynamickeho retezce
 * @param ds Ukazatel na dynamicky retezec
 */
void dstr_free(D_str *ds);

/**
 * Procedura pro uvedeni dyn. retezce do stavu po inicializaci
 * @param ds Ukazatel na dynamicky retezec
 */
void dstr_clear(D_str *ds);

/**
 * Funkce pro pripojeni znaku za dynamicky retezec
 * @param ds Ukazatel na dynamicky retezec
 * @param c  Pripojovany znak
 * @return Vraci 0 pokud byla operace uspesna, jinak 1
 */
int dstr_add_char(D_str *ds, char c);

/**
 * Funkce pro pripojeni retezce za dynamicky retezec
 * @param ds Ukazatel na dynamicky retezec
 * @param ss Ukazatel na retezec
 * @return Vraci 0 pokud byla operace uspesna, jinak 1
 */
int dstr_add_str(D_str *ds, char *s);

/**
 * Funkce pro porovnani dvou dynamickych retezcu
 * @param ds1 Ukazatel na prvni dynamicky retezec
 * @param ds2 Ukazatel na druhhy dynamicky retezec
 * @return Vraci 0 pokud jsou retezce shodne, jinak 1
 */
int dstr_cmp_dstr(D_str *ds1, D_str *ds2);

/**
 * Funkce pro porovnani dynamickeo retezce s retezcem
 * @param ds Ukazatel na dynamicky retezec
 * @param s Ukazatel na retezec
 * @return Vraci 0 pokud jsou retezce shodne, jinak 1
 */
int dstr_cmp_str(D_str *ds, char *s);

/**
 * Funkce pro vyvoreni kopie obsahu dynamickeho retezce
 * @param src Ukazatel na puvodni dynamicky retezec
 * @param dst Ukazatel na cilovy dynamicky retezec
 * @return Vraci 0 pokud byla operace uspesna, jinak 1
 */
int dstr_copy_dstr(D_str *src, D_str *dest);

#endif
