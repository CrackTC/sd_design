/*
 * FileName: utils.h
 * Author: 陈若珂
 * Description: 包含一些实用函数的声明
 * */

#ifndef UTILS_H
#define UTILS_H

/*
 * Name: IntToString
 * Description: 将整数转换为对应的字符串表示
 * Arguments:
 *     x: 要转换为字符串的整数
 * Returns: 整数的字符串表示
 * Remarks: 返回的字符串使用完毕后需手动释放空间
 * */
char *LongLongToString(long long x);

#endif
