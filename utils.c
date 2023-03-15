#include "utils.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *LongLongToString(long long x)
{
    int length = snprintf(NULL, 0, "%lld", x);
    char *str = malloc(length + 1);
    snprintf(str, length + 1, "%lld", x);
    return str;
}

char *CloneString(const char *str)
{
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
