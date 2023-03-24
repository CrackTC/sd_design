/*
 * FileName: data/operation.h
 * Author: 陈若珂
 * Description: 包含时间与时间间隔相关的数据表示和操作的声明
 * */

#ifndef TIME_H
#define TIME_H

#include <time.h>

/*
 * Name: Time
 * Description: 表示时间或时间间隔
 * */
typedef struct Time
{
    time_t value;
} Time;

typedef struct TimeInfo
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int isSpan;
} TimeInfo;

/*
 * Name: AddTime
 * Description: 获取表示基础时间经过特定时间间隔后所表示的时间的Time
 * Arguments:
 *     baseTime: 表示基础时间的Time
 *     timeSpan: 表示基础时间经过的时间间隔的Time
 * Returns: 一个Time
 * */
Time AddTime(const Time *baseTime, const Time *timeSpan);

/*
 * Name: CompareTime
 * Description: 获取两个Time
 * Arguments:
 *     timeA: 要比较的第一个Time
 *     timeB: 要比较的第二个Time
 * Returns: 若timeA等于timeB，返回0
 *          若timeA小于timeB，返回-1
 *          若timeA大于timeB，返回1
 * */
int CompareTime(const Time *timeA, const Time *timeB);

/*
 * Name: TimeToString
 * Description: 获取TimeInfo的字符串表示
 * Arguments:
 *     info: 要获取字符串表示的TimeInfo
 * Returns: 字符串表示
 * Remarks: 需要在使用完毕后释放返回的字符串
 * */
char *TimeToString(const TimeInfo info);

/*
 * Name: ParseTime
 * Description: 将时间的字符串表示转换为TimeInfo
 * Arguments:
 *     time: 字符串表示
 *     isSpan: 指示传递的时间是否为时间间隔
 * Returns: 一个TimeInfo, 表示对应的时间
 * Return*/
TimeInfo ParseTime(const char *time, int isSpan);

/*
 * Name: NewDateTime
 * Description: 使用指定日期时间信息创建新的Time
 * Arguments:
 *     year: 年
 *     month: 月
 *     day: 日
 *     hour: 小时
 *     minute: 分钟
 *     second: 秒
 * Returns: 若日期时间信息超出范围，返回value为-1的Time
 *          否则，返回所创建的Time
 * */
Time NewDateTime(int year, int month, int day, int hour, int minute, int second);

/*
 * Name: NewTimeSpan
 * Description: 使用指定时间间隔信息创建新的Time
 * Arguments:
 *     year: 间隔年数
 *     month: 间隔月数
 *     day: 间隔天数
 *     hour: 间隔小时数
 *     minute: 间隔分钟数
 *     second: 间隔秒数
 * Returns: 若时间间隔信息超出范围，返回value为-1的Time
 *          否则，返回所创建的Time
 * */
Time NewTimeSpan(int day, int hour);

/*
 * Name: GetTimeInfo
 * Description: 获取指定时间的友好表示
 * Arguments:
 *     time: 指定的时间
 *     isDateTime: 指示传递的时间是否是日期时间
 * Returns: 一个TimeInfo，包含时间相关的信息
 * */
TimeInfo GetTimeInfo(const Time *time, int isDateTime);

/*
 * Name: GetSystemTime
 * Description: 获取当前的系统时间
 * Returns: 表示当前系统时间的Time
 * */
Time GetSystemTime();

#endif
