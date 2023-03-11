#include "utils.h"
#include <malloc.h>
#include <stdio.h>

char *IntToString(long long x)
{
    int length = snprintf(NULL, 0, "%lld", x);
    char *str = malloc(length + 1);
    snprintf(str, length + 1, "%lld", x);
    return str;
}
