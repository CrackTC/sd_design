#include "profit.h"
#include "../utils.h"
#include "amount.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <malloc.h>
#include <stdio.h>

static const char *path = "data/profit.txt";
static const char *amountRow = "amount";
static const char *matterRow = "matter";
static const char *timeRow = "time";
static LinkedList *systemList = NULL;

struct Profit
{
    Amount amount;
    char *matter;
    Time time;
};

Profit *NewProfit(Amount *amount, const char *matter, Time *time)
{
    if (amount->value < 0 || matter == NULL || time == NULL)
    {
        return NULL;
    }

    Profit *profit = malloc(sizeof(Profit));
    profit->amount = *amount;
    profit->matter = CloneString(matter);
    profit->time = *time;

    return profit;
}

void FreeProfit(Profit *profit)
{
    free(profit->matter);
    free(profit);
}

LinkedList *GetAllProfits()
{
    if (systemList != NULL)
        return systemList;

    Table *table;
    int result = Unserialize(&table, path);
    if (result == 1)
    {
        ProfitSave();
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

        Amount amount;
        char *matter;
        Time time;

        sscanf(GetRowItemByColumnName(table, row, amountRow), "%lld", &amount.value);
        matter = CloneString(GetRowItemByColumnName(table, row, matterRow));
        sscanf(GetRowItemByColumnName(table, row, timeRow), "%ld", &time.value);

        Profit *profit = malloc(sizeof(Profit));
        profit->amount = amount;
        profit->matter = matter;
        profit->time = time;

        LinkedList *node = malloc(sizeof(LinkedList));
        node->data = profit;
        node->next = NULL;

        list = AppendNode(list, node);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

Amount GetProfitAmount(const Profit *profit)
{
    return profit->amount;
}

const char *GetProfitMatter(const Profit *profit)
{
    return profit->matter;
}

Time GetProfitTime(const Profit *profit)
{
    return profit->time;
}

void SetProfitAmount(Profit *profit, Amount *value)
{
    profit->amount = *value;
}

void SetProfitMatter(Profit *profit, const char *value)
{
    profit->matter = CloneString(value);
}

void SetProfitTime(Profit *profit, Time *value)
{
    profit->time = *value;
}

int AppendProfit(Profit *profit)
{
    if (systemList == NULL)
    {
        GetAllProfits();
    }
    if (profit == NULL)
    {
        return 1;
    }
    if (ExistsNode(systemList, profit))
    {
        return 1;
    }

    LinkedList *node = malloc(sizeof(LinkedList));
    node->data = profit;
    node->next = NULL;
    systemList = AppendNode(systemList, node);

    return 0;
}

int RemoveProfit(Profit *profit)
{
    LinkedList *now = systemList;
    while (now != NULL)
    {
        if (now->data == profit)
        {
            systemList = RemoveNode(systemList, now);
            return 0;
        }
        now = now->next;
    }

    return 1;
}

void ProfitSave()
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, amountRow);
    AppendTableRow(row, matterRow);
    AppendTableRow(row, timeRow);

    Table *table = NewTable(row, NULL);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        Profit *profit = now->data;
        row = NewTableRow();

        char *amountString = LongLongToString(profit->amount.value);
        char *timeString = LongLongToString(profit->time.value);

        AppendTableRow(row, amountString);
        AppendTableRow(row, profit->matter);
        AppendTableRow(row, timeString);

        free(amountString);
        free(timeString);

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
