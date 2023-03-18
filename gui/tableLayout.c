#include "../data/linkedList.h"
#include "../data/table.h"
#include "layout.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void TableLayout(struct nk_context *context, const Table *table, LinkedList **checkList, const char *filter,
                 const char *value)
{
    if (table != NULL)
    {
        TableRow *row = GetTableTitle(table);
        nk_layout_row_begin(context, NK_STATIC, 30, row->columnCount + 1);
        {
            nk_layout_row_push(context, 30);
            {
                nk_label(context, "选中", NK_TEXT_LEFT);
            }

            LinkedList *itemNow = row->items;

            while (itemNow != NULL)
            {
                nk_layout_row_push(context, 100);
                {
                    nk_label(context, itemNow->data, NK_TEXT_CENTERED);
                }
                itemNow = itemNow->next;
            }

            nk_layout_row_end(context);
        }

        LinkedList *rowNow = table->rows->next;
        LinkedList *checkListNow = *checkList;
        while (rowNow != NULL)
        {
            row = rowNow->data;
            if (filter == NULL || strcmp(GetRowItemByColumnName(table, row, filter), value) == 0)
            {
                if (checkListNow->next == NULL)
                {
                    int *checked = malloc(sizeof(int));
                    *checked = 0;
                    *checkList = AppendData(*checkList, checked);
                }

                nk_layout_row_push(context, 30);
                {
                    nk_checkbox_label(context, "", checkListNow->next->data);
                }

                LinkedList *itemNow = row->items;
                while (itemNow != NULL)
                {
                    nk_layout_row_push(context, 100);
                    {
                        nk_label(context, itemNow->data, NK_TEXT_CENTERED);
                    }
                    itemNow = itemNow->next;
                }
            }
            rowNow = rowNow->next;
            checkListNow = checkListNow->next;
        }
    }
}
