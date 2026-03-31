#include <stdio.h>

int main(void)
{
    int var_a;
    int *pvar_a;
    int **ppvar_a;

    var_a = 100;
    pvar_a = &var_a;
    ppvar_a = &pvar_a;

    **ppvar_a = 200;
    printf("var_a: %d, &var_a: %p\n", var_a, &var_a);
    printf("pvar_a: %p, *pvar_a: %d\n", pvar_a, *pvar_a);
    printf("ppvar_a: %p, *ppvar_a: %p **ppvar_a: %d\n", ppvar_a, *ppvar_a, **ppvar_a);

    return 0;

}