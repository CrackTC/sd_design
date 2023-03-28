#include "design/linkedList.h"
#include "design/operation.h"
#include "design/table.h"
#include "design/journalService.h"
#include "design/judgeService.h"
#include "design/statisticService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include "design/mainWindow.h"
#include <stddef.h>
#include <malloc.h>

void SendProfitRequest(struct MainWindowData *data)
{
    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_READ_STATISTICS);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("缺少权限：读取统计");
        return;
    }

    AddJournal(NULL, data->id, OP_READ_STATISTICS);
    Table *response = GetStatistics(NULL);
    if (response != NULL)
    {
        if (response->remark != NULL && response->remark[0] != '\0')
        {
            data->messageCallback = MessageBoxCallback;
            data->message = CloneString(response->remark);
        }

        free(data->dataArray[PROFIT_INDEX].properties);
        FreeList(data->dataArray[PROFIT_INDEX].checkList);
        FreeTable(data->dataArray[PROFIT_INDEX].table);
        data->dataArray[PROFIT_INDEX].checkList = NewCheckList();
        data->dataArray[PROFIT_INDEX].table = response;
        data->dataArray[PROFIT_INDEX].properties = NULL;
        data->dataArray[PROFIT_INDEX].propertySelected = 0;
    }
    else
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("查询失败: 响应为NULL");
    }
}

void ProfitLookup(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[PROFIT_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[PROFIT_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->dataArray[PROFIT_INDEX].table));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(NewProfitDetail("收支详情", table));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个收支条目");
}

void ProfitPageLayout(struct nk_context *context, struct Window *window)
{
    struct MainWindowData *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);
    BasicFilterLayout(context, "统计", &data->dataArray[PROFIT_INDEX]);
    nk_layout_row_static(context, 10, 0, 0);
    OperationLayout(context,
            OP_TYPE_GET | OP_TYPE_DETAIL,
            (OperationHandler)SendProfitRequest,
            (OperationHandler)ProfitLookup,
            NULL,
            NULL,
            NULL,
            data);
    DrawSeparateLine(context);
    char *from, *to;
    DateRangeFilterLayout(context, "筛选时间", &from, &to);
    DrawSeparateLine(context);
    PageResultLayout(context, &data->dataArray[PROFIT_INDEX]);
}
