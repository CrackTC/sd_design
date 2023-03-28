#include "design/linkedList.h"
#include "design/operation.h"
#include "design/table.h"
#include "design/journalService.h"
#include "design/judgeService.h"
#include "design/staffService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include "design/mainWindow.h"
#include <stddef.h>
#include <malloc.h>

void StaffDelete(int ok, void *parameter)
{
    MessageBoxCallback(ok, parameter);
    if (ok == 0)
    {
        return;
    }

    struct MainWindowData *data = parameter;

    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_DELETE_STAFF);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("缺少权限：删除员工");
        return;
    }

    LinkedList *now = data->dataArray[STAFF_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[STAFF_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            char *id = GetRowItemByColumnName(data->dataArray[STAFF_INDEX].table, rowNow->data, "员工编号");

            TableRow *row = NewTableRow();
            AppendTableRow(row, "员工编号");
            Table *table = NewTable(row, NULL);
            row = NewTableRow();
            AppendTableRow(row, id);
            AppendTable(table, row);

            AddJournal(table, data->id, OP_DELETE_STAFF);
            Table *response = DeleteStaff(table);
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

void ConfirmStaffDelete(struct MainWindowData *data)
{
    data->messageCallback = StaffDelete;
    data->message = CloneString("是否确认要删除选中的员工条目");
}

void SendStaffRequest(struct MainWindowData *data)
{
    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_READ_STAFF);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("缺少权限：读取员工");
        return;
    }

    AddJournal(NULL, data->id, OP_READ_STAFF);
    Table *response = GetItemOfAllStaff(NULL);
    if (response != NULL)
    {
        if (response->remark != NULL && response->remark[0] != '\0')
        {
            data->messageCallback = MessageBoxCallback;
            data->message = CloneString(response->remark);
        }

        free(data->dataArray[STAFF_INDEX].properties);
        FreeList(data->dataArray[STAFF_INDEX].checkList);
        FreeTable(data->dataArray[STAFF_INDEX].table);
        data->dataArray[STAFF_INDEX].checkList = NewCheckList();
        data->dataArray[STAFF_INDEX].table = response;
        data->dataArray[STAFF_INDEX].properties = NULL;
        data->dataArray[STAFF_INDEX].propertySelected = 0;
    }
    else
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("查询失败: 响应为NULL");
    }
}

void StaffLookup(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[STAFF_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[STAFF_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->dataArray[STAFF_INDEX].table));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(NewStaffDetail("员工详情", table));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个员工条目");
}

void StaffAdd(struct MainWindowData *data)
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, "员工密码");
    AppendTableRow(row, "员工姓名");
    AppendTableRow(row, "员工联系方式");
    AppendTableRow(row, "员工可用性");
    AppendTableRow(row, "员工权限");
    Table *table = NewTable(row, "");

    row = NewTableRow();
    AppendTableRow(row, "");
    AppendTableRow(row, "");
    AppendTableRow(row, "");
    AppendTableRow(row, "1");
    AppendTableRow(row, "00000000000000000000000000000000000");
    AppendTable(table, row);

    PushWindow(NewStaffEdit("员工编辑", data->id, data->password, table, 0));
    FreeTable(table);
}

void StaffModify(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[STAFF_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[STAFF_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "员工编号");
            AppendTableRow(row, "员工密码");
            AppendTableRow(row, "员工姓名");
            AppendTableRow(row, "员工联系方式");
            AppendTableRow(row, "员工可用性");
            AppendTableRow(row, "员工权限");
            Table *table = NewTable(row, "");

            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[STAFF_INDEX].table, rowNow->data, "员工编号"));
            AppendTableRow(row, "");
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[STAFF_INDEX].table, rowNow->data, "员工姓名"));
            AppendTableRow(row,
                    GetRowItemByColumnName(data->dataArray[STAFF_INDEX].table, rowNow->data, "员工联系方式"));
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[STAFF_INDEX].table, rowNow->data, "员工可用性"));
            AppendTableRow(row, GetRowItemByColumnName(data->dataArray[STAFF_INDEX].table, rowNow->data, "员工权限"));
            AppendTable(table, row);

            PushWindow(NewStaffEdit("员工编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个员工条目");
}

void StaffPageLayout(struct nk_context *context, struct Window *window)
{
    struct MainWindowData *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);
    BasicFilterLayout(context, "员工", &data->dataArray[STAFF_INDEX]);
    nk_layout_row_static(context, 10, 0, 0);
    OperationLayout(context,
            OP_TYPE_GET | OP_TYPE_DETAIL | OP_TYPE_ADD | OP_TYPE_DELETE | OP_TYPE_UPDATE,
            (OperationHandler)SendStaffRequest,
            (OperationHandler)StaffLookup,
            (OperationHandler)StaffAdd,
            (OperationHandler)ConfirmStaffDelete,
            (OperationHandler)StaffModify,
            data);
    DrawSeparateLine(context);
    PageResultLayout(context, &data->dataArray[STAFF_INDEX]);
}
