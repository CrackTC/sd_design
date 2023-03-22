#include "../../data/operation.h"
#include "../../services/journalService.h"
#include "../../services/judgeService.h"
#include "../../services/saleService.h"
#include "../../utils.h"
#include "../config.h"
#include "../layout.h"
#include <malloc.h>
#include <string.h>

struct Data
{
    struct Table *refund;
    int id;
    const char *password;
    char *message;
    int modify;
};

static int SendRequest(struct Data *data)
{
    int hasPermission;
    Operation operation = data->modify ? OP_UPDATE_REFUND : OP_ADD_REFUND;
    Judge(data->id, &hasPermission, data->password, operation);
    if (!hasPermission)
    {
        data->message = CloneString("没有权限");
        return 0;
    }

    TableRow *row = NewTableRow();

    AppendTableRow(row, "订单编号");
    AppendTableRow(row, "退款原因");
	AppendTableRow(row, "年");
	AppendTableRow(row, "月");
	AppendTableRow(row, "日");
	AppendTableRow(row, "时");
	AppendTableRow(row, "分");
	AppendTableRow(row, "秒");
    AppendTableRow(row, "元");
    AppendTableRow(row, "角");
    AppendTableRow(row, "分");
    AppendTableRow(row, "退回数目");
    AppendTableRow(row, "备注");
    Table *request = NewTable(row, NULL);

    row = NewTableRow();
    TableRow *sourceRow = GetRowByIndex(data->refund, 1);

    AppendTableRow(row, GetRowItemByColumnName(data->refund, sourceRow, "订单编号"));
    AppendTableRow(row, GetRowItemByColumnName(data->refund, sourceRow, "退款原因"));
	AppendTableRow(row, GetRowItemByColumnName(data->refund, sourceRow, "年"));
	AppendTableRow(row, GetRowItemByColumnName(data->refund, sourceRow, "月"));
	AppendTableRow(row, GetRowItemByColumnName(data->refund, sourceRow, "日"));
	AppendTableRow(row, GetRowItemByColumnName(data->refund, sourceRow, "时"));
	AppendTableRow(row, GetRowItemByColumnName(data->refund, sourceRow, "分"));
	AppendTableRow(row, GetRowItemByColumnName(data->refund, sourceRow, "秒"));
    AppendTableRow(row, GetRowItemByColumnName(data->refund, sourceRow, "元"));
    AppendTableRow(row, GetRowItemByColumnName(data->refund, sourceRow, "角"));
    AppendTableRow(row, GetRowItemByColumnName(data->refund, sourceRow, "分"));
    AppendTableRow(row, GetRowItemByColumnName(data->refund, sourceRow, "退回数目"));
    AppendTableRow(row, GetRowItemByColumnName(data->refund, sourceRow, "备注"));
    AppendTable(request, row);

    Table *response;
    if (data->modify)
    {
        AddJournal(request, data->id, OP_UPDATE_REFUND);
        response = UpdateRefund(request);
    }
    else
    {
        AddJournal(request, data->id, OP_ADD_REFUND);
        response = AddRefund(request);
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

void RefundEditLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, MessageBoxCallBack, data);
    TableRow *dataRow = GetRowByIndex(data->refund, 1);

    nk_style_push_font(context, &fontLarge->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "退款编辑", NK_TEXT_CENTERED);

        nk_style_pop_font(context);
    }

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "订单编号", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 100);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->refund, dataRow, "订单编号"), 512, nk_filter_decimal);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "退款原因", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 300);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->refund, dataRow, "退款原因"), 512, nk_filter_default);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "退回数目", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 100);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->refund, dataRow, "退回数目"), 512, nk_filter_decimal);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 7);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "退款", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }

            nk_layout_row_push(context, 100);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->refund, dataRow, "元"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "元", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->refund, dataRow, "角"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "角", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->refund, dataRow, "分"), 512, nk_filter_decimal);
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

void FreeRefundEdit(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->refund);
    free(window);
}

Window *NewRefundEdit(const char *title, int id, const char *password, Table *refund, int modify)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = RefundEditLayout;
    window->freeFunc = FreeRefundEdit;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->refund = CloneTableBuffered(refund, 512);
    data->id = id;
    data->password = password;
    data->modify = modify;

    window->data = data;
    window->next = NULL;

    return window;
}
