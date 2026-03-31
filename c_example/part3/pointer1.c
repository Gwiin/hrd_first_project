#include <stdio.h>

int main(void)
{
   int *p;
   int value;
   p= &value;
   *p = 200;
   printf("포인터의 주소는 %p 이고, 가리키는 값은 %d이다.", p, *p);
   return 0;

}