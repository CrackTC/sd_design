#include "design/amount.h"
#include "design/linkedList.h"
#include "design/operation.h"
#include "design/table.h"
#include "design/time.h"
#include "design/judgeService.h"
#include "design/inventoryService.h"
#include "design/journalService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include "design/mainWindow.h"
#include <malloc.h>

void SendItemRequest(struct MainWindowData *data)
{
    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_READ_ITEM);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("缺少权限：读取商品");
        return;
    }

    AddJournal(NULL, data->id, OP_READ_ITEM);
    Table *response = ShowItem(NULL);
    if (response != NULL)
    {
        if (response->remark != NULL && response->remark[0] != '\0')
        {
            data->messageCallback = MessageBoxCallback;
            data->message = CloneString(response->remark);
        }

        free(data->dataArray[ITEM_INDEX].properties);
        FreeList(data->dataArray[ITEM_INDEX].checkList);
        FreeTable(data->dataArray[ITEM_INDEX].table);
        data->dataArray[ITEM_INDEX].checkList = NewCheckList();
        data->dataArray[ITEM_INDEX].table = response;
        data->dataArray[ITEM_INDEX].properties = NULL;
        data->dataArray[ITEM_INDEX].propertySelected = 0;

        response = ShowLackInventory(NULL);
        if (response->rows->next != NULL)
        {
            PushWindow(NewResultDialog("缺货详情", response));
        }
    }
    else
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("查询失败: 响应为NULL");
    }
}

void ItemLookup(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[ITEM_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[ITEM_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->dataArray[ITEM_INDEX].table));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(NewItemDetail("商品详情", table));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个商品条目");
}

void ItemAdd(struct MainWindowData *data)
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, "商品名称");
    AppendTableRow(row, "天");
    AppendTableRow(row, "时");
    AppendTableRow(row, "元");
    AppendTableRow(row, "角");
    AppendTableRow(row, "分");
    Table *table = NewTable(row, "");

    row = NewTableRow();
    AppendTableRow(row, "");
    AppendTableRow(row, "1");
    AppendTableRow(row, "1");
    AppendTableRow(row, "2");
    AppendTableRow(row, "2");
    AppendTableRow(row, "2");
    AppendTable(table, row);

    PushWindow(NewItemEdit("商品编辑", data->id, data->password, table, 0));
    FreeTable(table);
}

void ItemModify(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[ITEM_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[ITEM_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();

            {
                AppendTableRow(row, "商品编号");
                AppendTableRow(row, "商品名称");
                AppendTableRow(row, "天");
                AppendTableRow(row, "时");
                AppendTableRow(row, "元");
                AppendTableRow(row, "角");
                AppendTableRow(row, "分");
            }

            Table *table = NewTable(row, "");

            {
                row = NewTableRow();
                AppendTableRow(row,
                        GetRowItemByColumnName(data->dataArray[ITEM_INDEX].table, rowNow->data, "商品编号"));
                AppendTableRow(row,
                        GetRowItemByColumnName(data->dataArray[ITEM_INDEX].table, rowNow->data, "商品名称"));

                const char *time = GetRowItemByColumnName(data->dataArray[ITEM_INDEX].table, rowNow->data, "保质期");
                TimeInfo info = ParseTime(time, 1);
                free(AppendTableRow(row, LongLongToString(info.day)));
                free(AppendTableRow(row, LongLongToString(info.hour)));

                Amount amount = ParseAmount(
                        GetRowItemByColumnName(data->dataArray[ITEM_INDEX].table, rowNow->data, "售价"));
                free(AppendTableRow(row, LongLongToString(GetAmountYuan(&amount))));
                free(AppendTableRow(row, LongLongToString(GetAmountJiao(&amount))));
                free(AppendTableRow(row, LongLongToString(GetAmountCent(&amount))));

                AppendTable(table, row);
            }

            PushWindow(NewItemEdit("商品编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个商品条目");
}

void ItemDelete(int ok, void *parameter)
{
    MessageBoxCallback(ok, parameter);
    if (ok == 0)
    {
        return;
    }

    struct MainWindowData *data = parameter;

    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_DELETE_ITEM);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("缺少权限：删除商品");
        return;
    }

    LinkedList *now = data->dataArray[ITEM_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[ITEM_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            char *id = GetRowItemByColumnName(data->dataArray[ITEM_INDEX].table, rowNow->data, "商品编号");

            TableRow *row = NewTableRow();
            AppendTableRow(row, "商品编号");
            Table *table = NewTable(row, NULL);

            row = NewTableRow();
            AppendTableRow(row, id);
            AppendTable(table, row);

            AddJournal(table, data->id, OP_DELETE_ITEM);
            Table *response = DeleteItemById(table);
            FreeTable(table);

            if (response != NULL)
            {
                int error = 0;
                if (response->remark != NULL && response->remark[0] != '\0')
                {
                    data->messageCallback = MessageBoxCallback;
                    data->message = CloneString(response->remark);
                    error = 1;
                }
                FreeTable(response);
                if (error)
                {
                    return;
                }
            }
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("删除成功");
}

void ConfirmItemDelete(struct MainWindowData *data)
{
    data->messageCallback = ItemDelete;
    data->message = CloneString("是否确认要删除选中的商品条目");
}

void ItemPageLayout(struct nk_context *context, struct Window *window)
{
    struct MainWindowData *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);
    BasicFilterLayout(context, "商品", &data->dataArray[ITEM_INDEX]);
    nk_layout_row_static(context, 10, 0, 0);
    OperationLayout(context,
            OP_TYPE_GET | OP_TYPE_DETAIL | OP_TYPE_ADD | OP_TYPE_DELETE | OP_TYPE_UPDATE,
            (OperationHandler)SendItemRequest,
            (OperationHandler)ItemLookup,
            (OperationHandler)ItemAdd,
            (OperationHandler)ConfirmItemDelete,
            (OperationHandler)ItemModify,
            data);
    DrawSeparateLine(context);
    PageResultLayout(context, &data->dataArray[ITEM_INDEX]);
}
