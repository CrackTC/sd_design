#include "design/linkedList.h"
#include "design/operation.h"
#include "design/table.h"
#include "design/time.h"
#include "design/journalService.h"
#include "design/judgeService.h"
#include "design/saleService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include "design/mainWindow.h"
#include <stddef.h>
#include <malloc.h>

void DiscountDelete(int ok, void *parameter)
{
    MessageBoxCallback(ok, parameter);
    if (ok == 0)
    {
        return;
    }

    struct MainWindowData *data = parameter;

    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_DELETE_DISCOUNT);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("缺少权限：删除折扣");
        return;
    }

    LinkedList *now = data->dataArray[DISCOUNT_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[DISCOUNT_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            char *id = GetRowItemByColumnName(data->dataArray[DISCOUNT_INDEX].table, rowNow->data, "折扣编号");

            TableRow *row = NewTableRow();
            AppendTableRow(row, "折扣编号");
            Table *table = NewTable(row, NULL);

            row = NewTableRow();
            AppendTableRow(row, id);
            AppendTable(table, row);

            AddJournal(table, data->id, OP_DELETE_DISCOUNT);
            Table *response = RemoveDiscount(table);
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

void ConfirmDiscountDelete(struct MainWindowData *data)
{
    data->messageCallback = DiscountDelete;
    data->message = CloneString("是否确认要删除选中的折扣条目");
}

void SendDiscountRequest(struct MainWindowData *data)
{
    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_READ_DISCOUNT);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("缺少权限：读取折扣");
        return;
    }

    AddJournal(NULL, data->id, OP_READ_DISCOUNT);
    Table *response = GetAllDiscount(NULL);
    if (response != NULL)
    {
        if (response->remark != NULL && response->remark[0] != '\0')
        {
            data->messageCallback = MessageBoxCallback;
            data->message = CloneString(response->remark);
        }

        free(data->dataArray[DISCOUNT_INDEX].properties);
        FreeTable(data->dataArray[DISCOUNT_INDEX].table);
        FreeList(data->dataArray[DISCOUNT_INDEX].checkList);
        data->dataArray[DISCOUNT_INDEX].checkList = NewCheckList();
        data->dataArray[DISCOUNT_INDEX].table = response;
        data->dataArray[DISCOUNT_INDEX].properties = NULL;
        data->dataArray[DISCOUNT_INDEX].propertySelected = 0;
    }
    else
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("查询失败: 响应为NULL");
    }
}

void DiscountLookup(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[DISCOUNT_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[DISCOUNT_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->dataArray[DISCOUNT_INDEX].table));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(NewDiscountDetail("折扣详情", table));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个折扣条目");
}

void DiscountAdd(struct MainWindowData *data)
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
            AppendTableRow(row, "折扣比率");
            AppendTableRow(row, "客户等级");
            AppendTableRow(row, "年");
            AppendTableRow(row, "月");
            AppendTableRow(row, "日");
            AppendTableRow(row, "时");
            AppendTableRow(row, "分");
            AppendTableRow(row, "秒");
            Table *table = NewTable(row, "");

            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[ITEM_INDEX].table, rowNow->data, "商品编号"));
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[ITEM_INDEX].table, rowNow->data, "商品名称"));
            AppendTableRow(row, "100");
            AppendTableRow(row, "1");
            AppendTableRow(row, "");
            AppendTableRow(row, "");
            AppendTableRow(row, "");
            AppendTableRow(row, "");
            AppendTableRow(row, "");
            AppendTableRow(row, "");
            AppendTable(table, row);
            PushWindow(NewDiscountEdit("折扣编辑", data->id, data->password, table, 0));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请在商品页面选择一个商品条目");
}

void DiscountModify(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[DISCOUNT_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[DISCOUNT_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "折扣编号");
            AppendTableRow(row, "商品编号");
            AppendTableRow(row, "商品名称");
            AppendTableRow(row, "折扣比率");
            AppendTableRow(row, "客户等级");
            AppendTableRow(row, "年");
            AppendTableRow(row, "月");
            AppendTableRow(row, "日");
            AppendTableRow(row, "时");
            AppendTableRow(row, "分");
            AppendTableRow(row, "秒");
            Table *table = NewTable(row, NULL);

            row = NewTableRow();
            AppendTableRow(row,
                    GetRowItemByColumnName(data->dataArray[DISCOUNT_INDEX].table, rowNow->data, "折扣编号"));
            AppendTableRow(row,
                    GetRowItemByColumnName(data->dataArray[DISCOUNT_INDEX].table, rowNow->data, "商品编号"));
            AppendTableRow(row,
                    GetRowItemByColumnName(data->dataArray[DISCOUNT_INDEX].table, rowNow->data, "商品名称"));
            AppendTableRow(row,
                    GetRowItemByColumnName(data->dataArray[DISCOUNT_INDEX].table, rowNow->data, "折扣比率"));
            AppendTableRow(row,
                    GetRowItemByColumnName(data->dataArray[DISCOUNT_INDEX].table, rowNow->data, "客户等级"));

            const char *time = GetRowItemByColumnName(data->dataArray[DISCOUNT_INDEX].table, rowNow->data, "截止时间");
            TimeInfo info = ParseTime(time, 0);
            free(AppendTableRow(row, LongLongToString(info.year)));
            free(AppendTableRow(row, LongLongToString(info.month)));
            free(AppendTableRow(row, LongLongToString(info.day)));
            free(AppendTableRow(row, LongLongToString(info.hour)));
            free(AppendTableRow(row, LongLongToString(info.minute)));
            free(AppendTableRow(row, LongLongToString(info.second)));

            AppendTable(table, row);
            PushWindow(NewDiscountEdit("折扣编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个折扣条目");
}

void DiscountPageLayout(struct nk_context *context, struct Window *window)
{
    struct MainWindowData *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);
    BasicFilterLayout(context, "折扣", &data->dataArray[DISCOUNT_INDEX]);
    nk_layout_row_static(context, 10, 0, 0);
    OperationLayout(context, OP_TYPE_GET | OP_TYPE_DETAIL | OP_TYPE_ADD | OP_TYPE_DELETE | OP_TYPE_UPDATE,
            (OperationHandler)SendDiscountRequest,
            (OperationHandler)DiscountLookup, (OperationHandler)DiscountAdd, (OperationHandler)ConfirmDiscountDelete,
            (OperationHandler)DiscountModify, data);
    DrawSeparateLine(context);
    char *from, *to;
    DateRangeFilterLayout(context, "筛选截止时间", &from, &to);
    DrawSeparateLine(context);
    PageResultLayout(context, &data->dataArray[DISCOUNT_INDEX]);
}
