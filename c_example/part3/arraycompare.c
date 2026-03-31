#include <stdio.h>
#include <stdbool.h>

bool arrayCompare(int *array, int *array2, int index, int *diff);

int main(void)
{
    int nums1[] = {1, 3, 7, 4, 9};
    int nums2[] = {1, 3, 7, 4, 9};
    int nums3[] = {1, 3, 7, 9, 5};

    // (nums1 == nums2) 안된다.
    int diff = 0;

    if (arrayCompare(nums1, nums2, 5, &diff))
    {
        printf("두 배열은 같다.\n");
    }
    else
    {
        printf("두 배열은 같지 않다. %d index가 다르다.\n", diff);
    }

    if (arrayCompare(nums2, nums3, 5, &diff))
    {
        printf("두 배열은 같다.\n");
    }
    else
    {
        printf("두 배열은 같지 않다. %d index가 다르다.\n", diff);
    }
    return 0;
}

    bool arrayCompare(int *array, int *array2, int index, int *diff)
{
    int i;
    for (i = 0; i < index; ++i)
    {
        if (array[i] != array2[i])
        {
            break;
        }
    }
    *diff = i;
    if (i == index)
    {
        return true;
    }
    else
    {
        return false;
    }
}