/*
 * FileName: data/operation.h
 * Author: 陈若珂
 * Description: 包含时间与时间间隔相关的数据表示和操作的声明
 * */

#ifndef TIME_H
#define TIME_H

#include <stddef.h>

/*
 * Name: Time
 * Description: 表示时间或时间间隔
 * */
typedef struct Time Time;

/*
 * Name: AddTime
 * Description: 获取表示基础时间经过特定时间间隔后所表示的时间的Time对象
 * Arguments:
 *     baseTime: 表示基础时间的Time对象
 *     timeSpan: 表示基础时间经过的时间间隔的Time对象
 * Returns: 一个Time对象，表示baseTime经过timeSpan时间间隔后的时间
 * */
Time AddTime(Time baseTime, Time timeSpan);

/*
 * Name: CompareTime
 * Description: 获取两个Time对象的比较结果
 * Arguments:
 *     timeA: 要比较的第一个Time对象
 *     timeB: 要比较的第二个Time对象
 * Returns: 若timeA等于timeB，返回0
 *          若timeA小于timeB，返回-1
 *          若timeA大于timeB，返回1
 * */
int CompareTime(Time timeA, Time timeB);

/*
 * Name: TimeToString
 * Description: 获取Time对象的字符串表示
 * Arguments:
 *     destination: 用于存放结果的字符串
 *     maxSize: 指示最多能向destination中写入的字符数（包含'\0'）
 *     time: 要获取字符串表示的Time对象
 * Returns: 若结果字符串总字符数（包含'\0'）不大于maxSize，返回不包含'\0'的总字符数
 *          否则，返回0
 * */
size_t TimeToString(char *destination, size_t maxSize, Time time);

/*
 * Name: NewDateTime
 * Description: 使用指定日期时间信息创建新的Time对象
 * Arguments:
 *     destination: 指向用于存放创建结果的Time对象的指针
 *     year: 年
 *     month: 月
 *     day: 日
 *     hour: 小时
 *     minute: 分钟
 *     second: 秒
 * Returns: 若日期时间信息超出范围，返回1
 *          否则，返回0
 * */
int NewDateTime(Time *destination, int year, int month, int day, int hour, int minute, int second);

/*
 * Name: NewTimeSpan
 * Description: 使用指定时间间隔信息创建新的Time对象
 * Arguments:
 *     destination: 指向用于存放创建结果的Time对象的指针
 *     year: 间隔年数
 *     month: 间隔月数
 *     day: 间隔天数
 *     hour: 间隔小时数
 *     minute: 间隔分钟数
 *     second: 间隔秒数
 * Returns: 若时间间隔信息超出范围，返回1
 *          否则，返回0
 * */
int NewTimeSpan(Time *destination, int year, int month ,int day, int hour, int minute, int second);

#endif
