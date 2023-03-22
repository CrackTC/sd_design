#include "../../data/operation.h"
#include "../../services/inventoryService.h"
#include "../../services/journalService.h"
#include "../../services/judgeService.h"
#include "../../utils.h"
#include "../config.h"
#include "../layout.h"
#include <malloc.h>
#include <string.h>

struct Data
{
    struct Table *loss;
    int id;
    const char *password;
    char *message;
    int modify;
};

static int SendRequest(struct Data *data)
{
    int hasPermission;
    Operation operation = data->modify ? OP_UPDATE_LOSS : OP_ADD_LOSS;
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
    AppendTableRow(row, "inventoryId");
    AppendTableRow(row, "y");
    AppendTableRow(row, "m");
    AppendTableRow(row, "d");
    AppendTableRow(row, "h");
    AppendTableRow(row, "min");
    AppendTableRow(row, "s");
    AppendTableRow(row, "number");
    AppendTableRow(row, "reason");
    Table *request = NewTable(row, NULL);

    row = NewTableRow();
    TableRow *sourceRow = GetRowByIndex(data->loss, 1);
    if (data->modify)
    {
        AppendTableRow(row, GetRowItemByColumnName(data->loss, sourceRow, "id"));
    }
    AppendTableRow(row, GetRowItemByColumnName(data->loss, sourceRow, "库存编号"));
    AppendTableRow(row, GetRowItemByColumnName(data->loss, sourceRow, "年"));
    AppendTableRow(row, GetRowItemByColumnName(data->loss, sourceRow, "月"));
    AppendTableRow(row, GetRowItemByColumnName(data->loss, sourceRow, "日"));
    AppendTableRow(row, GetRowItemByColumnName(data->loss, sourceRow, "时"));
    AppendTableRow(row, GetRowItemByColumnName(data->loss, sourceRow, "分"));
    AppendTableRow(row, GetRowItemByColumnName(data->loss, sourceRow, "秒"));
    AppendTableRow(row, GetRowItemByColumnName(data->loss, sourceRow, "货损数量"));
    AppendTableRow(row, GetRowItemByColumnName(data->loss, sourceRow, "货损原因"));
    AppendTable(request, row);

    Table *response;
    if (data->modify)
    {
        AddJournal(request, data->id, OP_UPDATE_LOSS);
        response = ReviseLossInventory(request);
    }
    else
    {
        AddJournal(request, data->id, OP_ADD_LOSS);
        response = AddLossInventory(request);
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

static void MessageBoxCallBack(__attribute__((unused)) int ok, void *parameter)
{
    struct Data *data = parameter;
    free(data->message);
    data->message = NULL;
}

void LossEditLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, MessageBoxCallBack, data);
    TableRow *dataRow = GetRowByIndex(data->loss, 1);

    nk_style_push_font(context, &fontLarge->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "货损编辑", NK_TEXT_CENTERED);

        nk_style_pop_font(context);
    }

    nk_style_push_font(context, &fontSmall->handle);
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
                GetRowItemByColumnName(data->loss, dataRow, "库存编号"), 512, nk_filter_decimal);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "货损数量", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 300);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->loss, dataRow, "货损数量"), 512, nk_filter_decimal);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 100);
                nk_label(context, "货损原因", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 500);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->loss, dataRow, "货损原因"), 512, nk_filter_default);

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

void FreeLossEdit(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->loss);
    free(window);
}

Window *NewLossEdit(const char *title, int id, const char *password, Table *loss, int modify)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = LossEditLayout;
    window->freeFunc = FreeLossEdit;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->loss = CloneTableBuffered(loss, 512);
    data->id = id;
    data->password = password;
    data->modify = modify;

    window->data = data;
    window->next = NULL;

    return window;
}
