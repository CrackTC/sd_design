#include "design/utils.h"
#include "config.h"
#include "design/amount.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

int IntegerStringLength(long long x)
{
    return snprintf(NULL, 0, "%lld", x);
}

inline int abs(int x)
{
    return x < 0 ? -x : x;
}

char *AmountToString(const Amount *amount)
{
    int yuan = GetAmountYuan(amount);
    int jiao = GetAmountJiao(amount);
    int cent = GetAmountCent(amount);

    int len = IntegerStringLength(yuan) + strlen(".00元") + 1;

    char *result = malloc(len);
    sprintf(result, "%d.%1d%1d元", yuan, abs(jiao), abs(cent));
    return result;
}

char *LongLongToString(long long x)
{
    int length = IntegerStringLength(x);
    char *str = malloc(length + 1);
    snprintf(str, length + 1, "%lld", x);
    return str;
}

char *CloneString(const char *str)
{
    if (str == NULL)
    {
        return NULL;
    }
    int length = strlen(str);
    char *result = malloc((length + 1) * sizeof(char));
    strcpy(result, str);
    return result;
}

char *GetDirectory(const char *path)
{
    if (path == NULL)
    {
        return NULL;
    }
    int length = strlen(path);
    for (int i = length - 1; i >= 0; i--)
    {
        if (path[i] == PATH_SEPARATOR)
        {
            char *result = malloc(i * sizeof(char) + 1);
            strncpy(result, path, i * sizeof(char));
            result[i * sizeof(char)] = '\0';
            return result;
        }
    }
    return NULL;
}

char *JoinPath(const char *pathA, const char *pathB)
{
    if (pathA == NULL || pathB == NULL)
    {
        return NULL;
    }

    int lenA = strlen(pathA);
    int lenB = strlen(pathB);

    char *result = malloc((lenA + 1 + lenB + 1) * sizeof(char));
    strcpy(result, pathA);
    result[lenA] = PATH_SEPARATOR;
    strcpy(result + lenA + 1, pathB);

    return result;
}

int GenerateId(void *systemList, LinkedList *(*initialize)(), int *idCount, int fetched)
{
    if (fetched == 0)
    {
        initialize();
    }
    return (*idCount)++;
}

int IsValidDate(int y, int m, int d, int h, int min, int s)
{
    static int day[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (h < 0 || h >= 24 || min < 0 || min >= 60 || s < 0 || s >= 60)
    {
        return 0;
    }
    if (y < 1 || m < 1 || m > 12 || d < 1)
    {
        return 0;
    }

    if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)
    {
        day[2] = 29;
    }
    else
    {
        day[2] = 28;
    }
    if (d <= day[m]) return 1;
    return 0;
}