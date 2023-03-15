#include "order.h"
#include "../utils.h"
#include "amount.h"
#include "customer.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <malloc.h>
#include <stdio.h>

static int idCount = 0;
static const char *path = "data/order.txt";
static const char *idRow = "id";
static const char *inventoryIdRow = "inventoryId";
static const char *numberRow = "number";
static const char *customerIdRow = "customerId";
static const char *amountRow = "amount";
static LinkedList *systemList = NULL;

struct Order
{
    int id;
    int inventoryId;
    int number;
    int customerId;
    Amount amount;
};

Order *NewOrder(int inventoryId, int number, int customerId, Amount *amount)
{
    if (inventoryId < 0 || number < 0 || customerId < 0 || amount->value < 0)
        return NULL;

    Order *order = malloc(sizeof(Order));
    order->id = GenerateId(systemList, GetAllOrders, &idCount);
    order->inventoryId = inventoryId;
    order->number = number;
    order->customerId = customerId;
    order->amount = *amount;
    return order;
}

void FreeOrder(Order *order)
{
    free(order);
}

LinkedList *GetAllOrders()
{
    if (systemList != NULL)
        return systemList;

    Table *table;
    int result = Unserialize(&table, path);
    if (result == 1)
    {
        OrderSave();
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
        int inventoryId;
        int number;
        int customerId;
        Amount amount;

        sscanf(GetRowItemByColumnName(table, row, idRow), "%d", &id);
        sscanf(GetRowItemByColumnName(table, row, inventoryIdRow), "%d", &inventoryId);
        sscanf(GetRowItemByColumnName(table, row, numberRow), "%d", &number);
        sscanf(GetRowItemByColumnName(table, row, customerIdRow), "%d", &customerId);
        sscanf(GetRowItemByColumnName(table, row, amountRow), "%lld", &amount.value);

        Order *order = malloc(sizeof(Order));
        order->id = id;
        order->inventoryId = inventoryId;
        order->number = number;
        order->customerId = customerId;
        order->amount = amount;

        LinkedList *node = malloc(sizeof(LinkedList));
        node->data = order;
        node->next = NULL;

        list = AppendNode(list, node);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

Order *GetOrderById(int id)
{
    if (systemList == NULL)
        GetAllOrders();

    LinkedList *now = systemList;
    while (now != NULL)
    {
        Order *order = now->data;
        if (order->id == id)
        {
            return order;
        }
        now = now->next;
    }
    return NULL;
}

LinkedList *GetOrdersByCustomerId(int customerId)
{
    if (systemList == NULL)
        GetAllOrders();

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL)
    {
        Order *order = now->data;
        if (order->customerId == customerId)
        {
            LinkedList *node = malloc(sizeof(LinkedList));
            node->data = order;
            node->next = NULL;
            list = AppendNode(list, node);
        }
        now = now->next;
    }

    return list;
}

int GetOrderId(const Order *order)
{
    return order->id;
}

int GetOrderCustomerId(const Order *order)
{
    return order->customerId;
}

int GetOrderNumber(const Order *order)
{
    return order->number;
}

Amount GetOrderAmount(const Order *order)
{
    return order->amount;
}

void SetOrderCustomerId(Order *order, int value)
{
    order->customerId = value;
}

void SetOrderNumber(Order *order, int value)
{
    order->number = value;
}

void SetOrderAmount(Order *order, Amount *value)
{
    order->amount = *value;
}

int AppendOrder(Order *order)
{
    if (systemList == NULL)
    {
        GetAllOrders();
    }
    if (order == NULL)
    {
        return 1;
    }
    if (ExistsNode(systemList, order))
    {
        return 1;
    }

    LinkedList *node = malloc(sizeof(LinkedList));
    node->data = order;
    node->next = NULL;
    systemList = AppendNode(systemList, node);

    return 0;
}

int RemoveOrder(Order *order)
{
    LinkedList *now = systemList;
    while (now != NULL)
    {
        if (now->data == order)
        {
            systemList = RemoveNode(systemList, now);
            return 0;
        }
        now = now->next;
    }

    return 1;
}

void OrderSave()
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, idRow);
    AppendTableRow(row, inventoryIdRow);
    AppendTableRow(row, numberRow);
    AppendTableRow(row, customerIdRow);
    AppendTableRow(row, amountRow);

    char *remark = LongLongToString(idCount);
    Table *table = NewTable(row, remark);
    free(remark);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        Order *order = now->data;
        row = NewTableRow();

        char *idString = LongLongToString(order->id);
        char *inventoryIdString = LongLongToString(order->inventoryId);
        char *numberString = LongLongToString(order->number);
        char *customerIdString = LongLongToString(order->customerId);
        char *amountString = LongLongToString(order->amount.value);

        AppendTableRow(row, idString);
        AppendTableRow(row, inventoryIdString);
        AppendTableRow(row, numberString);
        AppendTableRow(row, customerIdString);
        AppendTableRow(row, amountString);

        free(idString);
        free(inventoryIdString);
        free(numberString);
        free(customerIdString);
        free(amountString);

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
