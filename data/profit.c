#include "profit.h"
#include "../utils.h"
#include "amount.h"
#include "../config.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <malloc.h>
#include <stdio.h>

static const char *fileName = "data" PATH_SEPARATOR_STRING "profit.txt";
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
    if (matter == NULL || time == NULL)
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
    int result = Unserialize(&table, fileName);
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

        Profit *profit = malloc(sizeof(Profit));

        sscanf(GetRowItemByColumnName(table, row, amountRow), "%lld", &profit->amount.value);
        profit->matter = CloneString(GetRowItemByColumnName(table, row, matterRow));
        sscanf(GetRowItemByColumnName(table, row, timeRow), "%lld", &profit->time.value);

        list = AppendData(list, profit);
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

    systemList = AppendData(systemList, profit);
    return 0;
}

void RemoveProfit(Profit *profit)
{
    systemList = RemoveNode(systemList, profit);
}

void ProfitSave()
{
    TableRow *row = NewTableRow();
    free(AppendTableRow(row, CloneString(amountRow)));
    free(AppendTableRow(row, CloneString(matterRow)));
    free(AppendTableRow(row, CloneString(timeRow)));

    Table *table = NewTable(row, NULL);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        Profit *profit = now->data;
        row = NewTableRow();

        free(AppendTableRow(row, LongLongToString(profit->amount.value)));
        AppendTableRow(row, profit->matter);
        free(AppendTableRow(row, LongLongToString(profit->time.value)));

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, fileName);
    FreeTable(table);
}
