#include "../data/amount.h"
#include "../data/linkedList.h"
#include "../data/operation.h"
#include "../data/table.h"
#include "../data/time.h"
#include "../services/judgeService.h"
#include "../utils.h"
#include "config.h"
#include "layout.h"
#include "mainWindow.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

static void MessageBoxCallBack(int ok, void *parameter)
{
    struct Data *data = parameter;
    free(data->message);
    data->message = NULL;
}

void SendOrderRequest(struct Data *data)
{
#warning
    data->messageCallback = MessageBoxCallBack;
    data->message = CloneString("缺少权限：读取订单");
    return;

    int hasPermission;
    data->orderTable = judge(data->id, &hasPermission, data->password, OP_READ_ORDER);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("缺少权限：读取订单");
        return;
    }

#warning finish read call
}

int OrderAdd(struct nk_context *context, struct Data *data)
{
    LinkedList *itemCheckNow = data->itemCheckList->next;
    LinkedList *itemRowNow = data->itemTable->rows->next;
    while (itemCheckNow != NULL)
    {
        if (*(int *)itemCheckNow->data == 1)
        {
            LinkedList *customerCheckNow = data->customerCheckList->next;
            LinkedList *customerRowNow = data->customerCheckList->next;
            while (customerCheckNow != NULL)
            {
                if (*(int *)customerCheckNow->data == 1)
                {
                    TableRow *row = NewTableRow();
                    AppendTableRow(row, "商品编号");
                    AppendTableRow(row, "商品名称");
                    AppendTableRow(row, "客户编号");
                    AppendTableRow(row, "客户姓名");
                    AppendTableRow(row, "购买数量");
                    Table *table = NewTable(row, "");

                    row = NewTableRow();
                    AppendTableRow(row, GetRowItemByColumnName(data->itemTable, itemRowNow->data, "商品编号"));
                    AppendTableRow(row, GetRowItemByColumnName(data->itemTable, itemRowNow->data, "商品名称"));
                    AppendTableRow(row, GetRowItemByColumnName(data->customerTable, customerRowNow->data, "客户编号"));
                    AppendTableRow(row, GetRowItemByColumnName(data->customerTable, customerRowNow->data, "客户姓名"));
                    AppendTableRow(row, "0");
                    AppendTable(table, row);

                    PushWindow(context, NewOrderEdit("订单编辑", data->id, data->password, table, 0));
                    FreeTable(table);
                    return 1;
                }
                customerCheckNow = customerCheckNow->next;
                customerRowNow = customerRowNow->next;
            }
            return 0;
        }
        itemCheckNow = itemCheckNow->next;
        itemRowNow = itemRowNow->next;
    }
    return 0;
}

int OrderModify(struct nk_context *context, struct Data *data)
{
    LinkedList *now = data->orderCheckList->next;
    LinkedList *rowNow = data->orderTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
#warning
            TableRow *row = NewTableRow();

            {
                AppendTableRow(row, "商品编号");
                AppendTableRow(row, "商品名称");
                AppendTableRow(row, "客户编号");
                AppendTableRow(row, "客户姓名");
                AppendTableRow(row, "购买数量");
            }

            Table *table = NewTable(row, "");

            {
                row = NewTableRow();
                AppendTableRow(row, GetRowItemByColumnName(data->orderTable, rowNow->data, "商品编号"));
                AppendTableRow(row, GetRowItemByColumnName(data->orderTable, rowNow->data, "商品名称"));
                AppendTableRow(row, GetRowItemByColumnName(data->orderTable, rowNow->data, "客户编号"));
                AppendTableRow(row, GetRowItemByColumnName(data->orderTable, rowNow->data, "客户姓名"));
                AppendTableRow(row, GetRowItemByColumnName(data->orderTable, rowNow->data, "购买数量"));
                AppendTable(table, row);
            }

            PushWindow(context, NewOrderEdit("订单编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

void OrderDelete(int ok, void *parameter)
{
    if (ok == 0)
    {
        MessageBoxCallBack(ok, parameter);
        return;
    }
#warning
    struct Data *data = parameter;
    data->messageCallback = MessageBoxCallBack;
    data->message = CloneString("Successfully deleted!");
    return;

    int hasPermission;
    judge(data->id, &hasPermission, data->password, OP_DELETE_ORDER);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("缺少权限：删除订单");
    }

    LinkedList *now = data->orderCheckList->next;
    LinkedList *rowNow = data->orderTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            char *id = GetRowItemByColumnName(data->orderTable, rowNow->data, "id");

            TableRow *row = NewTableRow();
            AppendTableRow(row, "id");
            Table *table = NewTable(row, NULL);
            row = NewTableRow();
            AppendTableRow(row, id);
            AppendTable(table, row);
#warning finish deletion call

            FreeTable(table);
        }
        now = now->next;
        rowNow = rowNow->next;
    }

    MessageBoxCallBack(ok, parameter);
}

void OrderPageLayout(struct nk_context *context, struct Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);

    // title
    nk_layout_row_dynamic(context, 0, 1);
    {
        if (nk_style_push_font(context, &fontLarge->handle))
        {
            nk_label(context, "订单", NK_TEXT_LEFT);
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
            TableRow *row = data->orderTable == NULL ? NULL : GetTableTitle(data->orderTable);
            columnCount = row == NULL ? 0 : row->columnCount;
            if (data->orderProperties == NULL)
            {
                data->orderProperties = malloc((columnCount + 1) * sizeof(char *));
                data->orderProperties[0] = "无";

                LinkedList *rowNow = row == NULL ? NULL : row->items;
                for (int i = 1; i < columnCount + 1; i++)
                {
                    data->orderProperties[i] = rowNow->data;
                    rowNow = rowNow->next;
                }
            }
        }

        nk_layout_row_push(context, 140);
        {
            if (nk_style_push_font(context, &fontSmall->handle))
            {
                nk_combobox(context, data->orderProperties, columnCount + 1, &data->orderPropertySelected, 35,
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
                                           data->orderValueBuffer, BUFFER_SIZE * sizeof(char), nk_filter_default);
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
                    SendOrderRequest(data);
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
                    LinkedList *now = data->orderCheckList->next;
                    LinkedList *rowNow = data->orderTable->rows->next;
                    while (now != NULL)
                    {
                        if (*(int *)now->data == 1)
                        {
                            TableRow *titleRow = CloneRow(GetTableTitle(data->orderTable));
                            Table *table = NewTable(titleRow, "");
                            AppendTable(table, CloneRow(rowNow->data));
                            PushWindow(context, NewOrderDetail("订单详情", table));
                            FreeTable(table);
                            break;
                        }
                        now = now->next;
                        rowNow = rowNow->next;
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
                    if (!OrderAdd(context, data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请确保在商品页面选择一个商品条目，且在客户页面选择一个客户条目");
                    }
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
                    data->messageCallback = OrderDelete;
                    data->message = CloneString("是否确认要删除选中的订单条目");
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
                    if (!OrderModify(context, data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请选择一个订单条目");
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
                TableLayout(context, data->orderTable, data->orderCheckList,
                            data->orderPropertySelected == 0 ? NULL
                                                             : data->orderProperties[data->orderPropertySelected],
                            data->orderValueBuffer);
                nk_group_end(context);
            }

            nk_style_pop_font(context);
        }
    }
}
