#include "../data/table.h"
#include "config.h"
#include "layout.h"
#include "mainWindow.h"
#include <stdio.h>

void InventoryPageLayout(struct nk_context *context, struct Window *window)
{
    struct Data *data = window->data;

    nk_layout_row_dynamic(context, 0, 1);
    {
        if (nk_style_push_font(context, &fontLarge->handle))
        {
            nk_label(context, "库存", NK_TEXT_LEFT);
            nk_style_pop_font(context);
        }
    }

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

    static const char *inventoryProperties[] = {"无", "商品名称"};

    nk_layout_row_begin(context, NK_STATIC, 35, 5);
    {
        nk_layout_row_push(context, 100);
        {
            nk_label(context, "通过条件 ", NK_TEXT_LEFT);
        }

        nk_layout_row_push(context, 140);
        {
            if (nk_style_push_font(context, &fontSmall->handle))
            {
                nk_combobox(context, inventoryProperties, 2, &data->inventoryPropertySelected, 35, nk_vec2(200, 100));
                nk_style_pop_font(context);
            }
        }

        nk_layout_row_push(context, 30);
        {
            nk_label(context, "为", NK_TEXT_CENTERED);
        }

        nk_layout_row_push(context, 200);
        {
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD) & ~NK_EDIT_MULTILINE,
                data->inventoryValueBuffer, sizeof(data->inventoryValueBuffer), nk_filter_default);
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
                if (data->inventoryTable != NULL)
                {
                    TableRow *row = GetTableTitle(data->inventoryTable);
                    nk_layout_row_begin(context, NK_STATIC, 30, row->columnCount);
                    {
                        LinkedList *itemNow = row->items;
                        while (itemNow != NULL)
                        {
                            nk_layout_row_push(context, 100);
                            {
                                nk_label(context, itemNow->data, NK_TEXT_CENTERED);
                            }
                            itemNow = itemNow->next;
                        }
                        nk_layout_row_end(context);
                    }
                    LinkedList *rowNow = data->inventoryTable->rows->next;
                    while (rowNow != NULL)
                    {
                        row = rowNow->data;
                        LinkedList *itemNow = row->items;
                        while (itemNow != NULL)
                        {
                            nk_layout_row_push(context, 100);
                            {
                                nk_label(context, itemNow->data, NK_TEXT_CENTERED);
                            }
                            itemNow = itemNow->next;
                        }
                        rowNow = rowNow->next;
                    }
                }
                nk_group_end(context);
            }

            nk_style_pop_font(context);
        }
    }
}
