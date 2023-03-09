/*
 * FileName: data/amount.c
 * Author: 陈若珂
 * Description: 包含与金额相关的数据表示和操作的实现
 * */

#include "amount.h"
#include <limits.h>
#include <malloc.h>
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

int GetAmountYuan(const Amount *amount)
{
    return amount->value / 100;
}

int GetAmountJiao(const Amount *amount)
{
    return (amount->value % 100) / 10;
}

int GetAmountCent(const Amount *amount)
{
    return amount->value % 10;
}

Amount *AmountAdd(const Amount *amountA, const Amount *amountB)
{
    long long resultValue = amountA->value + amountB->value;
    if (resultValue / 100 > INT_MIN && resultValue / 100 < INT_MAX)
    {
        Amount *amount = malloc(sizeof(Amount));
        amount->value = resultValue;
        return amount;
    }
    return NULL;
}

Amount *AmountMultiply(const Amount *amount, int multiple)
{
    long long resultValue = amount->value * multiple;
    if (resultValue / 100 > INT_MIN && resultValue / 100 < INT_MAX)
    {
        Amount *amount = malloc(sizeof(Amount));
        amount->value = resultValue;
        return amount;
    }
    return NULL;
}

Amount *AmountMultiplyRatio(const Amount *amount, int ratio)
{
    if (ratio > 100 || ratio < 0) return NULL;
    long long resultMul100 = amount->value * ratio;
    int bias = (resultMul100 % 100) / 10;
    long long result = resultMul100 / 100;
    Amount *resultAmount = malloc(sizeof(Amount));
    if (-4 <= bias && bias <= 4)
    {
        resultAmount->value = result;
        return resultAmount;
    }
    else
    {
        resultAmount->value = bias > 0 ? result + 1 : result - 1;
        return  resultAmount;
    }
}

Amount *NewAmount(int yuan, int jiao, int cent)
{
    if (yuan >= 0 && jiao >= 0 && cent >= 0 || yuan <= 0 && jiao <= 0 && cent <= 0)
    {
        if (jiao < -9 || jiao > 9)
            return NULL;
        if (cent < -9 || cent > 9)
            return NULL;
        Amount *result = malloc(sizeof(Amount));
        result->value = yuan * 100 + jiao + cent;
        return result;
    }
    return NULL;
}

void FreeAmount(Amount *amount)
{
    free(amount);
}
