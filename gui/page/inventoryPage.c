#include "../../data/amount.h"
#include "../../data/linkedList.h"
#include "../../data/operation.h"
#include "../../data/table.h"
#include "../../data/time.h"
#include "../../services/inventoryService.h"
#include "../../services/journalService.h"
#include "../../services/judgeService.h"
#include "../../utils.h"
#include "../config.h"
#include "../layout.h"
#include "../mainWindow.h"
#include "stddef.h"
#include <malloc.h>

static void MessageBoxCallBack(__attribute__((unused)) int ok, void *parameter)
{
    struct Data *data = parameter;
    free(data->message);
    data->message = NULL;
}

void InventoryDelete(int ok, void *parameter)
{
    MessageBoxCallBack(ok, parameter);
    if (ok == 0)
    {
        return;
    }

    struct Data *data = parameter;

    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_DELETE_INVENTORY);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("缺少权限：删除库存");
        return;
    }

    LinkedList *now = data->inventoryCheckList->next;
    LinkedList *rowNow = data->inventoryTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            char *id = GetRowItemByColumnName(data->inventoryTable, rowNow->data, "库存编号");

            TableRow *row = NewTableRow();
            AppendTableRow(row, "库存编号");
            Table *table = NewTable(row, NULL);

            row = NewTableRow();
            AppendTableRow(row, id);
            AppendTable(table, row);

            AddJournal(table, data->id, OP_DELETE_INVENTORY);
            Table *response = DeleteInventoryById(table);
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

void SendInventoryRequest(struct Data *data)
{
    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_READ_INVENTORY);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("缺少权限：读取库存");
        return;
    }

    AddJournal(NULL, data->id, OP_READ_INVENTORY);
    Table *response = ShowInventory(NULL);
    if (response != NULL)
    {
        if (response->remark != NULL && response->remark[0] != '\0')
        {
            data->messageCallback = MessageBoxCallBack;
            data->message = CloneString(response->remark);
        }

        FreeList(data->inventoryCheckList);
        data->inventoryCheckList = NewCheckList();
        data->inventoryTable = response;
    }
    else
    {
        data->messageCallback = MessageBoxCallBack;
        data->message = CloneString("查询失败: 响应为NULL");
    }
}

int InventoryLookup(struct Data *data)
{
    LinkedList *now = data->inventoryCheckList->next;
    LinkedList *rowNow = data->inventoryTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->inventoryTable));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(NewInventoryDetail("库存详情", table));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

int InventoryAdd(struct Data *data)
{
    LinkedList *now = data->itemCheckList->next;
    LinkedList *rowNow = data->itemTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "商品编号");
            AppendTableRow(row, "商品名称");
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
            Table *table = NewTable(row, "");
            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->itemTable, rowNow->data, "商品编号"));
            AppendTableRow(row, GetRowItemByColumnName(data->itemTable, rowNow->data, "商品名称"));
            AppendTableRow(row, "");
            AppendTableRow(row, "1");
            AppendTableRow(row, "1");
            AppendTableRow(row, "1");
            AppendTableRow(row, "1");
            AppendTableRow(row, "1");
            AppendTableRow(row, "1");
            AppendTableRow(row, "2");
            AppendTableRow(row, "2");
            AppendTableRow(row, "2");
            AppendTableRow(row, "2");
            AppendTableRow(row, "2");
            AppendTableRow(row, "2");
            AppendTableRow(row, "100");
            AppendTableRow(row, "3");
            AppendTableRow(row, "2");
            AppendTable(table, row);
            PushWindow(NewInventoryEdit("库存编辑", data->id, data->password, table, 0));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

int InventoryModify(struct Data *data)
{
    LinkedList *now = data->inventoryCheckList->next;
    LinkedList *rowNow = data->inventoryTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();
            AppendTableRow(row, "库存编号");
            AppendTableRow(row, "商品编号");
            AppendTableRow(row, "商品名称");
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
            Table *table = NewTable(row, "");
            row = NewTableRow();
            AppendTableRow(row, GetRowItemByColumnName(data->inventoryTable, rowNow->data, "库存编号"));
            AppendTableRow(row, GetRowItemByColumnName(data->inventoryTable, rowNow->data, "商品编号"));
            AppendTableRow(row, GetRowItemByColumnName(data->inventoryTable, rowNow->data, "商品名称"));
            AppendTableRow(row, GetRowItemByColumnName(data->inventoryTable, rowNow->data, "数量"));

            const char *time = GetRowItemByColumnName(data->inventoryTable, rowNow->data, "入库时间");
            TimeInfo info = ParseTime(time, 0);
            free(AppendTableRow(row, LongLongToString(info.year)));
            free(AppendTableRow(row, LongLongToString(info.month)));
            free(AppendTableRow(row, LongLongToString(info.day)));
            free(AppendTableRow(row, LongLongToString(info.hour)));
            free(AppendTableRow(row, LongLongToString(info.minute)));
            free(AppendTableRow(row, LongLongToString(info.second)));

            time = GetRowItemByColumnName(data->inventoryTable, rowNow->data, "生产日期");
            info = ParseTime(time, 0);
            free(AppendTableRow(row, LongLongToString(info.year)));
            free(AppendTableRow(row, LongLongToString(info.month)));
            free(AppendTableRow(row, LongLongToString(info.day)));
            free(AppendTableRow(row, LongLongToString(info.hour)));
            free(AppendTableRow(row, LongLongToString(info.minute)));
            free(AppendTableRow(row, LongLongToString(info.second)));

            Amount amount = ParseAmount(GetRowItemByColumnName(data->inventoryTable, rowNow->data, "购入单价"));
            free(AppendTableRow(row, LongLongToString(GetAmountYuan(&amount))));
            free(AppendTableRow(row, LongLongToString(GetAmountJiao(&amount))));
            free(AppendTableRow(row, LongLongToString(GetAmountCent(&amount))));

            AppendTable(table, row);
            PushWindow(NewInventoryEdit("库存编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return 1;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    return 0;
}

void InventoryPageLayout(struct nk_context *context, struct Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);

    // title
    nk_layout_row_dynamic(context, 0, 1);
    {
        if (nk_style_push_font(context, &fontLarge->handle))
        {
            nk_label(context, "库存", NK_TEXT_LEFT);
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
            TableRow *row = data->inventoryTable == NULL ? NULL : GetTableTitle(data->inventoryTable);
            columnCount = row == NULL ? 0 : row->columnCount;
            if (data->inventoryProperties == NULL)
            {
                data->inventoryProperties = malloc((columnCount + 1) * sizeof(char *));
                data->inventoryProperties[0] = "无";

                LinkedList *rowNow = row == NULL ? NULL : row->items;
                for (int i = 1; i < columnCount + 1; i++)
                {
                    data->inventoryProperties[i] = rowNow->data;
                    rowNow = rowNow->next;
                }
            }
        }

        nk_layout_row_push(context, 140);
        {
            if (nk_style_push_font(context, &fontSmall->handle))
            {
                nk_combobox(context, data->inventoryProperties, columnCount + 1, &data->inventoryPropertySelected, 35,
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
                                           data->inventoryValueBuffer, BUFFER_SIZE * sizeof(char), nk_filter_default);
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
                    SendInventoryRequest(data);
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
                    if (!InventoryLookup(data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请选择一个库存条目");
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
                    if (!InventoryAdd(data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请在商品页面选择一个商品条目");
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
                    data->messageCallback = InventoryDelete;
                    data->message = CloneString("是否确认要删除选中的库存条目");
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
                    if (!InventoryModify(data))
                    {
                        data->messageCallback = MessageBoxCallBack;
                        data->message = CloneString("请选择一个库存条目");
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
                TableLayout(context, data->inventoryTable, data->inventoryCheckList,
                            data->inventoryPropertySelected == 0
                                ? NULL
                                : data->inventoryProperties[data->inventoryPropertySelected],
                            data->inventoryValueBuffer);
                nk_group_end(context);
            }

            nk_style_pop_font(context);
        }
    }
}