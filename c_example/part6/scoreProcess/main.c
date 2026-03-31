// 구조체를 다뤄서 파일포인터로 저장, 읽어오기, 처리 를 하는 프로그램.
// Test result 출력 예시.
// -----------------------------------------------
//                  Test Result
// -----------------------------------------------
// ParkJungSeok         100 100 100 300 100.00  1
// LeeYoungHo            99  85  91 275  91.67  2
// LimYoHan              85  91  95 271  90.33  3
// KimTaekYong           95  83  91 269  89.67  4
// LeeYunYeol            80  90  85 255  85.00  5
// LeeJaeDong            95  85  51 231  77.00  6
// SongByungKoo          70  75  69 214  71.33  7
// MaJaeYoon             50  60  55 165  55.00  8
// ShinJiSoo             55  45  33 133  44.33  9
// HongJinHo             45  32  55 132  44.00 10
// -----------------------------------------------
// **table 포인터를 쓰지 않으려면 쓰지 않고 결과만 만들어도 됨
#include "scoreProcess.h"

int main(void){
    char *finPath = "/home/aa/kuBig2026/c_example/part6/scoreProcess/score.dat";
    char *foutPath = "/home/aa/kuBig2026/c_example/part6/scoreProcess/score.out";
    FILE *fin = fopen(finPath, "r");
    FILE *fout = fopen(foutPath, "w");

    int n = count_students(fin);

    if (fin == NULL || fout == NULL){
        fprintf(stderr, "파일을 열수 없습니다.\n");
        return 1;
    }

    Sdata *students = (Sdata *)malloc(sizeof(Sdata) * n);
    Sdata **table = (Sdata **)malloc(sizeof(Sdata) * n);

    if (students == NULL || table == NULL){
        fprintf(stderr, "메모리 할당 실패\n");
        return 1;
    }

    inputData(fin, students, n);
    calculateScore(students, n);
    calculateRank(students, n);
    sortPointers(students, table, n);
    printResult(fout, table, n);

    free(table);
    free(students);

    fclose(fin);
    fclose(fout);
    return 0;
}