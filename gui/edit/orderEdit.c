#include "design/operation.h"
#include "design/judgeService.h"
#include "design/journalService.h"
#include "design/saleService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include <malloc.h>
#include <string.h>

static void SendRequest(struct EditData *data)
{
    int hasPermission;
    Operation operation = data->modify ? OP_UPDATE_ORDER : OP_ADD_ORDER;
    Judge(data->id, &hasPermission, data->password, operation);
    if (!hasPermission)
    {
        data->messageCallback = FinishCallback;
        data->message = CloneString("没有权限");
        return;
    }

    TableRow *row = NewTableRow();
    if (data->modify)
    {
        AppendTableRow(row, "订单编号");
        AppendTableRow(row, "库存编号");
    }
    AppendTableRow(row, "商品编号");
    AppendTableRow(row, "客户编号");
    AppendTableRow(row, "购买数量");
    Table *request = NewTable(row, NULL);

    row = NewTableRow();
    TableRow *sourceRow = GetRowByIndex(data->data, 1);
    if (data->modify)
    {
        AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "订单编号"));
        AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "库存编号"));
    }
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "商品编号"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "客户编号"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "购买数量"));
    AppendTable(request, row);

    Table *response;
    if (data->modify)
    {
        AddJournal(request, data->id, OP_UPDATE_ORDER);
        response = UpdateOrder(request);
    }
    else
    {
        AddJournal(request, data->id, OP_ADD_ORDER);
        response = AddOrder(request);
    }
    FreeTable(request);

    if (response != NULL && response->remark != NULL && response->remark[0] != '\0')
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString(response->remark);
    }
    else
    {
        data->messageCallback = FinishCallback;
        data->message = CloneString("操作成功完成");
    }

    if (response != NULL)
    {
        FreeTable(response);
    }
}

void OrderEditLayout(struct nk_context *context, Window *window)
{
    struct EditData *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);
    TableRow *dataRow = GetRowByIndex(data->data, 1);

    nk_style_push_font(context, &fontLarge->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "订单编辑", NK_TEXT_CENTERED);

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
                    GetRowItemByColumnName(data->data, dataRow, "商品编号"), 512, nk_filter_decimal);

            nk_layout_row_end(context);
        }

        if (data->modify)
        {
            nk_layout_row_begin(context, NK_STATIC, 0, 2);
            {
                nk_style_push_font(context, &fontMedium->handle);
                {
                    nk_layout_row_push(context, 100);
                    nk_label(context, "库存编号", NK_TEXT_CENTERED);
                    nk_style_pop_font(context);
                }
                nk_layout_row_push(context, 100);
                nk_edit_string_zero_terminated(
                        context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                        GetRowItemByColumnName(data->data, dataRow, "库存编号"), 512, nk_filter_decimal);

                nk_layout_row_end(context);
            }
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
                    GetRowItemByColumnName(data->data, dataRow, "商品名称"), 512, nk_filter_default);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "客户编号", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 300);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "客户编号"), 512, nk_filter_default);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "客户姓名", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 300);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "客户姓名"), 512, nk_filter_default);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "购买数量", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 100);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "购买数量"), 512, nk_filter_decimal);

            nk_layout_row_end(context);
        }

        PlaceNothing(context);
        nk_layout_row_dynamic(context, 0, 5);
        {
            PlaceNothing(context);
            if (nk_button_label(context, "确定"))
            {
                SendRequest(data);
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

void FreeOrderEdit(Window *window)
{
    struct EditData *data = window->data;
    FreeTable(data->data);
    free(window);
}

Window *NewOrderEdit(const char *title, int id, const char *password, Table *order, int modify)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = OrderEditLayout;
    window->freeFunc = FreeOrderEdit;
    window->title = title;

    struct EditData *data = malloc(sizeof(struct EditData));
    memset(data, 0, sizeof(struct EditData));
    data->data = CloneTableBuffered(order, 512);
    data->id = id;
    data->password = password;
    data->modify = modify;
    data->window = window;

    window->data = data;
    window->next = NULL;

    return window;
}
