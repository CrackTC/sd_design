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

void SendItemRequest(struct Data *data)
{
#warning
    data->messageCallback = MessageBoxCallBack;
    data->message = CloneString("缺少权限：读取商品");
    return;

    int hasPermission;
    data->itemTable = judge(data->id, &hasPermission, data->password, OP_READ_ITEM);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("缺少权限：读取商品");
        return;
    }

#warning finish read call
}

void ItemAdd(struct nk_context *context, struct Data *data)
{
    TableRow *row = NewTableRow();
    AppendTableRow(row, "商品名称");
    AppendTableRow(row, "数量");
    AppendTableRow(row, "年");
    AppendTableRow(row, "月");
    AppendTableRow(row, "日");
    AppendTableRow(row, "时");
    AppendTableRow(row, "分");
    AppendTableRow(row, "秒");
    AppendTableRow(row, "元");
    AppendTableRow(row, "角");
    AppendTableRow(row, "分");
    Table *table = NewTable(row, "");

    row = NewTableRow();
    AppendTableRow(row, "");
    AppendTableRow(row, "");
    AppendTableRow(row, "2023");
    AppendTableRow(row, "1");
    AppendTableRow(row, "1");
    AppendTableRow(row, "1");
    AppendTableRow(row, "1");
    AppendTableRow(row, "2");
    AppendTableRow(row, "2");
    AppendTableRow(row, "2");
    AppendTableRow(row, "2");
    AppendTable(table, row);

    PushWindow(context, NewItemEdit("商品编辑", data->id, data->password, table, 0));
    FreeTable(table);
}

int ItemModify(struct nk_context *context, struct Data *data)
{
    LinkedList *now = data->itemCheckList->next;
    LinkedList *rowNow = data->itemTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
#warning
            TableRow *row = NewTableRow();

            {
                AppendTableRow(row, "商品名称");
                AppendTableRow(row, "数量");
                AppendTableRow(row, "年");
                AppendTableRow(row, "月");
                AppendTableRow(row, "日");
                AppendTableRow(row, "时");
                AppendTableRow(row, "分");
                AppendTableRow(row, "秒");
                AppendTableRow(row, "元");
                AppendTableRow(row, "角");
                AppendTableRow(row, "分");
            }

            Table *table = NewTable(row, "");

            {
                row = NewTableRow();
                AppendTableRow(row, GetRowItemByColumnName(data->itemTable, rowNow->data, "商品名称"));
                AppendTableRow(row, GetRowItemByColumnName(data->itemTable, rowNow->data, "数量"));

                const char *time = GetRowItemByColumnName(data->itemTable, rowNow->data, "保质期");
                TimeInfo info = ParseTime(time, 1);
                free(AppendTableRow(row, LongLongToString(info.year)));
                free(AppendTableRow(row, LongLongToString(info.month)));
                free(AppendTableRow(row, LongLongToString(info.day)));
                free(AppendTableRow(row, LongLongToString(info.hour)));
                free(AppendTableRow(row, LongLongToString(info.minute)));
                free(AppendTableRow(row, LongLongToString(info.second)));

                Amount amount = ParseAmount(GetRowItemByColumnName(data->itemTable, rowNow->data, "售价"));
                free(AppendTableRow(row, LongLongToString(GetAmountYuan(&amount))));
                free(AppendTableRow(row, LongLongToString(GetAmountJiao(&amount))));
                free(AppendTableRow(row, LongLongToString(GetAmountCent(&amount))));

                AppendTable(table, row);
            }

            PushWindow(context, NewInventoryEdit("商品编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

void ItemDelete(int ok, void *parameter)
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
    judge(data->id, &hasPermission, data->password, OP_DELETE_ITEM);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("缺少权限：删除商品");
    }

    LinkedList *now = data->itemCheckList->next;
    LinkedList *rowNow = data->itemTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            char *id = GetRowItemByColumnName(data->itemTable, rowNow->data, "id");

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

void ItemPageLayout(struct nk_context *context, struct Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);

    // title
    nk_layout_row_dynamic(context, 0, 1);
    {
        if (nk_style_push_font(context, &fontLarge->handle))
        {
            nk_label(context, "商品", NK_TEXT_LEFT);
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
            TableRow *row = data->itemTable == NULL ? NULL : GetTableTitle(data->itemTable);
            columnCount = row == NULL ? 0 : row->columnCount;
            if (data->itemProperties == NULL)
            {
                data->itemProperties = malloc((columnCount + 1) * sizeof(char *));
                data->itemProperties[0] = "无";

                LinkedList *rowNow = row == NULL ? NULL : row->items;
                for (int i = 1; i < columnCount + 1; i++)
                {
                    data->itemProperties[i] = rowNow->data;
                    rowNow = rowNow->next;
                }
            }
        }

        nk_layout_row_push(context, 140);
        {
            if (nk_style_push_font(context, &fontSmall->handle))
            {
                nk_combobox(context, data->itemProperties, columnCount + 1, &data->itemPropertySelected, 35,
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
                                           data->itemValueBuffer, BUFFER_SIZE * sizeof(char), nk_filter_default);
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
                    SendItemRequest(data);
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
                    LinkedList *now = data->itemCheckList->next;
                    LinkedList *rowNow = data->itemTable->rows->next;
                    while (now != NULL)
                    {
                        if (*(int *)now->data == 1)
                        {
                            TableRow *titleRow = CloneRow(GetTableTitle(data->itemTable));
                            Table *table = NewTable(titleRow, "");
                            AppendTable(table, CloneRow(rowNow->data));
                            PushWindow(context, NewItemDetail("商品详情", table));
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
                    ItemAdd(context, data);
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
                    data->messageCallback = ItemDelete;
                    data->message = CloneString("是否确认要删除选中的商品条目");
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
                    if (!ItemModify(context, data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请选择一个商品条目");
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
                TableLayout(context, data->itemTable, data->itemCheckList,
                            data->itemPropertySelected == 0 ? NULL : data->itemProperties[data->itemPropertySelected],
                            data->itemValueBuffer);
                nk_group_end(context);
            }

            nk_style_pop_font(context);
        }
    }
}