#include "design/crud.h"
#include "design/linkedList.h"
#include "design/operation.h"
#include "design/table.h"
#include "design/time.h"
#include "design/inventoryService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include "design/mainWindow.h"
#include <stddef.h>
#include <malloc.h>

void LossEntryDelete(int ok, void *parameter)
{
    struct MainWindowData *data = parameter;
    Delete(ok, parameter, &data->dataArray[LOSS_INDEX], DeleteSingleLossById, "缺少权限：删除货损", "货损编号", OP_DELETE_LOSS);
}

void ConfirmLossDelete(struct MainWindowData *data)
{
    data->messageCallback = LossEntryDelete;
    data->message = CloneString("是否确认要删除选中的货损条目");
}

void SendLossRequest(struct MainWindowData *data)
{
    Read(data, &data->dataArray[LOSS_INDEX], ShowLossInventory, "缺少权限：读取货损", OP_READ_LOSS);
}

void LossLookup(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[LOSS_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[LOSS_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->dataArray[LOSS_INDEX].table));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(NewLossDetail("货损详情", table));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个货损条目");
}

void LossAdd(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[INVENTORY_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[INVENTORY_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "库存编号");
            AppendTableRow(row, "货损数量");
            AppendTableRow(row, "货损原因");
            AppendTableRow(row, "年");
            AppendTableRow(row, "月");
            AppendTableRow(row, "日");
            AppendTableRow(row, "时");
            AppendTableRow(row, "分");
            AppendTableRow(row, "秒");
            Table *table = NewTable(row, "");

            row = NewTableRow();
            AppendTableRow(row,
                    GetRowItemByColumnName(data->dataArray[INVENTORY_INDEX].table, rowNow->data, "库存编号"));
            AppendTableRow(row, "0");
            AppendTableRow(row, "");
            AppendTableRow(row, "");
            AppendTableRow(row, "");
            AppendTableRow(row, "");
            AppendTableRow(row, "");
            AppendTableRow(row, "");
            AppendTableRow(row, "");
            AppendTable(table, row);

            PushWindow(NewLossEdit("货损编辑", data->id, data->password, table, 0));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请在库存页面选择一个库存条目");
}

void LossModify(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[LOSS_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[LOSS_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "货损编号");
            AppendTableRow(row, "库存编号");
            AppendTableRow(row, "货损数量");
            AppendTableRow(row, "货损原因");
            AppendTableRow(row, "年");
            AppendTableRow(row, "月");
            AppendTableRow(row, "日");
            AppendTableRow(row, "时");
            AppendTableRow(row, "分");
            AppendTableRow(row, "秒");
            Table *table = NewTable(row, "");

            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[LOSS_INDEX].table, rowNow->data, "货损编号"));
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[LOSS_INDEX].table, rowNow->data, "库存编号"));
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[LOSS_INDEX].table, rowNow->data, "货损数量"));
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[LOSS_INDEX].table, rowNow->data, "货损原因"));

            const char *time = GetRowItemByColumnName(data->dataArray[LOSS_INDEX].table, rowNow->data, "损耗时间");
            TimeInfo info = ParseTime(time, 0);
            free(AppendTableRow(row, LongLongToString(info.year)));
            free(AppendTableRow(row, LongLongToString(info.month)));
            free(AppendTableRow(row, LongLongToString(info.day)));
            free(AppendTableRow(row, LongLongToString(info.hour)));
            free(AppendTableRow(row, LongLongToString(info.minute)));
            free(AppendTableRow(row, LongLongToString(info.second)));

            AppendTable(table, row);

            PushWindow(NewLossEdit("货损编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个货损条目");
}

void LossPageLayout(struct nk_context *context, struct Window *window)
{
    struct MainWindowData *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);
    BasicFilterLayout(context, "货损", &data->dataArray[LOSS_INDEX]);
    nk_layout_row_static(context, 10, 0, 0);
    OperationLayout(context,
            OP_TYPE_GET | OP_TYPE_DETAIL | OP_TYPE_ADD | OP_TYPE_DELETE | OP_TYPE_UPDATE,
            (OperationHandler)SendLossRequest,
            (OperationHandler)LossLookup,
            (OperationHandler)LossAdd,
            (OperationHandler)ConfirmLossDelete,
            (OperationHandler)LossModify,
            data);
    DrawSeparateLine(context);
    char *from, *to;
    DateRangeFilterLayout(context, "筛选损耗时间", &from, &to);
    DrawSeparateLine(context);
    PageResultLayout(context, &data->dataArray[LOSS_INDEX], "损耗时间", from, to);
}
