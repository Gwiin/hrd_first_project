#include "date.h"

void printDate(Date *pDate)
{
    printf("%d-%d-%d\n", pDate->year, pDate->month, pDate->day);
}
void setDate(Date *pDate, const int year, const int month, const int day)
{
    pDate -> year = year;
    pDate -> month = month;
    pDate -> day = day;
}