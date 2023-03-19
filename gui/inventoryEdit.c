#include "config.h"
#include "layout.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Data
{
    struct Table *inventory;
    int id;
    const char *password;
};

void InventoryEditLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    TableRow *dataRow = GetRowByIndex(data->inventory, 1);

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "库存编辑", NK_TEXT_CENTERED);

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "商品编号", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT,
                                       GetRowItemByColumnName(data->inventory, dataRow, "商品编号"), 512,
                                       nk_filter_decimal);

        nk_label(context, "商品名称", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT,
                                       GetRowItemByColumnName(data->inventory, dataRow, "商品名称"), 512,
                                       nk_filter_default);

        nk_label(context, "数量", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT,
                                       GetRowItemByColumnName(data->inventory, dataRow, "数量"), 512,
                                       nk_filter_decimal);

        nk_label(context, "入库时间", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT,
                                       GetRowItemByColumnName(data->inventory, dataRow, "入库时间"), 512,
                                       nk_filter_default);

        nk_label(context, "购入单价", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(context, NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT,
                                       GetRowItemByColumnName(data->inventory, dataRow, "购入单价"), 512,
                                       nk_filter_default);
        if (nk_button_label(context, "确定"))
        {
            window->isClosed = 1;
        }
        nk_style_pop_font(context);
    }
}

void FreeInventoryEdit(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->inventory);
    free(window);
}

Window *NewInventoryEdit(const char *title, int id, const char *password, Table *inventory)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = InventoryEditLayout;
    window->freeFunc = FreeInventoryEdit;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));
    data->inventory = CloneTableBuffered(inventory, 512);
    data->id = id;
    data->password = password;

    window->data = data;
    window->next = NULL;

    return window;
}
