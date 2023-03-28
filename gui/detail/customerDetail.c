#include "../../config.h"
#include "design/layout.h"
#include <stdlib.h>
#include <string.h>

void CustomerDetailLayout(struct nk_context *context, Window *window)
{
    struct Table *data = window->data;
    TableRow *dataRow = GetRowByIndex(data, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "客户详情", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "客户编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "客户编号"), 512, nk_filter_default);

        nk_label(context, "客户等级", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "客户等级"), 512,
                nk_filter_default);

        nk_label(context, "客户姓名", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "客户姓名"), 512,
                nk_filter_default);

        nk_label(context, "客户联系方式", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "客户联系方式"), 512,
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
    FreeTable(window->data);
    free(window);
}

Window *NewCustomerDetail(const char *title, const Table *customer)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = CustomerDetailLayout;
    window->freeFunc = FreeCustomerDetail;
    window->title = title;

    window->data = CloneTableBuffered(customer, 512);
    window->next = NULL;

    return window;
}
