#include "config.h"
#include "layout.h"
#include <stdlib.h>
#include <string.h>

struct Data
{
    struct Table *inventory;
};

void InventoryDetailLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    TableRow *dataRow = GetRowByIndex(data->inventory, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "库存详情", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "id", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->inventory, dataRow, "id"), 512, nk_filter_default);

        nk_label(context, "商品编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->inventory, dataRow, "商品编号"), 512,
                                       nk_filter_default);

        nk_label(context, "商品名称", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->inventory, dataRow, "商品名称"), 512,
                                       nk_filter_default);

        nk_label(context, "数量", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->inventory, dataRow, "数量"), 512,
                                       nk_filter_default);

        nk_label(context, "入库时间", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->inventory, dataRow, "入库时间"), 512,
                                       nk_filter_default);

        nk_label(context, "生产日期", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->inventory, dataRow, "生产日期"), 512,
                                       nk_filter_default);

        nk_label(context, "购入单价", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->inventory, dataRow, "购入单价"), 512,
                                       nk_filter_default);
        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeInventoryDetail(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->inventory);
    free(window);
}

Window *NewInventoryDetail(const char *title, const Table *inventory)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = InventoryDetailLayout;
    window->freeFunc = FreeInventoryDetail;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->inventory = CloneTableBuffered(inventory, 512);

    window->data = data;
    window->next = NULL;

    return window;
}
