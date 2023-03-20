#include "config.h"
#include "layout.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Data
{
    struct Table *basicDiscount;
};

void BasicDiscountDetailLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    TableRow *dataRow = GetRowByIndex(data->basicDiscount, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "折扣详情", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "id", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->basicDiscount, dataRow, "id"), 512,
                                       nk_filter_default);

        nk_label(context, "商品编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->basicDiscount, dataRow, "商品编号"), 512,
                                       nk_filter_default);

        nk_label(context, "折扣比率", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->basicDiscount, dataRow, "折扣比率"), 512,
                                       nk_filter_default);

        nk_label(context, "客户等级", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->basicDiscount, dataRow, "客户等级"), 512,
                                       nk_filter_default);

        nk_label(context, "截止时间", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->basicDiscount, dataRow, "截止时间"), 512,
                                       nk_filter_default);

        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeBasicDiscountDetail(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->basicDiscount);
    free(window);
}

Window *NewBasicDiscountDetail(const char *title, const Table *basicDiscount)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = BasicDiscountDetailLayout;
    window->freeFunc = FreeBasicDiscountDetail;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->basicDiscount = CloneTableBuffered(basicDiscount, 512);

    window->data = data;
    window->next = NULL;

    return window;
}
