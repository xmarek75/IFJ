/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: expression.c
* Popis souboru: implementace zpracovani vyrazu
* Autor: Daniel Ponizil (xponiz01)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/

/**********************************************************************
* Expression se zabyva Precedencni analyzou
* Vraci zpet strukturu expr_return (zde jako ret_expr)
* Pokud najde chybu, vrati zpatky do syntax_analysis danou chybu
* Pokud vse projde, vrati zpatky ERR_OK a posledni index

**********************************************************************/

#include "expression.h"
#include "scanner.h"
#include "stack.h"
#include "error.h"
#include "code_generator.h"
#include "symtable.h"

#include <stdio.h>
#include <stdlib.h>

void token_stack_pop_count(token_stack *s, int counter);
static int reduce(Data *data);
static expr_table_index syntax_rules(int count, token_stack_item *it1, token_stack_item *it2, token_stack_item *it3, Data *data);
static DATA_TYPE token_data_type(Data *data);
static expr_table_tokens token_type(Token *token, expr_table_tokens prev_token);
static expr_table_index get_index(expr_table_tokens token);
static int semantic(expr_table_rules token_rule, token_stack_item *it1, token_stack_item *it2, token_stack_item *it3, DATA_TYPE *final_data_type);
/**
 * Precedencni tabulka
 */
int expr_table[9][9] = {
    //|# |*,/,//| +,-| ..  | REL |  id |  (  |  )  |  $  |
    {O_LE, O_MO, O_MO, O_MO, O_MO, O_LE, O_LE, O_ER, O_MO}, // #
    {O_LE, O_MO, O_MO, O_MO, O_MO, O_LE, O_LE, O_MO, O_MO}, // *,/,//
    {O_LE, O_LE, O_MO, O_MO, O_MO, O_LE, O_LE, O_MO, O_MO}, // +,-
    {O_LE, O_LE, O_LE, O_LE, O_MO, O_LE, O_LE, O_MO, O_MO}, // ..
    {O_LE, O_LE, O_LE, O_LE, O_ER, O_LE, O_LE, O_MO, O_MO}, // REL < <= > >= == ~=
    {O_ER, O_MO, O_MO, O_MO, O_MO, O_ID, O_ER, O_MO, O_MO}, // id
    {O_LE, O_LE, O_LE, O_LE, O_LE, O_LE, O_LE, O_EQ, O_ER}, // (
    {O_MO, O_MO, O_MO, O_MO, O_MO, O_ER, O_ER, O_MO, O_MO}, // )
    {O_LE, O_LE, O_LE, O_LE, O_LE, O_LE, O_LE, O_ER, O_ER}  // $
};

// pristup ke stacku
token_stack stack;

// pristup ke strukture expr_return
expr_return ret_expr;


/**
 * Z tokenu ziska index specificky pro precedencni tabulku
 */
static expr_table_index get_index(expr_table_tokens token)
{
    switch (token)
    {
    case HASH:
        return I_HASH;
    case MUL:
    case DIV:
    case INT_DIV:
        return I_MUL_DIV_INTDIV;
    case ADD:
    case SUB:
        return I_ADD_SUB;
    case CONCAT:
        return I_CONCAT;
    case LESS:
    case LESS_EQ:
    case MORE:
    case MORE_EQ:
    case EQ:
    case NOT_EQ:
        return I_REL;
    case ID:
        return I_ID;
    case INTEGER:
    case DOUBLE:
    case STRING:
    case NIL:
        return I_ID;
    case LEFT_BRACKET:
        return I_LBRACKET;
    case RIGHT_BRACKET:
        return I_RBRACKET;
    default:
        return I_DOLLAR;
    }
}

/**
 * Zjednoduseni prace s typem tokenu
 * S typem nasledne pracujeme pri ziskani pravidla precedencni syntakticke analyzy
 */
static expr_table_tokens token_type(Token *token, expr_table_tokens prev_token)
{
    switch (token->type)
    {
    case TYPE_len:
        return HASH;
    case TYPE_mul:
        return MUL;
    case TYPE_div:
        return DIV;
    case TYPE_int_div:
        return INT_DIV;
    case TYPE_add:
        return ADD;
    case TYPE_sub:
        return SUB;
    case TYPE_concat:
        return CONCAT;
    case TYPE_lt:
        return LESS;
    case TYPE_lt_eq:
        return LESS_EQ;
    case TYPE_gt:
        return MORE;
    case TYPE_gt_eq:
        return MORE_EQ;
    case TYPE_eq:
        return EQ;
    case TYPE_not_eq:
        return NOT_EQ;
    case TYPE_l_par:
        return LEFT_BRACKET;
    case TYPE_r_par:
        return RIGHT_BRACKET;
    case TYPE_identifier:
        if (prev_token == ID || prev_token == INTEGER || prev_token == STRING || prev_token == DOUBLE || prev_token == NIL || prev_token == DOLLAR)
        {
            return DOLLAR;
        }
        else
        {
            return ID;
        }
    case TYPE_integer:
        return INTEGER;
    case TYPE_number:
        return DOUBLE;
    case TYPE_string:
        return STRING;
    case TYPE_keyword:
        if (token->attribute.kw == KW_nil)
        {
            return NIL;
        }
        else
        {
            return DOLLAR;
        }
    default:
        return DOLLAR;
    }
}
/**
 * Funkce ziskava datovy typ
 * Dulezity pro praci v semanticke analyze
 */
static DATA_TYPE token_data_type(Data *data)
{
    SYM_variable *s;
    if (data->token->type == TYPE_integer)
    {
        return DT_INT;
    }
    else if (data->token->type == TYPE_number)
    {
        return DT_NUM;
    }
    else if (data->token->type == TYPE_string)
    {
        return DT_STR;
    }
    else if (data->token->type == TYPE_keyword && data->token->attribute.kw == KW_nil)
    {
        return DT_NIL;
    }
    else if (data->token->type == TYPE_identifier)
    {
        s = STV_L_search(data->var_table, data->token->attribute.str.string);
        if (s == NULL)
        {
            return DT_UNDEF;
        }
        return s->data_type;
    }
    else
    {
        DT_UNDEF;
    }
}

/**
 * Funkce reduce spocita pocet prvku po prvek stop
 * Podle poctu prvku zavola tomu urcenou Precedencni syntaktickou analyzu
 * Z precedencni syntakticke analyzy dostane pravidlo
 * Pokud pravidlo je R_NOT_A_RULE tak ukoncuje funkci s chybou ERR_SYN
 * Jinak vola funkci semanticke analyzy, ktere dane pravidlo preda
 * Na konci funkce zavola funkce, ktere vymazou ze zasobniku data po prvek STOP a vlozi tam neterminal obsahujici datovy typ
 */
static int reduce(Data *data)
{
    int result;

    expr_table_rules rule_for_semantic;
    DATA_TYPE final_data_type;

    bool isFound = false;
    int counter = 0;

    token_stack_item *it1 = NULL;
    token_stack_item *it2 = NULL;
    token_stack_item *it3 = NULL;

    token_stack_item *tmp = token_on_top(&stack);
    for (; tmp != NULL; tmp = tmp->nextPtr)
    {
        if (tmp->data != STOP)
        {
            isFound = false;
            counter++;
        }
        else
        {
            isFound = true;
            break;
        }
    }

    if (isFound == false)
    {
        return ERR_SYN;
    }
    if (counter == 3 && isFound)
    {
        it1 = stack.topPtr->nextPtr->nextPtr;
        it2 = stack.topPtr->nextPtr;
        it3 = stack.topPtr;
        rule_for_semantic = syntax_rules(counter, it1, it2, it3, data);
    }
    else if (counter == 1 && isFound)
    {
        it1 = stack.topPtr;
        rule_for_semantic = syntax_rules(counter, it1, NULL, NULL, data);
    }
    else if (counter == 2 && isFound)
    {
        it1 = stack.topPtr->nextPtr;
        it2 = stack.topPtr;
        rule_for_semantic = syntax_rules(counter, it1, it2, NULL, data);
    }
    else
    {
        return ERR_SYN;
    }

    if (rule_for_semantic != R_NOT_A_RULE)
    {
        if (result = semantic(rule_for_semantic, it1, it2, it3, &final_data_type))
        {
            return result;
        }
        token_stack_pop_count(&stack, counter + 1);
        if(rule_for_semantic == R_BRACKETS)
        {
            (data->term_index)--;
        }
        token_stack_push(&stack, NON_TERMINAL, final_data_type, data->term_index);
        if(rule_for_semantic == R_BRACKETS)
        {
            (data->term_index)++;
        }
        data->term_index++; // TODO
        ret_expr.dtype = final_data_type;
    }
    else
    {
        return ERR_SYN;
    }

    return ERR_OK;
}

/**
 * Precedencni syntakticka analyza
 * Podle poctu prvku 1/2/3 predany v parametru counter
 * Otestuje data predana v parametrech it1, it2 a it3 a vrati pravidlo jim urcene
 * Pokud nenajde zadne pravidlo, vrati R_NOT_A_RULE
 */
static expr_table_index syntax_rules(int counter, token_stack_item *it1, token_stack_item *it2, token_stack_item *it3, Data *data)
{
    if (counter == 1)
    {
        if (it1->data == ID || it1->data == INTEGER || it1->data == DOUBLE || it1->data == STRING || it1->data == NIL)
        {
            if (it1->data == INTEGER)
            {
                generate_expr_int_nonterm(data->last_term_value.attribute.int_val, data->term_index, data->expression_index);
            }
            else if (it1->data == DOUBLE)
            {
                generate_expr_num_nonterm(data->last_term_value.attribute.num_val, data->term_index, data->expression_index);
            }
            else if (it1->data == STRING)
            {
                generate_expr_str_nonterm(data->last_term_value.attribute.str, data->term_index, data->expression_index);
            }
            else if (it1->data == NIL)
            {
                generate_expr_nil_nonterm(data->term_index, data->expression_index);
            }
            else if (it1->data == ID)
            {
                SYM_variable *tmp = STV_L_search(data->var_table, data->last_term_value.attribute.str.string);
                if(tmp == NULL)
                {
                    return ERR_SEM_DEFINITION;
                }
                generate_expr_id_nonterm(data->last_term_value.attribute.str.string, tmp->level, data->term_index, data->expression_index);
            }
            return R_OPERAND;
        }
        else
        {
            return R_NOT_A_RULE;
        }
    }
    else if (counter == 3)
    {
        if (it1->data == LEFT_BRACKET && it2->data == NON_TERMINAL && it3->data == RIGHT_BRACKET)
        {
            return R_BRACKETS;
        }
        if (it1->data == NON_TERMINAL && it3->data == NON_TERMINAL)
        {
            if (it2->data == MUL)
            {
                generate_expr_mul(data->term_index, it1->nonterm_index, it3->nonterm_index, data->expression_index);
                return R_MUL;
            }
            else if (it2->data == DIV)
            {
                generate_expr_div(data->term_index, it1->nonterm_index, it3->nonterm_index, data->expression_index);
                return R_DIV;
            }
            else if (it2->data == INT_DIV)
            {
                generate_expr_int_div(data->term_index, it1->nonterm_index, it3->nonterm_index, data->expression_index);
                return R_INT_DIV;
            }
            else if (it2->data == ADD)
            {
                generate_expr_add(data->term_index, it1->nonterm_index, it3->nonterm_index, data->expression_index);
                return R_ADD;
            }
            else if (it2->data == SUB)
            {
                generate_expr_sub(data->term_index, it1->nonterm_index, it3->nonterm_index, data->expression_index);
                return R_SUB;
            }
            else if (it2->data == CONCAT)
            {
                generate_expr_concat(data->term_index, it1->nonterm_index, it3->nonterm_index, data->expression_index);
                return R_CONCAT;
            }
            else if (it2->data == LESS)
            {
                generate_expr_less(data->term_index, it1->nonterm_index, it3->nonterm_index, data->expression_index);             //////  druhy beh
                return R_LESS;
            }
            else if (it2->data == LESS_EQ)
            {
                generate_expr_less_eq(data->term_index, it1->nonterm_index, it3->nonterm_index, data->expression_index);
                return R_LESS_EQ;
            }
            else if (it2->data == MORE)
            {
                generate_expr_more(data->term_index, it1->nonterm_index, it3->nonterm_index, data->expression_index);
                return R_MORE;
            }
            else if (it2->data == MORE_EQ)
            {
                generate_expr_more_eq(data->term_index, it1->nonterm_index, it3->nonterm_index, data->expression_index);
                return R_MORE_EQ;
            }
            else if (it2->data == EQ)
            {
                generate_expr_eq(data->term_index, it1->nonterm_index, it3->nonterm_index, data->expression_index);
                return R_EQ;
            }
            else if (it2->data == NOT_EQ)
            {
                generate_expr_not_eq(data->term_index, it1->nonterm_index, it3->nonterm_index, data->expression_index);
                return R_NOT_EQ;
            }
            else
            {
                return R_NOT_A_RULE;
            }
        }
        return R_NOT_A_RULE;
    }
    else if (counter == 2)
    {
        if (it1->data == HASH)
        {
            generate_expr_hash(data->last_term_value.attribute.str, data->term_index, data->expression_index);
            return R_HASH;
        }
    }
    return R_NOT_A_RULE;
}

/**
 * Podle parametru counter 2-4 maze ze zasobniku data
 * Podle poctu prvku by mel vzdy smazat prvek Stop a pak se ukoncit
 */
void token_stack_pop_count(token_stack *s, int counter)
{
    int i = 0;
    while (i < counter)
    {
        token_stack_pop(s);
        i++;
    }
}

/**
 * Semanticka analyza podle pravidla (ziskaneho ze syntakticke analyzy) kontroluje, zda datove typy
 * jsou navzajem kompatibilni
 * Kontroluje, zda jsou promene definovane
 * Pokud vsechno odpovida, vrati ERR_OK, naopak vraci typ problemu
 */
static int semantic(expr_table_rules sem_rule, token_stack_item *it1, token_stack_item *it2, token_stack_item *it3, DATA_TYPE *final_data_type)
{
    if (sem_rule != R_BRACKETS && sem_rule != R_OPERAND && sem_rule != R_HASH)
    {
        if (it1->type == DT_UNDEF || it3->type == DT_UNDEF)
        {
            return ERR_SEM_DEFINITION;
        }
    }

    if (sem_rule == R_HASH)
    {
        if (it2->type == DT_STR)
        {
            *final_data_type = DT_INT;
            return ERR_OK;
        }
        else
        {
            return ERR_SEM_TYPE_COMP;
        }
    }
    else if (sem_rule == R_BRACKETS || sem_rule == R_OPERAND)
    {
        if (it1->type == DT_UNDEF)
        {
            return ERR_SEM_DEFINITION;
        }

        if (sem_rule == R_BRACKETS)
        {
            *final_data_type = it2->type;
        }
        else if (sem_rule == R_OPERAND)
        {
            *final_data_type = it1->type;
        }
    }
    else if (sem_rule == R_EQ || sem_rule == R_NOT_EQ)
    {
        if ((it1->type != DT_NIL) && (it3->type != DT_NIL))
        {
            if (it1->type != it3->type)
            {
                return ERR_SEM_TYPE_COMP;
            }
        }
    }
    else if (sem_rule == R_LESS || sem_rule == R_LESS_EQ || sem_rule == R_MORE || sem_rule == R_MORE_EQ)
    {
        if (it1->type == DT_STR || it3->type == DT_STR)
        {
            return ERR_SEM_TYPE_COMP;
        }
        else if (it1->type != it3->type)
        {
            return ERR_SEM_TYPE_COMP;
        }
    }
    else if (sem_rule == R_ADD)
    {
        if (it1->type == DT_STR || it3->type == DT_STR)
        {
            return ERR_SEM_TYPE_COMP;
        }
        else if (it1->type == DT_INT && it3->type == DT_INT)
        {
            *final_data_type = DT_INT;
        }
        else if (it1->type == DT_NUM && it3->type == DT_NUM)
        {
            *final_data_type = DT_NUM;
        }
        else if (it1->type == DT_INT && it3->type == DT_NUM)
        {
            *final_data_type = DT_NUM;
        }
        else if (it1->type == DT_NUM && it3->type == DT_INT)
        {
            *final_data_type = DT_NUM;
        }
        else
        {
            return ERR_SEM_TYPE_COMP;
        }
    }
    else if (sem_rule == R_SUB || sem_rule == R_MUL)
    {
        if (it1->type == DT_STR || it3->type == DT_STR)
        {
            return ERR_SEM_TYPE_COMP;
        }
        else if (it1->type == DT_INT && it3->type == DT_INT)
        {
            *final_data_type = DT_INT;
        }
        else if (it1->type == DT_NUM && it3->type == DT_NUM)
        {
            *final_data_type = DT_NUM;
        }
        else if (it1->type == DT_INT && it3->type == DT_NUM)
        {
            *final_data_type = DT_NUM;
        }
        else if (it1->type == DT_NUM && it3->type == DT_INT)
        {
            *final_data_type = DT_NUM;
        }
        else
        {
            return ERR_SEM_TYPE_COMP;
        }
    }
    else if (sem_rule == R_DIV)
    {
        if (it1->type == DT_STR || it3->type == DT_STR)
        {
            return ERR_SEM_TYPE_COMP;
        }
        else if (it1->type == DT_INT && it3->type == DT_INT)
        {
            *final_data_type = DT_NUM;
        }
        else if (it1->type == DT_NUM && it3->type == DT_NUM)
        {
            *final_data_type = DT_NUM;
        }
        else if (it1->type == DT_INT && it3->type == DT_NUM)
        {
            *final_data_type = DT_NUM;
        }
        else if (it1->type == DT_NUM && it3->type == DT_INT)
        {
            *final_data_type = DT_NUM;
        }
        else
        {
            return ERR_SEM_TYPE_COMP;
        }
    }
    else if (sem_rule == R_INT_DIV)
    {
        if (it1->type == DT_INT && it3->type == DT_INT)
        {
            *final_data_type = DT_NUM;
        }
        else
        {
            return ERR_SEM_TYPE_COMP;
        }
    }
    else if (sem_rule == R_CONCAT)
    {
        if (it1->type == DT_STR && it3->type == DT_STR)
        {
            *final_data_type = DT_STR;
        }
        else
        {
            return ERR_SEM_TYPE_COMP;
        }
    }
    return ERR_OK;
}

/**
 * Hlavni funkce expression je volana ze syntakticke analyzy
 * Podle pravidel Precedencni tabulky
 * O_MO - spusti funkci reduce, ukonci program pokud funkce reduce nasla nekde chybu
 * O_LE - vlozi na zasobnik prvek Stop, vlozi aktualni token a ziska novy (funkce get_token)
 * O_EQ - vlozi na zasobnik aktualni token a ziska novy (get_token)
 * O_ER - pokud je zasobnik vyprazdnen (zustaly jen $), vrati ERR_OK, naopak vrati ERR_SYN
 * I_ID - pravidlo pro praci s vice korektnimy vyrazy za sebou, pokud se sem program dostane, ukonci se jako v poradku
 */
expr_return expression(Data *data)
{
    token_stack_init(&stack);
    data->term_index = 0;
    int test = token_stack_push(&stack, DOLLAR, DT_UNDEF, data->term_index);
    if (!test)
    {
        token_stack_free(&stack);
        ret_expr.err = ERR_INTERNAL;
        return ret_expr;
    }

    int result = ERR_SYN;

    token_stack_item *terminal_on_top;
    expr_table_tokens actual_token;
    expr_table_tokens prev_token = EQ;

    bool correct = false;
    do
    {
        actual_token = token_type(data->token, prev_token);
        prev_token = actual_token;

        terminal_on_top = token_stack_top_term(&stack);

        if (terminal_on_top == NULL)
        {
            token_stack_free(&stack);
            ret_expr.err = ERR_INTERNAL;
            return ret_expr;
        }

        //ziskani indexu z tabulky
        char index = expr_table[get_index(terminal_on_top->data)][get_index(actual_token)];
        if (index == O_ID)
        {
            token_stack_free(&stack);
            (data->term_index)--;
            ret_expr.err = ERR_OK;
            return ret_expr;
        }
        else if (index == O_MO)
        {
            result = reduce(data);
            if (result)
            {
                token_stack_free(&stack);
                ret_expr.err = result;
                return ret_expr;
            }
        }
        else if (index == O_ER)
        {
            if (actual_token == DOLLAR && terminal_on_top->data == DOLLAR)
            {
                correct = true;
            }
            else
            {
                token_stack_free(&stack);
                ret_expr.err = ERR_SYN;
                return ret_expr;
            }
        }
        else if (index == O_LE)
        {
            if (!token_stack_insert_after_top_terminal(&stack, STOP, DT_UNDEF))
            {
                token_stack_free(&stack);
                ret_expr.err = ERR_INTERNAL;
                return ret_expr;
            }
            if (!token_stack_push(&stack, actual_token, token_data_type(data), data->term_index))
            {
                token_stack_free(&stack);
                ret_expr.err = ERR_INTERNAL;
                return ret_expr;
            }
            if (data->token->type == TYPE_string || data->token->type == TYPE_identifier)
            {
                if(dstr_init(&(data->last_term_value.attribute.str)))
                {
                    ret_expr.err = ERR_INTERNAL;
                    return ret_expr;
                }

                if(dstr_copy_dstr(&(data->token->attribute.str), &(data->last_term_value.attribute.str)))
                {
                    ret_expr.err = ERR_INTERNAL;
                    return ret_expr;
                }
                data->last_term_value.type = data->token->type;
            }
            else
            {
                data->last_term_value.attribute = data->token->attribute;
                data->last_term_value.type = data->token->type;
            }
            if (result = get_token(data->token))
            {

                token_stack_free(&stack);
                ret_expr.err = result;
                return ret_expr;
            }
        }
        else if (index == O_EQ)
        {
            token_stack_push(&stack, actual_token, token_data_type(data),data->term_index);
            result = get_token(data->token);
            if (result)
            {
                token_stack_free(&stack);
                ret_expr.err = result;
                return ret_expr;
            }
        }
    } while (!correct);

    token_stack_item *last_token = token_on_top(&stack);
    if (last_token == NULL)
    {
        token_stack_free(&stack);
        ret_expr.err = ERR_INTERNAL;
        return ret_expr;
    }
    if (last_token->data != NON_TERMINAL)
    {
        token_stack_free(&stack);
        ret_expr.err = ERR_SYN;
        return ret_expr;
    }

    token_stack_free(&stack);
    (data->term_index)--;
    ret_expr.err = ERR_OK;
    return ret_expr;
}
