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
    struct Table *inventory;
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
    Operation operation = data->modify ? OP_UPDATE_INVENTORY : OP_ADD_INVENTORY;
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

void InventoryEditLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, MessageBoxCallBack, data);
    TableRow *dataRow = GetRowByIndex(data->inventory, 1);

    nk_style_push_font(context, &fontLarge->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "库存编辑", NK_TEXT_CENTERED);

        nk_style_pop_font(context);
    }

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "商品编号", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 100);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "商品编号"), 512, nk_filter_decimal);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "商品名称", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 300);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "商品名称"), 512, nk_filter_default);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "数量", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 100);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "数量"), 512, nk_filter_decimal);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 13);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "入库时间", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }

            nk_layout_row_push(context, 70);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "年1"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "年", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "月1"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "月", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "日1"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "日", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "时1"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "时", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "分1"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "分", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "秒1"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "秒", NK_TEXT_CENTERED);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 13);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "生产日期", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }

            nk_layout_row_push(context, 70);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "年2"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "年", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "月2"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "月", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "日2"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "日", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "时2"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "时", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "分2"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "分", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "秒2"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "秒", NK_TEXT_CENTERED);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 7);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "购入单价", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }

            nk_layout_row_push(context, 100);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "元"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "元", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "角"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "角", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->inventory, dataRow, "分"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "分", NK_TEXT_CENTERED);

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

void FreeInventoryEdit(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->inventory);
    free(window);
}

Window *NewInventoryEdit(const char *title, int id, const char *password, Table *inventory, int modify)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = InventoryEditLayout;
    window->freeFunc = FreeInventoryEdit;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->inventory = CloneTableBuffered(inventory, 512);
    data->id = id;
    data->password = password;
    data->modify = modify;

    window->data = data;
    window->next = NULL;

    return window;
}
