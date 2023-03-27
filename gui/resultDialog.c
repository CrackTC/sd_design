#include "design/layout.h"
#include "design/table.h"
#include <malloc.h>

void ResultDialogLayout(struct nk_context *context, Window *window)
{
    TableLayout(context, window->data, NULL, NULL, NULL);
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
