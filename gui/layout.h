#ifndef LAYOUT_H
#define LAYOUT_H

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_glfw_gl3.h"

typedef struct Window Window;

typedef void (*LayoutFunc)(struct nk_context *context, Window *window);
typedef void (*FreeFunc)(Window *window);

struct Window
{
    int isVisible;
    LayoutFunc layoutFunc;
    FreeFunc freeFunc;
    const char *title;
    void *data;
    struct Window *next;
};

Window *NewMainWindow(int isVisible, const char *title);
Window *NewLoginWindow(int isVisible, const char *title);

void PlaceNothing(struct nk_context *context);
void EnsureWindowSize(struct nk_context *context, Window *window, float width, float height);

#endif
