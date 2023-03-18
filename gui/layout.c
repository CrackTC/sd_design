#include "layout.h"
#include "../utils.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void DrawUpdateBox(struct nk_context *context, const char *title, Table **before, Table **result, size_t bufferSize)
{
    if (*before != NULL)
    {
        TableRow *titleRow = GetTableTitle(*before);
        TableRow *dataRow = GetRowByIndex(*before, 1);

        if (*result == NULL)
        {
            LinkedList *titleNow = titleRow->items;
            LinkedList *dataNow = dataRow->items;
            TableRow *resultTitle = NewTableRow();
            TableRow *resultData = NewTableRow();

            while (titleNow != NULL)
            {
                AppendTableRow(resultTitle, titleNow->data);
                char *buffer = malloc(bufferSize);
                strcpy(buffer, dataNow->data);
                resultData->items = AppendData(resultData->items, buffer);
                resultData->columnCount++;

                titleNow = titleNow->next;
                dataNow = dataNow->next;
            }
            *result = NewTable(resultTitle, NULL);
            AppendTable(*result, resultData);
        }

        if (nk_popup_begin(context, NK_POPUP_DYNAMIC, title, 0, nk_rect(0, 0, 400, 1000)))
        {
            LinkedList *titleNow = GetTableTitle(*result)->items;
            LinkedList *dataNow = GetRowByIndex(*result, 1)->items;
            while (titleNow != NULL)
            {
                nk_layout_row_begin(context, NK_STATIC, 0, 2);
                {
                    nk_layout_row_push(context, 50);
                    {
                        nk_label(context, titleNow->data, NK_TEXT_RIGHT);
                    }
                    nk_layout_row_push(context, 200);
                    {
                        if (strcmp(titleNow->data, "id") == 0)
                        {
                            nk_edit_string_zero_terminated(context, NK_EDIT_READ_ONLY, dataNow->data, bufferSize,
                                                           nk_filter_default);
                        }
                        else
                        {
                            nk_edit_string_zero_terminated(context,
                                                           NK_EDIT_BOX | NK_EDIT_AUTO_SELECT | NK_EDIT_CLIPBOARD,
                                                           dataNow->data, bufferSize, nk_filter_default);
                        }
                    }
                    nk_layout_row_end(context);
                }
                titleNow = titleNow->next;
                dataNow = dataNow->next;
            }

            nk_layout_row_dynamic(context, 0, 2);
            if (nk_button_label(context, "确定"))
            {
                FreeTable(*before);
                *before = NULL;
                nk_popup_close(context);
            }
            if (nk_button_label(context, "取消"))
            {
                FreeTable(*before);
                *before = NULL;
                FreeTable(*result);
                *result = NULL;
                nk_popup_close(context);
            }

            nk_popup_end(context);
        }
    }
}
