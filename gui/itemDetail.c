#include "config.h"
#include "layout.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Data
{
    struct Table *item;
};

void ItemDetailLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    TableRow *dataRow = GetRowByIndex(data->item, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "商品详情", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "商品编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->item, dataRow, "商品编号"), 512, nk_filter_default);

        nk_label(context, "商品名称", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->item, dataRow, "商品名称"), 512, nk_filter_default);

        nk_label(context, "售价", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->item, dataRow, "售价"), 512, nk_filter_default);

        nk_label(context, "保质期", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->item, dataRow, "保质期"), 512, nk_filter_default);

        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeItemDetail(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->item);
    free(window);
}

Window *NewItemDetail(const char *title, const Table *item)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = ItemDetailLayout;
    window->freeFunc = FreeItemDetail;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->item = CloneTableBuffered(item, 512);

    window->data = data;
    window->next = NULL;

    return window;
}
