#include "utils.h"
#include "data/amount.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

int IntegerStringLength(long long x)
{
    return snprintf(NULL, 0, "%lld", x);
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
    if (str == NULL) {
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
        if (path[i] == '/')
        {
            char *result = malloc(i * sizeof(char));
            strncpy(result, path, i * sizeof(char));
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
    result[lenA] = '/';
    strcpy(result + lenA + 1, pathB);

    return result;
}

int GenerateId(void *systemList, LinkedList *(*initialize)(), int *idCount, int fetched)
{
    if (fetched == 0) {
        initialize();
    }
    return (*idCount)++;
}
