#include "design/linkedList.h"
#include "design/operation.h"
#include "design/table.h"
#include "design/journalService.h"
#include "design/judgeService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include "design/mainWindow.h"
#include <stddef.h>
#include <malloc.h>

void SendJournalRequest(struct MainWindowData *data)
{
    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_READ_JOURNAL);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("缺少权限：读取日志");
        return;
    }

    AddJournal(NULL, data->id, OP_READ_JOURNAL);
    Table *response = GetAllJournal(NULL);
    if (response != NULL)
    {
        if (response->remark != NULL && response->remark[0] != '\0')
        {
            data->messageCallback = MessageBoxCallback;
            data->message = CloneString(response->remark);
        }

        free(data->dataArray[JOURNAL_INDEX].properties);
        FreeList(data->dataArray[JOURNAL_INDEX].checkList);
        FreeTable(data->dataArray[JOURNAL_INDEX].table);
        data->dataArray[JOURNAL_INDEX].checkList = NewCheckList();
        data->dataArray[JOURNAL_INDEX].table = response;
        data->dataArray[JOURNAL_INDEX].properties = NULL;
        data->dataArray[JOURNAL_INDEX].propertySelected = 0;
    }
    else
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("查询失败: 响应为NULL");
    }
}

void JournalLookup(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[JOURNAL_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[JOURNAL_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->dataArray[JOURNAL_INDEX].table));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(NewJournalDetail("日志详情", table));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个日志条目");
}

void JournalPageLayout(struct nk_context *context, struct Window *window)
{
    struct MainWindowData *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);
    BasicFilterLayout(context, "日志", &data->dataArray[JOURNAL_INDEX]);
    nk_layout_row_static(context, 10, 0, 0);
    OperationLayout(context,
            OP_TYPE_GET | OP_TYPE_DETAIL,
            (OperationHandler)SendJournalRequest,
            (OperationHandler)JournalLookup,
            NULL,
            NULL,
            NULL,
            data);
    DrawSeparateLine(context);
    char *from, *to;
    DateRangeFilterLayout(context, "筛选时间", &from, &to);
    DrawSeparateLine(context);
    PageResultLayout(context, &data->dataArray[JOURNAL_INDEX]);
}
