#include "../utils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

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

void layout(struct nk_context *context)
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
