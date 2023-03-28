#include "design/crud.h"
#include "design/amount.h"
#include "design/linkedList.h"
#include "design/operation.h"
#include "design/table.h"
#include "design/time.h"
#include "design/inventoryService.h"
#include "design/journalService.h"
#include "design/judgeService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include "design/mainWindow.h"
#include <stddef.h>
#include <malloc.h>

void InventoryDelete(int ok, void *parameter)
{
    struct MainWindowData *data = parameter;
    Delete(ok, parameter, &data->dataArray[INVENTORY_INDEX], DeleteInventoryById, "缺少权限：删除库存", "库存编号");
}

void ConfirmInventoryDelete(struct MainWindowData *data)
{
    data->messageCallback = InventoryDelete;
    data->message = CloneString("是否确认要删除选中的库存条目");
}

void SendInventoryRequest(struct MainWindowData *data)
{
    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_READ_INVENTORY);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("缺少权限：读取库存");
        return;
    }

    AddJournal(NULL, data->id, OP_READ_INVENTORY);
    Table *response = ShowInventory(NULL);
    if (response != NULL)
    {
        if (response->remark != NULL && response->remark[0] != '\0')
        {
            data->messageCallback = MessageBoxCallback;
            data->message = CloneString(response->remark);
        }

        free(data->dataArray[INVENTORY_INDEX].properties);
        FreeTable(data->dataArray[INVENTORY_INDEX].table);
        FreeList(data->dataArray[INVENTORY_INDEX].checkList);
        data->dataArray[INVENTORY_INDEX].checkList = NewCheckList();
        data->dataArray[INVENTORY_INDEX].table = response;
        data->dataArray[INVENTORY_INDEX].properties = NULL;
        data->dataArray[INVENTORY_INDEX].propertySelected = 0;
    }
    else
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("查询失败: 响应为NULL");
    }
}

void InventoryLookup(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[INVENTORY_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[INVENTORY_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->dataArray[INVENTORY_INDEX].table));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(NewInventoryDetail("库存详情", table));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个库存条目");
}

void InventoryAdd(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[ITEM_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[ITEM_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "商品编号");
            AppendTableRow(row, "商品名称");
            AppendTableRow(row, "数量");
            AppendTableRow(row, "年1");
            AppendTableRow(row, "月1");
            AppendTableRow(row, "日1");
            AppendTableRow(row, "时1");
            AppendTableRow(row, "分1");
            AppendTableRow(row, "秒1");
            AppendTableRow(row, "年2");
            AppendTableRow(row, "月2");
            AppendTableRow(row, "日2");
            AppendTableRow(row, "时2");
            AppendTableRow(row, "分2");
            AppendTableRow(row, "秒2");
            AppendTableRow(row, "元");
            AppendTableRow(row, "角");
            AppendTableRow(row, "分");
            Table *table = NewTable(row, "");
            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[ITEM_INDEX].table, rowNow->data, "商品编号"));
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[ITEM_INDEX].table, rowNow->data, "商品名称"));
            AppendTableRow(row, "");
            AppendTableRow(row, "1");
            AppendTableRow(row, "1");
            AppendTableRow(row, "1");
            AppendTableRow(row, "1");
            AppendTableRow(row, "1");
            AppendTableRow(row, "1");
            AppendTableRow(row, "2");
            AppendTableRow(row, "2");
            AppendTableRow(row, "2");
            AppendTableRow(row, "2");
            AppendTableRow(row, "2");
            AppendTableRow(row, "2");
            AppendTableRow(row, "100");
            AppendTableRow(row, "3");
            AppendTableRow(row, "2");
            AppendTable(table, row);
            PushWindow(NewInventoryEdit("库存编辑", data->id, data->password, table, 0));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请在商品页面选择一个商品条目");
}

void InventoryModify(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[INVENTORY_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[INVENTORY_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "库存编号");
            AppendTableRow(row, "商品编号");
            AppendTableRow(row, "商品名称");
            AppendTableRow(row, "数量");
            AppendTableRow(row, "年1");
            AppendTableRow(row, "月1");
            AppendTableRow(row, "日1");
            AppendTableRow(row, "时1");
            AppendTableRow(row, "分1");
            AppendTableRow(row, "秒1");
            AppendTableRow(row, "年2");
            AppendTableRow(row, "月2");
            AppendTableRow(row, "日2");
            AppendTableRow(row, "时2");
            AppendTableRow(row, "分2");
            AppendTableRow(row, "秒2");
            AppendTableRow(row, "元");
            AppendTableRow(row, "角");
            AppendTableRow(row, "分");
            Table *table = NewTable(row, "");
            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[INVENTORY_INDEX].table, rowNow->data, "库存编号"));
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[INVENTORY_INDEX].table, rowNow->data, "商品编号"));
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[INVENTORY_INDEX].table, rowNow->data, "商品名称"));
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[INVENTORY_INDEX].table, rowNow->data, "数量"));

            const char *time = GetRowItemByColumnName(data->dataArray[INVENTORY_INDEX].table, rowNow->data, "入库时间");
            TimeInfo info = ParseTime(time, 0);
            free(AppendTableRow(row, LongLongToString(info.year)));
            free(AppendTableRow(row, LongLongToString(info.month)));
            free(AppendTableRow(row, LongLongToString(info.day)));
            free(AppendTableRow(row, LongLongToString(info.hour)));
            free(AppendTableRow(row, LongLongToString(info.minute)));
            free(AppendTableRow(row, LongLongToString(info.second)));

            time = GetRowItemByColumnName(data->dataArray[INVENTORY_INDEX].table, rowNow->data, "生产日期");
            info = ParseTime(time, 0);
            free(AppendTableRow(row, LongLongToString(info.year)));
            free(AppendTableRow(row, LongLongToString(info.month)));
            free(AppendTableRow(row, LongLongToString(info.day)));
            free(AppendTableRow(row, LongLongToString(info.hour)));
            free(AppendTableRow(row, LongLongToString(info.minute)));
            free(AppendTableRow(row, LongLongToString(info.second)));

            Amount amount = ParseAmount(GetRowItemByColumnName(data->dataArray[INVENTORY_INDEX].table, rowNow->data, "购入单价"));
            free(AppendTableRow(row, LongLongToString(GetAmountYuan(&amount))));
            free(AppendTableRow(row, LongLongToString(GetAmountJiao(&amount))));
            free(AppendTableRow(row, LongLongToString(GetAmountCent(&amount))));

            AppendTable(table, row);
            PushWindow(NewInventoryEdit("库存编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个库存条目");
}

void InventoryPageLayout(struct nk_context *context, struct Window *window)
{
    struct MainWindowData *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);
    BasicFilterLayout(context, "库存", &data->dataArray[INVENTORY_INDEX]);
    nk_layout_row_static(context, 10, 0, 0);
    OperationLayout(context,
            OP_TYPE_GET | OP_TYPE_DETAIL | OP_TYPE_ADD | OP_TYPE_DELETE | OP_TYPE_UPDATE,
            (OperationHandler)SendInventoryRequest,
            (OperationHandler)InventoryLookup,
            (OperationHandler)InventoryAdd,
            (OperationHandler)ConfirmInventoryDelete,
            (OperationHandler)InventoryModify,
            data);
    DrawSeparateLine(context);
    char *from, *to;
    DateRangeFilterLayout(context, "筛选入库时间", &from, &to);
    DrawSeparateLine(context);
    PageResultLayout(context, &data->dataArray[INVENTORY_INDEX]);
}
