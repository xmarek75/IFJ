#include "scanner.h"
#include "error.h"
#include <stdio.h>

void print_token(Token token)
{
    switch (token.type)
    {
        case TYPE_keyword:
            switch (token.attribute.kw)
            {
                case KW_do:
                    printf("do \n");
                    break;
                case KW_nil:
                    printf("nil \n");
                    break;
                case KW_else:
                    printf("else \n");
                    break;
                case KW_end:
                    printf("end \n");
                    break;
                case KW_function:
                    printf("function \n");
                    break;
                case KW_global:
                    printf("global \n");
                    break;
                case KW_if:
                    printf("if \n");
                    break;
                case KW_integer:
                    printf("integer \n");
                    break;
                case KW_local:
                    printf("local \n");
                    break;
                case KW_number:
                    printf("number \n");
                    break;
                case KW_require:
                    printf("require \n");
                    break;
                case KW_return:
                    printf("return \n");
                    break;
                case KW_string:
                    printf("string \n");
                    break;
                case KW_then:
                    printf("then \n");
                    break;
                case KW_while:
                    printf("while \n");
                    break;
            }
            break;
        case TYPE_identifier:
            printf("identif: %s\n", token.attribute.str.string);
            break;
        case TYPE_integer:
            printf("int: %d\n", token.attribute.int_val);
            break;
        case TYPE_number:
            printf("num: %f\n", token.attribute.num_val);
            break;
        case TYPE_string:
            printf("str: %s\n", token.attribute.str.string);
            break;
        case TYPE_l_par:
            printf("(\n");
            break;
        case TYPE_r_par:
            printf(")\n");
            break;
        case TYPE_colon:
            printf(":\n");
            break;
        case TYPE_comma:
            printf(",\n");
            break;
        case TYPE_assign:
            printf("=\n");
            break;
        case TYPE_len:
            printf("#\n");
            break;
        case TYPE_mul:
            printf("*\n");
            break;
        case TYPE_div:
            printf("/\n");
            break;
        case TYPE_int_div:
            printf("//\n");
            break;
        case TYPE_add:
            printf("+\n");
            break;
        case TYPE_sub:
            printf("-\n");
            break;
        case TYPE_concat:
            printf("..\n");
            break;
        case TYPE_lt:
            printf("<\n");
            break;
        case TYPE_lt_eq:
            printf("<=\n");
            break;
        case TYPE_gt:
            printf(">\n");
            break;
        case TYPE_gt_eq:
            printf(">=\n");
            break;
        case TYPE_eq:
            printf("==\n");
            break;
        case TYPE_not_eq:
            printf("~=\n");
            break;
        case TYPE_EOF:
            printf("EOF\n");
            break;
    }
}
int main()
{
    Token token;
    token.type = TYPE_keyword;
    while (token.type != TYPE_EOF)
    {
        int error = get_token(&token);
        if (error != ERR_OK)
        {
            return 0;
        }
        print_token(token);
        if (token.type == TYPE_identifier || token.type == TYPE_string)
        {
            dstr_free(&(token.attribute.str));
        }
    }
    return 0;
}
