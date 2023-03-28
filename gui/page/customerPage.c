#include "design/crud.h"
#include "design/linkedList.h"
#include "design/operation.h"
#include "design/table.h"
#include "design/customerService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include "design/mainWindow.h"
#include <stddef.h>

void CustomerDelete(int ok, void *parameter)
{
    struct MainWindowData *data = parameter;
    Delete(ok, parameter, &data->dataArray[CUSTOMER_INDEX], DeleteCustomer, "缺少权限：删除客户", "客户编号", OP_DELETE_CUSTOMER);
}

void ConfirmCustomerDelete(struct MainWindowData *data)
{
    data->messageCallback = CustomerDelete;
    data->message = CloneString("是否确认要删除选中的客户条目");
}

void SendCustomerRequest(struct MainWindowData *data)
{
    Read(data, &data->dataArray[CUSTOMER_INDEX], GetAllCustomer, "缺少权限：读取客户", OP_READ_CUSTOMER);
}

void CustomerLookup(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[CUSTOMER_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[CUSTOMER_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->dataArray[CUSTOMER_INDEX].table));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(NewCustomerDetail("客户详情", table));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个客户条目");
}

void CustomerAdd(struct MainWindowData *data)
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, "客户等级");
    AppendTableRow(row, "客户姓名");
    AppendTableRow(row, "客户联系方式");
    Table *table = NewTable(row, "");

    row = NewTableRow();
    AppendTableRow(row, "1");
    AppendTableRow(row, "");
    AppendTableRow(row, "");
    AppendTable(table, row);

    PushWindow(NewCustomerEdit("客户编辑", data->id, data->password, table, 0));
    FreeTable(table);
}

void CustomerModify(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[CUSTOMER_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[CUSTOMER_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "客户编号");
            AppendTableRow(row, "客户等级");
            AppendTableRow(row, "客户姓名");
            AppendTableRow(row, "客户联系方式");
            Table *table = NewTable(row, "");

            row = NewTableRow();
            AppendTableRow(row,
                    GetRowItemByColumnName(data->dataArray[CUSTOMER_INDEX].table, rowNow->data, "客户编号"));
            AppendTableRow(row,
                    GetRowItemByColumnName(data->dataArray[CUSTOMER_INDEX].table, rowNow->data, "客户等级"));
            AppendTableRow(row,
                    GetRowItemByColumnName(data->dataArray[CUSTOMER_INDEX].table, rowNow->data, "客户姓名"));
            AppendTableRow(row,
                    GetRowItemByColumnName(data->dataArray[CUSTOMER_INDEX].table, rowNow->data, "客户联系方式"));
            AppendTable(table, row);

            PushWindow(NewCustomerEdit("客户编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个客户条目");
}

void CustomerPageLayout(struct nk_context *context, struct Window *window)
{
    struct MainWindowData *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);
    BasicFilterLayout(context, "客户", &data->dataArray[CUSTOMER_INDEX]);
    nk_layout_row_static(context, 10, 0, 0);
    OperationLayout(context,
            OP_TYPE_GET | OP_TYPE_DETAIL | OP_TYPE_ADD | OP_TYPE_DELETE | OP_TYPE_UPDATE,
            (OperationHandler)SendCustomerRequest,
            (OperationHandler)CustomerLookup,
            (OperationHandler)CustomerAdd,
            (OperationHandler)ConfirmCustomerDelete,
            (OperationHandler)CustomerModify,
            data);
    DrawSeparateLine(context);
    PageResultLayout(context, &data->dataArray[CUSTOMER_INDEX]);
}
