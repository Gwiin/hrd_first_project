#include <stdio.h>
void swap(int *a, int *b);

int main(void)
{
    int nums[10] = {40, 20, 52, 12, 34, 64, 93, 42, 42, 5};

    int indexN = sizeof(nums) / sizeof(int);

    // j 버블이 올라오는 위치 i 는 갯수
    for (int i = indexN -1 ; i >= 1; --i)
    {
        for (int j = 0; j < i; ++j)
        {
            if (nums[j] > nums[j+1])
            {
                swap(&nums[j], &nums[j+1]);
            }
        }
    }

    for (int i = 0; i < indexN; ++i)
    {
        printf("%d, ", nums[i]);
    }
    printf("\n");
    return 0;
}

void swap(int *a, int *b)
{
    int tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}