/*
 * FileName: data/amount.c
 * Author: 陈若珂
 * Description: 包含与金额相关的数据表示和操作的实现
 * */

#include "amount.h"
#include <limits.h>
#include <malloc.h>

static const Amount zero = {0};

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

// 判断value是否可无溢出地被int表示
int IsValueValid(long long value)
{
    return value >= INT_MIN && value <= INT_MAX;
}

Amount AmountAdd(const Amount *amountA, const Amount *amountB)
{
    long long resultValue = amountA->value + amountB->value;
    if (IsValueValid(resultValue / 100))
    {
        Amount amount = {resultValue};
        return amount;
    }
    return zero;
}

Amount AmountMultiply(const Amount *amount, int multiple)
{
    long long resultValue = amount->value * multiple;
    if (IsValueValid(resultValue / 100))
    {
        Amount amount = {resultValue};
        return amount;
    }
    return zero;
}

Amount AmountMultiplyRatio(const Amount *amount, int ratio)
{
    if (ratio > 100 || ratio < 0)
        return zero;
    long long resultMul100 = amount->value * ratio;

    // 舍入判断指标
    int bias = (resultMul100 % 100) / 10;
    long long result = resultMul100 / 100;
    Amount resultAmount;
    if (-4 <= bias && bias <= 4)
    {
        resultAmount.value = result;
        return resultAmount;
    }
    else
    {
        resultAmount.value = bias > 0 ? result + 1 : result - 1;
        return resultAmount;
    }
}

Amount NewAmount(int yuan, int jiao, int cent)
{
    if ((yuan >= 0 && jiao >= 0 && cent >= 0) || (yuan <= 0 && jiao <= 0 && cent <= 0))
    {
        if (jiao < -9 || jiao > 9)
            return zero;
        if (cent < -9 || cent > 9)
            return zero;
        Amount result = {yuan * 100 + jiao + cent};
        return result;
    }
    return zero;
}
