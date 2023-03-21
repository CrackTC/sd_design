#include "../data/operation.h"
#include "../services/judgeService.h"
#include "../utils.h"
#include "config.h"
#include "layout.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Data
{
    struct Table *staff;
    int id;
    const char *password;
    char *message;
    int modify;
};

static int SendRequest(struct Data *data)
{
    data->message = CloneString("没有权限");
    return 0;

#warning
    int hasPermission;
    Operation operation = data->modify ? OP_UPDATE_STAFF : OP_ADD_STAFF;
    judge(data->id, &hasPermission, data->password, operation);
    if (!hasPermission)
    {
        data->message = CloneString("没有权限");
        return 0;
    }
#warning finish edit call
    return 1;
}

static void MessageBoxCallBack(int ok, void *parameter)
{
    struct Data *data = parameter;
    free(data->message);
    data->message = NULL;
}

void StaffEditLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, MessageBoxCallBack, data);
    TableRow *dataRow = GetRowByIndex(data->staff, 1);

    nk_style_push_font(context, &fontLarge->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "员工编辑", NK_TEXT_CENTERED);

        nk_style_pop_font(context);
    }

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 200);
                nk_label(context, "员工密码", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 300);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->staff, dataRow, "员工密码"), 512, nk_filter_default);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 200);
                nk_label(context, "员工姓名", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 100);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->staff, dataRow, "员工姓名"), 512, nk_filter_default);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 200);
                nk_label(context, "员工联系方式", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 300);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->staff, dataRow, "员工联系方式"), 512, nk_filter_default);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 200);
                nk_label(context, "已启用", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 100);
#warning use checkbox
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->staff, dataRow, "已启用"), 512, nk_filter_decimal);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 200);
                nk_label(context, "员工权限", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 600);
#warning use checkbox
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->staff, dataRow, "员工权限"), 512, nk_filter_decimal);

            nk_layout_row_end(context);
        }

        PlaceNothing(context);
        nk_layout_row_dynamic(context, 0, 5);
        {
            PlaceNothing(context);
            if (nk_button_label(context, "确定"))
            {
                if (SendRequest(data))
                {
                    window->isClosed = 1;
                }
            }
            PlaceNothing(context);
            if (nk_button_label(context, "取消"))
            {
                window->isClosed = 1;
            }
        }

        nk_style_pop_font(context);
    }
}

void FreeStaffEdit(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->staff);
    free(window);
}

Window *NewStaffEdit(const char *title, int id, const char *password, Table *staff, int modify)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = StaffEditLayout;
    window->freeFunc = FreeStaffEdit;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->staff = CloneTableBuffered(staff, 512);
    data->id = id;
    data->password = password;
    data->modify = modify;

    window->data = data;
    window->next = NULL;

    return window;
}
