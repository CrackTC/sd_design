#include "customer.h"
#include "../utils.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

static int idCount = 0;
static const char *path = "data/customer.txt";
static const char *idRow = "id";
static const char *levelRow = "level";
static const char *nameRow = "name";
static const char *contactRow = "contact";
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
    customer->id = GenerateId(systemList, GetAllCustomers, &idCount);
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
    if (systemList != NULL)
        return systemList;

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

        int id;
        int level;
        char *name;
        char *contact;

        sscanf(GetRowItemByColumnName(table, row, idRow), "%d", &id);
        sscanf(GetRowItemByColumnName(table, row, levelRow), "%d", &level);
        name = CloneString(GetRowItemByColumnName(table, row, nameRow));
        contact = CloneString(GetRowItemByColumnName(table, row, contactRow));

        Customer *customer = malloc(sizeof(Customer));
        customer->id = id;
        customer->level = level;
        customer->name = name;
        customer->contact = contact;

        LinkedList *node = malloc(sizeof(LinkedList));
        node->data = customer;
        node->next = NULL;

        list = AppendNode(list, node);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

Customer *GetCustomerById(int id)
{
    if (systemList == NULL)
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
    if (systemList == NULL)
        GetAllCustomers();

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL)
    {
        Customer *customer = now->data;
        if (customer->level == level)
        {
            LinkedList *node = malloc(sizeof(LinkedList));
            node->data = customer;
            node->next = NULL;
            list = AppendNode(list, node);
        }
        now = now->next;
    }

    return list;
}

LinkedList *GetCustomersByName(const char *name)
{
    if (systemList == NULL)
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
            LinkedList *node = malloc(sizeof(LinkedList));
            node->data = customer;
            node->next = NULL;
            list = AppendNode(list, node);
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
    if (systemList == NULL)
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

    LinkedList *node = malloc(sizeof(LinkedList));
    node->data = customer;
    node->next = NULL;
    systemList = AppendNode(systemList, node);

    return 0;
}

int RemoveCustomer(Customer *customer)
{
    LinkedList *now = systemList;
    while (now != NULL)
    {
        if (now->data == customer)
        {
            systemList = RemoveNode(systemList, now);
            return 0;
        }
        now = now->next;
    }

    return 1;
}

void CustomerSave()
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, idRow);
    AppendTableRow(row, levelRow);
    AppendTableRow(row, nameRow);
    AppendTableRow(row, contactRow);

    char *remark = LongLongToString(idCount);
    Table *table = NewTable(row, remark);
    free(remark);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        Customer *customer = now->data;
        row = NewTableRow();

        char *idString = LongLongToString(customer->id);
        char *levelString = LongLongToString(customer->level);

        AppendTableRow(row, idString);
        AppendTableRow(row, levelString);
        AppendTableRow(row, customer->name);
        AppendTableRow(row, customer->contact);

        free(idString);
        free(levelString);

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
