#include "../data/amount.h"
#include "../data/linkedList.h"
#include "../data/operation.h"
#include "../data/table.h"
#include "../services/journalService.h"
#include "../services/judgeService.h"
#include "../services/saleService.h"
#include "../utils.h"
#include "config.h"
#include "layout.h"
#include "mainWindow.h"
#include "stddef.h"
#include <malloc.h>

static void MessageBoxCallBack(__attribute__((unused)) int ok, void *parameter)
{
    struct Data *data = parameter;
    free(data->message);
    data->message = NULL;
}

void RefundDelete(int ok, void *parameter)
{
    MessageBoxCallBack(ok, parameter);
    if (ok == 0)
    {
        return;
    }

    struct Data *data = parameter;

    int hasPermission;
    judge(data->id, &hasPermission, data->password, OP_DELETE_REFUND);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("缺少权限：删除退款条目");
        return;
    }

    LinkedList *now = data->refundCheckList->next;
    LinkedList *rowNow = data->refundTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            char *id = GetRowItemByColumnName(data->refundTable, rowNow->data, "id");

            TableRow *row = NewTableRow();
            AppendTableRow(row, "id");
            Table *table = NewTable(row, NULL);
            row = NewTableRow();
            AppendTableRow(row, id);
            AppendTable(table, row);

            AddJournal(table, data->id, OP_DELETE_REFUND);
            Table *response = RemoveRefund(table);
            FreeTable(table);

            if (response != NULL)
            {
                int error = 0;
                if (response->remark != NULL && response->remark[0] != '\0')
                {
                    data->messageCallback = MessageBoxCallBack;
                    data->message = CloneString(response->remark);
                    error = 1;
                }
                FreeTable(response);
                if (error)
                {
                    return;
                }
            }
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallBack;
    data->message = CloneString("删除成功");
}

void SendRefundRequest(struct Data *data)
{
    int hasPermission;
    judge(data->id, &hasPermission, data->password, OP_READ_REFUND);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("缺少权限：读取退款");
        return;
    }

    AddJournal(NULL, data->id, OP_READ_REFUND);
    Table *response = GetAllRefund(NULL);
    if (response != NULL)
    {
        if (response->remark != NULL && response->remark[0] != '\0')
        {
            data->messageCallback = MessageBoxCallBack;
            data->message = CloneString(response->remark);
        }

        FreeList(data->refundCheckList);
        data->refundCheckList = NewCheckList();
        data->refundTable = response;
    }
    else
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("查询失败: 响应为NULL");
    }
}

int RefundLookup(struct Data *data)
{
    LinkedList *now = data->refundCheckList->next;
    LinkedList *rowNow = data->refundTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->refundTable));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(NewRefundDetail("退款详情", table));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

int RefundAdd(struct Data *data)
{
    LinkedList *now = data->orderCheckList->next;
    LinkedList *rowNow = data->orderTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "订单编号");
            AppendTableRow(row, "退款原因");
            AppendTableRow(row, "退回数目");
            AppendTableRow(row, "元");
            AppendTableRow(row, "角");
            AppendTableRow(row, "分");
            AppendTableRow(row, "备注");
            Table *table = NewTable(row, "");

            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->orderTable, rowNow->data, "id"));
            AppendTableRow(row, "");

            AppendTableRow(row, GetRowItemByColumnName(data->orderTable, rowNow->data, "购买数量"));

            Amount amount = ParseAmount(GetRowItemByColumnName(data->orderTable, rowNow->data, "总价"));
            free(AppendTableRow(row, LongLongToString(GetAmountYuan(&amount))));
            free(AppendTableRow(row, LongLongToString(GetAmountJiao(&amount))));
            free(AppendTableRow(row, LongLongToString(GetAmountCent(&amount))));

            AppendTableRow(row, "");
            AppendTable(table, row);

            PushWindow(NewRefundEdit("退款编辑", data->id, data->password, table, 0));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

int RefundModify(struct Data *data)
{
    LinkedList *now = data->refundCheckList->next;
    LinkedList *rowNow = data->refundTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "订单编号");
            AppendTableRow(row, "退款原因");
            AppendTableRow(row, "退回数目");
            AppendTableRow(row, "元");
            AppendTableRow(row, "角");
            AppendTableRow(row, "分");
            AppendTableRow(row, "备注");
            Table *table = NewTable(row, "");

            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->refundTable, rowNow->data, "订单编号"));
            AppendTableRow(row, GetRowItemByColumnName(data->refundTable, rowNow->data, "退款原因"));
            AppendTableRow(row, GetRowItemByColumnName(data->refundTable, rowNow->data, "退回数目"));

            Amount amount = ParseAmount(GetRowItemByColumnName(data->refundTable, rowNow->data, "退款"));
            free(AppendTableRow(row, LongLongToString(GetAmountYuan(&amount))));
            free(AppendTableRow(row, LongLongToString(GetAmountJiao(&amount))));
            free(AppendTableRow(row, LongLongToString(GetAmountCent(&amount))));

            AppendTableRow(row, GetRowItemByColumnName(data->refundTable, rowNow->data, "备注"));
            AppendTable(table, row);

            PushWindow(NewRefundEdit("退款编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

void RefundPageLayout(struct nk_context *context, struct Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);

    // title
    nk_layout_row_dynamic(context, 0, 1);
    {
        if (nk_style_push_font(context, &fontLarge->handle))
        {
            nk_label(context, "退款", NK_TEXT_LEFT);
            nk_style_pop_font(context);
        }
    }

    // filter
    nk_layout_row_begin(context, NK_STATIC, 35, 5);
    {
        nk_layout_row_push(context, 100);
        {
            nk_label(context, "通过条件 ", NK_TEXT_LEFT);
        }

        int columnCount;
        {
            TableRow *row = data->refundTable == NULL ? NULL : GetTableTitle(data->refundTable);
            columnCount = row == NULL ? 0 : row->columnCount;
            if (data->refundProperties == NULL)
            {
                data->refundProperties = malloc((columnCount + 1) * sizeof(char *));
                data->refundProperties[0] = "无";

                LinkedList *rowNow = row == NULL ? NULL : row->items;
                for (int i = 1; i < columnCount + 1; i++)
                {
                    data->refundProperties[i] = rowNow->data;
                    rowNow = rowNow->next;
                }
            }
        }

        nk_layout_row_push(context, 140);
        {
            if (nk_style_push_font(context, &fontSmall->handle))
            {
                nk_combobox(context, data->refundProperties, columnCount + 1, &data->refundPropertySelected, 35,
                            nk_vec2(200, 400));
                nk_style_pop_font(context);
            }
        }

        nk_layout_row_push(context, 30);
        {
            nk_label(context, "为", NK_TEXT_CENTERED);
        }

        nk_layout_row_push(context, 200);
        {
            nk_edit_string_zero_terminated(context,
                                           (NK_EDIT_BOX | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD) & ~NK_EDIT_MULTILINE,
                                           data->refundValueBuffer, BUFFER_SIZE * sizeof(char), nk_filter_default);
        }

        nk_layout_row_push(context, 100);
        {
            nk_label(context, "进行筛选", NK_TEXT_LEFT);
        }

        nk_layout_row_end(context);
    }

    nk_layout_row_static(context, 10, 0, 0);

    nk_layout_row_begin(context, NK_DYNAMIC, 35, 10);
    {
        if (nk_style_push_font(context, &fontSmall->handle))
        {
            nk_layout_row_push(context, 0.15f);
            {
                if (nk_button_label(context, "查询"))
                {
                    SendRefundRequest(data);
                }
            }

            nk_layout_row_push(context, 0.01f);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.15f);
            {
                if (nk_button_label(context, "查看"))
                {
                    if (!RefundLookup(data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请选择一个退款条目");
                    }
                }
            }

            nk_layout_row_push(context, 0.01f);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.08f);
            {
                if (nk_button_label(context, "+"))
                {
                    if (!RefundAdd(data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请在商品页面选择一个订单条目");
                    }
                }
            }

            nk_layout_row_push(context, 0.01f);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.08f);
            {
                if (nk_button_label(context, "-"))
                {
                    data->messageCallback = RefundDelete;
                    data->message = CloneString("是否确认要删除选中的退款条目");
                }
            }

            nk_layout_row_push(context, 0.01f);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.08f);
            {
                if (nk_button_label(context, "~"))
                {
                    if (!RefundModify(data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请选择一个退款条目");
                    }
                }
            }

            nk_style_pop_font(context);
        }
        nk_layout_row_end(context);
    }

    nk_layout_row_dynamic(context, 10, 1);
    {
        struct nk_rect space;
        nk_widget(&space, context);
        struct nk_command_buffer *canvas = nk_window_get_canvas(context);
        nk_stroke_line(canvas, space.x, space.y + space.h / 2, space.x + space.w, space.y + space.h / 2, 1,
                       nk_rgb(100, 100, 100));
    }

    nk_layout_row_dynamic(context, nk_window_get_height(context) - 285, 1);
    {
        if (nk_style_push_font(context, &fontSmall->handle))
        {
            if (nk_group_begin(context, "查询结果", NK_WINDOW_BORDER))
            {
                TableLayout(context, data->refundTable, data->refundCheckList,
                            data->refundPropertySelected == 0 ? NULL
                                                              : data->refundProperties[data->refundPropertySelected],
                            data->refundValueBuffer);
                nk_group_end(context);
            }

            nk_style_pop_font(context);
        }
    }
}
