#include "config.h"
#include "layout.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Data
{
    struct Table *lossEntry;
};

void LossEntryDetailLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    TableRow *dataRow = GetRowByIndex(data->lossEntry, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "货损详情", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "id", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->lossEntry, dataRow, "id"), 512, nk_filter_default);

        nk_label(context, "库存编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->lossEntry, dataRow, "库存编号"), 512,
                                       nk_filter_default);

        nk_label(context, "数量", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->lossEntry, dataRow, "数量"), 512,
                                       nk_filter_default);

        nk_label(context, "原因", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->lossEntry, dataRow, "原因"), 512,
                                       nk_filter_default);

        nk_label(context, "时间", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->lossEntry, dataRow, "时间"), 512,
                                       nk_filter_default);
        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeLossEntryDetail(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->lossEntry);
    free(window);
}

Window *NewLossEntryDetail(const char *title, const Table *lossEntry)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = LossEntryDetailLayout;
    window->freeFunc = FreeLossEntryDetail;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->lossEntry = CloneTableBuffered(lossEntry, 512);

    window->data = data;
    window->next = NULL;

    return window;
}
