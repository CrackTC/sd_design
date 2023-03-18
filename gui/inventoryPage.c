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

void SendInventoryAddRequest(struct Data *data)
{
    printf("here\n");
}

void SendInventoryRequest(struct Data *data)
{
#warning
    data->message = CloneString("缺少权限：读取库存");
    return;

    int hasPermission;
    data->inventoryTable = judge(data->id, &hasPermission, data->password, OP_READ_INVENTORY, NULL);
    if (!hasPermission)
    {
        data->message = CloneString("缺少权限：读取库存");
    }
}

void SendItemRequest(struct Data *data)
{
#warning
    data->message = CloneString("缺少权限：读取商品");
    return;

    int hasPermission;
    data->inventoryTable = judge(data->id, &hasPermission, data->password, OP_READ_ITEM, NULL);
    if (!hasPermission)
    {
        data->message = CloneString("缺少权限：读取商品");
    }
}

void InventoryPageLayout(struct nk_context *context, struct Window *window)
{
    struct Data *data = window->data;

    // title
    nk_layout_row_dynamic(context, 0, 1);
    {
        if (nk_style_push_font(context, &fontLarge->handle))
        {
            nk_label(context, "库存", NK_TEXT_LEFT);
            nk_style_pop_font(context);
        }
    }

    // radio
    nk_layout_row_begin(context, NK_STATIC, 35, 3);
    {
        nk_layout_row_push(context, 50);
        {
            nk_label(context, "展示", NK_TEXT_LEFT);
        }

        static const char *const radioOptions[] = {"库存", "商品"};

        if (nk_style_push_font(context, &fontSmall->handle))
        {
            for (int i = 0; i < 2; i++)
            {
                nk_layout_row_push(context, 80);
                {
                    if (nk_option_label(context, radioOptions[i], data->inventorySelectedRadio == i))
                    {
                        data->inventorySelectedRadio = i;
                    }
                }
            }

            nk_style_pop_font(context);
        }

        nk_layout_row_end(context);
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
            TableRow *row = data->inventoryTable == NULL ? NULL : GetTableTitle(data->inventoryTable);
            columnCount = row == NULL ? 0 : row->columnCount;
            if (data->inventoryProperties == NULL)
            {
                data->inventoryProperties = malloc((columnCount + 1) * sizeof(char *));
                data->inventoryProperties[0] = "无";

                LinkedList *rowNow = row == NULL ? NULL : row->items;
                for (int i = 1; i < columnCount + 1; i++)
                {
                    data->inventoryProperties[i] = rowNow->data;
                    rowNow = rowNow->next;
                }
            }
        }

        nk_layout_row_push(context, 140);
        {
            if (nk_style_push_font(context, &fontSmall->handle))
            {
                nk_combobox(context, data->inventoryProperties, columnCount + 1, &data->inventoryPropertySelected, 35,
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
                                           data->inventoryValueBuffer, BUFFER_SIZE * sizeof(char), nk_filter_default);
        }

        nk_layout_row_push(context, 100);
        {
            nk_label(context, "进行筛选", NK_TEXT_LEFT);
        }

        nk_layout_row_end(context);
    }

    nk_layout_row_static(context, 10, 0, 0);

    DrawMessageBox(context, "", &data->message);

    nk_layout_row_begin(context, NK_DYNAMIC, 35, 10);
    {
        if (nk_style_push_font(context, &fontSmall->handle))
        {
            if (data->tableBefore != NULL)
            {
                DrawUpdateBox(context, "", &data->tableBefore, &data->tableResult, BUFFER_SIZE);
            }
            else if (data->callback != NULL)
            {
                data->callback(data);
                data->callback = NULL;
            }
            nk_layout_row_push(context, 0.15);
            {
                if (nk_button_label(context, "查询"))
                {
                    switch (data->inventorySelectedRadio)
                    {
                    case 0:
                        SendInventoryRequest(data);
                        break;
                    case 1:
                        SendItemRequest(data);
                        break;
                    default:
                        break;
                    }
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
                    TableRow *row = NewTableRow();
                    AppendTableRow(row, "id");
                    AppendTableRow(row, "商品编号");
                    AppendTableRow(row, "数量");
                    AppendTableRow(row, "y1");
                    AppendTableRow(row, "m1");
                    AppendTableRow(row, "d1");
                    AppendTableRow(row, "h1");
                    AppendTableRow(row, "min1");
                    AppendTableRow(row, "s1");
                    AppendTableRow(row, "y2");
                    AppendTableRow(row, "m2");
                    AppendTableRow(row, "d2");
                    AppendTableRow(row, "h2");
                    AppendTableRow(row, "min2");
                    AppendTableRow(row, "s2");
                    AppendTableRow(row, "yuan");
                    AppendTableRow(row, "jiao");
                    AppendTableRow(row, "cent");
                    data->tableBefore = NewTable(row, NULL);
                    row = NewTableRow();
                    AppendTableRow(row, "自动生成");
                    AppendTableRow(row, "1");
                    AppendTableRow(row, "100");
                    AppendTableRow(row, "2023");
                    AppendTableRow(row, "01");
                    AppendTableRow(row, "01");
                    AppendTableRow(row, "00");
                    AppendTableRow(row, "00");
                    AppendTableRow(row, "00");
                    AppendTableRow(row, "0000");
                    AppendTableRow(row, "5");
                    AppendTableRow(row, "00");
                    AppendTableRow(row, "00");
                    AppendTableRow(row, "00");
                    AppendTableRow(row, "00");
                    AppendTableRow(row, "30");
                    AppendTableRow(row, "0");
                    AppendTableRow(row, "0");
                    AppendTable(data->tableBefore, row);

                    data->callback = SendInventoryAddRequest;
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
                TableLayout(context, data->inventoryTable, data->inventoryCheckList,
                            data->inventoryPropertySelected == 0
                                ? NULL
                                : data->inventoryProperties[data->inventoryPropertySelected],
                            data->inventoryValueBuffer);
                nk_group_end(context);
            }

            nk_style_pop_font(context);
        }
    }
}
