#include <stdio.h>


struct date{
    int year;
    int month;
    int day;
};


int main(void)
{
    struct date d;
    d.year = 2026;
    d.month = 7;
    d.day = 27;
    printf("%d-%d-%d\n", d.year, d.month, d.day);

    return 0;
}