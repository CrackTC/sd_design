#include "../../config.h"
#include "design/layout.h"
#include <stdlib.h>
#include <string.h>

void RefundDetailLayout(struct nk_context *context, Window *window)
{
    struct Table *data = window->data;
    TableRow *dataRow = GetRowByIndex(data, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "退款详情", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);

        nk_label(context, "订单编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "订单编号"), 512,
                nk_filter_default);

        nk_label(context, "退款原因", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "退款原因"), 512, nk_filter_default);

        nk_label(context, "时间", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "时间"), 512, nk_filter_default);

        nk_label(context, "退款", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "退款"), 512, nk_filter_default);

        nk_label(context, "退回数目", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "退回数目"), 512,
                nk_filter_default);

        nk_label(context, "备注", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "备注"), 512, nk_filter_default);

        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeRefundDetail(Window *window)
{
    FreeTable(window->data);
    free(window);
}

Window *NewRefundDetail(const char *title, const Table *refund)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = RefundDetailLayout;
    window->freeFunc = FreeRefundDetail;
    window->title = title;

    window->data = CloneTableBuffered(refund, 512);
    window->next = NULL;

    return window;
}
