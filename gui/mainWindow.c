#include "design/mainWindow.h"
#include "design/utils.h"
#include "design/layout.h"
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define SECTION_COUNT 11

const char *const sections[SECTION_COUNT] = { "", "商品", "库存", "货损", "订单", "折扣",
                                              "顾客", "员工", "日志", "统计", "退货" };
const LayoutFunc pages[SECTION_COUNT] = { WelcomePageLayout, ItemPageLayout, InventoryPageLayout, LossPageLayout,
                                          OrderPageLayout, DiscountPageLayout, CustomerPageLayout, StaffPageLayout,
                                          JournalPageLayout, ProfitPageLayout, RefundPageLayout };

void MainWindowLayout(struct nk_context *context, Window *window)
{
    EnsureWindowSize(context, window, 400, 400);
    struct Data *data = window->data;

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
    struct Data *data = window->data;
    free(data->inventoryValueBuffer);
    free(data->password);
    free(data->message);
    free(data->inventoryProperties);
    FreeTable(data->inventoryTable);
    FreeList(data->inventoryCheckList);
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

    data->inventoryCheckList = NewCheckList();
    data->inventoryProperties = NULL;

    {
        TableRow *row = NewTableRow();
        AppendTableRow(row, "库存编号");
        AppendTableRow(row, "商品编号");
        AppendTableRow(row, "商品名称");
        AppendTableRow(row, "数量");
        AppendTableRow(row, "入库时间");
        AppendTableRow(row, "生产日期");
        AppendTableRow(row, "购入单价");
        Table *table = NewTable(row, NULL);
        data->inventoryTable = table;
    }

    data->itemPropertySelected = 0;
    data->itemValueBuffer = malloc(BUFFER_SIZE * sizeof(char));
    memset(data->itemValueBuffer, 0, BUFFER_SIZE * sizeof(char));

    data->itemCheckList = NewCheckList();

    data->itemProperties = NULL;

    {
        TableRow *row = NewTableRow();
        AppendTableRow(row, "商品编号");
        AppendTableRow(row, "商品名称");
        AppendTableRow(row, "售价");
        AppendTableRow(row, "保质期");
        Table *table = NewTable(row, NULL);
        data->itemTable = table;
    }

    data->discountPropertySelected = 0;
    data->discountValueBuffer = malloc(BUFFER_SIZE * sizeof(char));
    memset(data->discountValueBuffer, 0, BUFFER_SIZE * sizeof(char));

    data->discountCheckList = NewCheckList();

    data->discountProperties = NULL;

    {
        TableRow *row = NewTableRow();
        AppendTableRow(row, "折扣编号");
        AppendTableRow(row, "商品编号");
        AppendTableRow(row, "商品名称");
        AppendTableRow(row, "折扣比率");
        AppendTableRow(row, "客户等级");
        AppendTableRow(row, "截止日期");
        Table *table = NewTable(row, NULL);
        data->discountTable = table;
    }

    data->orderPropertySelected = 0;
    data->orderValueBuffer = malloc(BUFFER_SIZE * sizeof(char));
    memset(data->orderValueBuffer, 0, BUFFER_SIZE * sizeof(char));

    data->orderCheckList = NewCheckList();

    data->orderProperties = NULL;

    {
        TableRow *row = NewTableRow();
        AppendTableRow(row, "订单编号");
        AppendTableRow(row, "库存编号");
        AppendTableRow(row, "商品编号");
        AppendTableRow(row, "商品名称");
        AppendTableRow(row, "客户编号");
        AppendTableRow(row, "客户姓名");
        AppendTableRow(row, "购买数量");
        AppendTableRow(row, "购买时间");
        AppendTableRow(row, "总价");
        Table *table = NewTable(row, NULL);
        data->orderTable = table;
    }

    data->customerPropertySelected = 0;
    data->customerValueBuffer = malloc(BUFFER_SIZE * sizeof(char));
    memset(data->customerValueBuffer, 0, BUFFER_SIZE * sizeof(char));

    data->customerCheckList = NewCheckList();

    data->customerProperties = NULL;

    {
        TableRow *row = NewTableRow();
        AppendTableRow(row, "客户编号");
        AppendTableRow(row, "客户等级");
        AppendTableRow(row, "客户姓名");
        AppendTableRow(row, "客户联系方式");
        Table *table = NewTable(row, NULL);
        data->customerTable = table;
    }

    data->journalPropertySelected = 0;
    data->journalValueBuffer = malloc(BUFFER_SIZE * sizeof(char));
    memset(data->journalValueBuffer, 0, BUFFER_SIZE * sizeof(char));

    data->journalCheckList = NewCheckList();

    data->journalProperties = NULL;

    {
        TableRow *row = NewTableRow();
        AppendTableRow(row, "员工编号");
        AppendTableRow(row, "时间");
        AppendTableRow(row, "操作");
        AppendTableRow(row, "参数");
        Table *table = NewTable(row, NULL);
        data->journalTable = table;
    }

    data->lossPropertySelected = 0;
    data->lossValueBuffer = malloc(BUFFER_SIZE * sizeof(char));
    memset(data->lossValueBuffer, 0, BUFFER_SIZE * sizeof(char));

    data->lossCheckList = NewCheckList();

    data->lossProperties = NULL;

    {
        TableRow *row = NewTableRow();
        AppendTableRow(row, "货损编号");
        AppendTableRow(row, "库存编号");
        AppendTableRow(row, "损耗时间");
        AppendTableRow(row, "货损数量");
        AppendTableRow(row, "货损原因");
        Table *table = NewTable(row, NULL);
        data->lossTable = table;
    }

    data->profitPropertySelected = 0;
    data->profitValueBuffer = malloc(BUFFER_SIZE * sizeof(char));
    memset(data->profitValueBuffer, 0, BUFFER_SIZE * sizeof(char));

    data->profitCheckList = NewCheckList();

    data->profitProperties = NULL;

    {
        TableRow *row = NewTableRow();
        AppendTableRow(row, "收支");
        AppendTableRow(row, "事项");
        AppendTableRow(row, "时间");
        Table *table = NewTable(row, NULL);
        data->profitTable = table;
    }

    data->refundPropertySelected = 0;
    data->refundValueBuffer = malloc(BUFFER_SIZE * sizeof(char));
    memset(data->refundValueBuffer, 0, BUFFER_SIZE * sizeof(char));

    data->refundCheckList = NewCheckList();

    data->refundProperties = NULL;

    {
        TableRow *row = NewTableRow();
        AppendTableRow(row, "订单编号");
        AppendTableRow(row, "退款原因");
        AppendTableRow(row, "时间");
        AppendTableRow(row, "退款");
        AppendTableRow(row, "退回数目");
        AppendTableRow(row, "备注");
        Table *table = NewTable(row, NULL);
        data->refundTable = table;
    }

    data->staffPropertySelected = 0;
    data->staffValueBuffer = malloc(BUFFER_SIZE * sizeof(char));
    memset(data->staffValueBuffer, 0, BUFFER_SIZE * sizeof(char));

    data->staffCheckList = NewCheckList();

    data->staffProperties = NULL;

    {
        TableRow *row = NewTableRow();
        AppendTableRow(row, "员工编号");
        AppendTableRow(row, "员工可用性");
        AppendTableRow(row, "员工姓名");
        AppendTableRow(row, "员工联系方式");
        AppendTableRow(row, "员工权限");
        Table *table = NewTable(row, NULL);
        data->staffTable = table;
    }

    window->data = data;
    window->next = NULL;
    return window;
}
