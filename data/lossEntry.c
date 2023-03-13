#include "lossEntry.h"
#include "linkedList.h"
#include "../utils.h"
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
    while (rowNode->next != NULL) {
        rowNode = rowNode->next;
        const TableRow *row = rowNode->data;

        int inventoryId;
        char *reason;
        Time time;

        sscanf("%d", GetRowItemByColumnName(table, row, inventoryIdRow), &inventoryId);
        reason = CloneString(GetRowItemByColumnName(table, row, reasonRow));
        sscanf("%d", GetRowItemByColumnName(table, row, timeRow), &time.value);

        LossEntry *entry = malloc(sizeof(LossEntry));
        entry->inventoryId = inventoryId;
        entry->reason = reason;
        entry->time = time;

        LinkedList *node = malloc(sizeof(LinkedList));
        node->data = entry;
        node->next = NULL;

        list = AppendNode(list, node);
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
    while (now != NULL) {
        LossEntry *entry = now->data;
        if (entry->inventoryId == inventoryId) {
            LinkedList *node = malloc(sizeof(LinkedList));
            node->data = entry;
            node->next = NULL;
            list = AppendNode(list, node);
        }
        now = now->next;
    }

    return list;
}

int GetLossEntryInventoryId(const LossEntry *entry)
{
    return entry->inventoryId;
}

const char *GetLossEntryReason(const LossEntry *entry)
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
    if (systemList == NULL) {
        GetAllLoss();
    }
    if (entry == NULL) {
        return 1;
    }
    if (ExistsNode(systemList, entry)) {
        return 1;
    }

    LinkedList *node = malloc(sizeof(LinkedList));
    node->data = entry;
    node->next = NULL;
    systemList = AppendNode(systemList, node);

    return 0;
}

int RemoveLossEntry(LossEntry *entry)
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

void LossEntrySave()
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, inventoryIdRow);
    AppendTableRow(row, reasonRow);
    AppendTableRow(row, timeRow);

    Table *table = NewTable(row, NULL);

    LinkedList *now = systemList;
    while (now != NULL) {
        LossEntry *entry = now->data;
        row = NewTableRow();

        char *inventoryIdString = LongLongToString(entry->inventoryId);
        char *timeString = LongLongToString(entry->time.value);

        AppendTableRow(row, inventoryIdString);
        AppendTableRow(row, entry->reason);
        AppendTableRow(row, timeString);

        free(inventoryIdString);
        free(timeString);

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
