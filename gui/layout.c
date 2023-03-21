#include "layout.h"
#include <malloc.h>
#include <stddef.h>

static struct nk_rect none = {0, 0, 0, 0};

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

LinkedList *NewCheckList()
{
    LinkedList *result = malloc(sizeof(LinkedList));
    result->data = malloc(sizeof(int));
    *(int *)result->data = 0;
    return result;
}
