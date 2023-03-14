#include "utils.h"
#include <malloc.h>
#include <stdio.h>
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
