#include <stdio.h>

typedef struct list{
    int num;
    struct list *next;
}List;

int main(void)
{
    List a = {10,0}, b = {20,0}, c = {30,0};
    List *head = &a, *current;
    a.next = &b;
    b.next = &c;

    printf("head->num : %d\n", head->num);
    printf("head->next->num :%d\n", head->next->num);

    printf("list all : ");
    current = head;
    while(current !=NULL)
    {
        printf("%d", current->num);
        current = current->next;
    }
    printf("\n");
    return 0;
}