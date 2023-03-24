#include "customer.h"
#include "../utils.h"
#include "../config.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

static int idCount = 0;
static const char *fileName = "data" PATH_SEPARATOR_STRING "customer.txt";
static const char *idRow = "id";
static const char *levelRow = "level";
static const char *nameRow = "name";
static const char *contactRow = "contact";
static int fetched = 0;
static LinkedList *systemList = NULL;

struct Customer
{
    int id;
    int level;
    char *name;
    char *contact;
};

Customer *NewCustomer(int level, char *name, char *contact)
{
    if (level < 0 || name == NULL || contact == NULL)
        return NULL;

    Customer *customer = malloc(sizeof(Customer));
    customer->id = GenerateId(systemList, GetAllCustomers, &idCount, fetched);
    customer->level = level;
    customer->name = CloneString(name);
    customer->contact = CloneString(contact);
    return customer;
}

void FreeCustomer(Customer *customer)
{
    free(customer->name);
    free(customer->contact);
    free(customer);
}

LinkedList *GetAllCustomers()
{
    if (fetched == 1)
        return systemList;

    fetched = 1;

    Table *table;
    int result = Unserialize(&table, path);
    if (result == 1)
    {
        CustomerSave();
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

        Customer *customer = malloc(sizeof(Customer));

        sscanf(GetRowItemByColumnName(table, row, idRow), "%d", &customer->id);
        sscanf(GetRowItemByColumnName(table, row, levelRow), "%d", &customer->level);
        customer->name = CloneString(GetRowItemByColumnName(table, row, nameRow));
        customer->contact = CloneString(GetRowItemByColumnName(table, row, contactRow));

        list = AppendData(list, customer);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

Customer *GetCustomerById(int id)
{
    if (fetched == 0)
        GetAllCustomers();

    LinkedList *now = systemList;
    while (now != NULL)
    {
        Customer *customer = now->data;
        if (customer->id == id)
        {
            return customer;
        }
        now = now->next;
    }
    return NULL;
}

LinkedList *GetCustomersByLevel(int level)
{
    if (fetched == 0)
        GetAllCustomers();

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL)
    {
        Customer *customer = now->data;
        if (customer->level == level)
        {
            list = AppendData(list, customer);
        }
        now = now->next;
    }

    return list;
}

LinkedList *GetCustomersByName(const char *name)
{
    if (fetched == 0)
    {
        GetAllCustomers();
    }

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL)
    {
        Customer *customer = now->data;
        if (strcmp(name, customer->name) == 0)
        {
            list = AppendData(list, customer);
        }
        now = now->next;
    }

    return list;
}

int GetCustomerId(const Customer *customer)
{
    return customer->id;
}

int GetCustomerLevel(const Customer *customer)
{
    return customer->level;
}

const char *GetCustomerName(const Customer *customer)
{
    return customer->name;
}

const char *GetCustomerContact(const Customer *customer)
{
    return customer->contact;
}

void SetCustomerLevel(Customer *customer, int value)
{
    customer->level = value;
}

void SetCustomerName(Customer *customer, const char *name)
{
    free(customer->name);
    customer->name = CloneString(name);
}

void SetCustomerContact(Customer *customer, const char *contact)
{
    free(customer->contact);
    customer->contact = CloneString(contact);
}

int AppendCustomer(Customer *customer)
{
    if (fetched == 0)
    {
        GetAllCustomers();
    }
    if (customer == NULL)
    {
        return 1;
    }
    if (ExistsNode(systemList, customer))
    {
        return 1;
    }

    systemList = AppendData(systemList, customer);
    return 0;
}

void RemoveCustomer(Customer *customer)
{
    systemList = RemoveNode(systemList, customer);
}

void CustomerSave()
{
    TableRow *row = NewTableRow();
    free(AppendTableRow(row, CloneString(idRow)));
    free(AppendTableRow(row, CloneString(levelRow)));
    free(AppendTableRow(row, CloneString(nameRow)));
    free(AppendTableRow(row, CloneString(contactRow)));

    char *remark = LongLongToString(idCount);
    Table *table = NewTable(row, remark);
    free(remark);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        Customer *customer = now->data;
        row = NewTableRow();

        free(AppendTableRow(row, LongLongToString(customer->id)));
        free(AppendTableRow(row, LongLongToString(customer->level)));
        AppendTableRow(row, customer->name);
        AppendTableRow(row, customer->contact);

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
