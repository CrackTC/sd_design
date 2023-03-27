#include "design/layout.h"
#include "design/table.h"
#include "../config.h"
#include <malloc.h>

void ResultDialogLayout(struct nk_context *context, Window *window)
{
    nk_window_set_size(context, window->title, nk_vec2(800, 600));

    nk_layout_row_dynamic(context, 450, 1);
    nk_style_push_font(context, &fontSmall->handle);
    {
        if (nk_group_begin(context,"groupTable", NK_WINDOW_BORDER))
        {
            TableLayout(context, window->data, NULL, NULL, NULL, NULL, NULL, NULL);
            nk_group_end(context);
        }
        nk_style_pop_font(context);
    }
    nk_layout_row_dynamic(context, 20, 0);
    nk_layout_row_dynamic(context, 0, 3);
    PlaceNothing(context);
    if (nk_button_label(context, "确定"))
    {
        window->isClosed = 1;
    }
}

void FreeResultDialog(Window *window)
{
    FreeTable(window->data);
    free(window);
}

Window *NewResultDialog(const char *title, Table *data)
{
    Window *window = malloc(sizeof(Window));
    window->title = title;
    window->isClosed = 0;
    window->next = NULL;
    window->data = data;
    window->layoutFunc = ResultDialogLayout;
    window->freeFunc = FreeResultDialog;
    return window;
}
