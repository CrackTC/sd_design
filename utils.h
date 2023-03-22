/*
 * FileName: utils.h
 * Author: 陈若珂
 * Description: 包含一些实用函数的声明
 * */

#ifndef UTILS_H
#define UTILS_H

#include "data/linkedList.h"
#include <stdlib.h>
#include <string.h>

/*
 * Name: IntToString
 * Description: 将整数转换为对应的字符串表示
 * Arguments:
 *     x: 要转换为字符串的整数
 * Returns: 整数的字符串表示
 * Remarks: 返回的字符串使用完毕后需手动释放空间
 * */
char *LongLongToString(long long x);

/*
 * Name: CloneString
 * Description: 创建一个字符串拷贝
 * Arguments:
 *     str: 要拷贝的字符串
 * Returns: 一个指针，指向拷贝结果
 * */
char *CloneString(const char *str);

/*
 * Name: GetDirectory
 * Description: 获取文件路径的目录部分
 * Arguments:
 *     path: 文件路径
 * Returns: 路径对应的目录部分，若path为NULL或目录部分不存在，返回NULL
 * */
char *GetDirectory(const char *path);

/*
 * Name: JoinPath
 * Description: 连接两个路径
 * Arguments:
 *      pathA: 第一个路径
 *      pathB: 第二个路径
 * Returns: 连接后的路径，若pathA或pathB为NULL，返回NULL
 * */
char *JoinPath(const char *pathA, const char *pathB);

/*
 * Name: GenerateId
 * Description: 生成递增id
 * Arguments:
 *     systemList: 系统表格，用于判断是否已初始化idCount
 *     initialize: 指向初始化函数的指针
 *     idCount: id计数器
 *     fetched: 是否已访问过文件
 * Returns: 生成的id
 * */
int GenerateId(void *systemList, LinkedList *(*initialize)(), int *idCount, int fetched);

#endif
