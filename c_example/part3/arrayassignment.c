#include <stdio.h>

int main(void)
{
    int nums[] = {6, 5, 8, 3, 2, 1};
    int nums2[6];

    //nums2 = nums; 안된다
    for (int i = 0; i < 6; ++i){
        nums2[i] = nums[i];
    }
    for (int i = 0; i < 6; ++i){
        printf("%d, ", nums2[i]);
    }
    printf("\n");
    return 0;
}