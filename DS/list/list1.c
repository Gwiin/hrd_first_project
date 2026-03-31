#include <stdio.h>
#include <stdlib.h>

typedef struct node{
    int data;
    struct node *next;
} Node;

void insertNode(int data, Node *p);
void deleteList(Node *p);
void deleteNode(Node *p, int data);

int main(void)
{
    Node *p;
    p = malloc(sizeof(Node));

    p->data = 100;
    // 두번째 데이터 생성
    p->next = malloc(sizeof(Node));
    p->next->data = 200;

    // 세번째 데이터 생성
    p->next->next = malloc(sizeof(Node));
    p->next->next->data = 300;

    p->next->next->next = malloc(sizeof(Node));
    p->next->next->next->data = 400;

    // ...

    // 삽입 코드
    insertNode(30, p);
    insertNode(40, p->next);
    // insertNode(3, p->next->next);

    // 삭제 코드
    deleteNode(p, 30);

    // 출력 코드
    Node *tmp;
    tmp = p;
    while(tmp ){
        printf("%d\n", tmp->data);
        tmp = tmp->next;
    }

    // heap 에서 list 를 전체를 free
    deleteList(p);
    return 0;
}

void insertNode(int data, Node *p)
{
    Node *tmp = malloc(sizeof(Node));
    tmp->data = data;
    tmp->next = p->next;
    p->next = tmp;
}

void deleteList(Node *p)
{
    Node *curr = p;
    Node *tmp2;
    while (curr != NULL)
    {
        tmp2 = curr->next;
        free(curr);
        curr = tmp2;
    }
}

void deleteNode(Node *p, int data)
{
    Node *curr = p;
    Node *prev = NULL;

    while(curr != NULL && curr->data != data){
        prev = curr;
        curr = curr->next;
    }
    if (curr ==NULL)
        return;
    if(prev == NULL){
        p = curr->next;
    }else{
        prev->next = curr->next;
    }
    free(curr);
}