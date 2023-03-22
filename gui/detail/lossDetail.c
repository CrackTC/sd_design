#include "../config.h"
#include "../layout.h"
#include <stdlib.h>
#include <string.h>

struct Data
{
    struct Table *loss;
};

void LossDetailLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    TableRow *dataRow = GetRowByIndex(data->loss, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "货损详情", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "id", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->loss, dataRow, "货损编号"), 512, nk_filter_default);

        nk_label(context, "库存编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->loss, dataRow, "库存编号"), 512,
                                       nk_filter_default);

        nk_label(context, "货损数量", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->loss, dataRow, "货损数量"), 512,
                                       nk_filter_default);

        nk_label(context, "货损原因", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->loss, dataRow, "货损原因"), 512,
                                       nk_filter_default);

        nk_label(context, "损耗时间", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->loss, dataRow, "损耗时间"), 512,
                                       nk_filter_default);
        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeLossDetail(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->loss);
    free(window);
}

Window *NewLossDetail(const char *title, const Table *loss)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = LossDetailLayout;
    window->freeFunc = FreeLossDetail;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->loss = CloneTableBuffered(loss, 512);

    window->data = data;
    window->next = NULL;

    return window;
}
