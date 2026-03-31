#include "scoreProcess.h"

int count_students(FILE *fp)
{
    int count = 0;
    char tmp[100];

    while(fgets(tmp, sizeof(tmp), fp) != NULL){
        if (tmp[0] != '\n' && tmp[0] != '\r'){
            count++;
        }
    }
    rewind(fp);
    return count;
}

void inputData(FILE *fp, Sdata *s, int n)
{
    for (int i = 0; i < n; ++i)
    {
        fscanf(fp, "%s %d %d %d", s[i].name, &s[i].kor, &s[i].eng, &s[i].mat);
    }
}
void calculateScore(Sdata *s, int n)
{
    for (int i = 0; i < n; ++i)
    {
        s[i].sum = s[i].kor + s[i].eng + s[i].mat;
        s[i].average = (double)s[i].sum / 3.0;
        s[i].rank = 1;
    }
}
void calculateRank(Sdata *s, int n)
{
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (s[i].sum < s[j].sum)
                s[i].rank++;
        }
    }
}
void sortPointers(Sdata *s, Sdata **table, int n)
{
    for (int i = 0; i < n; ++i)
        table[i] = &s[i];
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (table[i]->sum > table[j]->sum)
            {
                Sdata *tmp = table[i];
                table[i] = table[j];
                table[j] = tmp;
            }
        }
    }
}
void printResult(FILE *fp, Sdata **table, int n)
{
    fprintf(fp, "-----------------------------------------------\n");
    fprintf(fp, "                  Test Result\n");
    fprintf(fp, "-----------------------------------------------\n");
    for (int i = 0; i < n; ++i)
    {
        fprintf(fp, "%-20s %3d %3d %3d %3d %6.2f %2d\n", table[i]->name, table[i]->kor, table[i]->eng, table[i]->mat, table[i]->sum, table[i]->average, table[i]->rank);
    }
    fprintf(fp, "-----------------------------------------------\n");
}