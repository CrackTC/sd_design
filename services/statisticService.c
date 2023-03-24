#include "design/statisticService.h"
#include "design/amount.h"
#include "design/profit.h"
#include "design/utils.h"
#include <string.h>
#include <stdio.h>
#include <malloc.h>

// 读取所有的收支条目并计算总金额、输出总金额
Table *GetStatistics(Table *a)
{
    // 判断并计算总金额
    LinkedList *profitHead = GetAllProfits();

    TableRow *row = NewTableRow();
    AppendTableRow(row, "收支");
    AppendTableRow(row, "事项");
    AppendTableRow(row, "时间");
    Table *table = NewTable(row, NULL);

    int count = 0;

    while (profitHead != NULL)
    {
        count++;
        Profit *profit = profitHead->data;

        row = NewTableRow();
        Amount amount = GetProfitAmount(profit);
        free(AppendTableRow(row, AmountToString(&amount)));
        free(AppendTableRow(row, CloneString(GetProfitMatter(profit))));
        Time time = GetProfitTime(profit);
        free(AppendTableRow(row, TimeToString(GetTimeInfo(&time, 1))));

        AppendTable(table, row);
        profitHead = profitHead->next;
    }

    size_t len = strlen("查询到") + IntegerStringLength(count) + strlen("条信息") + 1;
    char *remark = malloc(len);
    sprintf(remark, "查询到%d条信息", count);
    SetTableRemark(table, remark);
    free(remark);
    return table;
}
