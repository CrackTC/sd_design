#include "inventory.h"
#include "customer.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <malloc.h>
#include <stdio.h>

static int idCount = 0;
static const char *path = "data/inventory.txt";
static const char *idRow = "id";
static const char *itemIdRow = "itemId";
static const char *numberRow = "number";
static const char *inboundTimeRow = "inboundTime";
static const char *productionTimeRow = "productionTime";
static const char *inboundUnitPriceRow = "inboundUnitPrice";
static LinkedList *systemList = NULL;

struct InventoryEntry
{
    int id;
    int itemId;
    int number;
    Time inboundTime;
    Time productionTime;
    Amount inboundUnitPrice;
};

int GenerateId()
{
    if (systemList == NULL)
        GetAllInventory();
    return idCount++;
}

LinkedList *GetAllInventory()
{
    if (systemList != NULL)
        return systemList;

    Table *table;
    int result = Unserialize(&table, path);
    if (result == 1) {
        InventorySave();
        return NULL;
    }
    else if (result != 0)
        return NULL;

    LinkedList *list = NULL;
    LinkedList *rowNode = table->rows;
    while (rowNode->next != NULL) {
        rowNode = rowNode->next;
        const TableRow *row = rowNode->data;

        int id;
        int itemId;
        int number;
        Time inboundTime;
        Time productionTime;
        Amount inboundUnitPrice;

        sscanf("%d", GetRowItemByColumnName(table, row, idRow), &id);
        sscanf("%d", GetRowItemByColumnName(table, row, itemIdRow), &itemId);
        sscanf("%d", GetRowItemByColumnName(table, row, numberRow), &number);
        sscanf("%d", GetRowItemByColumnName(table, row, inboundTimeRow), &inboundTime.value);
    }
}

InventoryEntry *GetInventoryById(int id)
{
}

LinkedList *GetInventoryByItemId(int itemId)
{
}

InventoryEntry *NewInventoryEntry(int itemId, int number, const Time *inboundTime, const Time *productionTime,
                                  const Amount *inboundUnitPrice)
{
    if (itemId < 0 || number < 0 || inboundUnitPrice < 0)
        return NULL;

    InventoryEntry *entry = malloc(sizeof(InventoryEntry));
    entry->id = GenerateId();
    entry->number = number;
    entry->inboundTime = *inboundTime;
    entry->productionTime = *productionTime;
    entry->inboundUnitPrice = *inboundUnitPrice;
    return entry;
}

void FreeInventoryEntry(InventoryEntry *entry)
{
    free(entry);
}

int GetInventoryEntryId(const InventoryEntry *entry)
{
    return entry->id;
}

int GetInventoryEntryItemId(const InventoryEntry *entry)
{
    return entry->itemId;
}

int GetInventoryEntryNumber(const InventoryEntry *entry)
{
    return entry->number;
}

Time GetInventoryEntryInboundTime(const InventoryEntry *entry)
{
    return entry->inboundTime;
}

Time GetInventoryEntryProductionTime(const InventoryEntry *entry)
{
    return entry->productionTime;
}

Amount GetInventoryEntryInboundUnitPrice(const InventoryEntry *entry)
{
    return entry->inboundUnitPrice;
}

void SetInventoryEntryItemId(InventoryEntry *entry, int value)
{
    entry->itemId = value;
}

void SetInventoryEntryNumber(InventoryEntry *entry, int value)
{
    entry->number = value;
}

void SetInventoryEntryInboundTime(InventoryEntry *entry, const Time *value)
{
    entry->inboundTime = *value;
}

void SetInventoryEntryProductionTime(InventoryEntry *entry, const Time *value)
{
    entry->productionTime = *value;
}

void SetInventoryEntryInboundUnitPrice(InventoryEntry *entry, const Amount *value)
{
    entry->inboundUnitPrice = *value;
}

int AppendInventoryEntry(InventoryEntry *entry)
{
}

int RemoveInventoryEntry(InventoryEntry *entry)
{
}

void InventorySave()
{
}
