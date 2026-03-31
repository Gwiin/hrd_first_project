#include <stdio.h>
#include "queue3.h"

int main(void)
{
    Qu q1, q2;
    initQueue(&q1, 10, sizeof(int));
    initQueue(&q2, 100, sizeof(double));

    int i = 100;
    push(&q1, &i);
    i = 200;
    push(&q1, &i);
    i = 300;
    push(&q1, &i);

    int re;
    pop(&q1, &re);
    printf("q1 첫 pop() 리턴 값: %d\n", re); // 100
    pop(&q1, &re);
    printf("q1 두번째 pop() 리턴 값: %d\n", re); // 200
    pop(&q1, &re);
    printf("q1 세번째 pop() 리턴 값: %d\n", re); // 300

    double d = 1.1;
    push(&q2, &d);
    d = 2.2;
    push(&q2, &d);
    d = 3.3;
    push(&q2, &d);

    double re2;
    pop(&q2, &re2);
    printf("s2 첫 pop() 리턴 값: %lf\n", re2); // 1.1
    pop(&q2, &re2);
    printf("s2 두번째 pop() 리턴 값: %lf\n", re2); // 2.2
    pop(&q2, &re2);
    printf("s2 세번째 pop() 리턴 값: %lf\n", re2); // 3.3

    printf("s1 큐 메모리 사이즈는 : %d\n", q1.size);
    printf("s2 큐 메모리 사이즈는 : %d\n", q2.size);

    // size 를 넘겼을 때 assert작동 확인
    // max index 를 넘어 갔을 때 100 개채울 때 에러
    for (int i = 0; i < 101; ++i)
    {
        double d = i + 3.14;
        push(&q2, &d);
        printf("%lf , \n", (double)i + 3.14);
    }
    // 데이터가 없을 때 assert작동 확인
    // 데이터가 부족할때
    for (int i = 0; i < 102; ++i)
    {
        pop(&q2, &re2);
        printf("%d번째 pop(): %lf\n", i, re2);
    }

    cleanupQueue(&q1);
    cleanupQueue(&q2);
    return 0;
}