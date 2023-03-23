#include "../data/table.h"
#include "../services/judgeService.h"
#include "../utils.h"
#include "layout.h"
#include "../services/journalService.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

struct Data
{
    char id[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char message[BUFFER_SIZE];
};

int SendLoginRequest(const char *id, const char *password, char **name)
{
    int staffId;
    sscanf(id, "%d", &staffId);
    int loginSuccess;

    AddJournal(NULL, staffId, OP_LOGIN);
    Table *table = Judge(staffId, &loginSuccess, password, OP_LOGIN);

    if (loginSuccess)
    {
        *name = CloneString(GetRowItemByColumnName(table, GetRowByIndex(table, 1), "员工姓名"));
        FreeTable(table);
        return 1;
    }
    return 0;
}

void LoginButtonEventHandler(struct Data *data, Window *window)
{
    char *name;
    if (SendLoginRequest(data->id, data->password, &name))
    {
        Window *mainWindow = NewMainWindow("main", data->id, data->password, name);
        window->next = mainWindow;
        free(name);
    }
    else
    {
        strcpy(data->message, "用户名或密码错误");
    }
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
            if (nk_edit_string_zero_terminated(context,
                    NK_EDIT_BOX | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD | NK_EDIT_SIG_ENTER, data->id,
                    sizeof(data->id), nk_filter_decimal) & NK_EDIT_COMMITED)
            {
                LoginButtonEventHandler(data, window);
            }
        }
        nk_layout_row_end(context);
    }

    {
        nk_layout_row_begin(context, NK_DYNAMIC, 0, 2);
        {
            nk_layout_row_push(context, 0.25f);
            nk_label(context, "密码", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 0.75f);
            if (nk_edit_string_zero_terminated(context,
                    (NK_EDIT_BOX | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD | NK_EDIT_SIG_ENTER) & ~NK_EDIT_MULTILINE,
                    data->password, sizeof(data->password), nk_filter_ascii) & NK_EDIT_COMMITED)
            {
                LoginButtonEventHandler(data, window);
            }
        }
        nk_layout_row_end(context);
    }

    // focus on first textbox automatically
    if (!context->current->edit.active)
    {
        context->current->edit.active = 1;
    }

    // handle tab on first textbox
    if (nk_input_is_key_down(&context->input, NK_KEY_TAB) && context->current->edit.name == 0)
    {
        context->current->edit.name = 1;
        context->current->edit.cursor = 0;
    }

    nk_layout_row_dynamic(context, 50, 1);

    nk_layout_row_dynamic(context, 80, 3);
    PlaceNothing(context);
    if (nk_button_label(context, "登录"))
    {
        LoginButtonEventHandler(data, window);
    }
}

void FreeLoginWindow(Window *window)
{
    free(window->data);
    free(window);
}

Window *NewLoginWindow(const char *title)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
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
