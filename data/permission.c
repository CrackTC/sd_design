#include "permission.h"
#include "linkedList.h"
#include "operation.h"
#include "serialization.h"
#include "table.h"
#include "../utils.h"
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static const char *path = "data/permission.txt";
static const char *staffIdRow = "staffId";
static const char *permissionRow = "permission";
static LinkedList *systemList = NULL;

struct PermissionEntry
{
    int staffId;
    int *hasPermission;
};

PermissionEntry *NewPermissionEntry(int staffId, const int *hasPermission)
{
    if (staffId < 0 || hasPermission == NULL)
        return NULL;

    for (int i = 0; i < OPERATION_COUNT; i++)
    {
        if (hasPermission[i] < 0 || hasPermission[i] > 1) {
            return NULL;
        }
    }

    PermissionEntry *entry = malloc(sizeof(PermissionEntry));
    entry->staffId = staffId;

    entry->hasPermission = malloc(OPERATION_COUNT * sizeof(int));
    memcpy(entry->hasPermission, hasPermission, OPERATION_COUNT * sizeof(int));

    return entry;
}

void FreePermissionEntry(PermissionEntry *entry)
{
    free(entry->hasPermission);
    free(entry);
}

LinkedList *GetAllPermissionEntry()
{
    if (systemList != NULL)
        return systemList;

    Table *table;
    int result = Unserialize(&table, path);
    if (result == 1) {
        PermissionSave();
        return NULL;
    }
    else if (result != 0)
        return NULL;

    LinkedList *list = NULL;
    LinkedList *rowNode = table->rows;
    while (rowNode->next != NULL)
    {
        rowNode = rowNode->next;
        const TableRow *row = rowNode->data;

        int staffId;
        int *hasPermission;

        sscanf(GetRowItemByColumnName(table, row, staffIdRow), "%d", &staffId);
        char *permissionString = GetRowItemByColumnName(table, row, permissionRow);

        hasPermission = malloc(OPERATION_COUNT * sizeof(int));
        for (int i = 0; i < OPERATION_COUNT; i++)
            hasPermission[i] = permissionString[i] - '0';

        PermissionEntry *entry = malloc(sizeof(PermissionEntry));
        entry->staffId = staffId;
        entry->hasPermission = hasPermission;

        LinkedList *node = malloc(sizeof(LinkedList));
        node->data = entry;
        node->next = NULL;

        list = AppendNode(list, node);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

PermissionEntry *GetPermissionEntryByStaffId(int staffId)
{
    if (systemList == NULL)
        GetAllPermissionEntry();

    LinkedList *now = systemList;
    while (now != NULL) {
        PermissionEntry *entry = now->data;
        if (entry->staffId == staffId) {
            return entry;
        }
    }
    return NULL;
}

int GetPermissionEntryStaffId(const PermissionEntry *entry)
{
    return entry->staffId;
}

const int *GetPermissionEntryPermissions(const PermissionEntry *entry)
{
    return entry->hasPermission;
}

void SetPermissionEntryPermissions(PermissionEntry *entry, Operation operation, int allow)
{
    if ((allow == 0 || allow == 1) && operation >= 0 && operation < OPERATION_COUNT) {
        entry->hasPermission[operation] = allow;
    }
}

int AppendPermissionEntry(PermissionEntry *entry)
{
    if (systemList == NULL) {
        GetAllPermissionEntry();
    }
    if (entry == NULL) {
        return 1;
    }
    if (ExistsNode(systemList, entry)) {
        return 1;
    }

    LinkedList *node = malloc(sizeof(LinkedList));
    node->data = entry;
    node->next = NULL;
    systemList = AppendNode(systemList, node);

    return 0;
}

int RemovePermissionEntry(PermissionEntry *entry)
{
    LinkedList *now = systemList;
    while (now != NULL) {
        if (now->data == entry) {
            systemList = RemoveNode(systemList, now);
            return 0;
        }
        now = now->next;
    }

    return 1;
}

void PermissionSave()
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, staffIdRow);
    AppendTableRow(row, permissionRow);

    Table *table = NewTable(row, NULL);

    LinkedList *now = systemList;
    while (now != NULL) {
        PermissionEntry *entry = now->data;
        row = NewTableRow();

        char *staffIdString = LongLongToString(entry->staffId);
        char *permissionString = malloc((OPERATION_COUNT + 1) * sizeof(char));
        for (int i = 0; i < OPERATION_COUNT; i++)
            permissionString[i] = entry->hasPermission[i] + '0';
        permissionString[OPERATION_COUNT] = '\0';

        AppendTableRow(row, staffIdString);
        AppendTableRow(row, permissionString);

        free(staffIdString);
        free(permissionString);

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
