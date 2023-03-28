#include "design/layout.h"
#include "design/linkedList.h"
#include "design/table.h"
#include <stdlib.h>
#include <string.h>

int RowFiltered(const Table *table, const TableRow *row, const char *filter, const char *value, const char *rangeFilter, const char *from, const char *to)
{
    if (filter != NULL)
    {
        if (strcmp(GetRowItemByColumnName(table, row, filter), value) != 0)
        {
            return 1;
        }
    }
    if (rangeFilter != NULL && from != NULL && to != NULL)
    {
        const char *rangeData = GetRowItemByColumnName(table, row, rangeFilter);
        if (strcmp(from, rangeData) > 0 || strcmp(rangeData, to) > 0)
        {
            return 1;
        }
    }
    return 0;
}

void TableLayout(struct nk_context *context, const Table *table, LinkedList *checkList, const char *filter,
                 const char *value, const char *rangeFilter, const char *from, const char *to)
{
    if (table != NULL)
    {
        TableRow *row = GetTableTitle(table);
        nk_layout_row_begin(context, NK_STATIC, 30, row->columnCount + (checkList == NULL ? 0 : 1));
        {
            if (checkList != NULL)
            {
                nk_layout_row_push(context, 30);
                {
                    if (nk_checkbox_label(context, "", checkList->data))
                    {
                        LinkedList *now = checkList->next;
                        LinkedList *rowNow = table->rows->next;
                        while (now != NULL && !RowFiltered(table, rowNow->data, filter, value, rangeFilter, from, to))
                        {
                            *(int *)now->data = *(int *)checkList->data;
                            now = now->next;
                            rowNow = rowNow->next;
                        }
                    }
                }
            }

            LinkedList *itemNow = row->items;

            while (itemNow != NULL)
            {
                nk_layout_row_push(context, 200);
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
            if (checkList != NULL)
            {
                if (checkListNow->next == NULL)
                {
                    int *checked = malloc(sizeof(int));
                    *checked = 0;
                    AppendData(checkList, checked);
                }
            }
            if (!RowFiltered(table, row, filter, value, rangeFilter, from, to))
            {
                if (checkList != NULL)
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
                }

                LinkedList *itemNow = row->items;
                while (itemNow != NULL)
                {
                    nk_layout_row_push(context, 200);
                    {
                        nk_label(context, itemNow->data, NK_TEXT_CENTERED);
                    }
                    itemNow = itemNow->next;
                }
            }
            rowNow = rowNow->next;
            if (checkList != NULL)
                checkListNow = checkListNow->next;
        }
    }
}
