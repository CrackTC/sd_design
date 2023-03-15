#include "layout.h"
#include "../data/table.h"
#include <malloc.h>
#include <string.h>
#define BUFFER_SIZE 256

struct Data
{
    char id[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char message[BUFFER_SIZE];
};

int SendLoginRequest(const char *id, const char *password)
{
#warning
}

void loginWindowLayout(struct nk_context *context, void *data)
{
    struct Data *d = data;

    nk_layout_row_dynamic(context, 120, 1);
    nk_label(context, d->message, NK_TEXT_CENTERED);

    {
        nk_layout_row_begin(context, NK_DYNAMIC, 0, 2);
        nk_layout_row_push(context, 0.25f);
        nk_label(context, "工号", NK_TEXT_CENTERED);

        nk_layout_row_push(context, 0.75f);
        nk_edit_string_zero_terminated(
            context,
            NK_EDIT_BOX | NK_EDIT_AUTO_SELECT, // focus will auto select all text (NK_EDIT_BOX not sure)
            d->id, sizeof(d->id), nk_filter_ascii);
        nk_layout_row_end(context);
    }

    {
        nk_layout_row_begin(context, NK_DYNAMIC, 0, 2);
        nk_layout_row_push(context, 0.25f);
        nk_label(context, "密码", NK_TEXT_CENTERED);

        nk_layout_row_push(context, 0.75f);
        nk_edit_string_zero_terminated(
            context,
            NK_EDIT_BOX | NK_EDIT_AUTO_SELECT, // focus will auto select all text (NK_EDIT_BOX not sure)
            d->password, sizeof(d->password), nk_filter_ascii);
        nk_layout_row_end(context);
    }

    nk_layout_row_dynamic(context, 50, 1);
    nk_layout_row_dynamic(context, 80, 3);
    struct nk_rect none = nk_rect(0, 0, 0, 0);
    nk_widget(&none, context);
    if (nk_button_label(context, "登录"))
    {
        if (SendLoginRequest(d->id, d->password)) {
#warning
        }
        else
        {
            strcpy(d->message, "用户名或密码错误");
        }
    }
}

Window *NewLoginWindow(int isVisible, const char *title)
{
    Window *window = malloc(sizeof(Window));
    window->isVisible = isVisible;
    window->layoutFunc = loginWindowLayout;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    strcpy(data->id, "");
    strcpy(data->password, "");
    strcpy(data->message, "登录");
    window->data = data;

    window->next = NULL;
    return window;
}
