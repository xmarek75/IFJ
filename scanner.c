/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: scanner.c
* Popis souboru: implementace scanneru
* Autori: Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/
#include "error.h"
#include "scanner.h"
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

/**
 * @brief Implementace lexikalniho analyzator prekladace
 *
 * @author Vilem Gottwald [xgottw07]
 */

/**
 * @brief Vycet stavu stavoveho automatu scanneru
 *
 */
typedef enum
{
    START,
	COMM,
    COMM_INLINE,
    COMM_BLOCK_START,
    COMM_BLOCK,
    COMM_BLOCK_END,
    IDENTIFIER,
    INTEGER,
    NUMBER_DEC_FIRST,
    NUMBER_DEC,
    NUMBER_EXP_SIGN,
    NUMBER_EXP_FIRST,
    NUMBER_EXP,
    STRING,
    STRING_ESC,
    STRING_ESC_G1,
    STRING_ESC_G2,
    ASSIGN,
    DIV,
    SUB,
    CONCAT,
    LT,
    GT,
    NOT_EQ,
}State;

void destroy_token(Token *token)
{
    if (token->type == TYPE_identifier || token->type == TYPE_string)
    {
        dstr_free(&(token->attribute.str));
    }
}

void assign_keyword(Token *token)
{
    D_str *ds = &(token->attribute.str);
    char *str = token->attribute.str.string;

    if (!dstr_cmp_str(ds, "do"))
    {
        token->attribute.kw = KW_do;
    }
    else if (!dstr_cmp_str(ds, "else"))
    {
        token->attribute.kw = KW_else;
    }
    else if (!dstr_cmp_str(ds, "end"))
    {
        token->attribute.kw = KW_end;
    }
    else if (!dstr_cmp_str(ds, "function"))
    {
        token->attribute.kw = KW_function;
    }
    else if (!dstr_cmp_str(ds, "global"))
    {
        token->attribute.kw = KW_global;
    }
    else if (!dstr_cmp_str(ds, "if"))
    {
        token->attribute.kw = KW_if;
    }
    else if (!dstr_cmp_str(ds, "integer"))
    {
        token->attribute.kw = KW_integer;
    }
    else if (!dstr_cmp_str(ds, "local"))
    {
        token->attribute.kw = KW_local;
    }
    else if (!dstr_cmp_str(ds, "nil"))
    {
        token->attribute.kw = KW_nil;
    }
    else if (!dstr_cmp_str(ds, "number"))
    {
        token->attribute.kw = KW_number;
    }
    else if (!dstr_cmp_str(ds, "require"))
    {
        token->attribute.kw = KW_require;
    }
    else if (!dstr_cmp_str(ds, "return"))
    {
        token->attribute.kw = KW_return;
    }
    else if (!dstr_cmp_str(ds, "string"))
    {
        token->attribute.kw = KW_string;
    }
    else if (!dstr_cmp_str(ds, "then"))
    {
        token->attribute.kw = KW_then;
    }
    else if (!dstr_cmp_str(ds, "while"))
    {
        token->attribute.kw = KW_while;
    }
    else
    {
        token->type = TYPE_identifier;
        return;
    }
    token->type = TYPE_keyword;
    free(str);
}

int assign_int( Token *token)
{
    char *str = token->attribute.str.string;

    errno = 0;
    long int value = strtol(str, NULL, 10);
    if (errno || value > INT_MAX)
    {
        fprintf(stderr, "ERROR: Integer value is out of range\n");
        return ERR_LEX;
    }
    token->type = TYPE_integer;
    token->attribute.int_val = (int) value;
    free(str);
    return ERR_OK;
}

int assign_num( Token *token)
{
    char *str = token->attribute.str.string;

    errno = 0;
    double value = strtod(str, NULL);
    if (errno)
    {
        fprintf(stderr, "ERROR: Number value is out of range\n");
        return ERR_LEX;
    }
    token->type = TYPE_number;
    token->attribute.num_val = value;
    free(str);
    return ERR_OK;
}

int get_token(Token *token)
{
	int err = ERR_OK;
    int esc_char = 0;
    State state = START;
    // if((token->type == TYPE_string || token->type == TYPE_identifier) && token->attribute.str.alloc_size > 0)
    // {
    //     dstr_free(&(token->attribute.str));
    // }
    while(1)
    {
        int symbol = getchar();

        switch (state)
        {
        case START:
            if(isspace(symbol) || symbol == '\n')
            { // whitespaces or new line
                state = START;
            }
            else if(isdigit(symbol))
            { // integer or number
                state = INTEGER;
                if (dstr_init(&(token->attribute.str)) == DS_ERROR) return ERR_INTERNAL;
                if (dstr_add_char(&(token->attribute.str), symbol) == DS_ERROR) return ERR_INTERNAL;
            }
            else if(symbol == '\"')
            { // string
                state = STRING;
                if (dstr_init(&(token->attribute.str)) == DS_ERROR) return ERR_INTERNAL;
                token->type = TYPE_string;
            }
            else if(isalpha(symbol) || symbol == '_')
            { // identifier or keyword
                state = IDENTIFIER;
               if (dstr_init(&(token->attribute.str)) == DS_ERROR) return ERR_INTERNAL;
               if (dstr_add_char(&(token->attribute.str), symbol) == DS_ERROR) return ERR_INTERNAL;
            }
            else if(symbol == '-')
            { // sub or comment
                state = SUB;
            }
            else if(symbol == '/')
            { // divisior or int division
                state = DIV;
            }
            else if(symbol == '.')
            { // existance of second dot
                state = CONCAT;
            }
            else if(symbol == '<')
            { // LT or LTEQ
                state = LT;
            }
            else if(symbol == '>')
            { // GT or GTEQ
                state = GT;
            }
            else if(symbol == '=')
            { // assign or equals
                state = ASSIGN;
            }
            else if(symbol == '~')
            { // existance of equals sign
                state = NOT_EQ;
            }
            else if(symbol == '(')
            {
                token->type = TYPE_l_par;
                return ERR_OK;
            }
            else if(symbol == ')')
            {
                token->type = TYPE_r_par;
                return ERR_OK;
            }
            else if(symbol == ':')
            {
                token->type = TYPE_colon;
                return ERR_OK;
            }
            else if(symbol == ',')
            {
                token->type = TYPE_comma;
                return ERR_OK;
            }
            else if(symbol == '#')
            {
                token->type = TYPE_len;
                return ERR_OK;
            }
            else if(symbol == '*')
            {
                token->type = TYPE_mul;
                return ERR_OK;
            }
            else if(symbol == '+')
            {
                token->type = TYPE_add;
                return ERR_OK;
            }
            else if (symbol == EOF)
            {
                token->type = TYPE_EOF;
                return ERR_OK;
            }
            else
            {
				fprintf(stderr, "ERROR: Unsupported character on input\n");
                return ERR_LEX;
            }
            break;

        case DIV:
            if (symbol == '/')
            {
                token->type = TYPE_int_div;
                return ERR_OK;
            }
            else
            {
                ungetc(symbol, stdin);
                token->type = TYPE_div;
                return ERR_OK;
            }
            break;

        case CONCAT:
            if (symbol == '.')
            {
                token->type = TYPE_concat;
                return ERR_OK;
            }
            else
            { // '.' token doesnt exist
				fprintf(stderr, "ERROR: '.' isn't a valid token \n");
                return ERR_LEX;
            }
            break;

        case LT:
            if (symbol == '=')
            {// <= token
                token->type = TYPE_lt_eq;
                return ERR_OK;
            }
            else
            { // < token
                ungetc(symbol, stdin);
                token->type = TYPE_lt;
                return ERR_OK;
            }
            break;

        case GT:
            if (symbol == '=')
            { // >= token
                token->type = TYPE_gt_eq;
                return ERR_OK;
            }
            else
            { // > token
                ungetc(symbol, stdin);
                token->type = TYPE_gt;
                return ERR_OK;
            }
            break;

        case ASSIGN:
            if (symbol == '=')
            { // == token
                token->type = TYPE_eq;
                return ERR_OK;
            }
            else
            { // = token
                ungetc(symbol, stdin);
                token->type = TYPE_assign;
                return ERR_OK;
            }
            break;

        case NOT_EQ:
            if (symbol == '=')
            { // ~= token
                token->type = TYPE_not_eq;
                return ERR_OK;
            }
            else
            {
				fprintf(stderr, "ERROR: '~' isn't a valid token \n");
                return ERR_LEX;
            }
            break;

        case SUB:
            if (symbol == '-')
            { // comment start
                state = COMM;
            }
            else
            { // - token
                ungetc(symbol, stdin);
                token->type = TYPE_sub;
                return ERR_OK;
            }
            break;

        case COMM:
            if (symbol == '[')
            {
                state = COMM_BLOCK_START;
            }
            else if (symbol == '\n' || symbol == EOF)
            {
                state = START;
            }
            else
            {
                state = COMM_INLINE;
            }
            break;

		case COMM_INLINE:
            if (symbol == '\n' || symbol == EOF)
            {
                state = START;
            }
            else
            {
                state = COMM_INLINE;
            }
            break;

        case COMM_BLOCK_START:
            if (symbol == '[')
            {
                state = COMM_BLOCK;
            }
            else if (symbol == '\n' || symbol == EOF)
            {
                state = START;
            }
            else
            {
                state = COMM_INLINE;
            }
            break;

        case COMM_BLOCK:
            if (symbol == ']')
            {
                state = COMM_BLOCK_END;
            }
            else if (symbol == EOF)
            { // unterminated commnet
                fprintf(stderr, "ERROR: Unterminated block comment\n");
                return ERR_LEX;
            }
            else
            {
                state = COMM_BLOCK;
            }
            break;

        case COMM_BLOCK_END:
            if (symbol == ']')
            {
                state = START;
            }
            else if (symbol == EOF)
            { // unterminated comment
                fprintf(stderr, "ERROR: Unterminated block comment\n");
                return ERR_LEX;
            }
            else
            {
                state = COMM_BLOCK;
            }
            break;

        case IDENTIFIER:
            if (isalnum(symbol) || symbol == '_')
            {
				state = IDENTIFIER;
                if (dstr_add_char(&(token->attribute.str), symbol) == DS_ERROR) return ERR_INTERNAL;
            }
            else
            {
                assign_keyword(token);
                ungetc(symbol, stdin);
                return ERR_OK;
            }
            break;

        case STRING:
            if (symbol == '\\')
            {
                state = STRING_ESC;
            }
            else if (symbol == EOF || symbol == '\n')
            {
                fprintf(stderr, "ERROR: Unterminated string - missing terminating \" character\n");
                return ERR_LEX;
            }
            else if (symbol == '\"')
            {
                return ERR_OK;
            }
            else
            {
                state = STRING;
                if (dstr_add_char(&(token->attribute.str), symbol) == DS_ERROR) return ERR_INTERNAL;
            }
            break;

        case STRING_ESC:
            if (symbol == '\"')
            {
                state = STRING;
                if (dstr_add_char(&(token->attribute.str), '\"') == DS_ERROR) return ERR_INTERNAL;
            }
            else if (symbol == 'n')
            {
                state = STRING;
                if (dstr_add_char(&(token->attribute.str), '\n') == DS_ERROR) return ERR_INTERNAL;
            }
            else if (symbol == 't')
            {
                state = STRING;
                if (dstr_add_char(&(token->attribute.str), '\t') == DS_ERROR) return ERR_INTERNAL;
            }
            else if (symbol == '\\')
            {
                state = STRING;
                if (dstr_add_char(&(token->attribute.str), '\\') == DS_ERROR) return ERR_INTERNAL;
            }
            else if ( symbol >= '0' && symbol <= '2')
            {
                state = STRING_ESC_G1;
				esc_char = (symbol - '0') * 100;
            }
            else
            {
                fprintf(stderr, "ERROR: Nonexistent escape sequence inside a string\n");
                return ERR_LEX;
            }
            break;

        case STRING_ESC_G1:
            if (symbol >= '0' && symbol <= '9')
            {
				state = STRING_ESC_G2;
                esc_char += (symbol - '0') * 10;
            }
            else
            {
                fprintf(stderr, "ERROR: Nonexistent escape sequence inside a string\n");
                return ERR_LEX;
            }
            break;

        case STRING_ESC_G2:
            if (symbol >= '0' && symbol <= '9')
            {
                esc_char += symbol - '0';
                if (esc_char >= 1 && esc_char <= 255)
                { // escape sequence is valid (between 001 - 255)
					state = STRING;
                    if (dstr_add_char(&(token->attribute.str), (char) esc_char) == DS_ERROR) return ERR_INTERNAL;
                }
                else
                {
                    fprintf(stderr, "ERROR: Escape sequence inside a string is out of range (\\001 - \\255)\n");
                    return ERR_LEX;
                }
            }
            else
            {
                fprintf(stderr, "ERROR: Nonexistent escape sequence inside a string\n");
                return ERR_LEX;
            }
            break;

        case INTEGER:
            if (isdigit(symbol))
            {
                state = INTEGER;
				if (dstr_add_char(&(token->attribute.str), symbol) == DS_ERROR) return ERR_INTERNAL;
            }
            else if (symbol == '.')
            {
				state = NUMBER_DEC_FIRST;
                if (dstr_add_char(&(token->attribute.str), symbol) == DS_ERROR) return ERR_INTERNAL;
            }
            else if (symbol == 'E' || symbol == 'e')
            {
                state = NUMBER_EXP_SIGN;
				if (dstr_add_char(&(token->attribute.str), symbol) == DS_ERROR) return ERR_INTERNAL;
            }
            else
            {
                if ((err = assign_int(token)) != ERR_OK)
				{
					return err;
				}
                ungetc(symbol, stdin);
                return ERR_OK;
            }
            break;

        case NUMBER_DEC_FIRST:
            if (isdigit(symbol))
            {
                state = NUMBER_DEC;
				if (dstr_add_char(&(token->attribute.str), symbol) == DS_ERROR) return ERR_INTERNAL;
            }
            else
            {
                fprintf(stderr, "ERROR: Missing decimal part after the decimal dot inside a constant number\n");
                return ERR_LEX;
            }
            break;

        case NUMBER_DEC:
            if (isdigit(symbol))
            {
                state = NUMBER_DEC;
				if (dstr_add_char(&(token->attribute.str), symbol) == DS_ERROR) return ERR_INTERNAL;

            }
            else if (symbol == 'E' || symbol == 'e')
            {
                state = NUMBER_EXP_SIGN;
                if(dstr_add_char(&(token->attribute.str), symbol) == DS_ERROR) return ERR_INTERNAL;
            }
            else
            {
                if ((err = assign_num(token)) != ERR_OK)
				{
					return err;
				}
                ungetc(symbol, stdin);
                return ERR_OK;
            }
            break;

        case NUMBER_EXP_SIGN:
            if (symbol == '+' || symbol == '-')
            {
                state = NUMBER_EXP_FIRST;
                if (dstr_add_char(&(token->attribute.str), symbol) == DS_ERROR) return ERR_INTERNAL;
            }
            else if (isdigit(symbol))
            {
                state = NUMBER_EXP;
                if (dstr_add_char(&(token->attribute.str), symbol) == DS_ERROR) return ERR_INTERNAL;
            }
            else
            {
                fprintf(stderr, "ERROR: Missing exponent after 'e' inside a constant number\n");
                return ERR_LEX;
            }
            break;

        case NUMBER_EXP_FIRST:
            if (isdigit(symbol))
            {
                state = NUMBER_EXP;
                if (dstr_add_char(&(token->attribute.str), symbol) == DS_ERROR) return ERR_INTERNAL;
            }
            else
            {
                fprintf(stderr, "ERROR: Missing exponent after 'e' inside a constant number\n");
                return ERR_LEX;
            }
            break;

        case NUMBER_EXP:
            if (isdigit(symbol))
            {
                state = NUMBER_EXP;
                if (dstr_add_char(&(token->attribute.str), symbol) == DS_ERROR) return ERR_INTERNAL;
            }
            else
            {
                if ((err = assign_num(token)) != ERR_OK)
				{
					return err;
				}
                ungetc(symbol, stdin);
                return ERR_OK;
            }
            break;

        } // end switch
    } // end while
} // end of function get_token
