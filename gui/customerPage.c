#include "../data/linkedList.h"
#include "../data/operation.h"
#include "../data/table.h"
#include "../services/customerService.h"
#include "../services/journalService.h"
#include "../services/judgeService.h"
#include "../utils.h"
#include "config.h"
#include "layout.h"
#include "mainWindow.h"
#include "stddef.h"
#include <malloc.h>

static void MessageBoxCallBack(__attribute__((unused)) int ok, void *parameter)
{
    struct Data *data = parameter;
    free(data->message);
    data->message = NULL;
}

void CustomerDelete(int ok, void *parameter)
{
    MessageBoxCallBack(ok, parameter);
    if (ok == 0)
    {
        return;
    }

    struct Data *data = parameter;

    int hasPermission;
    judge(data->id, &hasPermission, data->password, OP_DELETE_CUSTOMER);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("缺少权限：删除客户");
        return;
    }

    LinkedList *now = data->customerCheckList->next;
    LinkedList *rowNow = data->customerTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            char *id = GetRowItemByColumnName(data->customerTable, rowNow->data, "id");

            TableRow *row = NewTableRow();
            AppendTableRow(row, "id");
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

void SendCustomerRequest(struct Data *data)
{
    int hasPermission;
    judge(data->id, &hasPermission, data->password, OP_READ_CUSTOMER);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("缺少权限：读取客户");
        return;
    }

    AddJournal(NULL, data->id, OP_READ_CUSTOMER);
    Table *response = GetAllCustomer(NULL);
    if (response != NULL)
    {
        if (response->remark != NULL && response->remark[0] != '\0')
        {
            data->messageCallback = MessageBoxCallBack;
            data->message = CloneString(response->remark);
        }

        FreeList(data->customerCheckList);
        data->customerCheckList = NewCheckList();
        data->customerTable = response;
    }
    else
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("查询失败: 响应为NULL");
    }
}

int CustomerLookup(struct Data *data)
{
    LinkedList *now = data->customerCheckList->next;
    LinkedList *rowNow = data->customerTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->customerTable));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(NewCustomerDetail("客户详情", table));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

void CustomerAdd(struct Data *data)
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

int CustomerModify(struct Data *data)
{
    LinkedList *now = data->customerCheckList->next;
    LinkedList *rowNow = data->customerTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "id");
            AppendTableRow(row, "客户等级");
            AppendTableRow(row, "客户姓名");
            AppendTableRow(row, "客户联系方式");
            Table *table = NewTable(row, "");

            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->customerTable, rowNow->data, "id"));
            AppendTableRow(row, GetRowItemByColumnName(data->customerTable, rowNow->data, "客户等级"));
            AppendTableRow(row, GetRowItemByColumnName(data->customerTable, rowNow->data, "客户姓名"));
            AppendTableRow(row, GetRowItemByColumnName(data->customerTable, rowNow->data, "客户联系方式"));
            AppendTable(table, row);

            PushWindow(NewCustomerEdit("客户编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

void CustomerPageLayout(struct nk_context *context, struct Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);

    // title
    nk_layout_row_dynamic(context, 0, 1);
    {
        if (nk_style_push_font(context, &fontLarge->handle))
        {
            nk_label(context, "客户", NK_TEXT_LEFT);
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
            TableRow *row = data->customerTable == NULL ? NULL : GetTableTitle(data->customerTable);
            columnCount = row == NULL ? 0 : row->columnCount;
            if (data->customerProperties == NULL)
            {
                data->customerProperties = malloc((columnCount + 1) * sizeof(char *));
                data->customerProperties[0] = "无";

                LinkedList *rowNow = row == NULL ? NULL : row->items;
                for (int i = 1; i < columnCount + 1; i++)
                {
                    data->customerProperties[i] = rowNow->data;
                    rowNow = rowNow->next;
                }
            }
        }

        nk_layout_row_push(context, 140);
        {
            if (nk_style_push_font(context, &fontSmall->handle))
            {
                nk_combobox(context, data->customerProperties, columnCount + 1, &data->customerPropertySelected, 35,
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
                                           data->customerValueBuffer, BUFFER_SIZE * sizeof(char), nk_filter_default);
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
            nk_layout_row_push(context, 0.15f);
            {
                if (nk_button_label(context, "查询"))
                {
                    SendCustomerRequest(data);
                }
            }

            nk_layout_row_push(context, 0.01f);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.15f);
            {
                if (nk_button_label(context, "查看"))
                {
                    if (!CustomerLookup(data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请选择一个客户条目");
                    }
                }
            }

            nk_layout_row_push(context, 0.01f);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.08f);
            {
                if (nk_button_label(context, "+"))
                {
                    CustomerAdd(data);
                }
            }

            nk_layout_row_push(context, 0.01f);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.08f);
            {
                if (nk_button_label(context, "-"))
                {
                    data->messageCallback = CustomerDelete;
                    data->message = CloneString("是否确认要删除选中的客户条目");
                }
            }

            nk_layout_row_push(context, 0.01f);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.08f);
            {
                if (nk_button_label(context, "~"))
                {
                    if (!CustomerModify(data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请选择一个客户条目");
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
                TableLayout(context, data->customerTable, data->customerCheckList,
                            data->customerPropertySelected == 0
                                ? NULL
                                : data->customerProperties[data->customerPropertySelected],
                            data->customerValueBuffer);
                nk_group_end(context);
            }

            nk_style_pop_font(context);
        }
    }
}
