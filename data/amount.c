/*
 * FileName: data/amount.c
 * Author: 陈若珂
 * Description: 包含与金额相关的数据表示和操作的实现
 * */

#include "amount.h"
#include <math.h>
#include <stdio.h>

struct Amount
{
    long long value;
};

size_t GetIntegerLength(int num)
{
    return (int)((ceil(log10(num)) + 1) * sizeof(char));
}

int GetAmountYuan(const Amount *amount) {
    return amount->value / 100;
}

int GetAmountJiao(const Amount *amount) {
    return (amount->value % 100) / 10;
}

int GetAmountCent(const Amount *amount) {
    return amount->value % 10;
}

size_t AmountToString(char *destination, size_t maxSize, const Amount *amount)
{
    int yuan = amount->value / 100;
    int jiao = (amount->value % 100) / 10;
    int cent = amount->value % 10;
    int length = GetIntegerLength(yuan) + GetIntegerLength(jiao) + GetIntegerLength(cent) + 2;
    if (length <= maxSize) {
        sprintf(destination, "%d.%d%d", yuan, jiao, cent);
        return length - 1;
    }
    return 0;
}

Amount *AmountAdd(const Amount *amountA, const Amount *amountB)
{
}

