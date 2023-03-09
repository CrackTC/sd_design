#include "item.h"

struct Item
{
    int id;
    const char *name;
    const Amount *price;
    const Time *shelfLife;
};

Item *NewItem(const char *name, Amount *price, Time *shelfLife)
{

}

void FreeItem(Item *item)
{

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

}

const char *GetItemName(const Item *item)
{

}

Amount *GetItemPrice(const Item *item)
{

}

Time *GetItemShelfLife(const Item *item)
{

}

void SetItemName(Item *item, const char *value)
{

}

void SetItemPrice(Item *item, Amount *value)
{

}

void SetItemShelfLife(Item *item, Time *value)
{

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
