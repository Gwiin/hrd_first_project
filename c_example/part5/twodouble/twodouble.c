#include "twodouble.h"

void exchange(Vision *robot);

void exchange(Vision *robot)
{
    double temp;
    temp = robot->left;
    robot->left = robot->right;
    robot->right = temp;
}