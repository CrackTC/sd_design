#include "../../config.h"
#include "design/layout.h"
#include <stdlib.h>
#include <string.h>

struct Data
{
    struct Table *staff;
};

void StaffDetailLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    TableRow *dataRow = GetRowByIndex(data->staff, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "员工详情", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "员工编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data->staff, dataRow, "员工编号"), 512, nk_filter_default);

        nk_label(context, "员工可用性", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data->staff, dataRow, "员工可用性"), 512,
                nk_filter_default);

        nk_label(context, "员工姓名", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data->staff, dataRow, "员工姓名"), 512,
                nk_filter_default);

        nk_label(context, "员工联系方式", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data->staff, dataRow, "员工联系方式"), 512,
                nk_filter_default);

        nk_label(context, "员工权限", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_SELECTABLE | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                GetRowItemByColumnName(data->staff, dataRow, "员工权限"), 512,
                nk_filter_default);

        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeStaffDetail(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->staff);
    free(window);
}

Window *NewStaffDetail(const char *title, const Table *staff)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = StaffDetailLayout;
    window->freeFunc = FreeStaffDetail;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->staff = CloneTableBuffered(staff, 512);

    window->data = data;
    window->next = NULL;

    return window;
}
