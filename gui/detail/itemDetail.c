#include "../../config.h"
#include "design/layout.h"
#include <stdlib.h>
#include <string.h>

void ItemDetailLayout(struct nk_context *context, Window *window)
{
    struct Table *data = window->data;
    TableRow *dataRow = GetRowByIndex(data, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "商品详情", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "商品编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "商品编号"), 512, nk_filter_default);

        nk_label(context, "商品名称", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "商品名称"), 512, nk_filter_default);

        nk_label(context, "保质期", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "保质期"), 512, nk_filter_default);

        nk_label(context, "售价", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "售价"), 512, nk_filter_default);

        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeItemDetail(Window *window)
{
    FreeTable(window->data);
    free(window);
}

Window *NewItemDetail(const char *title, const Table *item)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = ItemDetailLayout;
    window->freeFunc = FreeItemDetail;
    window->title = title;

    window->data = CloneTableBuffered(item, 512);
    window->next = NULL;

    return window;
}
