#include "design/mainWindow.h"
#include "design/utils.h"
#include "design/layout.h"
#include "../config.h"
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

//const char *const sections[SECTION_COUNT] = { "", "商品", "库存", "货损", "订单", "退货", "折扣",
//                                              "顾客", "员工", "日志", "统计" };
//const LayoutFunc pages[SECTION_COUNT] = { WelcomePageLayout, ItemPageLayout, InventoryPageLayout, LossPageLayout,
//                                          OrderPageLayout, RefundPageLayout, DiscountPageLayout, CustomerPageLayout, StaffPageLayout,
//                                          JournalPageLayout, ProfitPageLayout };
const char *sections[SECTION_COUNT];
LayoutFunc pages[SECTION_COUNT];

void MainWindowLayout(struct nk_context *context, Window *window)
{
    EnsureWindowSize(context, window, 400, 400);
    struct MainWindowData *data = window->data;

    if (data->context == NULL)
    {
        data->context = context;
    }

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

void FreeMainWindow(Window *window)
{
    struct MainWindowData *data = window->data;
    for (int i = 0; i < SECTION_COUNT; i++)
    {
        free(data->dataArray[i].valueBuffer);
        free(data->dataArray[i].properties);
        free(data->dataArray[i].table);
        free(data->dataArray[i].checkList);
    }

    free(data->password);
    free(data->message);
    free(data);
    free(window);
}

void InitSections()
{
    sections[WELCOME_INDEX] = WELCOME_NAME;
    sections[ITEM_INDEX] = ITEM_NAME;
    sections[INVENTORY_INDEX] = INVENTORY_NAME;
    sections[LOSS_INDEX] = LOSS_NAME;
    sections[ORDER_INDEX] = ORDER_NAME;
    sections[REFUND_INDEX] = REFUND_NAME;
    sections[DISCOUNT_INDEX] = DISCOUNT_NAME;
    sections[CUSTOMER_INDEX] = CUSTOMER_NAME;
    sections[STAFF_INDEX] = STAFF_NAME;
    sections[JOURNAL_INDEX] = JOURNAL_NAME;
    sections[PROFIT_INDEX] = PROFIT_NAME;

    pages[WELCOME_INDEX] = WelcomePageLayout;
    pages[ITEM_INDEX] = ItemPageLayout;
    pages[INVENTORY_INDEX] = InventoryPageLayout;
    pages[LOSS_INDEX] = LossPageLayout;
    pages[ORDER_INDEX] = OrderPageLayout;
    pages[REFUND_INDEX] = RefundPageLayout;
    pages[DISCOUNT_INDEX] = DiscountPageLayout;
    pages[CUSTOMER_INDEX] = CustomerPageLayout;
    pages[STAFF_INDEX] = StaffPageLayout;
    pages[JOURNAL_INDEX] = JournalPageLayout;
    pages[PROFIT_INDEX] = ProfitPageLayout;
}

Window *NewMainWindow(const char *title, const char *id, const char *password, const char *name)
{
    InitSections();

    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = MainWindowLayout;
    window->freeFunc = FreeMainWindow;
    window->title = title;

    struct MainWindowData *data = malloc(sizeof(struct MainWindowData));
    memset(data, 0, sizeof(struct MainWindowData));
    sscanf(id, "%d", &data->id);
    data->name = CloneString(name);
    data->password = CloneString(password);
    data->message = NULL;
    data->sectionSelected = 0;

    data->dataArray = malloc(SECTION_COUNT * sizeof(struct PageData));
    for (int i = 1; i < SECTION_COUNT; i++)
    {
        data->dataArray[i].propertySelected = 0;
        data->dataArray[i].valueBuffer = malloc(BUFFER_SIZE * sizeof(char));
        memset(data->dataArray[i].valueBuffer, 0, BUFFER_SIZE * sizeof(char));
        data->dataArray[i].checkList = NewCheckList();
        data->dataArray[i].properties = NULL;

        TableRow *row = NewTableRow();
        AppendTableRow(row, "点击查询以获取数据");
        data->dataArray[i].table = NewTable(row, NULL);
    }

    window->data = data;
    window->next = NULL;
    return window;
}
