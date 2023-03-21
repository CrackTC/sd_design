#include "../data/amount.h"
#include "../data/linkedList.h"
#include "../data/operation.h"
#include "../data/table.h"
#include "../data/time.h"
#include "../services/journalService.h"
#include "../services/judgeService.h"
#include "../services/staffService.h"
#include "../utils.h"
#include "config.h"
#include "layout.h"
#include "mainWindow.h"
#include "stddef.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

static void MessageBoxCallBack(int ok, void *parameter)
{
    struct Data *data = parameter;
    free(data->message);
    data->message = NULL;
}

void StaffDelete(int ok, void *parameter)
{
    MessageBoxCallBack(ok, parameter);
    if (ok == 0)
    {
        return;
    }

    struct Data *data = parameter;

    int hasPermission;
    judge(data->id, &hasPermission, data->password, OP_DELETE_STAFF);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("缺少权限：删除员工");
        return;
    }

    LinkedList *now = data->staffCheckList->next;
    LinkedList *rowNow = data->staffTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            char *id = GetRowItemByColumnName(data->staffTable, rowNow->data, "id");

            TableRow *row = NewTableRow();
            AppendTableRow(row, "id");
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
                    data->messageCallback = MessageBoxCallBack;
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
    data->messageCallback = MessageBoxCallBack;
    data->message = CloneString("删除成功");
}

void SendStaffRequest(struct Data *data)
{
    int hasPermission;
    judge(data->id, &hasPermission, data->password, OP_READ_STAFF);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("缺少权限：读取员工");
        return;
    }

    AddJournal(NULL, data->id, OP_READ_STAFF);
    Table *response = GetItemOfAllStaff(NULL);
    if (response != NULL)
    {
        if (response->remark != NULL && response->remark[0] != '\0')
        {
            data->messageCallback = MessageBoxCallBack;
            data->message = CloneString(response->remark);
        }

        FreeList(data->staffCheckList);
        data->staffCheckList = NewCheckList();
        data->staffTable = response;
    }
    else
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("查询失败: 响应为NULL");
    }
}

int StaffLookup(struct nk_context *context, struct Data *data)
{
    LinkedList *now = data->staffCheckList->next;
    LinkedList *rowNow = data->staffTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->staffTable));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(context, NewStaffDetail("员工详情", table));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

void StaffAdd(struct nk_context *context, struct Data *data)
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, "员工密码");
    AppendTableRow(row, "员工姓名");
    AppendTableRow(row, "员工联系方式");
    AppendTableRow(row, "已启用");
    AppendTableRow(row, "员工权限");
    Table *table = NewTable(row, "");

    row = NewTableRow();
    AppendTableRow(row, "");
    AppendTableRow(row, "");
    AppendTableRow(row, "");
    AppendTableRow(row, "1");
    AppendTableRow(row, "00000000000000000000000000000000000");
    AppendTable(table, row);

    PushWindow(context, NewStaffEdit("员工编辑", data->id, data->password, table, 0));
    FreeTable(table);
}

int StaffModify(struct nk_context *context, struct Data *data)
{
    LinkedList *now = data->staffCheckList->next;
    LinkedList *rowNow = data->staffTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "id");
            AppendTableRow(row, "员工密码");
            AppendTableRow(row, "员工姓名");
            AppendTableRow(row, "员工联系方式");
            AppendTableRow(row, "已启用");
            AppendTableRow(row, "员工权限");
            Table *table = NewTable(row, "");

            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->staffTable, rowNow->data, "id"));
            AppendTableRow(row, "");
            AppendTableRow(row, GetRowItemByColumnName(data->staffTable, rowNow->data, "员工姓名"));
            AppendTableRow(row, GetRowItemByColumnName(data->staffTable, rowNow->data, "员工联系方式"));
            AppendTableRow(row, GetRowItemByColumnName(data->staffTable, rowNow->data, "已启用"));
            AppendTableRow(row, GetRowItemByColumnName(data->staffTable, rowNow->data, "员工权限"));
            AppendTable(table, row);

            PushWindow(context, NewStaffEdit("员工编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

void StaffPageLayout(struct nk_context *context, struct Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);

    // title
    nk_layout_row_dynamic(context, 0, 1);
    {
        if (nk_style_push_font(context, &fontLarge->handle))
        {
            nk_label(context, "员工", NK_TEXT_LEFT);
            nk_style_pop_font(context);
        }
    }

    // filter
    nk_layout_row_begin(context, NK_STATIC, 35, 5);
    {
        nk_layout_row_push(context, 100);
        {
            nk_label(context, "通过条件 ", NK_TEXT_LEFT);
        }

        int columnCount;
        {
            TableRow *row = data->staffTable == NULL ? NULL : GetTableTitle(data->staffTable);
            columnCount = row == NULL ? 0 : row->columnCount;
            if (data->staffProperties == NULL)
            {
                data->staffProperties = malloc((columnCount + 1) * sizeof(char *));
                data->staffProperties[0] = "无";

                LinkedList *rowNow = row == NULL ? NULL : row->items;
                for (int i = 1; i < columnCount + 1; i++)
                {
                    data->staffProperties[i] = rowNow->data;
                    rowNow = rowNow->next;
                }
            }
        }

        nk_layout_row_push(context, 140);
        {
            if (nk_style_push_font(context, &fontSmall->handle))
            {
                nk_combobox(context, data->staffProperties, columnCount + 1, &data->staffPropertySelected, 35,
                            nk_vec2(200, 400));
                nk_style_pop_font(context);
            }
        }

        nk_layout_row_push(context, 30);
        {
            nk_label(context, "为", NK_TEXT_CENTERED);
        }

        nk_layout_row_push(context, 200);
        {
            nk_edit_string_zero_terminated(context,
                                           (NK_EDIT_BOX | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD) & ~NK_EDIT_MULTILINE,
                                           data->staffValueBuffer, BUFFER_SIZE * sizeof(char), nk_filter_default);
        }

        nk_layout_row_push(context, 100);
        {
            nk_label(context, "进行筛选", NK_TEXT_LEFT);
        }

        nk_layout_row_end(context);
    }

    nk_layout_row_static(context, 10, 0, 0);

    nk_layout_row_begin(context, NK_DYNAMIC, 35, 10);
    {
        if (nk_style_push_font(context, &fontSmall->handle))
        {
            nk_layout_row_push(context, 0.15);
            {
                if (nk_button_label(context, "查询"))
                {
                    SendStaffRequest(data);
                }
            }

            nk_layout_row_push(context, 0.01);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.15);
            {
                if (nk_button_label(context, "查看"))
                {
                    if (!StaffLookup(context, data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请选择一个员工条目");
                    }
                }
            }

            nk_layout_row_push(context, 0.01);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.08);
            {
                if (nk_button_label(context, "+"))
                {
                    StaffAdd(context, data);
                }
            }

            nk_layout_row_push(context, 0.01);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.08);
            {
                if (nk_button_label(context, "-"))
                {
                    data->messageCallback = StaffDelete;
                    data->message = CloneString("是否确认要删除选中的员工条目");
                }
            }

            nk_layout_row_push(context, 0.01);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.08);
            {
                if (nk_button_label(context, "~"))
                {
                    if (!StaffModify(context, data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请选择一个员工条目");
                    }
                }
            }

            nk_style_pop_font(context);
        }
        nk_layout_row_end(context);
    }

    nk_layout_row_dynamic(context, 10, 1);
    {
        struct nk_rect space;
        nk_widget(&space, context);
        struct nk_command_buffer *canvas = nk_window_get_canvas(context);
        nk_stroke_line(canvas, space.x, space.y + space.h / 2, space.x + space.w, space.y + space.h / 2, 1,
                       nk_rgb(100, 100, 100));
    }

    nk_layout_row_dynamic(context, nk_window_get_height(context) - 285, 1);
    {
        if (nk_style_push_font(context, &fontSmall->handle))
        {
            if (nk_group_begin(context, "查询结果", NK_WINDOW_BORDER))
            {
                TableLayout(context, data->staffTable, data->staffCheckList,
                            data->staffPropertySelected == 0 ? NULL
                                                             : data->staffProperties[data->staffPropertySelected],
                            data->staffValueBuffer);
                nk_group_end(context);
            }

            nk_style_pop_font(context);
        }
    }
}
