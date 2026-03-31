// random으로 int 10 배열을 만들어서 값을 넣고, 총합을 계산하는 프로그램을 만드세요. 


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    int sum = 0;
    int arr[10];
    srand(time(NULL));
    for (int i = 0; i < 10; i++)
    {
        arr[i] = rand() % 100;
        sum += arr[i];
    }
    printf("총합: %d\n", sum);
    return 0;
}