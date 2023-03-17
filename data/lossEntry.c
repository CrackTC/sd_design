#include "lossEntry.h"
#include "../utils.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <malloc.h>
#include <stdio.h>

static int idCount = 0;
static const char *path = "data/lossEntry.txt";
static const char *idRow = "id";
static const char *inventoryIdRow = "inventoryId";
static const char *numberRow = "number";
static const char *reasonRow = "reason";
static const char *timeRow = "time";
static LinkedList *systemList = NULL;

struct LossEntry
{
    int id;
    int inventoryId;
    int number;
    char *reason;
    Time time;
};

LossEntry *NewLossEntry(int inventoryId, int number, const char *reason, Time *time)
{
    if (inventoryId < 0 || number < 0 || reason == NULL)
        return NULL;

    LossEntry *entry = malloc(sizeof(LossEntry));
    entry->id = GenerateId(systemList, GetAllLoss, &idCount);
    entry->inventoryId = inventoryId;
    entry->number = number;
    entry->reason = CloneString(reason);
    entry->time = *time;

    return entry;
}

void FreeLossEntry(LossEntry *entry)
{
    free(entry->reason);
    free(entry);
}

LinkedList *GetAllLoss()
{
    if (systemList != NULL)
        return systemList;

    Table *table;
    int result = Unserialize(&table, path);
    if (result == 1)
    {
        LossEntrySave();
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

        LossEntry *entry = malloc(sizeof(LossEntry));

        sscanf(GetRowItemByColumnName(table, row, idRow), "%d", &entry->id);
        sscanf(GetRowItemByColumnName(table, row, inventoryIdRow), "%d", &entry->inventoryId);
        sscanf(GetRowItemByColumnName(table, row, numberRow), "%d", &entry->number);
        entry->reason = CloneString(GetRowItemByColumnName(table, row, reasonRow));
        sscanf(GetRowItemByColumnName(table, row, timeRow), "%ld", &entry->time.value);

        list = AppendData(list, entry);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

LossEntry *GetLossEntryById(int id)
{
    if (systemList == NULL) {
        GetAllLoss();
    }

    LinkedList *now = systemList;
    while (now != NULL) {
        LossEntry *entry = now->data;
        if (entry->id == id) {
            return entry;
        }
        now = now->next;
    }
    return NULL;
}

LinkedList *GetLossEntriesByInventoryId(int inventoryId)
{
    if (systemList == NULL)
        GetAllLoss();

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL)
    {
        LossEntry *entry = now->data;
        if (entry->inventoryId == inventoryId)
        {
            list = AppendData(list, entry);
        }
        now = now->next;
    }

    return list;
}

int GetLossEntryId(const LossEntry *entry)
{
    return entry->id;
}

int GetLossEntryInventoryId(const LossEntry *entry)
{
    return entry->inventoryId;
}

int GetLossEntryNumber(const LossEntry *entry)
{
    return entry->number;
}

char *GetLossEntryReason(const LossEntry *entry)
{
    return entry->reason;
}

Time GetLossEntryTime(const LossEntry *entry)
{
    return entry->time;
}

void SetLossEntryInventoryId(LossEntry *entry, int value)
{
    entry->inventoryId = value;
}

void SetLossEntryNumber(LossEntry *entry, int value)
{
    entry->number = value;
}

void SetLossEntryReason(LossEntry *entry, char *value)
{
    free(entry->reason);
    entry->reason = CloneString(value);
}

void SetLossEntryTime(LossEntry *entry, Time *value)
{
    entry->time = *value;
}

int AppendLossEntry(LossEntry *entry)
{
    if (systemList == NULL)
    {
        GetAllLoss();
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

void RemoveLossEntry(LossEntry *entry)
{
    systemList = RemoveNode(systemList, entry);
}

void LossEntrySave()
{
    TableRow *row = NewTableRow();
    free(AppendTableRow(row, CloneString(idRow)));
    free(AppendTableRow(row, CloneString(inventoryIdRow)));
    free(AppendTableRow(row, CloneString(numberRow)));
    free(AppendTableRow(row, CloneString(reasonRow)));
    free(AppendTableRow(row, CloneString(timeRow)));

    Table *table = NewTable(row, NULL);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        LossEntry *entry = now->data;
        row = NewTableRow();

        free(AppendTableRow(row, LongLongToString(entry->id)));
        free(AppendTableRow(row, LongLongToString(entry->inventoryId)));
        free(AppendTableRow(row, LongLongToString(entry->number)));
        AppendTableRow(row, entry->reason);
        free(AppendTableRow(row, LongLongToString(entry->time.value)));

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
