#include "inventory.h"
#include "customer.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include "../utils.h"
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

    sscanf(GetTableRemark(table), "%d", &idCount);

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

        sscanf(GetRowItemByColumnName(table, row, idRow), "%d", &id);
        sscanf(GetRowItemByColumnName(table, row, itemIdRow), "%d", &itemId);
        sscanf(GetRowItemByColumnName(table, row, numberRow), "%d", &number);
        sscanf(GetRowItemByColumnName(table, row, inboundTimeRow), "%ld", &inboundTime.value);
        sscanf(GetRowItemByColumnName(table, row, productionTimeRow), "%ld", &productionTime.value);
        sscanf(GetRowItemByColumnName(table, row, inboundUnitPriceRow), "%lld", &inboundUnitPrice.value);

        InventoryEntry *entry = malloc(sizeof(InventoryEntry));
        entry->id = id;
        entry->itemId = itemId;
        entry->number = number;
        entry->inboundTime = inboundTime;
        entry->productionTime = productionTime;
        entry->inboundUnitPrice = inboundUnitPrice;

        LinkedList *node = malloc(sizeof(LinkedList));
        node->data = entry;
        node->next = NULL;

        list = AppendNode(list, node);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

InventoryEntry *GetInventoryById(int id)
{
    if (systemList == NULL)
        GetAllInventory();

    LinkedList *now = systemList;
    while (now != NULL) {
        InventoryEntry *entry = now->data;
        if (entry->id == id) {
            return entry;
        }
        now = now->next;
    }
    return NULL;
}

LinkedList *GetInventoryByItemId(int itemId)
{
    if (systemList == NULL)
        GetAllInventory();

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL) {
        InventoryEntry *entry = now->data;
        if (entry->itemId == itemId) {
            LinkedList *node = malloc(sizeof(LinkedList));
            node->data = entry;
            node->next = NULL;
            list = AppendNode(list, node);
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
    if (systemList == NULL) {
        GetAllInventory();
    }
    if (entry == NULL) {
        return 1;
    }
    if (ExistsNode(systemList, entry)) {
        return 1;
    }

    LinkedList *node = malloc(sizeof(LinkedList));
    node->data = entry;
    node->next =  NULL;
    systemList = AppendNode(systemList, node);

    return 0;
}

int RemoveInventoryEntry(InventoryEntry *entry)
{
    LinkedList *now = systemList;
    while (now != NULL) {
        if (now->data == entry) {
            systemList = RemoveNode(systemList, now);
            return 0;
        }
        now = now->next;
    }

    return 1;
}

void InventorySave()
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, idRow);
    AppendTableRow(row, itemIdRow);
    AppendTableRow(row, numberRow);
    AppendTableRow(row, inboundTimeRow);
    AppendTableRow(row, productionTimeRow);
    AppendTableRow(row, inboundUnitPriceRow);

    char *remark = LongLongToString(idCount);
    Table *table = NewTable(row, remark);
    free(remark);

    LinkedList *now = systemList;
    while (now != NULL) {
        InventoryEntry *entry = now->data;
        row = NewTableRow();

        char *idString = LongLongToString(entry->id);
        char *itemIdString = LongLongToString(entry->itemId);
        char *numberString = LongLongToString(entry->number);
        char *inboundTimeString = LongLongToString(entry->inboundTime.value);
        char *productionTimeString = LongLongToString(entry->productionTime.value);
        char *inboundUnitPriceString = LongLongToString(entry->inboundUnitPrice.value);

        AppendTableRow(row, idString);
        AppendTableRow(row, itemIdString);
        AppendTableRow(row, numberString);
        AppendTableRow(row, inboundTimeString);
        AppendTableRow(row, productionTimeString);
        AppendTableRow(row, inboundUnitPriceString);

        free(idString);
        free(itemIdString);
        free(numberString);
        free(inboundTimeString);
        free(productionTimeString);
        free(inboundUnitPriceString);

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
