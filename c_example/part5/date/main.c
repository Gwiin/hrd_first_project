#include "date.h"

int main(void)
{
    Date d;

    setDate(&d, 2026, 3, 16);
    printDate(&d);
    return 0;
}