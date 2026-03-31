#pragma once
#include <stdio.h>

typedef struct
{
    int year;
    int month;
    int day;
}Date;

void printDate(Date *pD);
void setDate(Date *pDate, const int year, const int month, const int day);