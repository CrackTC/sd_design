#include "design/crud.h"
#include "design/linkedList.h"
#include "design/operation.h"
#include "design/table.h"
#include "design/staffService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include "design/mainWindow.h"
#include <stddef.h>

void StaffDelete(int ok, void *parameter)
{
    struct MainWindowData *data = parameter;
    Delete(ok, parameter, &data->dataArray[STAFF_INDEX], DeleteStaff, "缺少权限：删除员工", "员工编号", OP_DELETE_STAFF);
}

void ConfirmStaffDelete(struct MainWindowData *data)
{
    data->messageCallback = StaffDelete;
    data->message = CloneString("是否确认要删除选中的员工条目");
}

void SendStaffRequest(struct MainWindowData *data)
{
    Read(data, &data->dataArray[STAFF_INDEX], GetItemOfAllStaff, "缺少权限：读取员工", OP_READ_STAFF);
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
    PageResultLayout(context, &data->dataArray[STAFF_INDEX], NULL, NULL, NULL);
}
