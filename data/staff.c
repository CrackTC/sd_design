#include "staff.h"
#include "permission.h"
#include "../utils.h"
#include "../config.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include "operation.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int idCount = 1;
static const char *fileName = "data" PATH_SEPARATOR_STRING "staff.txt";
static const char *idRow = "id";
static const char *enableRow = "enable";
static const char *nameRow = "name";
static const char *passwordRow = "password";
static const char *contactRow = "contact";
static int fetched = 0;
static LinkedList *systemList = NULL;

struct Staff
{
    int id;
    int isEnabled;
    char *name;
    char *password;
    char *contact;
};

Staff *NewStaff(int isEnabled, const char *name, const char *password, const char *contact)
{
    if (name == NULL || password == NULL || contact == NULL)
    {
        return NULL;
    }

    Staff *staff = malloc(sizeof(Staff));
    staff->id = GenerateId(systemList, GetAllStaff, &idCount, fetched);
    staff->isEnabled = isEnabled;
    staff->name = CloneString(name);
    staff->password = CloneString(password);
    staff->contact = CloneString(contact);

    return staff;
}

void FreeStaff(Staff *staff)
{
    free(staff->name);
    free(staff->password);
    free(staff->contact);
    free(staff);
}

LinkedList *GetAllStaff()
{
    if (fetched == 1)
        return systemList;

    fetched = 1;

    Table *table;
    int result = Unserialize(&table, fileName);
    if (result == 1)
    {
        // 添加一个默认管理员账户
        Staff *staff = malloc(sizeof(Staff));
        staff->id = 0;
        staff->isEnabled = 1;
        staff->name = CloneString("admin");
        staff->password = CloneString("admin123");
        staff->contact = CloneString("");

        int *hasPermission = malloc(OPERATION_COUNT * sizeof(int));
        for (int i = 0; i < OPERATION_COUNT; i++)
        {
            hasPermission[i] = 1;
        }
        PermissionEntry *entry = NewPermissionEntry(staff->id, hasPermission);
        AppendPermissionEntry(entry);
        PermissionSave();
        free(hasPermission);

        systemList = AppendData(systemList, staff);

        StaffSave();
        return NULL;
    }
    else if (result != 0)
        return NULL;

    sscanf(GetTableRemark(table), "%d", &idCount);

    LinkedList *list = NULL;
    LinkedList *rowNode = table->rows;
    while (rowNode->next != NULL)
    {
        rowNode = rowNode->next;
        const TableRow *row = rowNode->data;

        Staff *staff = malloc(sizeof(Staff));

        sscanf(GetRowItemByColumnName(table, row, idRow), "%d", &staff->id);
        sscanf(GetRowItemByColumnName(table, row, enableRow), "%d", &staff->isEnabled);
        staff->name = CloneString(GetRowItemByColumnName(table, row, nameRow));
        staff->password = CloneString(GetRowItemByColumnName(table, row, passwordRow));
        staff->contact = CloneString(GetRowItemByColumnName(table, row, contactRow));

        list = AppendData(list, staff);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

Staff *GetStaffById(int id)
{
    if (fetched == 0)
        GetAllStaff();

    LinkedList *now = systemList;
    while (now != NULL)
    {
        Staff *staff = now->data;
        if (staff->id == id)
        {
            return staff;
        }
        now = now->next;
    }
    return NULL;
}

int GetStaffId(const Staff *staff)
{
    return staff->id;
}

int GetStaffAvailability(const Staff *staff)
{
    return staff->isEnabled;
}

const char *GetStaffName(const Staff *staff)
{
    return staff->name;
}

const char *GetStaffContact(const Staff *staff)
{
    return staff->contact;
}

int VerifyStaffPassword(const Staff *staff, const char *givenPassword)
{
    if (staff == NULL || givenPassword == NULL)
    {
        return 0;
    }
    return strcmp(staff->password, givenPassword) == 0;
}

void SetStaffAvailability(Staff *staff, int value)
{
    staff->isEnabled = value;
}

void SetStaffName(Staff *staff, const char *value)
{
    free(staff->name);
    staff->name = CloneString(value);
}

void SetStaffPassword(Staff *staff, const char *value)
{
    free(staff->password);
    staff->password = CloneString(value);
}

void SetStaffContact(Staff *staff, const char *value)
{
    free(staff->contact);
    staff->contact = CloneString(value);
}

int AppendStaff(Staff *staff)
{
    if (fetched == 0)
    {
        GetAllStaff();
    }
    if (staff == NULL)
    {
        return 1;
    }
    if (ExistsNode(systemList, staff))
    {
        return 1;
    }

    systemList = AppendData(systemList, staff);

    return 0;
}

void RemoveStaff(Staff *staff)
{
    staff->isEnabled = 0;
}

void StaffSave()
{
    TableRow *row = NewTableRow();
    free(AppendTableRow(row, CloneString(idRow)));
    free(AppendTableRow(row, CloneString(enableRow)));
    free(AppendTableRow(row, CloneString(nameRow)));
    free(AppendTableRow(row, CloneString(passwordRow)));
    free(AppendTableRow(row, CloneString(contactRow)));

    char *remark = LongLongToString(idCount);
    Table *table = NewTable(row, remark);
    free(remark);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        Staff *staff = now->data;
        row = NewTableRow();

        free(AppendTableRow(row, LongLongToString(staff->id)));
        free(AppendTableRow(row, LongLongToString(staff->isEnabled)));
        AppendTableRow(row, staff->name);
        AppendTableRow(row, staff->password);
        AppendTableRow(row, staff->contact);

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, fileName);
    FreeTable(table);
}
