/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: expression.c
* Popis souboru: rozhrani pro implementaci zpracovani vyrazu
* Autor: Daniel Ponizil (xponiz01), Pavel Marek (xmarek75)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "scanner.h"
#include "symtable.h"
#include "param_stack.h"
#include "dstr_queue.h"
////////////////////
typedef struct
{
    ST_funcs *function_table;
    SYM_function *current_function;
    SYM_function *cmp_function;

    ST_L_vars *var_table;
    SYM_variable *current_var;
    Token *token;

    /*pomocne*/
    unsigned int counter_param;
    unsigned int counter_rets;
    bool declare;
    bool define;
    bool returns;
    bool call;
    bool call_param;
    bool token_returned;

    //generovani kodu
    D_str called_fnc_id;
    D_str defined_fnc_id;
    D_str defined_var_id;
    int var_level;
    bool write_fnc;
    param_stack *p_stack;
    dstr_queue *queue;

    // expressions generovani
    int term_index;
    Token last_term_value;
    int expression_index;
    bool in_return;
    int cur_ret_index;

    // generovani if
    int if_index;
    int while_index;
    int params_index;

} Data;

typedef struct
{
    int err;
    Token token;
    DATA_TYPE dtype;
} expr_return;

/**
 * @enum Prioritni operatory
 */
typedef enum
{
    O_LE, // <
    O_MO, // >
    O_EQ, // =
    O_ER, // error
    O_ID, // ID
} expr_table_oper;

/**
 * @enum Vsechny tokeny vyuzite u precedencni analyzy
 */
typedef enum
{
    HASH,          // #
    MUL,           // *
    DIV,           // /
    INT_DIV,       // //
    ADD,           // +
    SUB,           // -
    CONCAT,        // ..
    LESS,          // <
    LESS_EQ,       // <=
    MORE,          // >
    MORE_EQ,       // >=
    EQ,            // ==
    NOT_EQ,        // ~=
    ID,            // ID
    LEFT_BRACKET,  // (
    RIGHT_BRACKET, // )
    DOLLAR,        // $
    INTEGER,       //int
    DOUBLE,        //double
    STRING,        //string
    NIL,           //NILL = NULL
    STOP,          //for stack when reducing
    NON_TERMINAL   //not a terminal
} expr_table_tokens;

/**
 * @enum Pravidla pro precedencni syntaktickou analyzu
 */
typedef enum
{
    R_OPERAND,    // E -> i
    R_NOT_A_RULE, // rule doesn't exist
    R_MUL,        // E -> E * E
    R_DIV,        // E -> E / E
    R_INT_DIV,    // E -> E // E
    R_ADD,        // E -> E + E
    R_SUB,        // E -> E - E
    R_CONCAT,     // ..
    R_LESS,       // E -> E < E
    R_LESS_EQ,    // E -> E <= E
    R_MORE,       // E -> E > E
    R_MORE_EQ,    // E -> E => E
    R_EQ,         // E -> E = E
    R_NOT_EQ,     // E -> E ~= E
    R_BRACKETS,   // E -> (E)
    R_HASH,       // E -> #E
} expr_table_rules;

/**
 * @enum Indexy z precedencni tabulky
 */
typedef enum
{
    I_HASH,           // #
    I_MUL_DIV_INTDIV, // * / //
    I_ADD_SUB,        // + -
    I_CONCAT,         // ..
    I_REL,            // REL Operators < <= > => == ~=
    I_ID,             // ID
    I_LBRACKET,       // (
    I_RBRACKET,       // )
    I_DOLLAR,         // $
} expr_table_index;

// deklarace hlavni funkce pro expression.c
expr_return expression(Data *data);

#endif
