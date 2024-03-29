#include "design/order.h"
#include "design/utils.h"
#include "../config.h"
#include "design/amount.h"
#include "design/linkedList.h"
#include "design/serialization.h"
#include "design/table.h"
#include "design/time.h"
#include <malloc.h>
#include <stdio.h>

static int idCount = 0;
static const char *fileName = "data" PATH_SEPARATOR_STRING "order.txt";
static const char *idRow = "id";
static const char *inventoryIdRow = "inventoryId";
static const char *numberRow = "number";
static const char *customerIdRow = "customerId";
static const char *timeRow = "time";
static const char *amountRow = "amount";
static int fetched = 0;
static LinkedList *systemList = NULL;

struct Order
{
    int id;
    int inventoryId;
    int number;
    int customerId;
    Time time;
    Amount amount;
};

Order *NewOrder(int inventoryId, int number, int customerId, Time *time, Amount *amount)
{
    if (inventoryId < 0 || number < 0 || customerId < 0 || amount->value < 0)
        return NULL;

    Order *order = malloc(sizeof(Order));
    order->id = GenerateId(systemList, GetAllOrders, &idCount, fetched);
    order->inventoryId = inventoryId;
    order->number = number;
    order->customerId = customerId;
    order->time = *time;
    order->amount = *amount;
    return order;
}

void FreeOrder(Order *order)
{
    free(order);
}

LinkedList *GetAllOrders()
{
    if (fetched == 1)
        return systemList;

    fetched = 1;

    Table *table;
    int result = Unserialize(&table, fileName);
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

        Order *order = malloc(sizeof(Order));

        sscanf(GetRowItemByColumnName(table, row, idRow), "%d", &order->id);
        sscanf(GetRowItemByColumnName(table, row, inventoryIdRow), "%d", &order->inventoryId);
        sscanf(GetRowItemByColumnName(table, row, numberRow), "%d", &order->number);
        sscanf(GetRowItemByColumnName(table, row, customerIdRow), "%d", &order->customerId);
        sscanf(GetRowItemByColumnName(table, row, timeRow), "%lld", &order->time.value);
        sscanf(GetRowItemByColumnName(table, row, amountRow), "%lld", &order->amount.value);

        list = AppendData(list, order);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

Order *GetOrderById(int id)
{
    if (fetched == 0)
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
    if (fetched == 0)
        GetAllOrders();

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL)
    {
        Order *order = now->data;
        if (order->customerId == customerId)
        {
            list = AppendData(list, order);
        }
        now = now->next;
    }

    return list;
}

LinkedList *GetOrdersByInventoryId(int inventoryId)
{
    if (fetched == 0)
        GetAllOrders();

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL)
    {
        Order *order = now->data;
        if (order->inventoryId == inventoryId)
        {
            list = AppendData(list, order);
        }
        now = now->next;
    }

    return list;
}

int GetOrderId(const Order *order)
{
    return order->id;
}

int GetOrderInventoryId(const Order *order)
{
    return order->inventoryId;
}

int GetOrderCustomerId(const Order *order)
{
    return order->customerId;
}

int GetOrderNumber(const Order *order)
{
    return order->number;
}

Time GetOrderTime(const Order *order)
{
    return order->time;
}

Amount GetOrderAmount(const Order *order)
{
    return order->amount;
}

void SetOrderInventoryId(Order *order, int value)
{
    order->inventoryId = value;
}

void SetOrderCustomerId(Order *order, int value)
{
    order->customerId = value;
}

void SetOrderNumber(Order *order, int value)
{
    order->number = value;
}

void SetOrderTime(Order *order, const Time *value)
{
    order->time = *value;
}

void SetOrderAmount(Order *order, Amount *value)
{
    order->amount = *value;
}

int AppendOrder(Order *order)
{
    if (fetched == 0)
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

    systemList = AppendData(systemList, order);
    return 0;
}

void RemoveOrder(Order *order)
{
    systemList = RemoveNode(systemList, order);
}

void OrderSave()
{
    TableRow *row = NewTableRow();
    free(AppendTableRow(row, CloneString(idRow)));
    free(AppendTableRow(row, CloneString(inventoryIdRow)));
    free(AppendTableRow(row, CloneString(numberRow)));
    free(AppendTableRow(row, CloneString(customerIdRow)));
    free(AppendTableRow(row, CloneString(timeRow)));
    free(AppendTableRow(row, CloneString(amountRow)));

    char *remark = LongLongToString(idCount);
    Table *table = NewTable(row, remark);
    free(remark);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        Order *order = now->data;
        row = NewTableRow();

        free(AppendTableRow(row, LongLongToString(order->id)));
        free(AppendTableRow(row, LongLongToString(order->inventoryId)));
        free(AppendTableRow(row, LongLongToString(order->number)));
        free(AppendTableRow(row, LongLongToString(order->customerId)));
        free(AppendTableRow(row, LongLongToString(order->time.value)));
        free(AppendTableRow(row, LongLongToString(order->amount.value)));

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, fileName);
    FreeTable(table);
}
