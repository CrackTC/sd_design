#include "../config.h"
#include "../layout.h"
#include <stdlib.h>
#include <string.h>

struct Data
{
    struct Table *order;
};

void OrderDetailLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    TableRow *dataRow = GetRowByIndex(data->order, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "订单详情", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "订单编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data->order, dataRow, "订单编号"), 512, nk_filter_default);

        nk_label(context, "库存编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data->order, dataRow, "库存编号"), 512,
                nk_filter_default);

        nk_label(context, "商品编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data->order, dataRow, "商品编号"), 512,
                nk_filter_default);

        nk_label(context, "商品名称", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data->order, dataRow, "商品名称"), 512,
                nk_filter_default);

        nk_label(context, "客户编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data->order, dataRow, "客户编号"), 512,
                nk_filter_default);

        nk_label(context, "客户姓名", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data->order, dataRow, "客户姓名"), 512,
                nk_filter_default);

        nk_label(context, "购买数量", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data->order, dataRow, "购买数量"), 512,
                nk_filter_default);

        nk_label(context, "购买时间", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data->order, dataRow, "购买时间"), 512, nk_filter_default);

        nk_label(context, "总价", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data->order, dataRow, "总价"), 512, nk_filter_default);
        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeOrderDetail(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->order);
    free(window);
}

Window *NewOrderDetail(const char *title, const Table *order)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = OrderDetailLayout;
    window->freeFunc = FreeOrderDetail;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->order = CloneTableBuffered(order, 512);

    window->data = data;
    window->next = NULL;

    return window;
}
