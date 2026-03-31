#include "stack.h"
// 전역 변수 사용, 포인터로 매개변수 넘기기
// 배열의 사용 범위가 정해져 있음

int main(void)
{
    push(100);
    push(200);
    push(300);

    printf("첫 pop() 리턴 값: %d\n", pop());     // 300
    printf("두번째 pop() 리턴 값: %d\n", pop()); // 200
    printf("세번째 pop() 리턴 값: %d\n", pop()); // 100
    return 0;
}