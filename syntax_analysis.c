/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: syntax_analysis.c
* Popis souboru: implementace syntakticke a semanticke analyzy
* Autori: Pavel Marek(xmarek75), Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/

#include "dynamic_string.h"
#include "error.h"
#include "syntax_analysis.h"
#include <stdbool.h>
#include "symtable.h"
#include "scanner.h"
#include "code_generator.h"
#include "param_stack.h"
#include "dstr_queue.h"

#define GET_TOKEN_ERR()    if (data->token_returned)                                        \
                            {                                                               \
                                data->token_returned = false;                               \
                            }                                                               \
                            else                                                            \
                            {                                                               \
                                if ((err = get_token(data->token)) != ERR_OK)  return err;  \
                            }

#define PROCEED(rule) if ((err = rule(data)) != ERR_OK) return err;
#define CHECK_TOKEN_KW(keyword) if (data->token->type != TYPE_keyword || data->token->attribute.kw != keyword) return ERR_SYN;
#define CHECK_TOKEN_TYPE(TYPE) if (data->token->type != TYPE) return ERR_SYN;

#define CHECK_VAR_AND_FUNC_ID()                 \
if(!check_var_id(data))                         \
        {                                       \
            if(!id_in_glob_symt(data))          \
            {                                   \
                return ERR_SEM_DEFINITION;      \
            }                                   \
            else                                \
            {                                   \
                data->call = true;              \
            }                                   \
        }                                       \
        else                                    \
        {                                       \
            ERR_SEM_DEFINITION;                 \
        }

#define CHECK_ID_IN_TABLE() if( (id_in_glob_symt(data)) == false) return ERR_SEM_DEFINITION; //kontrola jestli funkce je deklarovana nebo definovana
#define CHECK_VAR_ID() if((check_var_id(data)) == false) return ERR_SEM_DEFINITION;         //kontrola jestli promenna byla definovana
#define CHECK_PARAMS_IN_FUNCTION() if(cmp_params_in_function(data) == false) return ERR_SEM_FUNC; //kontrola parametru ve funkci pri definici deklarovane funkce
#define CHECK_PARAMS_IN_CALL() if(cmp_params_in_call(data) == false) return ERR_SEM_FUNC;       //kontrola parametru ve funkci pri volani funkce
#define CHECK_RET_IN_FUNCTION() if(cmp_ret_in_function(data) == false) return ERR_SEM_FUNC; //kontrola navratovych hodnot pri deifnici funnkce ktera jiz byla deklarovana
#define DECLARE_FUNCTION() if((err = declare_function(data)) != ERR_OK) return err; //deklarace funkce
#define DEFINE_FUNCTION() if((err = define_function(data)) != ERR_OK) return err; //definovani funkce
#define SET_PARAMS() if(set_param_to_function(data)== false) return ERR_SEM_DEFINITION; //nastaveni parametru funkce
#define SET_RETURNS() if(set_ret_to_function(data)== false) return ERR_SEM_DEFINITION;  //nastaveni navratovych hodnot funkce
#define CHECK_PARAM_COUNT() if(check_param_count(data)==false) return ERR_SEM_FUNC;  //kontrola poctu parametru
#define CHECK_RET_COUNT() if(check_ret_count(data)==false) return ERR_SEM_FUNC;     // kontrola poctu navratovych hodnot
#define CHECK_IFJ21() if(dstr_cmp_str(&(data->token->attribute.str), "ifj21")) return ERR_SEM_OTHERS;
#define EXPRESSION_ERR() expr_return ret = expression(data); if(ret.err != ERR_OK) return ret.err; // spusti expression a vrati hodnotu erroru
#define SAVE_CALLED_FNC_ID() if(save_called_fnc_id(data)!=ERR_OK) return ERR_INTERNAL;
#define SAVE_DEFINED_VAR_ID() if(save_defined_var_id(data)!=ERR_OK) return ERR_INTERNAL;
#define SAVE_DEFINED_FNC_ID() if(save_defined_fnc_id(data)!=ERR_OK) return ERR_INTERNAL;
#define VAR_ADD_SYMBOL_ERR() if((data->current_var = STV_L_add_symbol(data->var_table, data->token->attribute.str.string)) == NULL) {return ERR_INTERNAL;}
#define VAR_CHECK_FNC_IDENTIC_ID() if (var_check_identic_id(data)){return ERR_SEM_DEFINITION;}
#define VAR_ADD_DTYPE(datat) if (data->current_var) {data->current_var->data_type = datat; data->current_var = NULL;}
#define INSERT_IDENTIF_QUEUE() if (!dstr_queue_insert(data->queue, data->token->attribute.str)) return ERR_INTERNAL;
#define CHECK_NULL(fcall); if((fcall) == NULL) return ERR_INTERNAL;
#define CHECK_EXP_RET_TYPE() return check_exp_ret_type(data);
#define ZERO(); data->counter_param = 0; data->counter_rets = 0; data->declare = false; data->define = false; data->returns = false; data->call = false; data->write_fnc = false;

#define CHECK_TYPES_COUNT() if((err = check_types_count(data)) != ERR_OK) return err;
/*--------------------------------------------------------------------------------------------------------*/

/**-----------------------pomocne funkce---------------------------------*/





//funkce na kontrolu jestli je promenna definovana
bool check_var_id(Data *data)
{
    SYM_variable *tmp = STV_L_search(data->var_table, data->token->attribute.str.string);
    if(tmp != NULL)
    {
        data->current_var = tmp;
        return true;
    }
    else
    {
        return false;
    }
}
void p_error(int what_error)
{
    switch (what_error)
    {
        case ERR_OK:
            fprintf(stderr, "ERR_OK\n");
            break;
        case ERR_LEX:
            fprintf(stderr, "ERR_LEX\n");
            break;
        case ERR_SYN:
            fprintf(stderr, "ERR_SYN\n");
            break;
        case ERR_SEM_DEFINITION:
            fprintf(stderr, "ERR_SEM_DEFINITION\n");
            break;
        case ERR_SEM_ASSIGN:
            fprintf(stderr, "ERR_SEM_ASSIGN\n");
            break;
        case ERR_SEM_FUNC:
            fprintf(stderr, "ERR_SEM_FUNC\n");
            break;
        case ERR_SEM_TYPE_COMP:
            fprintf(stderr, "ERR_SEM_TYPE_COMP\n");
            break;
        case ERR_SEM_OTHERS:
            fprintf(stderr, "ERR_SEM_OTHERS\n");
            break;
        case ERR_NIL:
            fprintf(stderr, "ERR_NIL\n");
            break;
        case ERR_ZERO_DIV:
            fprintf(stderr, "ERR_ZERO_DIV\n");
            break;
        case ERR_INTERNAL:
            fprintf(stderr, "ERR_INTERNAL\n");
            break;
        default:
            fprintf(stderr, "alles gute\n");
    }
}

bool var_check_identic_id(Data *data)
{
    SYM_variable *tmp = STV_L_search(data->var_table, data->token->attribute.str.string);
    SYM_function *item = STF_search(data->function_table, data->token->attribute.str.string);
    if (tmp == NULL) // nebylo definovany jako promenna
    {
        if(item == NULL) //nebylo definovanyjako funkce ani deklarovany
        {
            return false;
        }
    }
    else if(STV_L_get_current_level(data->var_table) != tmp->level)
    {
        return false;
    }
    else
    {
        return true;
    }
}

//funkce na kontrolovani poctu parametru
bool check_param_count(Data *data) /// porovna counter parametru s poctem parametru ktere byly deklarovany
{
    if(!data->write_fnc)
    {
        if(data->counter_param == data->current_function->param_types.lenght)
        {
            return true;
        }
        else
        {
            //fprintf(stderr, "Spatny pocet parametru u definice funkce po deklaraci\n");
            return false;
        }
    }
    else
    {
        return true;
    }
}
//funkce na kontrolu poctu navratovych hodnot
bool check_ret_count(Data *data)
{
    if(data->counter_rets == data->current_function->ret_types.lenght)
    {
        return true;
    }
    else
    {
        //fprintf(stderr, "Spatny pocet navrat typu u definice funkce po deklaraci\n");
        return false;
    }
}
//funkce ktera zkontroluje pocet parametru i navratovych hodnot
int check_types_count(Data *data)
{
    if(!(data->current_function->defined))
    { // processing function & definition of function & declared before -> checking types */
        if(data->returns)
        {
            CHECK_RET_COUNT();
        }
        else
        {
            CHECK_PARAM_COUNT();
        }
    }
    return ERR_OK;
}

#define SET_TYPE_ERR() if((err = set_type(data)) != ERR_OK) return err; //nastavi typ parametru i navratovych hodnot
//nastavi typy parametru a navratovych hodnot u funkce
int set_type(Data *data)
{

    if (data->current_var)
    {
        data->current_var->data_type = KW_to_DT(data->token);
        data->current_var = NULL;
    }
    if(!(data->current_var))
    { // processing function
        if(data->declare)
        { // declaration of function
            if(data->returns)
            { // setting return types
                SET_RETURNS();
                (data->counter_rets)++;
            }
            else
            { // setting parameter types
                SET_PARAMS();
                (data->counter_param)++;
            }
        }
        else if(data->define)
        { // definition of function
            if(data->current_function->defined)
            { /* not declared before -> assign types */
                                        \
                if(data->returns)
                {
                    SET_RETURNS();
                    generate_return_value(data->counter_rets);
                    (data->counter_rets)++;

                }
                else
                {
                    SET_PARAMS();
                    (data->counter_param)++;
                }
            }
            else
            { /* declared before -> check types */

                if(data->returns)
                {
                    CHECK_RET_IN_FUNCTION();
                    (data->counter_rets)++;
                }
                else
                {
                    CHECK_PARAMS_IN_FUNCTION();
                    (data->counter_param)++;
                }
            }
        }
    }
    return ERR_OK;
}

// funkce na prevod stringu do DATA_TYPE
DATA_TYPE STR_TO_DT(Data *data)
{
    switch (data->current_function->param_types.string[data->counter_param])
    {
    case 'I':
        return DT_INT;
        break;
    case 'N':
        return DT_NUM;
        break;
    case 'S':
        return DT_STR;
        break;
    case 'Z':
        return DT_NIL;
        break;
    default:
        return DT_UNDEF;
    }
    return DT_UNDEF;
}
//funkce na prevod TYPE do DATA_TYPE
DATA_TYPE TYPE_TO_DT(Data *data) //u volani funkce
{
    switch (data->token->type)
    {
    case TYPE_integer:
        return DT_INT;
        break;
    case TYPE_number:
        return DT_NUM;
        break;
    case TYPE_string:
        return DT_STR;
        break;
    case TYPE_keyword:
        if(data->token->attribute.kw == KW_nil)
        return DT_NIL;
        break;
    default:
        return DT_UNDEF;
    }
    return DT_UNDEF;
}
// funkce na kontrolu parametru funkce pri volani funkce
bool cmp_params_in_call(Data *data) // porovna parametry zadane pres type s parametry funkce v niz se zrovna nachazima
{
    if(data->token->type == TYPE_identifier)
    {
        SYM_variable *var = STV_L_search(data->var_table, data->token->attribute.str.string);
        DATA_TYPE type = STR_TO_DT(data);
        if( (type == var->data_type ) || (type == DT_NUM && var->data_type == DT_INT))
            {
                return true;
            }
    }
    else if(data->current_function->param_types.string[data->counter_param] == TYPE_TO_DT(data))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//funkce na prevod KEW_WORD do DATA_TYPE
char KW_to_DT(Token *token)
{
    switch (token->attribute.kw)
    {
    case KW_integer:
        return DT_INT;
        break;
    case KW_number:
        return DT_NUM;
        break;
    case KW_string:
        return DT_STR;
        break;
    case KW_nil:
        return DT_NIL;
        break;
    default:
        return DT_UNDEF;
    }
    return DT_UNDEF;
}
// funkce pro kontrolu jest-li byla funkce deklarovana
bool id_in_glob_symt(Data *data)
{
    SYM_function *item = STF_search(data->function_table, data->token->attribute.str.string);
    if( item != NULL) // pokud plati podminka funkce byla drive deklarovana
    {
        data->current_function = item;
        return true;
    }
    else
    {
        return false; //funkce nebyla drive deklarovana
    }
}
//funkce pro kontrolu parametru funkce u definice funkce
bool cmp_params_in_function(Data *data) // porovna parametry zadane pres type s parametry funkce v niz se zrovna nachazima
{
    if(data->token->type == TYPE_identifier)
    {
        SYM_variable *var = STV_L_search(data->var_table, data->token->attribute.str.string);
        if(STR_TO_DT(data) == var->data_type || ( (STR_TO_DT(data) == DT_INT)&&(var->data_type == DT_NUM) ) )
            {
                return true;
            }
    }
    else if(data->current_function->param_types.string[data->counter_param] == KW_to_DT(data->token))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool cmp_ret_in_function(Data *data)
{
    if(data->current_function->ret_types.string[data->counter_rets] == KW_to_DT(data->token))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//funkce pro definovani funkce
int define_function(Data *data)
{
    SYM_function *item = STF_search(data->function_table, data->token->attribute.str.string);
    if(item) // pokud podminka plati tak funkce jiz byla drive deklarovana
    {
        if(item->defined)
        { // allready defined
            return ERR_SEM_DEFINITION;
        }
    }
    else
    { // put it into symtable
        if((data->current_function = STF_add_symbol(data->function_table, data->token->attribute.str.string)) != NULL)
        {
            if(dstr_copy_dstr(&(data->token->attribute.str), &(data->current_function->id)) == DS_ERROR)
            {
                return ERR_INTERNAL;
            }
            data->current_function->defined = true;
            return ERR_OK;
        }
        else
        {
            return ERR_INTERNAL;
        }
    }
    return ERR_OK;
}
// funkce pro deklaraci funkce
int declare_function(Data *data) // deklarace novych funkcich
{
    SYM_function *item = STF_search(data->function_table, data->token->attribute.str.string);
    if(item) // pokud podminka plati tak funkce jiz byla drive deklarovana
    {
        return ERR_SEM_DEFINITION;
    }
    else
    {
        if((data->current_function = STF_add_symbol(data->function_table, data->token->attribute.str.string)) != NULL)
        {
            if(dstr_copy_dstr(&(data->token->attribute.str), &(data->current_function->id)) == DS_ERROR)
            {
                return ERR_INTERNAL;
            }
            data->current_function->defined = false;
            return ERR_OK;
        }
        else
        {
            return ERR_INTERNAL;
        }
    }
}
bool set_param_to_function(Data *data)
{
    if (dstr_add_char(&(data->current_function->param_types), KW_to_DT(data->token)) == 0)
    {
        return true;
    }
    return false;

}
bool set_ret_to_function(Data *data)
{
    if (dstr_add_char(&(data->current_function->ret_types), KW_to_DT(data->token) )== 0)
    {
        return true;
    }
    return false;

}
int save_called_fnc_id(Data *data)
{
     if (dstr_copy_dstr(&(data->token->attribute.str), &(data->called_fnc_id)))
     {
         return ERR_INTERNAL;
     }
     return ERR_OK;
}
int save_defined_var_id(Data *data)
{
     if (dstr_copy_dstr(&(data->token->attribute.str), &(data->defined_var_id)))
     {
         return ERR_INTERNAL;
     }
     return ERR_OK;
}
int save_defined_fnc_id(Data *data)
{
     if (dstr_copy_dstr(&(data->token->attribute.str), &(data->defined_fnc_id)))
     {
         return ERR_INTERNAL;
     }
     return ERR_OK;
}
/*bool init_everything(Data *data)// pocatecni inicializace
{
    //STF_init(function_table);
    data->counter_param = 0;
    data->counter_rets = 0;
    data->declare = false; // true jsme v deklaraci funkce
    data->returns = false;
    data->define = false; // true - jsme v definici funkce
    data->call = false;   // true - jsme ve volani funkce
}*/

void unget_token(Data *data)
{
    data->token_returned = true;
}
//porovna KEY_WORD z tokenu s KEY_WORD od uzivatele
bool token_eq_kw(Data *data, Keyword kw)
{
    if (data->token->type == TYPE_keyword && data->token->attribute.kw == kw)
    {
        return true;
    }
    return false;
}
// porovna TYPE z tokenu s TYPE od uzivatele
bool token_eq_type(Data *data, Type type)
{
    if (data->token->type == type)
    {
        return true;
    }
    return false;
}

void generate_write_params(param_stack *p_stack, Data *data)
{
    int level;
    Token token;
    while(!param_stack_is_empty(p_stack))
    {
        token = param_stack_pop(p_stack);
        if (token.type == TYPE_identifier)
        {
            level = STV_L_search(data->var_table, token.attribute.str.string)->level;
        }
        generate_write_param(token, level);
    }
}

 ST_funcs *function_table; // inicializace tabulky pro funkce
/*-------------------------------------------------------------------------*/
int main()
{
    int err = ERR_OK;

    Data data_v;
    Token token_v;
    data_v.token = &token_v;
    data_v.token_returned = false;
    Data *data = &data_v;

    ST_funcs func_ST_h;
    ST_L_vars vars_ST_h;
    data->function_table = &func_ST_h;
    data->var_table = &vars_ST_h;
    if (dstr_init(&(data->called_fnc_id)))
    {
        return ERR_INTERNAL;
    }
    if (dstr_init(&(data->defined_var_id)))
    {
        return ERR_INTERNAL;
    }
    if (dstr_init(&(data->defined_fnc_id)))
    {
        return ERR_INTERNAL;
    }

    param_stack p_stack;
    param_stack_init(&p_stack);
    data->p_stack = &p_stack;

    dstr_queue queue;
    dstr_queue_init(&queue);
    data->queue = &queue;

    STF_init(data->function_table);

    data->current_function = STF_add_symbol(data->function_table, "reads");
    data->current_function->defined = true;
    data->current_function = STF_add_symbol(data->function_table, "readi");
    data->current_function->defined = true;
    data->current_function = STF_add_symbol(data->function_table, "readn");
    data->current_function->defined = true;
    data->current_function = STF_add_symbol(data->function_table, "write");
    data->current_function->defined = true;
    ////

    data->current_function = STF_add_symbol(data->function_table, "tointeger");//number
    dstr_add_char(&(data->current_function->param_types), DT_NUM);
    data->current_function->defined = true;
            ////test
    data->current_function = STF_add_symbol(data->function_table, "substr");//string int int
    dstr_add_char(&(data->current_function->param_types), DT_STR);
    dstr_add_char(&(data->current_function->param_types), DT_INT);
    dstr_add_char(&(data->current_function->param_types), DT_INT);
    data->current_function->defined = true;

    data->current_function = STF_add_symbol(data->function_table, "ord"); //string int
    dstr_add_char(&(data->current_function->param_types), DT_STR);
    dstr_add_char(&(data->current_function->param_types), DT_INT);
    data->current_function->defined = true;

    data->current_function = STF_add_symbol(data->function_table, "chr"); // int
    dstr_add_char(&(data->current_function->param_types), DT_INT);
    data->current_function->defined = true;

    STV_L_init(data->var_table);

    ZERO();
    GET_TOKEN_ERR();
    err = req(data); //zacatek analyzy
    if (err != ERR_OK)
    {
        fprintf(stderr, "ERROR WITH CODE %d\n", err);
    }
    else
    {
        fprintf(stderr, "CODE_OK\n");
    }

    STF_delete_all(data->function_table);
    STV_L_dispose(data->var_table);
    return err;
}


int req(Data *data)
{
    int err = ERR_OK;

    if (token_eq_kw(data, KW_require))
    { // REQ ::= require str_val PROG
        GET_TOKEN_ERR();
        CHECK_TOKEN_TYPE(TYPE_string);
        CHECK_IFJ21();

        data->if_index = 0;
        generate_code_start();
        GET_TOKEN_ERR();
        PROCEED(prog);
    }
    else
    { // syntax error
        return ERR_SYN;
    }
    return err;
}
int prog(Data *data)
{
    int err = ERR_OK;
    ZERO();
    if (token_eq_kw(data, KW_global))
    { // PROG ::= global id : function ( PAR_TYPES ) TYPES PROG
        data->declare = true;
        GET_TOKEN_ERR();

        CHECK_TOKEN_TYPE(TYPE_identifier);
        // check whether id in sym table
        // if yes error
        // if no -> add it
        DECLARE_FUNCTION(); //deklarace funkce data->declare = 1;
        GET_TOKEN_ERR();

        CHECK_TOKEN_TYPE(TYPE_colon);
        GET_TOKEN_ERR();

        CHECK_TOKEN_KW(KW_function);
        GET_TOKEN_ERR();

        CHECK_TOKEN_TYPE(TYPE_l_par);
        GET_TOKEN_ERR();

        PROCEED(par_types);

        CHECK_TOKEN_TYPE(TYPE_r_par);
        GET_TOKEN_ERR();

        data->returns = true;
        PROCEED(types);
        ZERO();
        PROCEED(prog);
    }
    else if (token_eq_type(data, TYPE_identifier))
    { // PROG ::= id ( ARGS ) PROG
        CHECK_ID_IN_TABLE();//kontrola jestli funkce byla deklarovana nebo definovana
        data->call = true; // ted jsme ve volani funkce
        if (!dstr_cmp_str(&(data->token->attribute.str), "write")) // generate
        {
            data->write_fnc = true;
        }
        SAVE_DEFINED_FNC_ID(); // generate
        generate_createframe();
        GET_TOKEN_ERR();

        CHECK_TOKEN_TYPE(TYPE_l_par);
        GET_TOKEN_ERR();

        PROCEED(args);

        CHECK_TOKEN_TYPE(TYPE_r_par);
        if(data->write_fnc)
        {
            generate_write_params(data->p_stack, data);
            generate_write_count(data->counter_param);
        }
        generate_call_function(data->defined_fnc_id.string);
        //CHECK_PARAM_COUNT(); // kontrola spravneho poctu parametru
        ZERO();
        GET_TOKEN_ERR();
        PROCEED(prog);
    }
    else if (token_eq_kw(data, KW_function)) // definince funkce
    { // PROG ::= function id ( PARAMS ) TYPES BODY  end PROG
        data->define = true; // definice funkce
        GET_TOKEN_ERR();

        STV_L_deeper(&(data->var_table));

        CHECK_TOKEN_TYPE(TYPE_identifier);
        DEFINE_FUNCTION(); //definice funkce

        SAVE_DEFINED_FNC_ID(); // generate
        generate_label(data->token->attribute.str.string);

        GET_TOKEN_ERR();

        CHECK_TOKEN_TYPE(TYPE_l_par);
        GET_TOKEN_ERR();

        data->params_index = 0;
        PROCEED(params);
        CHECK_TYPES_COUNT();
        CHECK_TOKEN_TYPE(TYPE_r_par);
        GET_TOKEN_ERR();

        data->returns = true;

        PROCEED(types);
        CHECK_TYPES_COUNT();

        ZERO();
        PROCEED(body);
        STV_L_upper(&(data->var_table));

        generate_function_end(data->defined_fnc_id);

        CHECK_TOKEN_KW(KW_end);
        GET_TOKEN_ERR();

        PROCEED(prog);
    }
    else if (token_eq_type(data, TYPE_EOF))
    { // PROG ::= eof
        generate_builtin();
        return ERR_OK;
    }
    else
    { // syntax error
        return ERR_SYN;
    }
    return err;
}
int types(Data *data)
{
    int err = ERR_OK;

    if (token_eq_kw(data, KW_global) || token_eq_type(data, TYPE_identifier) || token_eq_kw(data, KW_function) || token_eq_type(data, TYPE_r_par) ||
        token_eq_kw(data, KW_end)    || token_eq_type(data, TYPE_EOF)        || token_eq_kw(data, KW_local)    || token_eq_kw(data, KW_if)        ||
        token_eq_kw(data, KW_while)  || token_eq_kw(data, KW_return)                                                                                  )
    { // TYPES ::= ε
        return ERR_OK;
    }
    else if (token_eq_type(data, TYPE_colon))
    { // TYPES ::= : TYPE NEXT_TYPE
        GET_TOKEN_ERR();

        PROCEED(type); // don't get next token

        PROCEED(next_type);
        return ERR_OK;
    }
    else
    { // syntax error
        return ERR_SYN;
    }

    return err;
}
int par_types(Data *data)
{
    int err = ERR_OK;

    if (token_eq_kw(data, KW_global) || token_eq_type(data, TYPE_identifier) || token_eq_kw(data, KW_function) || token_eq_type(data, TYPE_r_par) ||
        token_eq_kw(data, KW_end)    || token_eq_type(data, TYPE_EOF)        || token_eq_kw(data, KW_local)    || token_eq_kw(data, KW_if)        ||
        token_eq_kw(data, KW_while)  || token_eq_kw(data, KW_return)                                                                                  )
    { // PAR_TYPES ::= ε

        ZERO(); //vynuluje pomocne hodnoty
        return ERR_OK;
    }
    else if (token_eq_kw(data, KW_integer) || token_eq_kw(data, KW_number) || token_eq_kw(data, KW_string) || token_eq_kw(data, KW_nil))
    { // PAR_TYPES ::= TYPE NEXT_TYPE
        PROCEED(type); // don't get next token

        PROCEED(next_type);
        return ERR_OK;
    }
    else
    { // syntax error
        return ERR_SYN;
    }

    return err;
}
int type(Data *data)
{
    int err = ERR_OK;

    if (token_eq_kw(data, KW_integer))
    { // TYPE ::= integer
        SET_TYPE_ERR();
        GET_TOKEN_ERR();
        return ERR_OK;
    }
    else if (token_eq_kw(data, KW_number))
    { // TYPE ::= number
        SET_TYPE_ERR();
        GET_TOKEN_ERR();
        return ERR_OK;
    }
    else if (token_eq_kw(data, KW_string))
    { // TYPE ::= string
        SET_TYPE_ERR();
        GET_TOKEN_ERR();
        return ERR_OK;
    }
    else if (token_eq_kw(data, KW_nil))
    { // TYPE ::= nil
        SET_TYPE_ERR();
        GET_TOKEN_ERR();
        return ERR_OK;
    }
    else
    { // syntax error
        return ERR_SYN;
    }

    return err;
}
int next_type(Data *data)
{
    int err = ERR_OK;

    if (token_eq_kw(data, KW_global) || token_eq_type(data, TYPE_identifier) || token_eq_kw(data, KW_function) || token_eq_type(data, TYPE_r_par) ||
        token_eq_kw(data, KW_end)    || token_eq_type(data, TYPE_EOF)        || token_eq_kw(data, KW_local)    || token_eq_kw(data, KW_if)        ||
        token_eq_kw(data, KW_while)  || token_eq_kw(data, KW_return)                                                                                )
    { // NEXT_TYPE ::= ε
        return ERR_OK;
    }
    else if (token_eq_type(data, TYPE_comma))
    { // NEXT_TYPE ::= , TYPE NEXT_TYPE
        GET_TOKEN_ERR();

        PROCEED(type);

        PROCEED(next_type);
    }
    else
    { // syntax error
        return ERR_SYN;
    }
    return err;
}
int params(Data *data)
{
    int err = ERR_OK;

    if (token_eq_type(data, TYPE_identifier))
    { // PARAMS ::= id : TYPE NEXT_PARAM
        VAR_CHECK_FNC_IDENTIC_ID()
        VAR_ADD_SYMBOL_ERR()// TODO VARS

        SYM_variable *var = STV_L_search(data->var_table, data->token->attribute.str.string);
        if(var)
        {
            generate_def_function_params(data->params_index, *(data->token), var->level);
            (data->params_index)++;
        }


        GET_TOKEN_ERR();

        CHECK_TOKEN_TYPE(TYPE_colon);
        GET_TOKEN_ERR();

        PROCEED(type);

        PROCEED(next_param);
    }
    else if(token_eq_type(data, TYPE_r_par))
    { // PARAMS ::= ε
        return ERR_OK;
    }
    else
    { // syntax error
        return ERR_SYN;
    }
    return err;
}
int next_param(Data *data)
{
    int err = ERR_OK;

    if(token_eq_type(data, TYPE_r_par) )
    { //next_param ::= ε
        return ERR_OK;
    }
    else if(token_eq_type(data, TYPE_comma))
    { // NEXT_PARAM ::= , id : TYPE NEXT_PARAM
        GET_TOKEN_ERR();

        CHECK_TOKEN_TYPE(TYPE_identifier);
        VAR_CHECK_FNC_IDENTIC_ID()
        VAR_ADD_SYMBOL_ERR()

        SYM_variable *var = STV_L_search(data->var_table, data->token->attribute.str.string);
        if(var)
        {
            generate_def_function_params(data->params_index, *(data->token), var->level);
            (data->params_index)++;
        }

        GET_TOKEN_ERR();

        CHECK_TOKEN_TYPE(TYPE_colon);
        GET_TOKEN_ERR();

        PROCEED(type);

        PROCEED(next_param);
    }
    else
    {   //syntax error
        return ERR_SYN;
    }
    return err;
}
int args(Data *data)
{
    int err = ERR_OK;

    if(token_eq_type(data, TYPE_r_par))
    { // ARGS ::= ε
        return ERR_OK;
    }
    else if(token_eq_type(data, TYPE_identifier) || token_eq_kw(data, KW_nil)        || token_eq_type(data, TYPE_integer) ||
            token_eq_type(data, TYPE_number)     || token_eq_type(data, TYPE_string)                                         )
    { // ARGS ::= TERM NEXT_TERM
        if(data->counter_param == (data->current_function->param_types.lenght - 1))
        {
            data->call_param = true;
        }
        PROCEED(term);
        PROCEED(next_term);
    }
    else
    { // syntax error
        return ERR_SYN;
    }
    return err;
}
int next_term(Data *data)
{
    int err = ERR_OK;

    if(token_eq_type(data, TYPE_r_par))
    { //NEXT_TERM ::= ε
        return ERR_OK;
    }
    else if(token_eq_type(data, TYPE_comma))
    { //NEXT_TERM ::= , TERM NEXT_TERM
        GET_TOKEN_ERR();

        PROCEED(term);

        PROCEED(next_term);
        CHECK_PARAM_COUNT();
    }
    else
    { //syntax error
        return ERR_SYN;
    }
    return err;
}
int term(Data *data)
{
    int err = ERR_OK;

    if(token_eq_type(data, TYPE_identifier))
    { //TERM ::= id
        CHECK_VAR_ID();
        data->call_param = false;

        if (!data->write_fnc)
        {
            CHECK_PARAMS_IN_CALL();
            int level = STV_L_search(data->var_table, data->token->attribute.str.string)->level;
            generate_function_params(data->counter_param, *data->token, data->var_level);
        }
        else
        {
            param_stack_push(data->p_stack, *data->token);
        }
        (data->counter_param)++;
        GET_TOKEN_ERR();
        return ERR_OK;
    }
    else if(token_eq_kw(data, KW_nil))
    { //TERM ::= nil
        if (!data->write_fnc)
        {
            CHECK_PARAMS_IN_CALL();
            generate_function_params(data->counter_param, *data->token, 0);
        }
        else
        {
            param_stack_push(data->p_stack, *data->token);
        }
        (data->counter_param)++;
        GET_TOKEN_ERR();
        return ERR_OK;
    }
    else if(token_eq_type(data, TYPE_integer))
    { //TERM ::= integer_val
        if (!data->write_fnc)
        {
            CHECK_PARAMS_IN_CALL();
            generate_function_params(data->counter_param, *data->token, 0);
        }
        else
        {
            param_stack_push(data->p_stack, *data->token);
        }
        (data->counter_param)++;
        GET_TOKEN_ERR();
        return ERR_OK;
    }
    else if(token_eq_type(data, TYPE_number))
    { //TERM ::= number_val
        if (!data->write_fnc)
        {
            CHECK_PARAMS_IN_CALL();
            generate_function_params(data->counter_param, *data->token, 0);
        }
        else
        {
            param_stack_push(data->p_stack, *data->token);
        }
        (data->counter_param)++;
        GET_TOKEN_ERR();
        return ERR_OK;
    }
    else if(token_eq_type(data, TYPE_string))
    {   //TERM ::= string_val
        if (!data->write_fnc) // kdyz to neni funkce write
        {
            CHECK_PARAMS_IN_CALL();
            generate_function_params(data->counter_param, *data->token, 0);
        }
        else
        {
            param_stack_push(data->p_stack, *data->token);
        }
        (data->counter_param)++;
        GET_TOKEN_ERR();
        return ERR_OK;
    }
    else
    { //syntax error
        return ERR_SYN;
    }
    return err;
}
int body(Data *data)
{
    int err = ERR_OK;
    if(token_eq_kw(data, KW_end) || token_eq_kw(data, KW_else))
    { // BODY ::= ε
        return ERR_OK;
    }
    else if(token_eq_type(data, TYPE_identifier))
    { // BODY ::= id ASSIGN BODY
        //if(STF_search(data->function_table, data->token->attribute.str.string)) // id is in function symtable
        if(id_in_glob_symt(data))//kontrola jestli to je funkce
        { // id is function -> generate function parameters
            data->call = true;
            if (!dstr_cmp_str(&(data->token->attribute.str), "write")) // generate
            {
                data->write_fnc = true;
            }
            SAVE_CALLED_FNC_ID(); // generate
            generate_createframe();

        }
        else
        {
            CHECK_VAR_ID();//kontrola jestli promenna je definovana
            INSERT_IDENTIF_QUEUE();
        }
        GET_TOKEN_ERR();

        PROCEED(assign);

        PROCEED(body);
    }
    else if(token_eq_kw(data, KW_local))
    { // BODY ::= local id : TYPE INIT BODY
        GET_TOKEN_ERR();

        CHECK_TOKEN_TYPE(TYPE_identifier);
        VAR_CHECK_FNC_IDENTIC_ID()
        VAR_ADD_SYMBOL_ERR()
        data->var_level = data->current_var->level;

        generate_var_def(data->token->attribute.str.string, data->var_level);
        SAVE_DEFINED_VAR_ID(); // generate
        GET_TOKEN_ERR();

        CHECK_TOKEN_TYPE(TYPE_colon);
        GET_TOKEN_ERR();

        PROCEED(type);

        PROCEED(init);

        PROCEED(body);
    }
    else if(token_eq_kw(data, KW_if))
    { // BODY ::= if EXPRESSION then BODY else BODY end BODY
        GET_TOKEN_ERR();
        EXPRESSION_ERR();

        int if_index = data->if_index;
        (data->if_index)++;
        generate_if_start(if_index,data->expression_index, data->term_index);
        (data->expression_index)++;


        CHECK_TOKEN_KW(KW_then);
        GET_TOKEN_ERR();

        STV_L_deeper(&(data->var_table));
        PROCEED(body);

        STV_L_upper(&(data->var_table));
        CHECK_TOKEN_KW(KW_else);
        STV_L_deeper(&(data->var_table));
        GET_TOKEN_ERR();

        generate_else_label(if_index);

        PROCEED(body);

        STV_L_upper(&(data->var_table));
        CHECK_TOKEN_KW(KW_end);
        GET_TOKEN_ERR();

        generate_end_label(if_index);

        PROCEED(body);
    }
    else if(token_eq_kw(data, KW_while))
    { // BODY ::= while EXPRESSION do BODY end BODY
        GET_TOKEN_ERR();

        EXPRESSION_ERR();
        int while_index = data->while_index;
        (data->while_index)++;
        generate_while_start(while_index,data->expression_index, data->term_index);
        (data->expression_index)++;


        CHECK_TOKEN_KW(KW_do);
        GET_TOKEN_ERR();

        STV_L_deeper(&(data->var_table));
        PROCEED(body);
        STV_L_upper(&(data->var_table));

        generate_while_end_label(while_index);

        CHECK_TOKEN_KW(KW_end);
        GET_TOKEN_ERR();


        PROCEED(body);
    }
    else if(token_eq_kw(data, KW_return))
    { // BODY ::= return EXPRESSIONS BODY
        GET_TOKEN_ERR();

        data->cur_ret_index = 0;
        data->in_return = true;
        PROCEED(expressions);
        data->in_return = false;
        PROCEED(body);
    }
    else
    { //syntax error
        return ERR_SYN;
    }
    return err;
}
int expr_or_fnc(Data *data)
{
    int err = ERR_OK;

    if(token_eq_type(data, TYPE_identifier) && STF_search(data->function_table, data->token->attribute.str.string)) // id is in function symtable
    { //EXPR_OR_FNC ::= id ( ARGS )
        data->call = true;
        data->current_function = STF_search(data->function_table, data->token->attribute.str.string);
        if (!dstr_cmp_str(&(data->token->attribute.str), "write")) // generate
        {
            data->write_fnc = true;
        }
        SAVE_CALLED_FNC_ID(); // generate
        generate_createframe();
        GET_TOKEN_ERR();

        CHECK_TOKEN_TYPE(TYPE_l_par);
        GET_TOKEN_ERR();

        PROCEED(args);

        CHECK_TOKEN_TYPE(TYPE_r_par);
        if(data->write_fnc)
        {
            generate_write_params(data->p_stack, data);
            generate_write_count(data->counter_param);
        }
        generate_call_function(data->called_fnc_id.string);
        generate_var_def_assign_retval(data->defined_var_id.string, data->var_level);
        //CHECK_PARAM_COUNT(); //kontrola spravneho poctu parametru
        GET_TOKEN_ERR();
        ZERO();
    }
    else
    { //EXPR_OR_FNC ::= EXPRESSION

        EXPRESSION_ERR();
        generate_var_def_assign_expr(data->defined_var_id.string, data->var_level, data->term_index, data->expression_index);
        (data->expression_index)++;

    }
    return err;
}
int init(Data *data)
{
    int err = ERR_OK;

    if(token_eq_type(data, TYPE_identifier) || token_eq_kw(data,KW_end)    || token_eq_kw(data, KW_local) ||  token_eq_kw(data, KW_if) ||
       token_eq_kw(data, KW_else)           || token_eq_kw(data, KW_while) || token_eq_kw(data, KW_return)                               )
    { //INIT ::= ε
        return ERR_OK;
    }
    else if(token_eq_type(data, TYPE_assign))
    { //INIT ::= = EXPR_OR_FNC
        GET_TOKEN_ERR();

        PROCEED(expr_or_fnc);
    }
    else
    {   //syntax error
        return ERR_SYN;
    }
    return err;
}
int expressions(Data *data)
{
    int err = ERR_OK;

    // identifier missing on purpose - if there is one, it is allways expression
    if(token_eq_kw(data, KW_end) || token_eq_kw(data, KW_else))
    { //EXPRESSIONS ::= ε
        return ERR_OK;
    }
    else
    { // EXPRESSIONS ::= EXPRESSION NEXT_EXPRESSION
        EXPRESSION_ERR();
        int ret_vals_c = STF_search(data->function_table, data->defined_fnc_id.string)->ret_types.lenght;
        if (data->cur_ret_index + 1 > ret_vals_c) // pokud je pocet vracenych vyrazu vetsi nez pocet navratovych hodnot fce
        {
            return ERR_SEM_FUNC;
        }
        generate_fnc_return(data->cur_ret_index, data->term_index, data->expression_index);
        (data->cur_ret_index)++;
        (data->expression_index)++;

        PROCEED(next_expression);
    }
    return err;
}
int next_expression(Data *data) //
{
    int err = ERR_OK;

    if(token_eq_type(data, TYPE_identifier) || token_eq_kw(data,KW_end) ||
    token_eq_kw(data, KW_local) ||  token_eq_kw(data, KW_if) || token_eq_kw(data, KW_else) || token_eq_kw(data, KW_while) || token_eq_kw(data, KW_return))
    { //NEXT_EXPRESSION ::= ε
        return ERR_OK;
    }
    else if(token_eq_type(data, TYPE_comma))
    { //NEXT_EXPRESSION ::= , EXPRESSION NEXT_EXPRESSION
        GET_TOKEN_ERR();

        EXPRESSION_ERR();
        if (data->in_return)
        {
            int ret_vals_c = STF_search(data->function_table, data->defined_fnc_id.string)->ret_types.lenght;
            if (data->cur_ret_index + 1 > ret_vals_c) // pokud je pocet vracenych vyrazu vetsi nez pocet navratovych hodnot fce
            {
                return ERR_SEM_FUNC;
            }
            generate_fnc_return(data->cur_ret_index, data->term_index, data->expression_index);
            (data->cur_ret_index)++;
        }
        else
        {
            D_str name = dstr_queue_pop(data->queue);
            int level = STV_L_search(data->var_table, name.string)->level;
            generate_var_def_assign_expr(name.string, level, data->term_index, data->expression_index);
        }
        (data->expression_index)++;

        PROCEED(next_expression);
    }
    else
    {   //syntax error
        return ERR_SYN;
    }
    return err;
}
int assign(Data *data)
{
    int err = ERR_OK;

    if(token_eq_type(data, TYPE_l_par))
    { //ASSIGN ::= ( ARGS )

        if(data->call == false) return ERR_SEM_DEFINITION;

        GET_TOKEN_ERR();

        PROCEED(args);

        CHECK_TOKEN_TYPE(TYPE_r_par);
        if(data->write_fnc)
        {
            generate_write_params(data->p_stack, data);
            generate_write_count(data->counter_param);
        }
        generate_call_function(data->called_fnc_id.string);
        ZERO();
        GET_TOKEN_ERR();
    }
    else if (token_eq_type(data, TYPE_assign))
    { // ASSIGN ::= NEXT_ID = EXPRS_OR_FNC --[when NEXT_ID ::= ε]
        GET_TOKEN_ERR();
        if(data->call == true) return ERR_SEM_DEFINITION;

        PROCEED(exprs_or_fnc);
    }
    else if (token_eq_type(data, TYPE_comma) || token_eq_type(data, TYPE_assign))
    { // ASSIGN ::= NEXT_ID = EXPRS_OR_FNC

        PROCEED(next_id);

        CHECK_TOKEN_TYPE(TYPE_assign);
        GET_TOKEN_ERR();

        PROCEED(exprs_or_fnc);
    }
    else
    { //syntax error
        return ERR_SYN;
    }
    return err;
}
int next_id(Data *data)
{
    int err = ERR_OK;

    if(token_eq_type(data, TYPE_assign))
    { // NEXT_ID ::= ε
        return ERR_OK;
    }
    else if(token_eq_type(data, TYPE_comma))
    { //NEXT_ID ::= , id NEXT_ID
        GET_TOKEN_ERR();

        CHECK_TOKEN_TYPE(TYPE_identifier);
        INSERT_IDENTIF_QUEUE();
        GET_TOKEN_ERR();

        PROCEED(next_id);
    }
    else
    {   //syntax error;
        return ERR_SYN;
    }
    return err;
}

int exprs_or_fnc(Data *data)
{
    int err = ERR_OK;

    if(token_eq_type(data, TYPE_identifier) && STF_search(data->function_table, data->token->attribute.str.string))
    { //EXPRS_OR_FNC ::= id ( ARGS )
        data->call = true; // ted jsme ve volani funkce
        data->current_function = STF_search(data->function_table, data->token->attribute.str.string);
        if (!dstr_cmp_str(&(data->token->attribute.str), "write")) // generate
        {
            data->write_fnc = true;
        }
        SAVE_CALLED_FNC_ID(); // generate
        generate_createframe();

        GET_TOKEN_ERR();

        CHECK_TOKEN_TYPE(TYPE_l_par);
        GET_TOKEN_ERR();

        PROCEED(args);

        CHECK_TOKEN_TYPE(TYPE_r_par);
        if(data->write_fnc)
        {
            generate_write_params(data->p_stack, data);
            generate_write_count(data->counter_param);
        }
        generate_call_function(data->called_fnc_id.string);
        while (!dstr_queue_empty(data->queue))
        {
            D_str name = dstr_queue_pop(data->queue);
            int level = STV_L_search(data->var_table, name.string)->level;
            generate_var_def_assign_retval(name.string, level);
        }
        generate_var_def_assign_retval(data->defined_var_id.string, data->var_level);
        //CHECK_PARAM_COUNT();    //kontrola spravneho poctu parametru
        ZERO();         //vynuluje pomocne promenne
        GET_TOKEN_ERR();
    }
    else
    { //EXPRS_OR_FNC ::= EXPRESSION NEXT_EXPRESSION
        //EXPRESSION_ERR();
        expr_return exp_ret = expression(data);
        if(exp_ret.err == ERR_OK)
        {
            if(data->current_var->data_type != exp_ret.dtype)
            {
                if(data->current_var->data_type == DT_NUM && exp_ret.dtype == DT_INT){}
                else
                {
                    return ERR_SEM_TYPE_COMP; // kontrola jestli typ vyrazu je stejny jako typ promenne
                }
            }
        }

        D_str name = dstr_queue_pop(data->queue);
        int level = STV_L_search(data->var_table, name.string)->level;
        generate_var_def_assign_expr(name.string, level, data->term_index, data->expression_index);

        (data->expression_index)++;

        PROCEED(next_expression);
    }
    return err;
}
