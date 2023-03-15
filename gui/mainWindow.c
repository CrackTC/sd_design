#include "layout.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

void mainWindowLayout(struct nk_context *context, void *data)
{
    nk_layout_row_dynamic(context, 120, 1);
    nk_label(context, "label", NK_TEXT_CENTERED);

    nk_layout_row_dynamic(context, 80, 3);
    struct nk_rect space;
    nk_widget(&space, context);
    if (nk_button_label(context, "按钮"))
    {
        printf("button pressed\n");
    }
}

Window *NewMainWindow(int isVisible, const char *title)
{
    Window *window = malloc(sizeof(Window));
    window->isVisible = isVisible;
    window->layoutFunc = mainWindowLayout;
    window->title = title;
    window->data = NULL;
    window->next = NULL;
    return window;
}
