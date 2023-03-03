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
struct Amount;

/*
 * Name: AmountToString
 * Description: 获取Amount对象的字符串表示
 * Arguments:
 *     destination: 用于存放结果的字符串
 *     maxSize: 指示最多能向destination中写入的字符数（包含'\0'）
 *     amount: 要获取字符串表示的Amount对象
 * Returns: 若结果字符串总字符数（包含'\0'）不大于maxSize，返回不包含'\0'的总字符数
 *          否则，返回0
 * */
size_t AmountToString(char *destination, size_t maxSize, Amount amount);

/*
 * Name: AmountAdd
 * Description: 获取两个Amount对象相加结果
 * Arguments:
 *     amountA: 第一个Amount对象
 *     amountB: 第二个Amount对象
 * Returns: 一个Amount对象，表示amountA和amountB相加的结果
 * */
Amount AmountAdd(Amount amountA, Amount amountB);

/*
 * Name: AmountMultiply
 * Description: 获取一个Amount对象增加至特定倍数的结果
 * Arguments:
 *     amount: 用于相乘的Amount对象
 *     multiple: 要增加至的倍数
 * Returns: 一个Amount对象，表示amount同multiple相乘的结果
 * */
Amount AmountMultiply(Amount amount, int multiple);

/*
 * Name: AmountMultiplyRatio
 * Desciption: 获取一个Amount对象缩小至特定百分比的结果
 * Arguments:
 *     amount: 用于缩小的Amount对象
 *     ratio: 要缩小至的百分比，范围[0, 100]
 * Returns: 一个Amount对象，表示amount缩小至ratio%的结果
 * */
Amount AmountMultiplyRatio(Amount amount, int ratio);

/*
 * Name: NewAmount
 * Description: 使用指定金额信息创建新的Amount对象
 * Arguments:
 *     destination: 指向用于存放创建结果的Amount对象的指针
 *     yuan: 元
 *     jiao: 角
 *     cent: 分
 * Returns: 若金额信息超出范围，返回1
 *          否则，返回0
 * */
int NewAmount(Amount *destination, int yuan, int jiao, int cent);

#endif
