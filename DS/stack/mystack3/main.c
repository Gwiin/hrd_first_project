#include "stack3.h"

int main(void)
{
    Stack s1, s2;

    initStack(&s1, 10);
    initStack(&s2, 100);

    push(&s1, 100);
    push(&s1, 200);
    push(&s1, 300);

    printf("s1 첫 pop() 리턴 값: %d\n", pop(&s1));     // 300
    printf("s1 두번째 pop() 리턴 값: %d\n", pop(&s1)); // 200
    printf("s1 세번째 pop() 리턴 값: %d\n", pop(&s1)); // 100

    push(&s2, 700);
    push(&s2, 800);
    push(&s2, 900);

    printf("s2 첫 pop() 리턴 값: %d\n", pop(&s2));     // 900
    printf("s2 두번째 pop() 리턴 값: %d\n", pop(&s2)); // 800
    printf("s2 세번째 pop() 리턴 값: %d\n", pop(&s2)); // 700

    printf("s1 스택 메모리 사이즈는 : %d\n", s1.size);
    printf("s2 스택 메모리 사이즈는 : %d\n", s2.size);



    cleanupStack(&s1);
    cleanupStack(&s2);
    return 0;
}