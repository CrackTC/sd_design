#include "../data/linkedList.h"
#include "../data/operation.h"
#include "../data/table.h"
#include "../services/judgeService.h"
#include "../utils.h"
#include "config.h"
#include "layout.h"
#include "mainWindow.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

void SendItemRequest(struct Data *data)
{
#warning
    data->message = CloneString("缺少权限：读取商品");
    return;

    int hasPermission;
    data->itemTable = judge(data->id, &hasPermission, data->password, OP_READ_ITEM, NULL);
    if (!hasPermission)
    {
        data->message = CloneString("缺少权限：读取商品");
    }
}

static void MessageBoxCallBack(void *parameter)
{
    struct Data *data = parameter;
    data->message = NULL;
}

void ItemPageLayout(struct nk_context *context, struct Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, MessageBoxCallBack, data);

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
