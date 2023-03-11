/*
 * FileName: data/serialization.h
 * Author: 陈若珂
 * Description: 提供对数据（反）序列化相关操作的支持
 * */

#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include "table.h"

/*
 * Name: Serialize
 * Description: 对表格进行序列化
 * Arguments:
 *     source: 指向要进行序列化的表格的指针
 *     path: 文件路径
 * Returns: 若序列化成功，返回0
 *          否则，返回-1
 * */
int Serialize(const Table *table, const char *path);

/*
 * Name: Unserialize
 * Description: 对表格进行反序列化
 * Arguments:
 *     destination: 指向Table指针的指针，用于返回反序列化得到的Table
 *     path: 文件路径
 * Returns: 若反序列化成功，返回0
 *          若文件不存在，返回1
 *          否则，返回-1
 * */
int Unserialize(Table **destination, const char *path);

#endif
