/*
 * FileName: data/amount.h
 * Author: 陈若珂
 * Description: 包含与金额相关的数据表示和操作的声明
 * */

#ifndef AMOUNT_H
#define AMOUNT_H

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
 * Description: 获取指定的Amount的元数
 * Arguments:
 *     amount: 一个指针，指向要获取元数的Amount
 * Returns: 指定的Amount的元数
 * */
int GetAmountYuan(const Amount *amount);

/*
 * Name: GetAmountJiao
 * Description: 获取指定的Amount的角数
 * Arguments:
 *     amount: 一个指针，指向要获取角数的Amount
 * Returns: 指定的Amount的角数
 * */
int GetAmountJiao(const Amount *amount);

/*
 * Name: GetAmountCent
 * Description: 获取指定的Amount的分数
 * Arguments:
 *     amount: 一个指针，指向要获取分数的Amount
 * Returns: 指定的Amount的分数
 * */
int GetAmountCent(const Amount *amount);

/*
 * Name: ParseAmount
 * Description: 获取金额的字符串表示所对应的Amount值
 * Arguments:
 *     amount: 金额的字符串表示
 * Returns: 对应的Amount
 * */
Amount ParseAmount(const char *amount);

/*
 * Name: AmountAdd
 * Description: 获取两个Amount相加结果
 * Arguments:
 *     amountA: 第一个Amount
 *     amountB: 第二个Amount
 * Returns: 一个Amount，表示amountA和amountB相加的结果，
 *          若相加的结果超出表示范围，返回金额0对应的Amount
 * */
Amount AmountAdd(const Amount *amountA, const Amount *amountB);

/*
 * Name: AmountMultiply
 * Description: 获取一个Amount增加至特定倍数的结果
 * Arguments:
 *     amount: 用于相乘的Amount
 *     multiple: 要增加至的倍数
 * Returns: 一个Amount，表示amount同multiple相乘的结果
 * */
Amount AmountMultiply(const Amount *amount, int multiple);

/*
 * Name: AmountMultiplyRatio
 * Desciption: 获取一个Amount对象缩小至特定百分比的结果
 * Arguments:
 *     amount: 用于缩小的Amount对象
 *     ratio: 要缩小至的百分比，范围[0, 100]
 * Returns: 一个Amount对象，表示amount缩小至ratio%的结果
 * */
Amount AmountMultiplyRatio(const Amount *amount, int ratio);

/*
 * Name: NewAmount
 * Description: 使用指定金额信息创建新的Amount
 * Arguments:
 *     yuan: 元
 *     jiao: 角
 *     cent: 分
 * Returns: 若金额信息超出范围，返回金额0对应的Amount
 *          否则，返回创建的Amount
 * */
Amount NewAmount(int yuan, int jiao, int cent);

#endif
