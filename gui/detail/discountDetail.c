#include "../../config.h"
#include "design/layout.h"
#include <stdlib.h>
#include <string.h>

void DiscountDetailLayout(struct nk_context *context, Window *window)
{
    struct Table *data = window->data;
    TableRow *dataRow = GetRowByIndex(data, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "折扣详情", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "折扣编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "折扣编号"), 512, nk_filter_default);

        nk_label(context, "商品编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "商品编号"), 512,
                nk_filter_default);

        nk_label(context, "商品名称", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "商品名称"), 512,
                nk_filter_default);

        nk_label(context, "折扣比率", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "折扣比率"), 512,
                nk_filter_default);

        nk_label(context, "客户等级", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "客户等级"), 512,
                nk_filter_default);

        nk_label(context, "截止时间", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "截止时间"), 512,
                nk_filter_default);

        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeDiscountDetail(Window *window)
{
    FreeTable(window->data);
    free(window);
}

Window *NewDiscountDetail(const char *title, const Table *discount)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = DiscountDetailLayout;
    window->freeFunc = FreeDiscountDetail;
    window->title = title;

    window->data = CloneTableBuffered(discount, 512);
    window->next = NULL;

    return window;
}
