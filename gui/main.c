#include "../utils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTEX_BUFFER 16 * 1024 * 1024
#define MAX_ELEMENT_BUFFER 4 * 1024 * 1024
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION

#include "layout.h"

#define LAYOUT_COUNT 2

static void error_callback(int e, const char *d)
{
    printf("Error %d: %s\n", e, d);
}

Window *windows[LAYOUT_COUNT];

int ShouldExit(struct nk_context *context)
{
    for (int i = 0; i < LAYOUT_COUNT; i++)
    {
        if (!nk_window_is_closed(context, windows[i]->title))
        {
            return 0;
        }
    }
    return 1;
}

void InitWindows()
{
    windows[0] = NewMainWindow(1, "main");
    windows[1] = NewLoginWindow(1, "another");
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

    // font baking
    {
        struct nk_font_config config = nk_font_config(30.0f);
        config.oversample_h = 1;
        config.oversample_v = 1;
        config.range = nk_font_chinese_glyph_ranges();
        config.pixel_snap = 1;

        struct nk_font_atlas *atlas;

        nk_glfw3_font_stash_begin(&glfw, &atlas);
        struct nk_font *font = nk_font_atlas_add_from_file(atlas, "resources/MiSans-Medium.ttf", 30.0f, &config);
        nk_glfw3_font_stash_end(&glfw);

        if (font)
            nk_style_set_font(context, &font->handle);
    }

    InitWindows();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        nk_glfw3_new_frame(&glfw);

        /* glfwGetFramebufferSize(window, &window_width, &window_height); */

        for (int i = 0; i < LAYOUT_COUNT; i++)
        {
            while (windows[i]->next != NULL) {
                windows[i] = windows[i]->next;
            }
            if (windows[i]->isVisible)
            {
                if (nk_begin(context, windows[i]->title, nk_rect(0, 0, window_width, window_height),
                             NK_WINDOW_CLOSABLE | NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MINIMIZABLE |
                                 NK_WINDOW_MOVABLE))
                {
                    windows[i]->layoutFunc(context, windows[i]->data);
                }
                nk_end(context);
            }
        }

        glViewport(0, 0, window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT);
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(window);

        if (ShouldExit(context))
            break;
    }

    nk_glfw3_shutdown(&glfw);
    glfwTerminate();
    return 0;
}
