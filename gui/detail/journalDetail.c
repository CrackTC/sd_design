#include "../config.h"
#include "../layout.h"
#include <stdlib.h>
#include <string.h>

struct Data
{
    struct Table *journal;
};

void JournalDetailLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    TableRow *dataRow = GetRowByIndex(data->journal, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "日志详情", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "员工编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->journal, dataRow, "员工编号"), 512, nk_filter_default);

        nk_label(context, "时间", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->journal, dataRow, "时间"), 512,
                                       nk_filter_default);

        nk_label(context, "操作", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->journal, dataRow, "操作"), 512,
                                       nk_filter_default);

        nk_label(context, "参数", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                       GetRowItemByColumnName(data->journal, dataRow, "参数"), 512,
                                       nk_filter_default);

        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeJournalDetail(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->journal);
    free(window);
}

Window *NewJournalDetail(const char *title, const Table *journal)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = JournalDetailLayout;
    window->freeFunc = FreeJournalDetail;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->journal = CloneTableBuffered(journal, 4096);

    window->data = data;
    window->next = NULL;

    return window;
}
