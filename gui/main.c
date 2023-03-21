#include "../utils.h"
#include "config.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VERTEX_BUFFER (16 * 1024 * 1024)
#define MAX_ELEMENT_BUFFER (4 * 1024 * 1024)
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION

#include "layout.h"

static void error_callback(int e, const char *d)
{
    printf("Error %d: %s\n", e, d);
}

LinkedList *windows;
int ShouldExit(struct nk_context *context)
{
    LinkedList *now = windows;
    while (now != NULL)
    {
        if (!nk_window_is_closed(context, ((Window *)now->data)->title))
        {
            return 0;
        }
        now = now->next;
    }
    return 1;
}

void PushWindow(Window *window)
{
    LinkedList *now = windows;
    while (now != NULL)
    {
        if (strcmp(((Window *)now->data)->title, window->title) == 0)
        {
            window->freeFunc(window);
            return;
        }
        now = now->next;
    }
    windows = AppendData(windows, window);
}

void InitWindows()
{
    windows = malloc(sizeof(LinkedList));
    windows->data = NewLoginWindow("login");
}

const char *executablePath;
struct nk_font *fontLarge;
struct nk_font *fontMedium;
struct nk_font *fontSmall;

int main(int argc, char **argv)
{
    char *path = GetDirectory(argv[0]);
    executablePath = path;
    printf("%s\n", path);
    int window_width = 1920;
    int window_height = 1080;

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
#ifdef __APPLE__
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
        fontMedium = nk_font_atlas_add_from_file(atlas, JoinPath(executablePath, fontRelativePath), 30.0f, &config);
        nk_glfw3_font_stash_end(&glfw);

        config = nk_font_config(35.0f);
        config.oversample_h = 1;
        config.oversample_v = 1;
        config.range = nk_font_chinese_glyph_ranges();
        config.pixel_snap = 1;

        nk_glfw3_font_stash_begin(&glfw, &atlas);
        fontLarge = nk_font_atlas_add_from_file(atlas, JoinPath(executablePath, fontRelativePath), 35.0f, &config);
        nk_glfw3_font_stash_end(&glfw);

        config = nk_font_config(25.0f);
        config.oversample_h = 1;
        config.oversample_v = 1;
        config.range = nk_font_chinese_glyph_ranges();
        config.pixel_snap = 1;

        nk_glfw3_font_stash_begin(&glfw, &atlas);
        fontSmall = nk_font_atlas_add_from_file(atlas, JoinPath(executablePath, fontRelativePath), 25.0f, &config);
        nk_glfw3_font_stash_end(&glfw);

        nk_style_set_font(context, &fontMedium->handle);
    }

    InitWindows();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        nk_glfw3_new_frame(&glfw);

        LinkedList *now = windows;
        while (now != NULL)
        {
            while (((Window *)now->data)->next != NULL)
            {
                nk_window_close(context, ((Window *)now->data)->title);
                Window *tmp = now->data;
                tmp->freeFunc(tmp);
                now->data = ((Window *)now->data)->next;
            }
            if (((Window *)now->data)->isClosed == 1)
            {
                if (!nk_window_is_closed(context, ((Window *)now->data)->title))
                {
                    nk_window_close(context, ((Window *)now->data)->title);
                }
                ((Window *)now->data)->freeFunc(now->data);
                RemoveNode(windows, now->data);

                LinkedList *next = now->next;
                free(now);
                now = next;
            }
            else
            {
                if (nk_begin(context, ((Window *)now->data)->title, nk_rect(0, 0, 1000, 800),
                             NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MINIMIZABLE | NK_WINDOW_MOVABLE |
                                 NK_WINDOW_SCALABLE))
                {
                    ((Window *)now->data)->layoutFunc(context, now->data);
                }
                nk_end(context);

                now = now->next;
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
