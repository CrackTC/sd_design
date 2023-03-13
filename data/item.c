#include "item.h"
#include "../utils.h"
#include "amount.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <limits.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int idCount = 0;
static const char *path = "data/inventory.txt";
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

int GenerateId()
{
    if (systemList == NULL)
        GetAllItems();
    return idCount++;
}

Item *NewItem(const char *name, Amount *price, Time *shelfLife)
{
    if (name == NULL || price->value < 0)
        return NULL;

    Item *item = malloc(sizeof(Item));
    item->id = GenerateId();
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
    if (result == 1) {
        ItemsSave();
        return NULL;
    }
    else if (result != 0)
        return NULL;

    sscanf("%d", GetTableRemark(table), &idCount);

    LinkedList *list = NULL;
    LinkedList *rowNode = table->rows;
    while (rowNode->next != NULL) {
        rowNode = rowNode->next;
        const TableRow *row = rowNode->data;

        int id;
        char *name;
        Amount price;
        Time shelfLife;

        sscanf("%d", GetRowItemByColumnName(table, row, idRow), &id);
        name = CloneString(GetRowItemByColumnName(table, row, nameRow));
        sscanf("%lld", GetRowItemByColumnName(table, row, priceRow), &price.value);
        sscanf("%d", GetRowItemByColumnName(table, row, shelfLifeRow), &shelfLife.value);

        Item *item = malloc(sizeof(Item));
        item->id = id;
        item->name = name;
        item->price = price;
        item->shelfLife = shelfLife;

        LinkedList *node = malloc(sizeof(LinkedList));
        node->data = item;
        node->next = NULL;

        list = AppendNode(list, node);
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
    while (now != NULL) {
        Item *item = now->data;
        if (item->id == id) {
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
    while (now != NULL) {
        Item *item = now->data;
        if (strcmp(name, item->name)) {
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

const char *GetItemName(const Item *item)
{
    return item->name;
}

Amount GetItemPrice(const Item *item)
{
    return item->price;
}

Time GetItemShelfLife(const Item *item)
{
    return item->shelfLife;;
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
    if (systemList == NULL) {
        GetAllItems();
    }
    if (item == NULL) {
        return 1;
    }
    if (ExistsNode(systemList, item)) {
        return 1;
    }

    LinkedList *node = malloc(sizeof(LinkedList));
    node->data = item;
    node->next = NULL;
    systemList = AppendNode(systemList, node);

    return 0;
}

int RemoveItem(Item *item)
{
    LinkedList *now = systemList;
    while (now != NULL) {
        if (now->data == item) {
            systemList = RemoveNode(systemList, now);
            return 0;
        }
        now = now->next;
    }
    return 1;
}

void ItemsSave()
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, idRow);
    AppendTableRow(row, nameRow);
    AppendTableRow(row, priceRow);
    AppendTableRow(row, shelfLifeRow);

    char *remark = LongLongToString(idCount);
    Table *table = NewTable(row, remark);
    free(remark);

    LinkedList *now = systemList;
    while (now != NULL) {
        Item *item = now->data;
        row = NewTableRow();

        char *idString = LongLongToString(item->id);
        char *priceString = LongLongToString(item->price.value);
        char *shelfLifeString = LongLongToString(item->shelfLife.value);

        AppendTableRow(row, idString);
        AppendTableRow(row, item->name);
        AppendTableRow(row, priceString);
        AppendTableRow(row, shelfLifeString);

        free(idString);
        free(priceString);
        free(shelfLifeString);

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
