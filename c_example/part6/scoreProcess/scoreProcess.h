#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    char name[20];
    int kor;
    int eng;
    int mat;
    int sum;
    double average;
    int rank;
} Sdata;

void inputData(FILE *fp, Sdata *s, int n);
void calculateScore(Sdata *s, int n);
void calculateRank(Sdata *s, int n);
void sortPointers(Sdata *s, Sdata **table, int n);
void printResult(FILE *fp, Sdata **table, int n);
int count_students(FILE *fp);