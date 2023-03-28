#include "../../config.h"
#include "design/layout.h"
#include <stdlib.h>
#include <string.h>

void ProfitDetailLayout(struct nk_context *context, Window *window)
{
    struct MainWindowData *data = window->data;
    TableRow *dataRow = GetRowByIndex(data, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "统计", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);

        nk_label(context, "收支", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "收支"), 512,
                nk_filter_default);

        nk_label(context, "事项", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "事项"), 512,
                nk_filter_default);

        nk_label(context, "时间", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data, dataRow, "时间"), 512,
                nk_filter_default);

        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeProfitDetail(Window *window)
{
    FreeTable(window->data);
    free(window);
}

Window *NewProfitDetail(const char *title, const Table *profit)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = ProfitDetailLayout;
    window->freeFunc = FreeProfitDetail;
    window->title = title;

    window->data = CloneTableBuffered(profit, 512);
    window->next = NULL;

    return window;
}
