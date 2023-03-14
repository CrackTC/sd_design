#include "../utils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTEX_BUFFER 16 * 1024 * 1024
#define MAX_ELEMENT_BUFFER 4 * 1024 * 1024
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_glfw_gl3.h"

static void error_callback(int e, const char *d)
{
    printf("Error %d: %s\n", e, d);
}

int main()
{
    int window_width = 800;
    int window_height = 600;

    struct nk_glfw glfw = {0};
    static GLFWwindow *window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        printf("[GLFW] failed to init!\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
#ifdef APPLE
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(window_width, window_height, "Demo", NULL, NULL);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        printf("[GLEW] failed to setup glew\n");
        return -1;
    }

    struct nk_context *context = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);

    {
        // font baking
        struct nk_font_config config = nk_font_config(30.0f);
        config.oversample_h = 1;
        config.oversample_v = 1;
        config.range = nk_font_chinese_glyph_ranges();
        config.pixel_snap = 1;

        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&glfw, &atlas);
        struct nk_font *font = nk_font_atlas_add_from_file(atlas, "resources/MiSans-Medium.ttf", 30.0f, &config);
        nk_glfw3_font_stash_end(&glfw);
        // set the font
        if (font)
            nk_style_set_font(context, &font->handle);
    }

    int i = 0;
    char *text = LongLongToString(-1);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        nk_glfw3_new_frame(&glfw);

        glfwGetFramebufferSize(window, &window_width, &window_height);

        if (nk_begin(context, "Demo", nk_rect(0, 0, window_width, window_height), NK_WINDOW_CLOSABLE | NK_WINDOW_TITLE))
        {
            nk_layout_row_dynamic(context, 120, 1);
            nk_label(context, text, NK_TEXT_CENTERED);

            nk_layout_row_dynamic(context, 80, 3);
            struct nk_rect space;
            nk_widget(&space, context);
            if (nk_button_label(context, "按钮"))
            {
                printf("button pressed\n");
                free(text);
                text = LongLongToString(i++);
            }
        }
        nk_end(context);

        glViewport(0, 0, window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT);
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(window);

        if (nk_window_is_closed(context, "Demo"))
        {
            break;
        }
    }

    nk_glfw3_shutdown(&glfw);
    glfwTerminate();
    return 0;
}
