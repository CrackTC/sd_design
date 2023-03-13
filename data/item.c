#include "item.h"
#include "../utils.h"
#include "linkedList.h"
#include <malloc.h>
#include <stdlib.h>

static int idCount = 0;
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

const LinkedList *GetAllItems()
{
}

Item *GetItemById(int id)
{
}

Item *GetItemByName(const char *name)
{
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
}

int RemoveItem(Item *item)
{
}

void ItemsSave()
{
}
