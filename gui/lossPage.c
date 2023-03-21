#include "../data/amount.h"
#include "../data/linkedList.h"
#include "../data/operation.h"
#include "../data/table.h"
#include "../data/time.h"
#include "../services/inventoryService.h"
#include "../services/journalService.h"
#include "../services/judgeService.h"
#include "../utils.h"
#include "config.h"
#include "layout.h"
#include "mainWindow.h"
#include "stddef.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

static void MessageBoxCallBack(int ok, void *parameter)
{
    struct Data *data = parameter;
    free(data->message);
    data->message = NULL;
}

void LossEntryDelete(int ok, void *parameter)
{
    MessageBoxCallBack(ok, parameter);
    if (ok == 0)
    {
        return;
    }

    struct Data *data = parameter;

    int hasPermission;
    judge(data->id, &hasPermission, data->password, OP_DELETE_LOSS);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("缺少权限：删除货损");
        return;
    }

    LinkedList *now = data->lossCheckList->next;
    LinkedList *rowNow = data->lossTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            char *id = GetRowItemByColumnName(data->lossTable, rowNow->data, "id");

            TableRow *row = NewTableRow();
            AppendTableRow(row, "id");
            Table *table = NewTable(row, NULL);
            row = NewTableRow();
            AppendTableRow(row, id);
            AppendTable(table, row);

            AddJournal(table, data->id, OP_DELETE_LOSS);
            Table *response = DeleteSingleLossById(table);
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

void SendLossRequest(struct Data *data)
{
    int hasPermission;
    judge(data->id, &hasPermission, data->password, OP_READ_LOSS);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("缺少权限：读取货损");
        return;
    }

    AddJournal(NULL, data->id, OP_READ_LOSS);
    Table *response = ShowLossInventory(NULL);
    if (response != NULL)
    {
        if (response->remark != NULL && response->remark[0] != '\0')
        {
            data->messageCallback = MessageBoxCallBack;
            data->message = CloneString(response->remark);
        }

        FreeList(data->lossCheckList);
        data->lossCheckList = NewCheckList();
        data->lossTable = response;
    }
    else
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("查询失败: 响应为NULL");
    }
}

int LossLookup(struct nk_context *context, struct Data *data)
{
    LinkedList *now = data->lossCheckList->next;
    LinkedList *rowNow = data->lossTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->lossTable));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(context, NewLossDetail("货损详情", table));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

int LossAdd(struct nk_context *context, struct Data *data)
{
    LinkedList *now = data->inventoryCheckList->next;
    LinkedList *rowNow = data->inventoryTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "库存编号");
            AppendTableRow(row, "货损数量");
            AppendTableRow(row, "货损原因");
            Table *table = NewTable(row, "");

            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->inventoryTable, rowNow->data, "id"));
            AppendTableRow(row, "0");
            AppendTableRow(row, "");
            AppendTable(table, row);

            PushWindow(context, NewLossEdit("货损编辑", data->id, data->password, table, 0));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

int LossModify(struct nk_context *context, struct Data *data)
{
    LinkedList *now = data->lossCheckList->next;
    LinkedList *rowNow = data->lossTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "id");
            AppendTableRow(row, "库存编号");
            AppendTableRow(row, "货损数量");
            AppendTableRow(row, "货损原因");
            Table *table = NewTable(row, "");

            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->lossTable, rowNow->data, "id"));
            AppendTableRow(row, GetRowItemByColumnName(data->lossTable, rowNow->data, "库存编号"));
            AppendTableRow(row, GetRowItemByColumnName(data->lossTable, rowNow->data, "货损数量"));
            AppendTableRow(row, GetRowItemByColumnName(data->lossTable, rowNow->data, "货损原因"));
            AppendTable(table, row);

            PushWindow(context, NewLossEdit("货损编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

void LossPageLayout(struct nk_context *context, struct Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);

    // title
    nk_layout_row_dynamic(context, 0, 1);
    {
        if (nk_style_push_font(context, &fontLarge->handle))
        {
            nk_label(context, "货损", NK_TEXT_LEFT);
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
            TableRow *row = data->lossTable == NULL ? NULL : GetTableTitle(data->lossTable);
            columnCount = row == NULL ? 0 : row->columnCount;
            if (data->lossProperties == NULL)
            {
                data->lossProperties = malloc((columnCount + 1) * sizeof(char *));
                data->lossProperties[0] = "无";

                LinkedList *rowNow = row == NULL ? NULL : row->items;
                for (int i = 1; i < columnCount + 1; i++)
                {
                    data->lossProperties[i] = rowNow->data;
                    rowNow = rowNow->next;
                }
            }
        }

        nk_layout_row_push(context, 140);
        {
            if (nk_style_push_font(context, &fontSmall->handle))
            {
                nk_combobox(context, data->lossProperties, columnCount + 1, &data->lossPropertySelected, 35,
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
                                           data->lossValueBuffer, BUFFER_SIZE * sizeof(char), nk_filter_default);
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
            nk_layout_row_push(context, 0.15);
            {
                if (nk_button_label(context, "查询"))
                {
                    SendLossRequest(data);
                }
            }

            nk_layout_row_push(context, 0.01);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.15);
            {
                if (nk_button_label(context, "查看"))
                {
                    if (!LossLookup(context, data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请选择一个货损条目");
                    }
                }
            }

            nk_layout_row_push(context, 0.01);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.08);
            {
                if (nk_button_label(context, "+"))
                {
                    if (!LossAdd(context, data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请在库存页面选择一个库存条目");
                    }
                }
            }

            nk_layout_row_push(context, 0.01);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.08);
            {
                if (nk_button_label(context, "-"))
                {
                    data->messageCallback = LossEntryDelete;
                    data->message = CloneString("是否确认要删除选中的货损条目");
                }
            }

            nk_layout_row_push(context, 0.01);
            {
                PlaceNothing(context);
            }

            nk_layout_row_push(context, 0.08);
            {
                if (nk_button_label(context, "~"))
                {
                    if (!LossModify(context, data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请选择一个货损条目");
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
                TableLayout(context, data->lossTable, data->lossCheckList,
                            data->lossPropertySelected == 0 ? NULL : data->lossProperties[data->lossPropertySelected],
                            data->lossValueBuffer);
                nk_group_end(context);
            }

            nk_style_pop_font(context);
        }
    }
}
