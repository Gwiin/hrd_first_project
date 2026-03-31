#include <stdio.h>
#include <string.h>


int main(void){
    char *ga = "djkvxcksgudsldvsdhdfhn";
    char *str = "wonderful tonight";

    //strlen함수
    int len = strlen(str);
    printf("len: %d\n", len);

    //strcpy 함수
    char str2[200];
    strcpy(str2, ga);
    strcpy(str2, str);
    for (int i = 0 ; 1 < 200 ; ++i){
               printf("C%d: %c\n", i, str2[i]);
    }    

    // strcmp 함수
    if(strcmp(str2, str) == 0 ){
        printf("str 과 str2 는 같다.\n");
    } else {
        printf("str 과 str2 는 다르다.\n");
    }

    // strcat 함수
    char *str3 = "eric claption";
    strcat(str2, " - ");
    strcat(str2, str3);
    printf("str2: %s\n", str2);
    return 0;
}