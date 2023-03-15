#include "layout.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Data
{
    char id[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char message[BUFFER_SIZE];
};

void mainWindowLayout(struct nk_context *context, Window *window)
{
    /* nk_layout_row_dynamic(context, 120, 1); */
    /* nk_label(context, "label", NK_TEXT_CENTERED); */

    /* nk_layout_row_dynamic(context, 80, 3); */
    /* struct nk_rect space; */
    /* nk_widget(&space, context); */
    /* if (nk_button_label(context, "按钮")) */
    /* { */
    /*     printf("button pressed\n"); */
    /* } */
    EnsureWindowSize(context, window, 400, 400);
    struct Data *data = window->data;

    nk_layout_row_begin(context, NK_DYNAMIC, nk_window_get_height(context) - 68, 2);
    {
        nk_layout_row_push(context, 0.2f);
        nk_group_begin(context, "menu", NK_WINDOW_BORDER);
        nk_group_end(context);

        nk_layout_row_push(context, 0.8f);
    }
    nk_layout_row_end(context);
}

void FreeMainWindow(Window *window)
{
    free(window->data);
    free(window);
}

Window *NewMainWindow(int isVisible, const char *title, const char *id, const char *password)
{
    Window *window = malloc(sizeof(Window));
    window->isVisible = isVisible;
    window->layoutFunc = mainWindowLayout;
    window->freeFunc = FreeMainWindow;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    strcpy(data->id, id);
    strcpy(data->password, password);
    strcpy(data->message, "");
    window->data = data;

    window->next = NULL;
    return window;
}
