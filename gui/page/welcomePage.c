#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include "design/mainWindow.h"

void WelcomePageLayout(struct nk_context *context, struct Window *window)
{
    struct Data *data = window->data;

    nk_layout_row_dynamic(context, 0, 2);

    // 创建具有不同字体大小的标签
    if (nk_style_push_font(context, &fontLarge->handle))
    {
        nk_label(context, "欢迎, ", NK_TEXT_ALIGN_BOTTOM | NK_TEXT_ALIGN_RIGHT);
        nk_label(context, data->name, NK_TEXT_ALIGN_BOTTOM | NK_TEXT_ALIGN_LEFT);

        nk_style_pop_font(context);
    }

    nk_layout_row_dynamic(context, 0, 1);
    nk_label(context, "在左侧的列表中选择想要进行的操作种类", NK_TEXT_CENTERED);
}
