#include "inventory.h"
#include "../utils.h"
#include "../config.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <malloc.h>
#include <stdio.h>

static int idCount = 0;
static const char *fileName = "data" PATH_SEPARATOR_STRING "inventory.txt";
static const char *idRow = "id";
static const char *itemIdRow = "itemId";
static const char *numberRow = "number";
static const char *inboundTimeRow = "inboundTime";
static const char *productionTimeRow = "productionTime";
static const char *inboundUnitPriceRow = "inboundUnitPrice";
static int fetched = 0;
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

LinkedList *GetAllInventory()
{
    if (fetched == 1)
        return systemList;

    fetched = 1;

    Table *table;
    int result = Unserialize(&table, path);
    if (result == 1)
    {
        InventorySave();
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

        InventoryEntry *entry = malloc(sizeof(InventoryEntry));

        sscanf(GetRowItemByColumnName(table, row, idRow), "%d", &entry->id);
        sscanf(GetRowItemByColumnName(table, row, itemIdRow), "%d", &entry->itemId);
        sscanf(GetRowItemByColumnName(table, row, numberRow), "%d", &entry->number);
        sscanf(GetRowItemByColumnName(table, row, inboundTimeRow), "%ld", &entry->inboundTime.value);
        sscanf(GetRowItemByColumnName(table, row, productionTimeRow), "%ld", &entry->productionTime.value);
        sscanf(GetRowItemByColumnName(table, row, inboundUnitPriceRow), "%lld", &entry->inboundUnitPrice.value);

        list = AppendData(list, entry);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

InventoryEntry *GetInventoryById(int id)
{
    if (fetched == 0)
        GetAllInventory();

    LinkedList *now = systemList;
    while (now != NULL)
    {
        InventoryEntry *entry = now->data;
        if (entry->id == id)
        {
            return entry;
        }
        now = now->next;
    }
    return NULL;
}

LinkedList *GetInventoryByItemId(int itemId)
{
    if (fetched == 0)
        GetAllInventory();

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL)
    {
        InventoryEntry *entry = now->data;
        if (entry->itemId == itemId)
        {
            list = AppendData(list, entry);
        }
        now = now->next;
    }

    return list;
}

InventoryEntry *NewInventoryEntry(int itemId, int number, const Time *inboundTime, const Time *productionTime,
        const Amount *inboundUnitPrice)
{
    if (itemId < 0 || number < 0 || inboundUnitPrice < 0)
        return NULL;

    InventoryEntry *entry = malloc(sizeof(InventoryEntry));
    entry->id = GenerateId(systemList, GetAllInventory, &idCount, fetched);
    entry->itemId = itemId;
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
    if (fetched == 0)
    {
        GetAllInventory();
    }
    if (entry == NULL)
    {
        return 1;
    }
    if (ExistsNode(systemList, entry))
    {
        return 1;
    }

    LinkedList *node = malloc(sizeof(LinkedList));
    node->data = entry;
    node->next = NULL;
    systemList = AppendData(systemList, entry);

    return 0;
}

void RemoveInventoryEntry(InventoryEntry *entry)
{
    systemList = RemoveNode(systemList, entry);
}

void InventorySave()
{
    TableRow *row = NewTableRow();
    free(AppendTableRow(row, CloneString(idRow)));
    free(AppendTableRow(row, CloneString(itemIdRow)));
    free(AppendTableRow(row, CloneString(numberRow)));
    free(AppendTableRow(row, CloneString(inboundTimeRow)));
    free(AppendTableRow(row, CloneString(productionTimeRow)));
    free(AppendTableRow(row, CloneString(inboundUnitPriceRow)));

    char *remark = LongLongToString(idCount);
    Table *table = NewTable(row, remark);
    free(remark);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        InventoryEntry *entry = now->data;
        row = NewTableRow();

        free(AppendTableRow(row, LongLongToString(entry->id)));
        free(AppendTableRow(row, LongLongToString(entry->itemId)));
        free(AppendTableRow(row, LongLongToString(entry->number)));
        free(AppendTableRow(row, LongLongToString(entry->inboundTime.value)));
        free(AppendTableRow(row, LongLongToString(entry->productionTime.value)));
        free(AppendTableRow(row, LongLongToString(entry->inboundUnitPrice.value)));

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
