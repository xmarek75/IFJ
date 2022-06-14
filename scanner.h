/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: scanner.h
* Popis souboru: rozhrani pro implementaci scanneru
* Autori: Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/
#ifndef SCANNER
#define SCANNER

#include "dynamic_string.h"

/**
 * @brief Rozhrani lexikalniho analyzator prekladace
 *
 * @author Vilem Gottwald [xgottw07]
 */

/**
 * @brief  Vycet moznych typu tokenu
 *
 */
typedef enum
{
    TYPE_keyword,
    TYPE_identifier,

    TYPE_integer,
    TYPE_number,
    TYPE_string,

    TYPE_l_par,     // (
    TYPE_r_par,     // )
    TYPE_colon,     // :
    TYPE_comma,     // ,

    TYPE_assign,    // =

    TYPE_len,       // #
    TYPE_mul,       // *
    TYPE_div,       // /
    TYPE_int_div,   // //
    TYPE_add,       // +
    TYPE_sub,       // -
    TYPE_concat,    // ..
    TYPE_lt,        // <
    TYPE_lt_eq,     // <=
    TYPE_gt,        // >
    TYPE_gt_eq,     // >=
    TYPE_eq,        // ==
    TYPE_not_eq,    // ~=

    TYPE_EOF        // EOF - end of scanned code
}Type;


/**
 * @brief  Vycet klicovych slov, ketere muze token reprezentovat
 *
 */
typedef enum
{
    KW_do,
    KW_else,
    KW_end,
    KW_function,
    KW_global,
    KW_if,
    KW_integer,
    KW_local,
    KW_nil,
    KW_number,
    KW_require,
    KW_return,
    KW_string,
    KW_then,
    KW_while
} Keyword;

/**
 * @brief Unie reprezentujici atribut tokenu
 * Pokud je token:
 * - retezec - obsah retezce je ulozen v str
 * - integer hodota - hodnota je ulozena v int_val
 * - number hodnota - hodnota je ulozena v num_val
 * - klicove slovo - konkretni klicove slovo je ulozeno v kw
 *
 */
typedef union
{
    D_str str;
    int int_val;
    double num_val;
    Keyword kw;
}Attribute;

/**
 * @brief Struktura reprezetujici token
 * Sklada se z:
 * - Typ: klicove slovo / retezec / celociselna hodnota / desetinna hodota
 * - Atribut: viz. Unie reprezentujici atribut tokenu
 */
typedef struct
{
    Type type;
    Attribute attribute;
}Token;


/**
 * Funkce, ktera uvolni alokovanou pamet tokenu
 *
 * @param token Ukazatel na token
 */
void destroy_token(Token *token);

/**
 * Funkce, ktera prevede token identifikatoru na token klicoveho slova, pokud je identifikatorem jedno z klicovych slov
 *
 * @param token Ukazatel na token
 */
void assign_keyword(Token *token);

/**
 * Funkce pro prevod celeho cisla ulozeneho jako retezec v atributu tokenu
 * na atribut reprezentujici celociselnou hodnotu
 *
 * @param token Ukazatel na token
 */
int assign_int( Token *token);

/**
 * Funkce pro prevod desetinneho cisla ulozeneho jako retezec v atributu tokenu
 * na atribut reprezentujici desetinnou hodnotu
 *
 * @param token Ukazatel na token
 */
int assign_num( Token *token);

/**
 * Funkce pro ziskani tokenu ze standartniho vstupu
 *
 * @param token Ukazatel na token, jehoz hodnoty tato funkce nahradi novymi
 * @return int Chybovy kod ERR_OK pokud probehlo ziskani tokenu uspesne,
 *                         ERR_LEX pokud je na vstupu lexikalni chyba,
 *                         ERR_INTERNAL pokud doslo k interni chybe programu
 */
int get_token(Token *token);
#endif
