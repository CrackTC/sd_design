#include "../config.h"
#include "design/utils.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VERTEX_BUFFER (16 * 1024 * 1024)
#define MAX_ELEMENT_BUFFER (4 * 1024 * 1024)
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION

#include "design/layout.h"

static void error_callback(int e, const char *d)
{
    printf("Error %d: %s\n", e, d);
}

LinkedList *windows;

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
    windows->next = NULL;
}

const char *executablePath;
struct nk_font *fontLarge;
struct nk_font *fontMedium;
struct nk_font *fontSmall;

void InitGraphic(int width, int height, struct nk_glfw **glfw, struct nk_context **pcontext,
                 struct GLFWwindow **pwindow)
{
    *glfw = calloc(1, sizeof(struct nk_glfw));
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        printf("[GLFW] failed to init!\n");
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    *pwindow = glfwCreateWindow(width, height, "酒水销售管理系统", NULL, NULL);
    glfwMakeContextCurrent(*pwindow);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("[GLAD] failed to setup glad\n");
        exit(-1);
    }

    *pcontext = nk_glfw3_init(*glfw, *pwindow, NK_GLFW3_INSTALL_CALLBACKS);
}

struct nk_font *LoadFont(struct nk_glfw *glfw, float size)
{
    struct nk_font_config config;
    struct nk_font_atlas *atlas;

    config = nk_font_config(size);
    config.oversample_h = 1;
    config.oversample_v = 1;
    config.range = nk_font_chinese_glyph_ranges();
    config.pixel_snap = 1;

    nk_glfw3_font_stash_begin(glfw, &atlas);
    struct nk_font *result =
        nk_font_atlas_add_from_file(atlas, JoinPath(executablePath, fontRelativePath), size, &config);
    nk_glfw3_font_stash_end(glfw);

    return result;
}

void InitFont(struct nk_glfw *glfw)
{
    fontMedium = LoadFont(glfw, 30.0f);
    fontLarge = LoadFont(glfw, 35.0f);
    fontSmall = LoadFont(glfw, 25.0f);
}

void Draw(struct nk_context *context)
{
    LinkedList *now = windows;
    while (now != NULL)
    {
        while (((Window *)now->data)->next != NULL)
        {
            nk_window_close(context, ((Window *)now->data)->title);
            Window *tmp = now->data;
            now->data = ((Window *)now->data)->next;
            tmp->freeFunc(tmp);
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
}

void Submit(struct nk_glfw *glfw, int width, int height, GLFWwindow *window)
{
    glfwPollEvents();
    nk_glfw3_new_frame(glfw);

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    nk_glfw3_render(glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    glfwSwapBuffers(window);
}

int main(int argc, char **argv)
{
#ifdef _WIN32
    // Windows使用utf-8代码页
    system("chcp 65001");
#endif

    // 程序二进制路径
    executablePath = GetDirectory(argv[0]);

    int window_width = 1920;
    int window_height = 1080;

    struct nk_glfw *glfw;
    struct nk_context *context;
    struct GLFWwindow *window;

    InitGraphic(window_width, window_height, &glfw, &context, &window);
    InitFont(glfw);
    nk_style_set_font(context, &fontMedium->handle);

    InitWindows();

    while (!glfwWindowShouldClose(window))
    {
        Draw(context);
        Submit(glfw, window_width, window_height, window);
    }

    nk_glfw3_shutdown(glfw);
    glfwTerminate();
    return 0;
}
