#include "config.h"
#include "layout.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Data
{
    struct Table *refund;
};

void RefundDetailLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    TableRow *dataRow = GetRowByIndex(data->refund, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "退款详情", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);

        nk_label(context, "订单编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->refund, dataRow, "订单编号"), 512,
                                       nk_filter_default);

        nk_label(context, "原因", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->refund, dataRow, "原因"), 512, nk_filter_default);

        nk_label(context, "时间", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->refund, dataRow, "时间"), 512, nk_filter_default);

        nk_label(context, "退款", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->refund, dataRow, "退款"), 512, nk_filter_default);

        nk_label(context, "退回数目", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->refund, dataRow, "退回数目"), 512,
                                       nk_filter_default);

        nk_label(context, "备注", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->refund, dataRow, "备注"), 512, nk_filter_default);

        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeRefundDetail(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->refund);
    free(window);
}

Window *NewRefundEntryDetail(const char *title, const Table *refund)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = RefundDetailLayout;
    window->freeFunc = FreeRefundDetail;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->refund = CloneTableBuffered(refund, 512);

    window->data = data;
    window->next = NULL;

    return window;
}
