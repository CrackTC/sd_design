#include "operation.h"

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
