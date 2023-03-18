#include "operation.h"
#include <string.h>

static const char *operationStrings[] = {
    "Login",
    "Add Staff",
    "Delete Staff",
    "Update Staff",
    "Read Staff",
    "Read Journal",
    "Read Statistics",
    "Add Inventory",
    "Delete Inventory",
    "Update Inventory",
    "Read Inventory",
    "Add Item",
    "Delete Item",
    "Update Item",
    "Read Item",
    "Add Customer",
    "Delete Customer",
    "Update Customer",
    "Read Customer",
    "Add Order",
    "Delete Order",
    "Update Order",
    "Read Order",
    "Add Discount",
    "Delete Discount",
    "Update Discount",
    "Read Discount",
    "Add Refund",
    "Delete Refund",
    "Update Refund",
    "Read Refund"
};

const char *OperationToString(Operation operation)
{
    return operationStrings[operation];
}

Operation StringToOperation(const char *string)
{
    for (int i = 0; i < OPERATION_COUNT; i++)
    {
        if (strcmp(string, operationStrings[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}
