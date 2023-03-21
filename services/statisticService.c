#include "statisticService.h"
#include "../data/amount.h"
#include "../data/profit.h"
#include "../utils.h"

// 读取所有的收支条目并计算总金额、输出总金额
Table *GetStatistics(__attribute__((unused)) Table *a)
{
    Amount totalamount = NewAmount(0, 0, 0);

    // 判断并计算总金额
    LinkedList *totalprofithead = GetAllProfits();
    if (totalprofithead == NULL)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "不存在收支条目");
        return goback;
    }
    LinkedList *profitnode = totalprofithead;
    while (profitnode != NULL)
    {
        Profit *thisprofit = profitnode->data;
        Amount profitamount = GetProfitAmount(thisprofit);

        totalamount = AmountAdd(&totalamount, &profitamount);

        profitnode = profitnode->next;
    }

    // 数据准备
    int yuan = GetAmountYuan(&totalamount);
    int jiao = GetAmountJiao(&totalamount);
    int cent = GetAmountCent(&totalamount);
    char *yuanstring = LongLongToString(yuan);
    char *jiaostring = LongLongToString(jiao);
    char *centstring = LongLongToString(cent);

    // 标题行
    TableRow *row = NewTableRow();
    AppendTableRow(row, "yuan");
    AppendTableRow(row, "jiao");
    AppendTableRow(row, "cent");
    Table *goback = NewTable(row, NULL);

    // 传入数据
    row = NewTableRow();
    AppendTableRow(row, yuanstring);
    AppendTableRow(row, jiaostring);
    AppendTableRow(row, centstring);
    AppendTable(goback, row);

    free(yuanstring);
    free(jiaostring);
    free(centstring);

    // 返回值
    return goback;
}
