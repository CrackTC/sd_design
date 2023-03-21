#include "../data/operation.h"
#include "../services/inventoryService.h"
#include "../services/journalService.h"
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
    struct Table *item;
    int id;
    const char *password;
    char *message;
    int modify;
};

static int SendRequest(struct Data *data)
{
    int hasPermission;
    Operation operation = data->modify ? OP_UPDATE_ITEM : OP_ADD_ITEM;
    judge(data->id, &hasPermission, data->password, operation);
    if (!hasPermission)
    {
        data->message = CloneString("没有权限");
        return 0;
    }

    TableRow *row = NewTableRow();
    if (data->modify)
    {
        AppendTableRow(row, "id");
    }
    AppendTableRow(row, "name");
    AppendTableRow(row, "yuan");
    AppendTableRow(row, "jiao");
    AppendTableRow(row, "cent");
    AppendTableRow(row, "y1");
    AppendTableRow(row, "m1");
    AppendTableRow(row, "d1");
    AppendTableRow(row, "h1");
    AppendTableRow(row, "min1");
    AppendTableRow(row, "s1");
    Table *request = NewTable(row, NULL);

    row = NewTableRow();
    TableRow *sourceRow = GetRowByIndex(data->item, 1);
    if (data->modify)
    {
        AppendTableRow(row, GetRowItemByColumnName(data->item, sourceRow, "id"));
    }
    AppendTableRow(row, GetRowItemByColumnName(data->item, sourceRow, "商品名称"));
    AppendTableRow(row, GetRowItemByColumnName(data->item, sourceRow, "元"));
    AppendTableRow(row, GetRowItemByColumnName(data->item, sourceRow, "角"));
    AppendTableRow(row, GetRowItemByColumnName(data->item, sourceRow, "分"));
    AppendTableRow(row, GetRowItemByColumnName(data->item, sourceRow, "年"));
    AppendTableRow(row, GetRowItemByColumnName(data->item, sourceRow, "月"));
    AppendTableRow(row, GetRowItemByColumnName(data->item, sourceRow, "日"));
    AppendTableRow(row, GetRowItemByColumnName(data->item, sourceRow, "时"));
    AppendTableRow(row, GetRowItemByColumnName(data->item, sourceRow, "分"));
    AppendTableRow(row, GetRowItemByColumnName(data->item, sourceRow, "秒"));
    AppendTable(request, row);

    Table *response;
    if (data->modify)
    {
        AddJournal(request, data->id, OP_UPDATE_ITEM);
        response = ReviseAnItemByItemId(request);
    }
    else
    {
        AddJournal(request, data->id, OP_ADD_ITEM);
        response = AddItem(request);
    }
    FreeTable(request);

    if (response != NULL && response->remark != NULL && response->remark[0] != '\0')
    {
        data->message = CloneString(response->remark);
    }
    else
    {
        data->message = CloneString("操作成功完成");
    }

    if (response != NULL)
    {
        FreeTable(response);
    }

    return 1;
}

static void MessageBoxCallBack(int ok, void *parameter)
{
    struct Data *data = parameter;
    free(data->message);
    data->message = NULL;
}

void ItemEditLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, MessageBoxCallBack, data);
    TableRow *dataRow = GetRowByIndex(data->item, 1);

    nk_style_push_font(context, &fontLarge->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "商品编辑", NK_TEXT_CENTERED);

        nk_style_pop_font(context);
    }

    nk_style_push_font(context, &fontSmall->handle);
    {
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
                GetRowItemByColumnName(data->item, dataRow, "商品名称"), 512, nk_filter_default);

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
            nk_layout_row_push(context, 300);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->item, dataRow, "数量"), 512, nk_filter_decimal);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 13);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "保质期", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }

            nk_layout_row_push(context, 70);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->item, dataRow, "年"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "年", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->item, dataRow, "月"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "月", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->item, dataRow, "日"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "日", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->item, dataRow, "时"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "时", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->item, dataRow, "分"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "分", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->item, dataRow, "秒"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "秒", NK_TEXT_CENTERED);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 7);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "售价", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }

            nk_layout_row_push(context, 100);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->item, dataRow, "元"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "元", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->item, dataRow, "角"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "角", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->item, dataRow, "分"), 512, nk_filter_decimal);
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

void FreeItemEdit(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->item);
    free(window);
}

Window *NewItemEdit(const char *title, int id, const char *password, Table *item, int modify)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = ItemEditLayout;
    window->freeFunc = FreeItemEdit;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->item = CloneTableBuffered(item, 512);
    data->id = id;
    data->password = password;
    data->modify = modify;

    window->data = data;
    window->next = NULL;

    return window;
}
