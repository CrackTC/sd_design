#include "design/linkedList.h"
#include "design/operation.h"
#include "design/table.h"
#include "design/customerService.h"
#include "design/journalService.h"
#include "design/judgeService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include "design/mainWindow.h"
#include <stddef.h>
#include <malloc.h>

void CustomerDelete(int ok, void *parameter)
{
    MessageBoxCallback(ok, parameter);
    if (ok == 0)
    {
        return;
    }

    struct MainWindowData *data = parameter;

    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_DELETE_CUSTOMER);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("缺少权限：删除客户");
        return;
    }

    LinkedList *now = data->dataArray[CUSTOMER_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[CUSTOMER_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            char *id = GetRowItemByColumnName(data->dataArray[CUSTOMER_INDEX].table, rowNow->data, "客户编号");

            TableRow *row = NewTableRow();
            AppendTableRow(row, "客户编号");
            Table *table = NewTable(row, NULL);

            row = NewTableRow();
            AppendTableRow(row, id);
            AppendTable(table, row);

            AddJournal(table, data->id, OP_DELETE_CUSTOMER);
            Table *response = DeleteCustomer(table);
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

void ConfirmCustomerDelete(struct MainWindowData *data)
{
    data->messageCallback = CustomerDelete;
    data->message = CloneString("是否确认要删除选中的客户条目");
}

void SendCustomerRequest(struct MainWindowData *data)
{
    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_READ_CUSTOMER);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("缺少权限：读取客户");
        return;
    }

    AddJournal(NULL, data->id, OP_READ_CUSTOMER);
    Table *response = GetAllCustomer(NULL);
    if (response != NULL)
    {
        if (response->remark != NULL && response->remark[0] != '\0')
        {
            data->messageCallback = MessageBoxCallback;
            data->message = CloneString(response->remark);
        }

        free(data->dataArray[CUSTOMER_INDEX].properties);
        FreeTable(data->dataArray[CUSTOMER_INDEX].table);
        FreeList(data->dataArray[CUSTOMER_INDEX].checkList);
        data->dataArray[CUSTOMER_INDEX].checkList = NewCheckList();
        data->dataArray[CUSTOMER_INDEX].table = response;
        data->dataArray[CUSTOMER_INDEX].properties = NULL;
        data->dataArray[CUSTOMER_INDEX].propertySelected = 0;
    }
    else
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("查询失败: 响应为NULL");
    }
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
