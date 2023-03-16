#include "refundEntry.h"
#include "../utils.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <stdio.h>
#include <stdlib.h>

static const char *path = "data/refundEntry.txt";
static const char *orderIdRow = "orderId";
static const char *reasonRow = "reason";
static const char *timeRow = "time";
static const char *refundAmountRow = "refundAmount";
static const char *numberRow = "number";
static const char *remarkRow = "remark";
static LinkedList *systemList = NULL;

struct RefundEntry
{
    int orderId;
    char *reason;
    Time time;
    Amount refundAmount;
    int number;
    char *remark;
};

RefundEntry *NewRefundEntry(int orderId, const char *reason, Time *time, Amount *refundAmount, int number,
                            const char *remark)
{
    if (orderId < 0 || reason == NULL || time == NULL || refundAmount == NULL || refundAmount->value < 0 ||
        number < 0 || remark == NULL)
    {
        return NULL;
    }

    RefundEntry *entry = malloc(sizeof(RefundEntry));
    entry->orderId = orderId;
    entry->reason = CloneString(reason);
    entry->time = *time;
    entry->refundAmount = *refundAmount;
    entry->number = number;
    entry->remark = CloneString(remark);

    return entry;
}

void FreeRefundEntry(RefundEntry *entry)
{
    free(entry->reason);
    free(entry->remark);
    free(entry);
}

LinkedList *GetAllRefunds()
{
    if (systemList != NULL)
    {
        return systemList;
    }

    Table *table;
    int result = Unserialize(&table, path);
    if (result == 1)
    {
        RefundEntrySave();
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

        RefundEntry *entry = malloc(sizeof(RefundEntry));

        sscanf(GetRowItemByColumnName(table, row, orderIdRow), "%d", &entry->orderId);
        entry->reason = CloneString(GetRowItemByColumnName(table, row, reasonRow));
        sscanf(GetRowItemByColumnName(table, row, timeRow), "%ld", &entry->time.value);
        sscanf(GetRowItemByColumnName(table, row, refundAmountRow), "%lld", &entry->refundAmount.value);
        sscanf(GetRowItemByColumnName(table, row, numberRow), "%d", &entry->number);
        entry->remark = CloneString(GetRowItemByColumnName(table, row, remarkRow));

        list = AppendData(list, entry);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

RefundEntry *GetRefundByOrderId(int orderId)
{
    if (systemList == NULL)
    {
        GetAllRefunds();
    }

    LinkedList *now = systemList;
    while (now != NULL)
    {
        RefundEntry *entry = now->data;
        if (entry->orderId == orderId)
        {
            return entry;
        }
        now = now->next;
    }
    return NULL;
}

int GetRefundEntryOrderId(const RefundEntry *entry)
{
    return entry->orderId;
}

const char *GetRefundEntryReason(const RefundEntry *entry)
{
    return entry->reason;
}

Time GetRefundEntryTime(const RefundEntry *entry)
{
    return entry->time;
}
Amount GetRefundEntryAmount(const RefundEntry *entry)
{
    return entry->refundAmount;
}

int GetRefundEntryNumber(const RefundEntry *entry)
{
    return entry->number;
}

const char *GetRefundEntryRemark(const RefundEntry *entry)
{
    return entry->remark;
}

void SetRefundEntryOrderId(RefundEntry *entry, int value)
{
    entry->orderId = value;
}

void SetRefundEntryReason(RefundEntry *entry, const char *value)
{
    entry->reason = CloneString(value);
}

void SetRefundEntryTime(RefundEntry *entry, Time *value)
{
    entry->time = *value;
}

void SetRefundEntryAmount(RefundEntry *entry, Amount *value)
{
    entry->refundAmount = *value;
}

void SetRefundEntryNumber(RefundEntry *entry, int value)
{
    entry->number = value;
}

void SetRefundEntryRemark(RefundEntry *entry, const char *remark)
{
    entry->remark = CloneString(remark);
}

int AppendRefundEntry(RefundEntry *entry)
{
    if (systemList == NULL)
    {
        GetAllRefunds();
    }
    if (entry == NULL)
    {
        return 1;
    }
    if (ExistsNode(systemList, entry))
    {
        return 1;
    }

    systemList = AppendData(systemList, entry);
    return 0;
}

void RemoveRefundEntry(RefundEntry *entry)
{
    systemList = RemoveNode(systemList, entry);
}

void RefundEntrySave()
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, orderIdRow);
    AppendTableRow(row, reasonRow);
    AppendTableRow(row, timeRow);
    AppendTableRow(row, refundAmountRow);
    AppendTableRow(row, numberRow);
    AppendTableRow(row, remarkRow);

    Table *table = NewTable(row, NULL);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        RefundEntry *entry = now->data;
        row = NewTableRow();

        free(AppendTableRow(row, LongLongToString(entry->orderId)));
        AppendTableRow(row, entry->reason);
        free(AppendTableRow(row, LongLongToString(entry->time.value)));
        free(AppendTableRow(row, LongLongToString(entry->refundAmount.value)));
        free(AppendTableRow(row, LongLongToString(entry->number)));
        AppendTableRow(row, entry->remark);

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
