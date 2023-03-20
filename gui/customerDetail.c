#include "config.h"
#include "layout.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Data
{
    struct Table *customer;
};

void CustomerDetailLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    TableRow *dataRow = GetRowByIndex(data->customer, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "客户详情", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "id", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->customer, dataRow, "id"), 512, nk_filter_default);

        nk_label(context, "等级", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->customer, dataRow, "等级"), 512,
                                       nk_filter_default);

        nk_label(context, "姓名", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->customer, dataRow, "姓名"), 512,
                                       nk_filter_default);

        nk_label(context, "联系方式", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->customer, dataRow, "联系方式"), 512,
                                       nk_filter_default);

        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeCustomerDetail(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->customer);
    free(window);
}

Window *NewCustomerDetail(const char *title, const Table *customer)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = CustomerDetailLayout;
    window->freeFunc = FreeCustomerDetail;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->customer = CloneTableBuffered(customer, 512);

    window->data = data;
    window->next = NULL;

    return window;
}
