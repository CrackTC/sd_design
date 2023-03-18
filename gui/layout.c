#include "layout.h"
#include <stdio.h>
#include <stdlib.h>

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

void DrawMessageBox(struct nk_context *context, const char *title, char **message)
{
    if (*message != NULL)
    {
        if (nk_popup_begin(context, NK_POPUP_DYNAMIC, title, 0, nk_rect(0, 100, 400, 400)))
        {
            nk_layout_row_dynamic(context, 0, 1);
            {
                nk_label(context, *message, NK_TEXT_CENTERED);
            }

            nk_layout_row_dynamic(context, 0, 1);
            {
                if (nk_button_label(context, "确定"))
                {
                    free(*message);
                    *message = NULL;
                    nk_popup_close(context);
                }
            }
            nk_popup_end(context);
        }
    }
}
