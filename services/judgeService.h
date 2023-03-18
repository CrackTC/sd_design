#ifndef JUDGE_H
#define JUDGE_H
#include "../data/table.h"
#include "../data/operation.h"

Table *judge(int staffId, int *hasPermission, const char *password, Operation operation, Table *a);

#endif
