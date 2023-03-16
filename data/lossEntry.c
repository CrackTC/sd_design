#include "lossEntry.h"
#include "../utils.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <malloc.h>

static const char *path = "data/lossEntry.txt";
static const char *inventoryIdRow = "inventoryId";
static const char *reasonRow = "reason";
static const char *timeRow = "time";
static LinkedList *systemList = NULL;

struct LossEntry
{
    int inventoryId;
    char *reason;
    Time time;
};

LossEntry *NewLossEntry(int inventoryId, const char *reason, Time *time)
{
    if (inventoryId < 0 || reason == NULL)
        return NULL;

    LossEntry *entry = malloc(sizeof(LossEntry));
    entry->inventoryId = inventoryId;
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

        sscanf(GetRowItemByColumnName(table, row, inventoryIdRow), "%d", &entry->inventoryId);
        entry->reason = CloneString(GetRowItemByColumnName(table, row, reasonRow));
        sscanf(GetRowItemByColumnName(table, row, timeRow), "%ld", &entry->time.value);

        list = AppendData(list, entry);
    }

    FreeTable(table);

    systemList = list;
    return list;
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

int GetLossEntryInventoryId(const LossEntry *entry)
{
    return entry->inventoryId;
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
    free(AppendTableRow(row, CloneString(inventoryIdRow)));
    free(AppendTableRow(row, CloneString(reasonRow)));
    free(AppendTableRow(row, CloneString(timeRow)));

    Table *table = NewTable(row, NULL);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        LossEntry *entry = now->data;
        row = NewTableRow();

        free(AppendTableRow(row, LongLongToString(entry->inventoryId)));
        AppendTableRow(row, entry->reason);
        free(AppendTableRow(row, LongLongToString(entry->time.value)));

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
