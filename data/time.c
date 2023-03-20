#include "time.h"
#include "../utils.h"
#include <stdio.h>
#include <time.h>

Time AddTime(const Time *baseTime, const Time *timeSpan)
{
    Time result = {baseTime->value + timeSpan->value};
    return result;
}

int CompareTime(const Time *timeA, const Time *timeB)
{
    if (timeA->value > timeB->value)
    {
        return 1;
    }
    if (timeA->value == timeB->value)
    {
        return 0;
    }
    return -1;
}

char *TimeToString(const TimeInfo info)
{
    char *result = malloc(20 * sizeof(char));
    if (info.isSpan)
    {
        sprintf(result, "%02d天%02d小时", info.day, info.hour);
    }
    else
    {
        sprintf(result, "%04d-%02d-%02d %02d:%02d:%02d", info.year, info.month, info.day, info.hour, info.minute,
                info.second);
    }
    return result;
}

Time NewDateTime(int year, int month, int day, int hour, int minute, int second)
{
    struct tm info;
    info.tm_year = year - 1900;
    info.tm_mon = month - 1;
    info.tm_mday = day;
    info.tm_hour = hour;
    info.tm_min = minute;
    info.tm_sec = second;
    info.tm_isdst = -1;

    Time result = {mktime(&info)};
    return result;
}

Time NewTimeSpan(int day, int hour)
{
    time_t result = 0;
    result += day * 24 * 3600;
    result += hour * 3600;

    Time time = {result};
    return time;
}

TimeInfo GetTimeInfo(const Time *time, int isDateTime)
{
    if (isDateTime)
    {
        struct tm *info = localtime(&time->value);
        TimeInfo result = {
            info->tm_year + 1900, info->tm_mon + 1, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec, 0};
        return result;
    }
    else
    {
        TimeInfo result = {0, 0, time->value / 3600 / 24, (time->value % (24 * 3600)) / 3600, 0, 0, 1};
        return result;
    }
}

Time GetSystemTime()
{
    time_t value;
    time(&value);
    Time result = {value};
    return result;
}
