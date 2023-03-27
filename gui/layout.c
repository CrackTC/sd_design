#include "design/layout.h"
#include "design/time.h"
#include <malloc.h>
#include <stddef.h>
#include "../config.h"

static struct nk_rect none = { 0, 0, 0, 0 };

void PlaceNothing(struct nk_context *context)
{
    nk_widget(&none, context);
}

float Max(float a, float b)
{
    return a > b ? a : b;
}

void EnsureWindowSize(struct nk_context *context, Window *window, float width, float height)
{
    float newHeight = Max(height, nk_window_get_height(context));
    float newWidth = Max(width, nk_window_get_width(context));
    nk_window_set_size(context, window->title, nk_vec2(newWidth, newHeight));
}

void DrawMessageBox(struct nk_context *context, const char *title, int draw, const char *message,
        void (*callback)(int, void *), void *parameter)
{
    if (draw)
    {
        if (nk_popup_begin(context, NK_POPUP_DYNAMIC, title, 0, nk_rect(0, 100, 800, 400)))
        {
            nk_layout_row_dynamic(context, 0, 1);
            {
                nk_label(context, message, NK_TEXT_CENTERED);
            }

            nk_layout_row_dynamic(context, 0, 2);
            {
                if (nk_button_label(context, "确定"))
                {
                    if (callback != NULL)
                    {
                        callback(1, parameter);
                    }
                    nk_popup_close(context);
                }
                if (nk_button_label(context, "取消"))
                {
                    if (callback != NULL)
                    {
                        callback(0, parameter);
                    }
                    nk_popup_close(context);
                }
            }

            nk_popup_end(context);
        }
    }
}

void MessageBoxCallback(int ok, void *parameter)
{
    struct EditData *data = parameter;
    free(data->message);
    data->message = NULL;
}

void FinishCallback(int ok, void *parameter)
{
    MessageBoxCallback(ok, parameter);
    struct EditData *data = parameter;
    data->window->isClosed = 1;
}

void OperationLayout(struct nk_context *context,
        enum OperationTypes types,
        OperationHandler getHandler,
        OperationHandler detailHandler,
        OperationHandler addHandler,
        OperationHandler deleteHandler,
        OperationHandler updateHandler,
        void *data)
{
    nk_layout_row_begin(context, NK_DYNAMIC, 35, 10);
    {
        if (nk_style_push_font(context, &fontSmall->handle))
        {
            if (types & OP_TYPE_GET)
            {
                nk_layout_row_push(context, 0.15f);
                {
                    if (nk_button_label(context, "查询"))
                    {
                        getHandler(data);
                    }
                }

                nk_layout_row_push(context, 0.01f);
                {
                    PlaceNothing(context);
                }
            }

            if (types & OP_TYPE_DETAIL)
            {
                nk_layout_row_push(context, 0.15f);
                {
                    if (nk_button_label(context, "查看"))
                    {
                        detailHandler(data);
                    }
                }

                nk_layout_row_push(context, 0.01f);
                {
                    PlaceNothing(context);
                }
            }

            if (types & OP_TYPE_ADD)
            {
                nk_layout_row_push(context, 0.08f);
                {
                    if (nk_button_label(context, "+"))
                    {
                        addHandler(data);
                    }
                }

                nk_layout_row_push(context, 0.01f);
                {
                    PlaceNothing(context);
                }
            }

            if (types & OP_TYPE_DELETE)
            {
                nk_layout_row_push(context, 0.08f);
                {
                    if (nk_button_label(context, "-"))
                    {
                        deleteHandler(data);
                    }
                }

                nk_layout_row_push(context, 0.01f);
                {
                    PlaceNothing(context);
                }
            }

            if (types & OP_TYPE_UPDATE)
            {
                nk_layout_row_push(context, 0.08f);
                {
                    if (nk_button_label(context, "~"))
                    {
                        updateHandler(data);
                    }
                }

                nk_layout_row_push(context, 0.01f);
                {
                    PlaceNothing(context);
                }
            }

            nk_style_pop_font(context);
        }
        nk_layout_row_end(context);
    }
}

LinkedList *NewCheckList()
{
    LinkedList *result = malloc(sizeof(LinkedList));
    result->data = malloc(sizeof(int));
    *(int *)result->data = 0;
    result->next = NULL;
    return result;
}

void DateRangeFilterLayout(struct nk_context *context, const char *title, char **from, char **to)
{
    static char y1[5];
    static char m1[3];
    static char d1[3];
    static char h1[3] = "0";
    static char min1[3] = "0";
    static char s1[3] = "0";
    static char y2[5];
    static char m2[3];
    static char d2[3];
    static char h2[3] = "0";
    static char min2[3] = "0";
    static char s2[3] = "0";
    static char dateFrom[20];
    static char dateTo[20];
    static int filterDate;

    *from = *to = NULL;
    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_static(context, 0, 200, 1);
        nk_checkbox_label(context, title, &filterDate);
        nk_style_pop_font(context);
    }

    if (filterDate == 0)
    {
        return;
    }

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_begin(context, NK_STATIC, 0, 13);
        {
            nk_layout_row_push(context, 30);
            nk_label(context, "从", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 60);
            nk_edit_string_zero_terminated(context, NK_EDIT_BOX & ~NK_EDIT_MULTILINE | NK_EDIT_AUTO_SELECT, y1, 5,
                    nk_filter_decimal);

            nk_layout_row_push(context, 30);
            nk_label(context, "年", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(context, NK_EDIT_BOX & ~NK_EDIT_MULTILINE | NK_EDIT_AUTO_SELECT, m1, 3,
                    nk_filter_decimal);

            nk_layout_row_push(context, 30);
            nk_label(context, "月", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(context, NK_EDIT_BOX & ~NK_EDIT_MULTILINE | NK_EDIT_AUTO_SELECT, d1, 3,
                    nk_filter_decimal);

            nk_layout_row_push(context, 30);
            nk_label(context, "日", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(context, NK_EDIT_BOX & ~NK_EDIT_MULTILINE | NK_EDIT_AUTO_SELECT, h1, 3,
                    nk_filter_decimal);

            nk_layout_row_push(context, 30);
            nk_label(context, "时", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(context, NK_EDIT_BOX & ~NK_EDIT_MULTILINE | NK_EDIT_AUTO_SELECT, min1, 3,
                    nk_filter_decimal);

            nk_layout_row_push(context, 30);
            nk_label(context, "分", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(context, NK_EDIT_BOX & ~NK_EDIT_MULTILINE | NK_EDIT_AUTO_SELECT, s1, 3,
                    nk_filter_decimal);

            nk_layout_row_push(context, 30);
            nk_label(context, "秒", NK_TEXT_CENTERED);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 13);
        {
            nk_layout_row_push(context, 30);
            nk_label(context, "到", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 60);
            nk_edit_string_zero_terminated(context, NK_EDIT_BOX & ~NK_EDIT_MULTILINE | NK_EDIT_AUTO_SELECT, y2, 5,
                    nk_filter_decimal);

            nk_layout_row_push(context, 30);
            nk_label(context, "年", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(context, NK_EDIT_BOX & ~NK_EDIT_MULTILINE | NK_EDIT_AUTO_SELECT, m2, 3,
                    nk_filter_decimal);

            nk_layout_row_push(context, 30);
            nk_label(context, "月", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(context, NK_EDIT_BOX & ~NK_EDIT_MULTILINE | NK_EDIT_AUTO_SELECT, d2, 3,
                    nk_filter_decimal);

            nk_layout_row_push(context, 30);
            nk_label(context, "日", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(context, NK_EDIT_BOX & ~NK_EDIT_MULTILINE | NK_EDIT_AUTO_SELECT, h2, 3,
                    nk_filter_decimal);

            nk_layout_row_push(context, 30);
            nk_label(context, "时", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(context, NK_EDIT_BOX & ~NK_EDIT_MULTILINE | NK_EDIT_AUTO_SELECT, min2, 3,
                    nk_filter_decimal);

            nk_layout_row_push(context, 30);
            nk_label(context, "分", NK_TEXT_CENTERED);

            nk_layout_row_push(context, 40);
            nk_edit_string_zero_terminated(context, NK_EDIT_BOX & ~NK_EDIT_MULTILINE | NK_EDIT_AUTO_SELECT, s2, 3,
                    nk_filter_decimal);

            nk_layout_row_push(context, 30);
            nk_label(context, "秒", NK_TEXT_CENTERED);
        }
        nk_style_pop_font(context);
    }

    if ((y1[0] == 0 || m1[0] == 0 || d1[0] == 0 || h1[0] == 0 || min1[0] == 0 || s1[0] == 0)
        || (y2[0] == 0 || m2[0] == 0 || d2[0] == 0 || h2[0] == 0 || min2[0] == 0 || s2[0] == 0))
    {
        return;
    }

    int year1, month1, day1, hour1, minute1, second1;
    int year2, month2, day2, hour2, minute2, second2;
    sscanf(y1, "%d", &year1);
    sscanf(m1, "%d", &month1);
    sscanf(d1, "%d", &day1);
    sscanf(h1, "%d", &hour1);
    sscanf(min1, "%d", &minute1);
    sscanf(s1, "%d", &second1);
    sscanf(y2, "%d", &year2);
    sscanf(m2, "%d", &month2);
    sscanf(d2, "%d", &day2);
    sscanf(h2, "%d", &hour2);
    sscanf(min2, "%d", &minute2);
    sscanf(s2, "%d", &second2);

    if (year1 >= 0 && month1 >= 0 && day1 >= 0 && hour1 >= 0 && minute1 >= 0 && second1 >= 0
        && year2 >= 0 && month2 >= 0 && day2 >= 0 && hour2 >= 0 && minute2 >= 0 && second2 >= 0)
    {
        sprintf(dateFrom, "%04d-%02d-%02d %02d:%02d:%02d", year1, month1, day1, hour1, minute1, second1);
        sprintf(dateTo, "%04d-%02d-%02d %02d:%02d:%02d", year2, month2, day2, hour2, minute2, second2);
        *from = dateFrom;
        *to = dateTo;
    }
}