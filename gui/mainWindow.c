#include "mainWindow.h"
#include "../data/table.h"
#include "../utils.h"
#include "layout.h"
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define SECTION_COUNT 7

const char *const sections[SECTION_COUNT] = {"", "商品", "库存", "销售", "顾客", "员工", "日志"};
const LayoutFunc pages[SECTION_COUNT] = {WelcomePageLayout, ItemPageLayout,    InventoryPageLayout, WelcomePageLayout,
                                         WelcomePageLayout, WelcomePageLayout, WelcomePageLayout};

void MainWindowLayout(struct nk_context *context, Window *window)
{
    EnsureWindowSize(context, window, 400, 400);
    struct Data *data = window->data;

    nk_layout_row_begin(context, NK_DYNAMIC, nk_window_get_height(context) - 68, 2);
    {
        nk_layout_row_push(context, 0.2f);
        if (nk_group_begin(context, "menu", NK_WINDOW_BORDER))
        {
            nk_layout_row_dynamic(context, 40, 1);
            for (int i = 1; i < SECTION_COUNT; i++)
            {
                struct nk_style_button backup = context->style.button;
                if (i == data->sectionSelected)
                {
                    context->style.button.normal = nk_style_item_color(nk_rgb(150, 150, 150));
                    context->style.button.hover = nk_style_item_color(nk_rgb(200, 200, 200));
                    context->style.button.text_normal = nk_rgb(50, 50, 50);
                    context->style.button.text_hover = nk_rgb(50, 50, 50);
                }
                if (nk_button_label(context, sections[i]))
                {
                    data->sectionSelected = i;
                }
                if (i == data->sectionSelected)
                {
                    context->style.button = backup;
                }
            }
            nk_group_end(context);
        }

        nk_layout_row_push(context, 0.799f);
        if (nk_group_begin(context, "page", NK_WINDOW_BORDER))
        {
            pages[data->sectionSelected](context, window);
            nk_group_end(context);
        }

        nk_layout_row_end(context);
    }
}

static void FreeCheckList(LinkedList *checkList)
{
    if (checkList == NULL)
        return;
    FreeCheckList(checkList->next);
    free(checkList->data);
    free(checkList);
}

void FreeMainWindow(Window *window)
{
    struct Data *data = window->data;
    free(data->inventoryValueBuffer);
    free(data->password);
    free(data->message);
    free(data->inventoryProperties);
    FreeTable(data->inventoryTable);
    FreeCheckList(data->inventoryCheckList);
    free(data);
    free(window);
}

Window *NewMainWindow(const char *title, const char *id, const char *password, const char *name)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = MainWindowLayout;
    window->freeFunc = FreeMainWindow;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    sscanf(id, "%d", &data->id);
    data->name = CloneString(name);
    data->password = CloneString(password);
    data->message = NULL;
    data->sectionSelected = 0;

    data->inventoryPropertySelected = 0;
    data->inventoryValueBuffer = malloc(BUFFER_SIZE * sizeof(char));
    memset(data->inventoryValueBuffer, 0, BUFFER_SIZE * sizeof(char));

    int *a = malloc(sizeof(int *));
    *a = 0;
    data->inventoryCheckList = AppendData(data->inventoryCheckList, a);

    data->inventoryProperties = NULL;

    {
#warning
        TableRow *row = NewTableRow();
        AppendTableRow(row, "id");
        AppendTableRow(row, "商品编号");
        AppendTableRow(row, "商品名称");
        AppendTableRow(row, "数量");
        AppendTableRow(row, "入库时间");
        AppendTableRow(row, "生产日期");
        AppendTableRow(row, "购入单价");
        Table *table = NewTable(row, NULL);

        row = NewTableRow();
        AppendTableRow(row, "id");
        AppendTableRow(row, "商品编号");
        AppendTableRow(row, "商品名称");
        AppendTableRow(row, "数量");
        AppendTableRow(row, "入库时间");
        AppendTableRow(row, "生产日期");
        AppendTableRow(row, "购入单价");
        AppendTable(table, row);

        row = NewTableRow();
        AppendTableRow(row, "id");
        AppendTableRow(row, "商品编号");
        AppendTableRow(row, "商品名称");
        AppendTableRow(row, "数量");
        AppendTableRow(row, "入库时间");
        AppendTableRow(row, "生产日期");
        AppendTableRow(row, "购入单价");
        AppendTable(table, row);

        row = NewTableRow();
        AppendTableRow(row, "id");
        AppendTableRow(row, "商品编号");
        AppendTableRow(row, "商品名称");
        AppendTableRow(row, "数量");
        AppendTableRow(row, "入库时间");
        AppendTableRow(row, "生产日期");
        AppendTableRow(row, "购入单价");
        AppendTable(table, row);

        data->inventoryTable = table;
    }

    data->itemPropertySelected = 0;
    data->itemValueBuffer = malloc(BUFFER_SIZE * sizeof(char));
    memset(data->itemValueBuffer, 0, BUFFER_SIZE * sizeof(char));

    a = malloc(sizeof(int *));
    *a = 0;
    data->itemCheckList = AppendData(data->itemCheckList, a);

    data->itemProperties = NULL;

    {
#warning
        TableRow *row = NewTableRow();
        AppendTableRow(row, "id");
        AppendTableRow(row, "商品名称");
        Table *table = NewTable(row, NULL);

        row = NewTableRow();
        AppendTableRow(row, "0");
        AppendTableRow(row, "农夫山泉");
        AppendTable(table, row);

        row = NewTableRow();
        AppendTableRow(row, "1");
        AppendTableRow(row, "二锅头");
        AppendTable(table, row);

        row = NewTableRow();
        AppendTableRow(row, "2");
        AppendTableRow(row, "威士忌");
        AppendTable(table, row);

        data->itemTable = table;
    }

    window->data = data;
    window->next = NULL;
    return window;
}
