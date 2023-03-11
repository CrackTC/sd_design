/*
 * FileName: data/amount.h
 * Author: 陈若珂
 * Description: 包含与金额相关的数据表示和操作的声明
 * */

#ifndef AMOUNT_H
#define AMOUNT_H

#include <stddef.h>

/*
 * Name: Amount
 * Description: 表示一个特定的金额
 * */
typedef struct Amount
{
    long long value;
} Amount;

/*
 * Name: GetAmountYuan
 * Description: 获取指定的Amount对象的元数
 * Arguments:
 *     amount: 一个指针，指向要获取元数的Amount对象
 * Returns: 指定的Amount对象的元数
 * */
int GetAmountYuan(const Amount *amount);

/*
 * Name: GetAmountJiao
 * Description: 获取指定的Amount对象的角数
 * Arguments:
 *     amount: 一个指针，指向要获取角数的Amount对象
 * Returns: 指定的Amount对象的角数
 * */
int GetAmountJiao(const Amount *amount);

/*
 * Name: GetAmountCent
 * Description: 获取指定的Amount对象的分数
 * Arguments:
 *     amount: 一个指针，指向要获取分数的Amount对象
 * Returns: 指定的Amount对象的分数
 * */
int GetAmountCent(const Amount *amount);

/*
 * Name: AmountAdd
 * Description: 获取两个Amount对象相加结果
 * Arguments:
 *     amountA: 第一个Amount对象
 *     amountB: 第二个Amount对象
 * Returns: 一个Amount对象，表示amountA和amountB相加的结果，
 *          若相加的结果超出表示范围，返回NULL
 * */
Amount *AmountAdd(const Amount *amountA, const Amount *amountB);

/*
 * Name: AmountMultiply
 * Description: 获取一个Amount对象增加至特定倍数的结果
 * Arguments:
 *     amount: 用于相乘的Amount对象
 *     multiple: 要增加至的倍数
 * Returns: 一个Amount对象，表示amount同multiple相乘的结果
 * */
Amount *AmountMultiply(const Amount *amount, int multiple);

/*
 * Name: AmountMultiplyRatio
 * Desciption: 获取一个Amount对象缩小至特定百分比的结果
 * Arguments:
 *     amount: 用于缩小的Amount对象
 *     ratio: 要缩小至的百分比，范围[0, 100]
 * Returns: 一个Amount对象，表示amount缩小至ratio%的结果
 * */
Amount *AmountMultiplyRatio(const Amount *amount, int ratio);

/*
 * Name: NewAmount
 * Description: 使用指定金额信息创建新的Amount对象
 * Arguments:
 *     yuan: 元
 *     jiao: 角
 *     cent: 分
 * Returns: 若金额信息超出范围，返回NULL
 *          否则，返回指向创建的Amount对象的指针
 * */
Amount *NewAmount(int yuan, int jiao, int cent);

/*
 * Name: FreeAmount
 * Description: 释放指定Amount对象占用的空间
 * Arguments:
 *     amount: 指向需要释放空间的Amount对象的指针
 * */
void FreeAmount(Amount *amount);

#endif
