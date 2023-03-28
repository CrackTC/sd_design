#include "design/amount.h"
#include "design/linkedList.h"
#include "design/operation.h"
#include "design/table.h"
#include "design/journalService.h"
#include "design/judgeService.h"
#include "design/saleService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include "design/mainWindow.h"
#include <stddef.h>
#include <malloc.h>

void RefundDelete(int ok, void *parameter)
{
    MessageBoxCallback(ok, parameter);
    if (ok == 0)
    {
        return;
    }

    struct MainWindowData *data = parameter;

    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_DELETE_REFUND);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("缺少权限：删除退款条目");
        return;
    }

    LinkedList *now = data->dataArray[REFUND_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[REFUND_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            char *id = GetRowItemByColumnName(data->dataArray[REFUND_INDEX].table, rowNow->data, "订单编号");

            TableRow *row = NewTableRow();
            AppendTableRow(row, "订单编号");
            Table *table = NewTable(row, NULL);
            row = NewTableRow();
            AppendTableRow(row, id);
            AppendTable(table, row);

            AddJournal(table, data->id, OP_DELETE_REFUND);
            Table *response = RemoveRefund(table);
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

void ConfirmRefundDelete(struct MainWindowData *data)
{
    data->messageCallback = RefundDelete;
    data->message = CloneString("是否确认要删除选中的退款条目");
}

void SendRefundRequest(struct MainWindowData *data)
{
    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_READ_REFUND);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("缺少权限：读取退款");
        return;
    }

    AddJournal(NULL, data->id, OP_READ_REFUND);
    Table *response = GetAllRefund(NULL);
    if (response != NULL)
    {
        if (response->remark != NULL && response->remark[0] != '\0')
        {
            data->messageCallback = MessageBoxCallback;
            data->message = CloneString(response->remark);
        }

        free(data->dataArray[REFUND_INDEX].properties);
        FreeList(data->dataArray[REFUND_INDEX].checkList);
        FreeTable(data->dataArray[REFUND_INDEX].table);
        data->dataArray[REFUND_INDEX].checkList = NewCheckList();
        data->dataArray[REFUND_INDEX].table = response;
        data->dataArray[REFUND_INDEX].properties = NULL;
        data->dataArray[REFUND_INDEX].propertySelected = 0;
    }
    else
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("查询失败: 响应为NULL");
    }
}

void RefundLookup(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[REFUND_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[REFUND_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->dataArray[REFUND_INDEX].table));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(NewRefundDetail("退款详情", table));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个退款条目");
}

void RefundAdd(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[ORDER_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[ORDER_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "订单编号");
            AppendTableRow(row, "退款原因");
            AppendTableRow(row, "退回数目");
            AppendTableRow(row, "元");
            AppendTableRow(row, "角");
            AppendTableRow(row, "分");
            AppendTableRow(row, "备注");
            Table *table = NewTable(row, NULL);

            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[ORDER_INDEX].table, rowNow->data, "订单编号"));
            AppendTableRow(row, "");

            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[ORDER_INDEX].table, rowNow->data, "购买数量"));

            Amount amount = ParseAmount(GetRowItemByColumnName(data->dataArray[ORDER_INDEX].table, rowNow->data, "总价"));
            free(AppendTableRow(row, LongLongToString(GetAmountYuan(&amount))));
            free(AppendTableRow(row, LongLongToString(GetAmountJiao(&amount))));
            free(AppendTableRow(row, LongLongToString(GetAmountCent(&amount))));

            AppendTableRow(row, "");
            AppendTable(table, row);

            PushWindow(NewRefundEdit("退款编辑", data->id, data->password, table, 0));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请在订单页面选择一个订单条目");
}

void RefundModify(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[REFUND_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[REFUND_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "订单编号");
            AppendTableRow(row, "退款原因");
            AppendTableRow(row, "退回数目");
            AppendTableRow(row, "元");
            AppendTableRow(row, "角");
            AppendTableRow(row, "分");
            AppendTableRow(row, "备注");
            Table *table = NewTable(row, NULL);

            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[REFUND_INDEX].table, rowNow->data, "订单编号"));
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[REFUND_INDEX].table, rowNow->data, "退款原因"));
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[REFUND_INDEX].table, rowNow->data, "退回数目"));

            Amount amount = ParseAmount(GetRowItemByColumnName(data->dataArray[REFUND_INDEX].table, rowNow->data, "退款"));
            free(AppendTableRow(row, LongLongToString(GetAmountYuan(&amount))));
            free(AppendTableRow(row, LongLongToString(GetAmountJiao(&amount))));
            free(AppendTableRow(row, LongLongToString(GetAmountCent(&amount))));

            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[REFUND_INDEX].table, rowNow->data, "备注"));
            AppendTable(table, row);

            PushWindow(NewRefundEdit("退款编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个退款条目");
}

void RefundPageLayout(struct nk_context *context, struct Window *window)
{
    struct MainWindowData *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);

    BasicFilterLayout(context, "退款", &data->dataArray[REFUND_INDEX]);

    nk_layout_row_static(context, 10, 0, 0);

    OperationLayout(context,
            OP_TYPE_GET | OP_TYPE_DETAIL | OP_TYPE_ADD | OP_TYPE_DELETE | OP_TYPE_UPDATE,
            (OperationHandler)SendRefundRequest,
            (OperationHandler)RefundLookup,
            (OperationHandler)RefundAdd,
            (OperationHandler)ConfirmRefundDelete,
            (OperationHandler)RefundModify,
            data);
    DrawSeparateLine(context);
    char *from, *to;
    DateRangeFilterLayout(context, "筛选退款时间", &from, &to);
    DrawSeparateLine(context);
    PageResultLayout(context, &data->dataArray[REFUND_INDEX]);
}
