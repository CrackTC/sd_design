#include "operation.h"
#include <string.h>

static const char *operationStrings[] = {
    "Add Staff",
    "Delete Staff",
    "Update Staff",
    "Read Staff",
    "Read Journal",
    "Read Statistics",
    "Add Inventory",
    "Update Inventory",
    "Read Inventory",
    "Add Customer",
    "Delete Customer",
    "Update Customer",
    "Read Customer",
    "Read Order",
    "Add Discount",
    "Delete Discount",
    "Update Discount",
    "Read Discount",
    "Add Refund",
    "Delete Refund",
    "Update Refund",
    "Read Refund",
};

const char *OperationToString(Operation operation)
{
    return operationStrings[operation];
}

Operation StringToOperation(const char *string)
{
    for (int i = 0; i < OPERATION_COUNT; i++) {
        if (strcmp(string, operationStrings[i]) == 0) {
            return i;
        }
    }
    return -1;
}
