#include "time.h"
#include "../utils.h"
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

char *TimeToString(const Time *time)
{
    return CloneString(ctime(&time->value));
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

Time NewTimeSpan(int year, int month, int day, int hour, int minute, int second)
{
    struct tm info;
    info.tm_year = year;
    info.tm_mon = month;
    info.tm_mday = day + 1;
    info.tm_hour = hour;
    info.tm_min = minute;
    info.tm_sec = second;
    info.tm_isdst = -1;

    Time result = {mktime(&info)};
    return result;
}

Time GetSystemTime()
{
    time_t value;
    time(&value);
    Time result = {value};
    return result;
}
