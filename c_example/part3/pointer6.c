#include <stdio.h>

int main(void)
{
    int i = 100;
    double d = 3.14;

    void *p;

    p = &i;
    *(int*)p = 200;

    p= &d;
    *(double *)p = 2.718;
    printf("i : %d, d: %f, *p : %f\n", i, d, *(double *)p);
    return 0;

}
  