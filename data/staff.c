#include "staff.h"
#include "../utils.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int idCount = 0;
static const char *path = "data/staff.txt";
static const char *idRow = "id";
static const char *enableRow = "enable";
static const char *nameRow = "name";
static const char *passwordRow = "password";
static const char *contactRow = "contact";
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
    staff->id = GenerateId(systemList, GetAllStaff, &idCount);
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
    if (systemList != NULL)
        return systemList;

    Table *table;
    int result = Unserialize(&table, path);
    if (result == 1)
    {
        // 添加一个默认管理员账户
        Staff *staff = NewStaff(1, "admin", "admin123", "");
        LinkedList *node = malloc(sizeof(LinkedList));
        node->data = staff;
        node->next = NULL;
        systemList = AppendNode(systemList, node);

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

        int id;
        int isEnabled;
        char *name;
        char *password;
        char *contact;

        sscanf(GetRowItemByColumnName(table, row, idRow), "%d", &id);
        sscanf(GetRowItemByColumnName(table, row, enableRow), "%d", &isEnabled);
        name = CloneString(GetRowItemByColumnName(table, row, nameRow));
        password = CloneString(GetRowItemByColumnName(table, row, passwordRow));
        contact = CloneString(GetRowItemByColumnName(table, row, contactRow));

        Staff *staff = malloc(sizeof(Staff));
        staff->id = id;
        staff->isEnabled = isEnabled;
        staff->name = name;
        staff->password = password;
        staff->contact = contact;

        LinkedList *node = malloc(sizeof(LinkedList));
        node->data = staff;
        node->next = NULL;

        list = AppendNode(list, node);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

Staff *GetStaffById(int id)
{
    if (systemList == NULL)
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
    if (systemList == NULL)
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

    LinkedList *node = malloc(sizeof(LinkedList));
    node->data = staff;
    node->next = NULL;
    systemList = AppendNode(systemList, node);

    return 0;
}

int RemoveStaff(Staff *staff)
{
    LinkedList *now = systemList;
    while (now != NULL)
    {
        if (now->data == staff)
        {
            systemList = RemoveNode(systemList, now);
            return 0;
        }
        now = now->next;
    }

    return 1;
}

void StaffSave()
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, idRow);
    AppendTableRow(row, enableRow);
    AppendTableRow(row, nameRow);
    AppendTableRow(row, passwordRow);
    AppendTableRow(row, contactRow);

    char *remark = LongLongToString(idCount);
    Table *table = NewTable(row, remark);
    free(remark);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        Staff *staff = now->data;
        row = NewTableRow();

        char *idString = LongLongToString(staff->id);
        char *enableString = LongLongToString(staff->isEnabled);

        AppendTableRow(row, idString);
        AppendTableRow(row, enableString);
        AppendTableRow(row, staff->name);
        AppendTableRow(row, staff->password);
        AppendTableRow(row, staff->contact);

        free(idString);
        free(enableString);

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
