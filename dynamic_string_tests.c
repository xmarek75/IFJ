#include "dynamic_string.h"
#include <stdio.h>

void print_str (char *test_name, D_str *ds)
{
    printf("**** %s ****\n", test_name);
    printf("Text: %s \n", ds->string);
    printf("Delka: %d \n", ds->lenght);
    printf("Pouzity prostor: %d/%d bajtu \n", ds->lenght + 1, ds->alloc_size);
    printf("******************\n\n");
}
void print_cmp (D_str *ds1, D_str *ds2)
{
    printf("**** Porovnani ds1 a ds2 ****\n");
    printf("Text ds1: %s \n", ds1->string);
    printf("Text ds2: %s \n", ds2->string);
    printf("Porovnani vratilo: %d \n", dstr_cmp_dstr(ds1, ds2));
    printf("******************\n\n");
}

void print_cmp_str (D_str *ds, char *s)
{
    printf("**** Porovnani ds1 a s ****\n");
    printf("Text ds: %s \n", ds->string);
    printf("Text s: %s \n", s);
    printf("Porovnani vratilo: %d \n", dstr_cmp_str(ds, s));
    printf("******************\n\n");
}

int main()
{
    D_str str;
    D_str *ds = &str;

    dstr_init(ds);
    print_str("Inicaializace ds", ds);

    dstr_add_str(ds, "abcdefg");
    print_str("Pridani retezce do ds", ds);

    dstr_add_char(ds, 'c');
    print_str("Pridani znaku do ds", ds);

    dstr_clear(ds);
    print_str("Vycisteni ds", ds);

    dstr_add_str(ds, "abcdefghijklmnopqrstuv");
    print_str("Pridani retezce velke delky do ds", ds);

    D_str str2;
    D_str *ds2 = &str2;

    dstr_init(ds2);
    print_str("Inicaializace ds2", ds2);

    dstr_copy_dstr(ds, ds2);
    print_str("Kopie ds do ds2", ds2);

    print_cmp(ds, ds2);

    dstr_clear(ds);
    print_str("Vynulovani ds", ds);

    dstr_add_str(ds, "keyword1");
    print_str("Vlozeni \"keyword1\" do ds", ds);
    print_cmp_str(ds, "keyword1");

    dstr_free(ds);
    dstr_free(ds2);
}
