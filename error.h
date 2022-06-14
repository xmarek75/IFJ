/**********************************************************************
* Nazev projektu: Implementace překladače imperativního jazyka IFJ21
* Nazev souboru: error.h
* Popis souboru: vycet vsech erroru
* Autori: Vilem Gottwald (xgottw07)
* Datum posledni zmeny: 8. 12. 2021
**********************************************************************/
#ifndef ERROR_H
#define ERROR_H

enum
{ // vycet moznych chybovych navratovych hodnot
    ERR_OK = 0,         // 0 - vse ok
    ERR_LEX,            // 1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému).
    ERR_SYN,            // 2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu).
    ERR_SEM_DEFINITION, // 3 - sémantická chyba v programu – nedefinovaná funkce/proměnná, pokus o redefinici proměnné, atp.
    ERR_SEM_ASSIGN,     // 4 - sémantická chyba v příkazu přiřazení (typová nekompatibilita).
    ERR_SEM_FUNC,       // 5 - sémantická chyba v programu – špatný počet/typ parametrů či návratových hodnot u volání funkce či návratu z funkce.
    ERR_SEM_TYPE_COMP,  // 6 - sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech.
    ERR_SEM_OTHERS,     // 7 - ostatní sémantické chyby.
    ERR_NIL,            // 8 - běhová chyba při práci s neočekávanou hodnotou nil.
    ERR_ZERO_DIV,       // 9 - běhová chyba celočíselného dělení nulovou konstantou.
    ERR_INTERNAL = 99   // 99 - interní chyba
};

#endif
