#include <stdio.h>


typedef struct
{
    int year;
    int month;
    int day;
}Date;


int main(void)
{
    Date d;
    Date *pD;
    d.year = 2026;
    d.month = 7;
    d.day = 19;

    pD = &d;

    printf("%d-%d-%d\n", d.year, d.month, d.day);
    printf("%d-%d-%d\n", (*pD).year, (*pD).month, (*pD).day);
    printf("%d-%d-%d\n", pD->year, pD->month, pD->day);

    return 0;
}


//예시는 데이터가 없지만   typedef 쓰면 생략이 많이 된다.