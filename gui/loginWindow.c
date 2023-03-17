#include "../data/table.h"
#include "../services/judgeService.h"
#include "layout.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

struct Data
{
    char id[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char message[BUFFER_SIZE];
};

int SendLoginRequest(const char *id, const char *password)
{
#warning
    return 1;
    int staffId;
    sscanf(id, "%d", &staffId);
    Table *table = judge(staffId, password, OP_LOGIN, NULL);

    int result = 0;
    if (strcmp(GetRowItemByColumnName(table, GetRowByIndex(table, 1), "success"), "1")) {
        result = 1;
    }

    FreeTable(table);
    return result;
}

void loginWindowLayout(struct nk_context *context, Window *window)
{
    EnsureWindowSize(context, window, 400, 500);
    struct Data *data = window->data;

    nk_layout_row_dynamic(context, 120, 1);
    nk_label(context, data->message, NK_TEXT_CENTERED);

    {
        nk_layout_row_begin(context, NK_DYNAMIC, 0, 2);
        {
            nk_layout_row_push(context, 0.25f);
            nk_label(context, "工号", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 0.75f);
            nk_edit_string_zero_terminated(context, NK_EDIT_BOX | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD, data->id,
                                           sizeof(data->id), nk_filter_decimal);
        }
        nk_layout_row_end(context);
    }

    {
        nk_layout_row_begin(context, NK_DYNAMIC, 0, 2);
        {
            nk_layout_row_push(context, 0.25f);
            nk_label(context, "密码", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 0.75f);
            nk_edit_string_zero_terminated(context,
                                           (NK_EDIT_BOX | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD) & ~NK_EDIT_MULTILINE,
                                           data->password, sizeof(data->password), nk_filter_ascii);
        }
        nk_layout_row_end(context);
    }

    nk_layout_row_dynamic(context, 50, 1);

    nk_layout_row_dynamic(context, 80, 3);
    PlaceNothing(context);
    if (nk_button_label(context, "登录"))
    {
        if (SendLoginRequest(data->id, data->password))
        {
            Window *mainWindow = NewMainWindow(1, "main", data->id, data->password);
            window->next = mainWindow;
        }
        else
        {
            strcpy(data->message, "用户名或密码错误");
        }
    }
}

void FreeLoginWindow(Window *window)
{
    free(window->data);
    free(window);
}

Window *NewLoginWindow(int isVisible, const char *title)
{
    Window *window = malloc(sizeof(Window));
    window->isVisible = isVisible;
    window->layoutFunc = loginWindowLayout;
    window->freeFunc = FreeLoginWindow;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    strcpy(data->id, "");
    strcpy(data->password, "");
    strcpy(data->message, "登录");
    window->data = data;

    window->next = NULL;
    return window;
}
