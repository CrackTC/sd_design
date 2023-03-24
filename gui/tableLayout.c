#include "design/linkedList.h"
#include "design/table.h"
#include "design/layout.h"
#include <stdlib.h>
#include <string.h>

int RowFiltered(const Table *table, const TableRow *row, const char *filter, const char *value)
{
    return filter != NULL && strcmp(GetRowItemByColumnName(table, row, filter), value) != 0;
}

void TableLayout(struct nk_context *context, const Table *table, LinkedList *checkList, const char *filter,
        const char *value)
{
    if (table != NULL)
    {
        TableRow *row = GetTableTitle(table);
        nk_layout_row_begin(context, NK_STATIC, 30, row->columnCount + 1);
        {
            nk_layout_row_push(context, 30);
            {
                if (nk_checkbox_label(context, "", checkList->data))
                {
                    LinkedList *now = checkList->next;
                    LinkedList *rowNow = table->rows->next;
                    while (now != NULL && !RowFiltered(table, rowNow->data, filter, value))
                    {
                        *(int *)now->data = *(int *)checkList->data;
                        now = now->next;
                        rowNow = rowNow->next;
                    }
                }
            }

            LinkedList *itemNow = row->items;

            while (itemNow != NULL)
            {
                nk_layout_row_push(context, 150);
                {
                    nk_label(context, itemNow->data, NK_TEXT_CENTERED);
                }
                itemNow = itemNow->next;
            }

            nk_layout_row_end(context);
        }

        LinkedList *rowNow = table->rows->next;
        LinkedList *checkListNow = checkList;
        while (rowNow != NULL)
        {
            row = rowNow->data;
            if (checkListNow->next == NULL)
            {
                int *checked = malloc(sizeof(int));
                *checked = 0;
                AppendData(checkList, checked);
            }
            if (!RowFiltered(table, row, filter, value))
            {
                nk_layout_row_push(context, 30);
                {
                    if (nk_checkbox_label(context, "", checkListNow->next->data))
                    {
                        if (*(int *)checkListNow->next->data == 0)
                        {
                            *(int *)checkList->data = 0;
                        }
                    }
                }

                LinkedList *itemNow = row->items;
                while (itemNow != NULL)
                {
                    nk_layout_row_push(context, 150);
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
