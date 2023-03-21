#include "item.h"
#include "../utils.h"
#include "amount.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

static int idCount = 0;
static const char *path = "data/item.txt";
static const char *idRow = "id";
static const char *nameRow = "name";
static const char *priceRow = "price";
static const char *shelfLifeRow = "shelfLife";
static LinkedList *systemList;

struct Item
{
    int id;
    char *name;
    Amount price;
    Time shelfLife;
};

Item *NewItem(const char *name, Amount *price, Time *shelfLife)
{
    if (name == NULL || price->value < 0)
        return NULL;

    Item *item = malloc(sizeof(Item));
    item->id = GenerateId(systemList, GetAllItems, &idCount);
    item->name = CloneString(name);
    item->shelfLife = *shelfLife;
    return item;
}

void FreeItem(Item *item)
{
    free(item->name);
    free(item);
}

LinkedList *GetAllItems()
{
    if (systemList != NULL)
        return systemList;

    Table *table;
    int result = Unserialize(&table, path);
    if (result == 1)
    {
        ItemsSave();
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

        Item *item = malloc(sizeof(Item));

        sscanf(GetRowItemByColumnName(table, row, idRow), "%d", &item->id);
        item->name = CloneString(GetRowItemByColumnName(table, row, nameRow));
        sscanf(GetRowItemByColumnName(table, row, priceRow), "%lld", &item->price.value);
        sscanf(GetRowItemByColumnName(table, row, shelfLifeRow), "%ld", &item->shelfLife.value);

        list = AppendData(list, item);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

Item *GetItemById(int id)
{
    if (systemList == NULL)
        GetAllItems();

    LinkedList *now = systemList;
    while (now != NULL)
    {
        Item *item = now->data;
        if (item->id == id)
        {
            return item;
        }
        now = now->next;
    }
    return NULL;
}

Item *GetItemByName(const char *name)
{
    if (systemList == NULL)
        GetAllItems();

    LinkedList *now = systemList;
    while (now != NULL)
    {
        Item *item = now->data;
        if (strcmp(name, item->name) != 0 == 0)
        {
            return item;
        }
        now = now->next;
    }
    return NULL;
}

int GetItemId(const Item *item)
{
    return item->id;
}

char *GetItemName(const Item *item)
{
    return item->name;
}

Amount GetItemPrice(const Item *item)
{
    return item->price;
}

Time GetItemShelfLife(const Item *item)
{
    return item->shelfLife;
}

void SetItemName(Item *item, const char *value)
{
    free(item->name);
    item->name = CloneString(value);
}

void SetItemPrice(Item *item, Amount *value)
{
    item->price = *value;
}

void SetItemShelfLife(Item *item, Time *value)
{
    item->shelfLife = *value;
}

int AppendItem(Item *item)
{
    if (systemList == NULL)
    {
        GetAllItems();
    }
    if (item == NULL)
    {
        return 1;
    }
    if (ExistsNode(systemList, item))
    {
        return 1;
    }

    systemList = AppendData(systemList, item);
    return 0;
}

void RemoveItem(Item *item)
{
    systemList = RemoveNode(systemList, item);
}

void ItemsSave()
{
    TableRow *row = NewTableRow();
    free(AppendTableRow(row, CloneString(idRow)));
    free(AppendTableRow(row, CloneString(nameRow)));
    free(AppendTableRow(row, CloneString(priceRow)));
    free(AppendTableRow(row, CloneString(shelfLifeRow)));

    char *remark = LongLongToString(idCount);
    Table *table = NewTable(row, remark);
    free(remark);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        Item *item = now->data;
        row = NewTableRow();

        free(AppendTableRow(row, LongLongToString(item->id)));
        AppendTableRow(row, item->name);
        free(AppendTableRow(row, LongLongToString(item->price.value)));
        free(AppendTableRow(row, LongLongToString(item->shelfLife.value)));

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
