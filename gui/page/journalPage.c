#include "design/linkedList.h"
#include "design/operation.h"
#include "design/table.h"
#include "design/journalService.h"
#include "design/judgeService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include "design/mainWindow.h"
#include <stddef.h>
#include <malloc.h>

void SendJournalRequest(struct Data *data)
{
    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_READ_JOURNAL);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("缺少权限：读取日志");
        return;
    }

    AddJournal(NULL, data->id, OP_READ_JOURNAL);
    Table *response = GetAllJournal(NULL);
    if (response != NULL)
    {
        if (response->remark != NULL && response->remark[0] != '\0')
        {
            data->messageCallback = MessageBoxCallback;
            data->message = CloneString(response->remark);
        }

        free(data->journalProperties);
        FreeList(data->journalCheckList);
        FreeTable(data->journalTable);
        data->journalCheckList = NewCheckList();
        data->journalTable = response;
        data->journalProperties = NULL;
    }
    else
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString("查询失败: 响应为NULL");
    }
}

void JournalLookup(struct Data *data)
{
    LinkedList *now = data->journalCheckList->next;
    LinkedList *rowNow = data->journalTable->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->journalTable));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(NewJournalDetail("日志详情", table));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个日志条目");
}

void JournalPageLayout(struct nk_context *context, struct Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);

    // title
    nk_layout_row_dynamic(context, 0, 1);
    {
        if (nk_style_push_font(context, &fontLarge->handle))
        {
            nk_label(context, "日志", NK_TEXT_LEFT);
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
            TableRow *row = data->journalTable == NULL ? NULL : GetTableTitle(data->journalTable);
            columnCount = row == NULL ? 0 : row->columnCount;
            if (data->journalProperties == NULL)
            {
                data->journalProperties = malloc((columnCount + 1) * sizeof(char *));
                data->journalProperties[0] = "无";

                LinkedList *rowNow = row == NULL ? NULL : row->items;
                for (int i = 1; i < columnCount + 1; i++)
                {
                    data->journalProperties[i] = rowNow->data;
                    rowNow = rowNow->next;
                }
            }
        }

        nk_layout_row_push(context, 140);
        {
            if (nk_style_push_font(context, &fontSmall->handle))
            {
                nk_combobox(context, data->journalProperties, columnCount + 1, &data->journalPropertySelected, 35,
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
                    data->journalValueBuffer, BUFFER_SIZE * sizeof(char), nk_filter_default);
        }

        nk_layout_row_push(context, 100);
        {
            nk_label(context, "进行筛选", NK_TEXT_LEFT);
        }

        nk_layout_row_end(context);
    }

    nk_layout_row_static(context, 10, 0, 0);

    OperationLayout(context,
            OP_TYPE_GET | OP_TYPE_DETAIL,
            (OperationHandler)SendJournalRequest,
            (OperationHandler)JournalLookup,
            NULL,
            NULL,
            NULL,
            data);

    nk_layout_row_dynamic(context, 10, 1);
    {
        struct nk_rect space;
        nk_widget(&space, context);
        struct nk_command_buffer *canvas = nk_window_get_canvas(context);
        nk_stroke_line(canvas, space.x, space.y + space.h / 2, space.x + space.w, space.y + space.h / 2, 1,
                nk_rgb(100, 100, 100));
    }

    char *from, *to;
    DateRangeFilterLayout(context, "筛选时间", &from, &to);

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
                TableLayout(context, data->journalTable, data->journalCheckList,
                        data->journalPropertySelected == 0 ? NULL
                                                           : data->journalProperties[data->journalPropertySelected],
                        data->journalValueBuffer,
                        "时间",
                        from,
                        to);
                nk_group_end(context);
            }

            nk_style_pop_font(context);
        }
    }
}
