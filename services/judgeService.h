#ifndef JUDGE_H
#define JUDGE_H
#include "../data/operation.h"
#include "../data/table.h"

Table *judge(int staffId, int *hasPermission, const char *password, Operation operation, Table *a);

#endif
