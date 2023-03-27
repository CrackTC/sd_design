#include "design/operation.h"
#include "design/inventoryService.h"
#include "design/journalService.h"
#include "design/judgeService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include <malloc.h>
#include <string.h>

static void SendRequest(struct EditData *data)
{
    int hasPermission;
    Operation operation = data->modify ? OP_UPDATE_INVENTORY : OP_ADD_INVENTORY;
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
        AppendTableRow(row, "库存编号");
    }
    AppendTableRow(row, "商品编号");
    AppendTableRow(row, "数量");
    AppendTableRow(row, "年1");
    AppendTableRow(row, "月1");
    AppendTableRow(row, "日1");
    AppendTableRow(row, "时1");
    AppendTableRow(row, "分1");
    AppendTableRow(row, "秒1");
    AppendTableRow(row, "年2");
    AppendTableRow(row, "月2");
    AppendTableRow(row, "日2");
    AppendTableRow(row, "时2");
    AppendTableRow(row, "分2");
    AppendTableRow(row, "秒2");
    AppendTableRow(row, "元");
    AppendTableRow(row, "角");
    AppendTableRow(row, "分");
    Table *request = NewTable(row, NULL);

    row = NewTableRow();
    TableRow *sourceRow = GetRowByIndex(data->data, 1);
    if (data->modify)
    {
        AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "库存编号"));
    }
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "商品编号"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "数量"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "年1"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "月1"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "日1"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "时1"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "分1"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "秒1"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "年2"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "月2"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "日2"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "时2"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "分2"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "秒2"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "元"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "角"));
    AppendTableRow(row, GetRowItemByColumnName(data->data, sourceRow, "分"));
    AppendTable(request, row);

    Table *response;
    if (data->modify)
    {
        AddJournal(request, data->id, OP_UPDATE_INVENTORY);
        response = ReviseInventory(request);
    }
    else
    {
        AddJournal(request, data->id, OP_ADD_INVENTORY);
        response = AddInventory(request);
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

void InventoryEditLayout(struct nk_context *context, Window *window)
{
    struct EditData *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);
    TableRow *dataRow = GetRowByIndex(data->data, 1);

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
                    GetRowItemByColumnName(data->data, dataRow, "商品编号"), 512, nk_filter_decimal);

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
                    GetRowItemByColumnName(data->data, dataRow, "商品名称"), 512, nk_filter_default);

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
                    GetRowItemByColumnName(data->data, dataRow, "数量"), 512, nk_filter_decimal);

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
                    GetRowItemByColumnName(data->data, dataRow, "年1"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "年", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "月1"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "月", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "日1"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "日", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "时1"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "时", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "分1"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "分", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "秒1"), 512, nk_filter_decimal);
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
                    GetRowItemByColumnName(data->data, dataRow, "年2"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "年", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "月2"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "月", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "日2"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "日", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "时2"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "时", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "分2"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "分", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "秒2"), 512, nk_filter_decimal);
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
                    GetRowItemByColumnName(data->data, dataRow, "元"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "元", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "角"), 512, nk_filter_decimal);
            nk_layout_row_push(context, 30);
            nk_label(context, "角", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(
                    context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                    GetRowItemByColumnName(data->data, dataRow, "分"), 512, nk_filter_decimal);
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

void FreeInventoryEdit(Window *window)
{
    struct EditData *data = window->data;
    FreeTable(data->data);
    free(window);
}

Window *NewInventoryEdit(const char *title, int id, const char *password, Table *inventory, int modify)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = InventoryEditLayout;
    window->freeFunc = FreeInventoryEdit;
    window->title = title;

    struct EditData *data = malloc(sizeof(struct EditData));
    memset(data, 0, sizeof(struct EditData));
    data->data = CloneTableBuffered(inventory, 512);
    data->id = id;
    data->password = password;
    data->modify = modify;
    data->window = window;

    window->data = data;
    window->next = NULL;

    return window;
}
