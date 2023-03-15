#include "layout.h"

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
