#include <stdio.h>

int main(void)
{
    int nums1[5] = {1, 2, 3, 4, 5};
    int nums2[] = {1, 2, 3, 4, 5};
    int nums3[10] = {1, 2, 3, 4, 5};
    int nums4[50];
    int nums5[50] = {0};

    int n = sizeof(nums4) / sizeof(int);
    for (int i = 0; i < n;++i){
        printf("%d ", nums4[i]);
    }
    printf("\n");
    return 0;
}