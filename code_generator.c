/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: code_generator.c
* Popis souboru: implementace funkci pro generovani vnitrniho kodu
* Autori: Stepan Bilek (xbilek25), Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"

#include "expression.h"
#include "code_generator.h"


#define READS							\
	"# Vestavena funkce READS\n" 		\
	"LABEL $reads\n"					\
	"PUSHFRAME\n"						\
	"DEFVAR LF@%%retval0\n"				\
	"READ LF@%%retval0 string\n"		\
	"POPFRAME\n"						\
	"RETURN\n\n"

#define READI							\
	"# Vestavena funkce READI\n" 		\
	"LABEL $readi\n"					\
	"PUSHFRAME\n"						\
	"DEFVAR LF@%%retval0\n"				\
	"READ LF@%%retval0 int\n"			\
	"POPFRAME\n"						\
	"RETURN\n\n"

#define READN							\
	"# Vestavena funkce READN\n" 		\
	"LABEL $readn\n"					\
	"PUSHFRAME\n"						\
	"DEFVAR LF@%%retval0\n"				\
	"READ LF@%%retval0 float\n"			\
	"POPFRAME\n"						\
	"RETURN\n\n"

#define WRITE									\
	"# Vestavena funkce WRITE\n" 				\
	"LABEL $write\n"							\
	"PUSHFRAME\n"								\
	"DEFVAR LF@%%print\n"						\
	"DEFVAR LF@%%aux\n"							\
	"LABEL $write$defvar\n"						\
	"JUMPIFEQ $write$end LF@%%param0 int@0\n" 	\
	"POPS LF@%%print\n"							\
	"JUMPIFEQ $write$nil nil@nil LF@%%print\n"	\
	"WRITE LF@%%print\n"						\
	"MOVE LF@%%aux LF@%%param0\n"				\
	"SUB LF@%%param0 LF@%%aux int@1\n"			\
	"JUMP $write$defvar\n"						\
	"LABEL $write$end\n"						\
	"POPFRAME\n"								\
	"RETURN\n\n"								\
	"LABEL $write$nil\n"						\
	"WRITE string@nil\n"						\
	"JUMP $write$end\n\n"

#define TOINTEGER										\
	"# Vestavena funkce TOINTEGER\n" 					\
	"LABEL $tointeger\n"								\
	"PUSHFRAME\n"										\
	"DEFVAR LF@%%retval0\n"								\
	"MOVE LF@%%retval0 nil@nil\n"						\
	"JUMPIFEQ $tointeger$return LF@%%param0 nil@nil\n"	\
	"FLOAT2INT LF@%%retval0 LF@%%param0\n"				\
	"LABEL $tointeger$return\n"							\
	"POPFRAME\n"										\
	"RETURN\n\n"


#define CHR												\
	"# Vestavena funkce CHR\n"							\
	"LABEL $chr\n"										\
	"PUSHFRAME\n"										\
	"DEFVAR LF@%%podminka\n"							\
	"EQ LF@%%podminka LF@%%param0 nil@nil\n"			\
	"JUMPIFEQ $chr$error LF@%%podminka bool@true\n"		\
	"DEFVAR LF@%%retval0\n"								\
	"MOVE LF@%%retval0 nil@nil \n"						\
	"LT LF@%%podminka LF@%%param0 int@0\n"				\
	"JUMPIFEQ $chr$return LF@%%podminka bool@true\n"	\
	"GT LF@%%podminka LF@%%param0 int@255\n"			\
	"JUMPIFEQ $chr$return LF@%%podminka bool@true\n"	\
	"INT2CHAR LF@%%retval0 LF@%%param0\n"				\
	"LABEL $chr$return\n"								\
	"POPFRAME\n"										\
	"RETURN\n\n"										\
	"LABEL $chr$error\n"								\
	"EXIT int@8\n\n"


#define ORD												\
	"# Vestavena funkce ORD\n"							\
	"LABEL $ord\n"										\
	"PUSHFRAME\n"										\
	"DEFVAR LF@%%podminka\n"							\
	"EQ LF@%%podminka LF@%%param0 nil@nil\n"			\
	"JUMPIFEQ $ord$error LF@%%podminka bool@true\n"		\
	"EQ LF@%%podminka LF@%%param1 nil@nil\n"			\
	"JUMPIFEQ $ord$error LF@%%podminka bool@true\n"		\
	"DEFVAR LF@%%retval0\n"								\
	"MOVE LF@%%retval0 nil@nil\n"						\
	"LT LF@%%podminka LF@%%param1 int@1\n"				\
	"JUMPIFEQ $ord$return LF@%%podminka bool@true\n"	\
	"DEFVAR LF@%%delka\n"								\
	"STRLEN LF@%%delka  LF@%%param0\n"					\
	"GT LF@%%podminka LF@%%param1 LF@%%delka\n"			\
	"JUMPIFEQ $ord$return LF@%%podminka bool@true\n"	\
	"SUB LF@%%param1 LF@%%param1 int@1\n"				\
	"STRI2INT LF@%%retval0 LF@%%param0 LF@%%param1\n"	\
	"LABEL $ord$return\n"								\
	"POPFRAME\n"										\
	"RETURN\n\n"										\
	"LABEL $ord$error\n"								\
	"EXIT int@8\n\n"


#define SUBSTR											\
	"# Vestavena funkce SUBSTR\n"						\
	"LABEL $substr\n"									\
	"PUSHFRAME\n"										\
	"DEFVAR LF@%%podminka\n"							\
	"EQ LF@%%podminka LF@%%param0 nil@nil\n"			\
	"JUMPIFEQ $substr$error LF@%%podminka bool@true\n"	\
	"EQ LF@%%podminka LF@%%param1 nil@nil\n"			\
	"JUMPIFEQ $substr$error LF@%%podminka bool@true\n"	\
	"EQ LF@%%podminka LF@%%param2 nil@nil\n"			\
	"JUMPIFEQ $substr$error LF@%%podminka bool@true\n"	\
	"DEFVAR LF@%%retval0\n"								\
	"MOVE LF@%%retval0 string@\n"						\
	"DEFVAR LF@%%delka_str\n"							\
	"STRLEN LF@%%delka_str LF@%%param0\n"				\
	"GT LF@%%podminka LF@%%param1 LF@%%param2\n"		   /* kontrola zda je prvni index mensi nez druhy*/\
	"JUMPIFEQ $substr$return LF@%%podminka bool@true\n"	\
	"LT LF@%%podminka LF@%%param1 int@1\n"					/* kontrola zda je prvni index vetsi nez 0*/\
	"JUMPIFEQ $substr$return LF@%%podminka bool@true\n"	\
	"GT LF@%%podminka LF@%%param2 LF@%%delka_str\n"			/* kontrola zda je druhy index mensi nez delka retezce*/\
	"JUMPIFEQ $substr$return LF@%%podminka bool@true\n"	\
	"DEFVAR LF@%%pocitadlo\n"							\
	"DEFVAR LF@%%tmp\n"									\
	"MOVE LF@%%tmp string@\n"							\
	"MOVE LF@%%pocitadlo LF@%%param1\n"					\
	"SUB LF@%%pocitadlo LF@%%pocitadlo int@1\n"			\
	"LABEL $substr$loop\n"								\
	"GETCHAR LF@%%tmp LF@%%param0 LF@%%pocitadlo\n"		\
	"CONCAT LF@%%retval0 LF@%%retval0 LF@%%tmp\n"		\
	"ADD LF@%%pocitadlo LF@%%pocitadlo int@1\n"			\
	"EQ LF@%%podminka LF@%%pocitadlo LF@%%param2\n"		\
	"JUMPIFEQ $substr$return LF@%%podminka bool@true\n"	\
	"JUMP $substr$loop\n"								\
	"LABEL $substr$return\n"							\
	"POPFRAME\n"										\
	"RETURN\n\n"										\
	"LABEL $substr$error\n"								\
	"EXIT int@8\n\n"									\







void generate_builtin()
{
	printf("\n # --- Konec programu --- \n");
	printf("EXIT int@0\n\n");
	printf(READS);
	printf(READI);
	printf(READN);
	printf(WRITE);
	printf(TOINTEGER);
	printf(CHR);
	printf(ORD);
	printf(SUBSTR);
}


void generate_code_start()
{
	printf(".IFJcode21\n\n");
}

// Definice funkce
void generate_label(char *name)
{
	printf("# Uzivatelem definovana funkce %s\n", name);
	printf("JUMP $%s!defend\n", name);
	printf("LABEL $%s\n", name);
	printf("PUSHFRAME\n");
}

void generate_return_value(int index)
{
	printf("DEFVAR LF@%%retval%d\n", index);
	printf("MOVE LF@%%retval%d nil@nil\n", index);
}

void generate_function_end(char *name)
{
	printf("POPFRAME\n");
	printf("RETURN\n");
	printf("LABEL $%s!defend\n\n", name);

}

// Volani funkce
void generate_createframe()
{
	printf("\n# Priprava parametru a nasledne volani funkce\n");
	printf("CREATEFRAME\n");
}

void generate_function_params(int index, Token token, int level)
{
	printf("DEFVAR TF@%%param%d\n", index);

	if(token.type == TYPE_keyword && token.attribute.kw == KW_nil)
	{
		printf("MOVE TF@%%param%d nil@nil\n", index);
	}
	else if (token.type == TYPE_identifier)
	{
		printf("MOVE TF@%%param%d LF@%s$%d\n", index, token.attribute.str.string, level);
	}
	else if (token.type == TYPE_integer)
	{
		printf("MOVE TF@%%param%d int@%d\n", index, token.attribute.int_val);
	}
	else if (token.type == TYPE_number)
	{
		printf("MOVE TF@%%param%d float@%a\n", index, token.attribute.num_val);
	}
	else if(token.type == TYPE_string)
	{
		printf("MOVE TF@%%param%d string@%s\n", index, convert_STR(token.attribute.str));
	}
}
void generate_def_function_params(int index, Token token, int level)
{
	printf("DEFVAR LF@%s$%d\n", token.attribute.str.string, level);
	printf("MOVE LF@%s$%d LF@%%param%d \n", token.attribute.str.string, level, index);
}
void generate_write_param(Token token, int level)
{
	if(token.type == TYPE_keyword && token.attribute.kw == KW_nil)
	{
		printf("PUSHS nil@nil\n");
	}
	else if (token.type == TYPE_identifier)
	{
		printf("PUSHS LF@%s$%d\n", token.attribute.str.string, level);
	}
	else if (token.type == TYPE_integer)
	{
		printf("PUSHS int@%d\n", token.attribute.int_val);
	}
	else if (token.type == TYPE_number)
	{
		printf("PUSHS float@%a\n", token.attribute.num_val);
	}
	else if(token.type == TYPE_string)
	{
		printf("PUSHS string@%s\n", convert_STR(token.attribute.str));
	}
}



void generate_write_count(int count)
{
	printf("DEFVAR TF@%%param0\n");
	printf("MOVE TF@%%param0 int@%d\n", count);
}

void generate_call_function(char *function_name)
{
	printf("CALL $%s\n", function_name);
}

void generate_var_def(char *name, int level)
{
	printf("DEFVAR LF@%s$%d\n", name, level);
}

 // prirazeni hodnoty volanim funkce pri definici promenne
void generate_var_def_assign_retval(char *name, int level)
{
	printf("# Prirazeni navratove hodnoty zavolane funkce\n");
	printf("MOVE LF@%s$%d TF@%%retval0\n", name, level);
}
 // prirazeni hodnoty vyrazu pri definici promenne
void generate_var_def_assign_expr(char *name, int level, int nonterm_index, int expression_index)
{
	printf("# Prirazeni hodnoty vyrazu\n");
	printf("MOVE LF@%s$%d LF@-E%d-%d\n", name, level, expression_index, nonterm_index );
}

void generate_expr_int_nonterm(int value, int nonterm_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n",expression_index, nonterm_index);
	printf("MOVE LF@-E%d-%d int@%d\n", expression_index, nonterm_index, value);
}

void generate_expr_num_nonterm(double value, int nonterm_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n",expression_index, nonterm_index);
	printf("MOVE LF@-E%d-%d float@%a\n", expression_index, nonterm_index, value);
}

void generate_expr_str_nonterm(D_str string, int nonterm_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n",expression_index, nonterm_index);
	printf("MOVE LF@-E%d-%d string@%s\n", expression_index, nonterm_index, convert_STR(string));
}

void generate_expr_id_nonterm(char *name, int level, int nonterm_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n",expression_index, nonterm_index);
	printf("MOVE LF@-E%d-%d LF@%s$%d\n", expression_index, nonterm_index, name, level);
}

void generate_expr_nil_nonterm(int nonterm_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n",expression_index, nonterm_index);
	printf("MOVE LF@-E%d-%d nil@nil\n", expression_index, nonterm_index);
}

void generate_expr_mul(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n", expression_index, out_nonterm_index);
	printf("MUL LF@-E%d-%d LF@-E%d-%d LF@-E%d-%d \n", expression_index, out_nonterm_index, expression_index, nonterm1_index, expression_index, nonterm2_index);
}

void generate_expr_add(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n", expression_index, out_nonterm_index);
	printf("ADD LF@-E%d-%d LF@-E%d-%d LF@-E%d-%d \n", expression_index, out_nonterm_index, expression_index, nonterm1_index, expression_index, nonterm2_index);
}

void generate_expr_div(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n", expression_index, out_nonterm_index);
	printf("DIV LF@-E%d-%d LF@-E%d-%d LF@-E%d-%d \n", expression_index, out_nonterm_index, expression_index, nonterm1_index, expression_index, nonterm2_index);
}

void generate_expr_int_div(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n", expression_index, out_nonterm_index);
	printf("IDIV LF@-E%d-%d LF@-E%d-%d LF@-E%d-%d \n", expression_index, out_nonterm_index, expression_index, nonterm1_index, expression_index, nonterm2_index);
}

void generate_expr_sub(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n", expression_index, out_nonterm_index);
	printf("SUB LF@-E%d-%d LF@-E%d-%d LF@-E%d-%d \n", expression_index, out_nonterm_index, expression_index, nonterm1_index, expression_index, nonterm2_index);
}

void generate_expr_concat(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n", expression_index, out_nonterm_index);
	printf("CONCAT LF@-E%d-%d LF@-E%d-%d LF@-E%d-%d \n", expression_index, out_nonterm_index, expression_index, nonterm1_index, expression_index, nonterm2_index);
}

void generate_expr_less(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n", expression_index, out_nonterm_index);
	printf("LT LF@-E%d-%d LF@-E%d-%d LF@-E%d-%d \n", expression_index, out_nonterm_index, expression_index, nonterm1_index, expression_index, nonterm2_index);
}

void generate_expr_less_eq(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n", expression_index, out_nonterm_index);
	printf("LT LF@-E%d-%d LF@-E%d-%d LF@-E%d-%d \n", expression_index, out_nonterm_index, expression_index, nonterm1_index, expression_index, nonterm2_index);
	printf("JUMPIFEQ $less$eq$end-%d LF@-E%d-%d bool@true\n",expression_index, expression_index, out_nonterm_index);
	printf("EQ LF@-E%d-%d LF@-E%d-%d LF@-E%d-%d \n",expression_index, out_nonterm_index, expression_index, nonterm1_index, expression_index, nonterm2_index);
	printf("LABEL $less$eq$end-%d\n", expression_index);
}

void generate_expr_more(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n", expression_index, out_nonterm_index);
	printf("GT LF@-E%d-%d LF@-E%d-%d LF@-E%d-%d \n", expression_index, out_nonterm_index, expression_index, nonterm1_index, expression_index, nonterm2_index);
}

void generate_expr_more_eq(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n", expression_index, out_nonterm_index);
	printf("GT LF@-E%d-%d LF@-E%d-%d LF@-E%d-%d \n", expression_index, out_nonterm_index, expression_index, nonterm1_index, expression_index, nonterm2_index);
	printf("JUMPIFEQ $more$eq$end-%d LF@-E%d-%d bool@true\n",expression_index, expression_index, out_nonterm_index);
	printf("EQ LF@-E%d-%d LF@-E%d-%d LF@-E%d-%d \n",expression_index, out_nonterm_index, expression_index, nonterm1_index, expression_index, nonterm2_index);
	printf("LABEL $more$eq$end-%d\n", expression_index);
}

void generate_expr_eq(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n", expression_index, out_nonterm_index);
	printf("EQ LF@-E%d-%d LF@-E%d-%d LF@-E%d-%d \n", expression_index, out_nonterm_index, expression_index, nonterm1_index, expression_index, nonterm2_index);
}

void generate_expr_not_eq(int out_nonterm_index, int nonterm1_index, int nonterm2_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n", expression_index, out_nonterm_index);
	printf("EQ LF@-E%d-%d LF@-E%d-%d LF@-E%d-%d \n", expression_index, out_nonterm_index, expression_index, nonterm1_index, expression_index, nonterm2_index);
	printf("NOT LF@-E%d-%d LF@-E%d-%d\n", expression_index, out_nonterm_index, expression_index, out_nonterm_index);
}

void generate_expr_hash(D_str string, int nonterm_index, int expression_index)
{
	printf("DEFVAR LF@-E%d-%d\n",expression_index, nonterm_index);
	printf("MOVE LF@-E%d-%d string@%s\n", expression_index, nonterm_index, convert_STR(string));
	printf("STRLEN LF@-E%d-%d LF@-E%d-%d\n", expression_index, nonterm_index, expression_index, nonterm_index);
}


// return expressions
void generate_fnc_return(int ret_index, int nonterm_index, int expression_index)
{
	printf("# Nastaveni navratove hodnoty\n");
	printf("MOVE LF@%%retval%d LF@-E%d-%d\n", ret_index, expression_index, nonterm_index);
}
//
void generate_if_start(int if_index, int expression_index, int nonterm_index)
{
	printf("# IF id:%d\n", if_index);
	printf("JUMPIFEQ $else$%d  LF@-E%d-%d bool@false\n", if_index, expression_index, nonterm_index );
}
void generate_else_label(int if_index)
{
	printf("# ELSE id:%d\n", if_index);
	printf("JUMP $end$%d\n", if_index);
	printf("LABEL $else$%d\n",if_index);
}
void generate_end_label(int if_index)
{
	printf("# IF END id:%d\n", if_index);
	printf("LABEL $end$%d\n", if_index);
}



void generate_while_start(int while_index, int expression_index, int nonterm_index)
{
	printf("# WHILE id:%d\n", while_index);
	printf("LABEL $while$start$%d\n", while_index);
	printf("JUMPIFEQ $while$end$%d  LF@-E%d-%d bool@false\n", while_index, expression_index, nonterm_index );
}

void generate_while_end_label(int while_index)
{
	printf("# WHILE END id:%d\n", while_index);
	printf("JUMP $while$start$%d\n", while_index);
	printf("LABEL $while$end$%d\n", while_index);
}















// Pomocne funkce
char* convert_STR(D_str str)
{
	D_str str_out;
	dstr_init(&(str_out));
	for (int i = 0; i < str.lenght; i++)
	{
		if((str.string[i] > 9 && str.string[i] < 33) || str.string[i] == 35 || str.string[i] == 92 )
		{
			dstr_add_char(&(str_out), '\\');
			dstr_add_char(&(str_out), '0');
			char tmp_str[4];
			sprintf(tmp_str, "%d", str.string[i]);
			dstr_add_char(&(str_out), tmp_str[0]);
			dstr_add_char(&(str_out), tmp_str[1]);
		}
		else if (str.string[i] < 10)
		{
			dstr_add_char(&(str_out), '\\');
			dstr_add_char(&(str_out), '0');
			dstr_add_char(&(str_out), '0');
			dstr_add_char(&(str_out), str.string[i] + 48);
		}
		else
		{
			dstr_add_char(&(str_out), str.string[i]);
		}
	}
	return str_out.string;
}
