#include "basicDiscount.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include "../utils.h"
#include <malloc.h>
#include <stdio.h>

static const char *path = "data/basicDiscount.txt";
static const char *itemIdRow = "itemId";
static const char *ratioRow = "ratio";
static const char *customerLevelRow = "customerLevel";
static const char *deadlineRow = "deadline";
static LinkedList *systemList = NULL;

struct BasicDiscount
{
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
    if (systemList != NULL)
        return systemList;

    Table *table;
    int result = Unserialize(&table, path);
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

        int itemId;
        int ratio;
        int customerLevel;
        Time deadline;

        sscanf("%d", GetRowItemByColumnName(table, row, itemIdRow), &itemId);
        sscanf("%d", GetRowItemByColumnName(table, row, ratioRow), &ratio);
        sscanf("%d", GetRowItemByColumnName(table, row, customerLevelRow), &customerLevel);
        sscanf("%d", GetRowItemByColumnName(table, row, deadlineRow), deadline.value);

        BasicDiscount *discount = malloc(sizeof(BasicDiscount));
        discount->itemId = itemId;
        discount->ratio = ratio;
        discount->customerLevel = customerLevel;
        discount->deadline = deadline;

        LinkedList *node = malloc(sizeof(LinkedList));
        node->data = discount;
        node->next = NULL;

        list = AppendNode(list, node);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

LinkedList *GetBasicDiscountsByItemId(int itemId)
{
    if (systemList == NULL)
        GetAllBasicDiscounts();

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL)
    {
        BasicDiscount *discount = now->data;
        if (discount->itemId == itemId)
        {
            LinkedList *node = malloc(sizeof(LinkedList));
            node->data = discount;
            node->next = NULL;
            list = AppendNode(list, node);
        }
        now = now->next;
    }

    return list;
}

LinkedList *GetBasicDiscountsByCustomerLevel(int level)
{
    if (systemList == NULL)
        GetAllBasicDiscounts();

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL)
    {
        BasicDiscount *discount = now->data;
        if (discount->customerLevel == level)
        {
            LinkedList *node = malloc(sizeof(LinkedList));
            node->data = discount;
            node->next = NULL;
            list = AppendNode(list, node);
        }
        now = now->next;
    }

    return list;
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
    if (systemList == NULL) {
        GetAllBasicDiscounts();
    }
    if (discount == NULL)
        return 1;
    if (ExistsNode(systemList, discount))
        return 1;

    LinkedList *node = malloc(sizeof(LinkedList));
    node->data = discount;
    node->next = NULL;
    systemList = AppendNode(systemList, node);

    return 0;
}

int RemoveBasicDiscount(BasicDiscount *discount)
{
    LinkedList *now = systemList;
    while (now != NULL) {
        if (now->data == discount)
        {
            systemList = RemoveNode(systemList, now);
            return 0;
        }
        now = now->next;
    }

    return 1;
}

void BasicDiscountSave()
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, itemIdRow);
    AppendTableRow(row, ratioRow);
    AppendTableRow(row, customerLevelRow);
    AppendTableRow(row, deadlineRow);

    Table *table = NewTable(row, NULL);

    LinkedList *now = systemList;
    while (now != NULL) {
        BasicDiscount *discount = now->data;
        row = NewTableRow();

        char *itemIdString = LongLongToString(discount->itemId);
        char *ratioString = LongLongToString(discount->ratio);
        char *customerLevelString = LongLongToString(discount->customerLevel);
        char *deadlineString = LongLongToString(discount->deadline.value);

        AppendTableRow(row, itemIdString);
        AppendTableRow(row, ratioString);
        AppendTableRow(row, customerLevelString);
        AppendTableRow(row, deadlineString);

        free(itemIdString);
        free(ratioString);
        free(customerLevelString);
        free(deadlineString);

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
