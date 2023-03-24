#include "basicDiscount.h"
#include "../utils.h"
#include "../config.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <malloc.h>
#include <stdio.h>

static int idCount = 0;
static const char *fileName = "data" PATH_SEPARATOR_STRING "basicDiscount.txt";
static const char *idRow = "id";
static const char *itemIdRow = "itemId";
static const char *ratioRow = "ratio";
static const char *customerLevelRow = "customerLevel";
static const char *deadlineRow = "deadline";
static int fetched = 0;
static LinkedList *systemList = NULL;

struct BasicDiscount
{
    int id;
    int itemId;
    int ratio;
    int customerLevel;
    Time deadline;
};

BasicDiscount *NewBasicDiscount(int itemId, int ratio, int customerLevel, Time *deadline)
{
    if (itemId < 0 || ratio < 0 || ratio > 100 || customerLevel < 0 || deadline == NULL)
        return NULL;

    BasicDiscount *discount = malloc(sizeof(BasicDiscount));
    discount->id = GenerateId(systemList, GetAllBasicDiscounts, &idCount, fetched);
    discount->itemId = itemId;
    discount->ratio = ratio;
    discount->customerLevel = customerLevel;
    discount->deadline = *deadline;
    return discount;
}

void FreeBasicDiscount(BasicDiscount *discount)
{
    free(discount);
}

LinkedList *GetAllBasicDiscounts()
{
    if (fetched == 1)
        return systemList;

    fetched = 1;

    Table *table;
    int result = Unserialize(&table, fileName);
    if (result == 1)
    {
        BasicDiscountSave();
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

        BasicDiscount *discount = malloc(sizeof(BasicDiscount));

        sscanf(GetRowItemByColumnName(table, row, idRow), "%d", &discount->id);
        sscanf(GetRowItemByColumnName(table, row, itemIdRow), "%d", &discount->itemId);
        sscanf(GetRowItemByColumnName(table, row, ratioRow), "%d", &discount->ratio);
        sscanf(GetRowItemByColumnName(table, row, customerLevelRow), "%d", &discount->customerLevel);
        sscanf("%d", GetRowItemByColumnName(table, row, deadlineRow), &discount->deadline.value);

        list = AppendData(list, discount);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

BasicDiscount *GetBasicDiscountById(int id)
{
    if (fetched == 0)
    {
        GetAllBasicDiscounts();
    }

    LinkedList *now = systemList;
    while (now != NULL)
    {
        BasicDiscount *discount = now->data;
        if (discount->id == id)
        {
            return discount;
        }
    }
    return NULL;
}

LinkedList *GetBasicDiscountsByItemId(int itemId)
{
    if (fetched == 0)
        GetAllBasicDiscounts();

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL)
    {
        BasicDiscount *discount = now->data;
        if (discount->itemId == itemId)
        {
            list = AppendData(list, discount);
        }
        now = now->next;
    }

    return list;
}

LinkedList *GetBasicDiscountsByCustomerLevel(int level)
{
    if (fetched == 0)
        GetAllBasicDiscounts();

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL)
    {
        BasicDiscount *discount = now->data;
        if (discount->customerLevel == level)
        {
            list = AppendData(list, discount);
        }
        now = now->next;
    }

    return list;
}

int GetBasicDiscountId(const BasicDiscount *discount)
{
    return discount->id;
}

int GetBasicDiscountItemId(const BasicDiscount *discount)
{
    return discount->itemId;
}

int GetBasicDiscountRatio(const BasicDiscount *discount)
{
    return discount->ratio;
}

int GetBasicDiscountCustomerLevel(const BasicDiscount *discount)
{
    return discount->customerLevel;
}

Time GetBasicDiscountDeadline(const BasicDiscount *discount)
{
    return discount->deadline;
}

void SetBasicDiscountItemId(BasicDiscount *discount, int value)
{
    discount->itemId = value;
}

void SetBasicDiscountRatio(BasicDiscount *discount, int value)
{
    discount->ratio = value;
}

void SetBasicDiscountCustomerLevel(BasicDiscount *discount, int value)
{
    discount->customerLevel = value;
}

void SetBasicDiscountDeadline(BasicDiscount *discount, const Time *value)
{
    discount->deadline = *value;
}

int AppendBasicDiscount(BasicDiscount *discount)
{
    if (fetched == 0)
    {
        GetAllBasicDiscounts();
    }
    if (discount == NULL)
        return 1;
    if (ExistsNode(systemList, discount))
        return 1;

    systemList = AppendData(systemList, discount);
    return 0;
}

void RemoveBasicDiscount(BasicDiscount *discount)
{
    systemList = RemoveNode(systemList, discount);
}

void BasicDiscountSave()
{
    TableRow *row = NewTableRow();
    free(AppendTableRow(row, CloneString(idRow)));
    free(AppendTableRow(row, CloneString(itemIdRow)));
    free(AppendTableRow(row, CloneString(ratioRow)));
    free(AppendTableRow(row, CloneString(customerLevelRow)));
    free(AppendTableRow(row, CloneString(deadlineRow)));

    Table *table = NewTable(row, NULL);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        BasicDiscount *discount = now->data;
        row = NewTableRow();

        free(AppendTableRow(row, LongLongToString(discount->id)));
        free(AppendTableRow(row, LongLongToString(discount->itemId)));
        free(AppendTableRow(row, LongLongToString(discount->ratio)));
        free(AppendTableRow(row, LongLongToString(discount->customerLevel)));
        free(AppendTableRow(row, LongLongToString(discount->deadline.value)));

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, fileName);
    FreeTable(table);
}
