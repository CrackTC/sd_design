#include "table.h"
#include "../utils.h"
#include "linkedList.h"
#include <malloc.h>
#include <string.h>

TableRow *NewTableRow()
{
    TableRow *row = malloc(sizeof(TableRow));
    row->columnCount = 0;
    row->items = NULL;
    return row;
}

void FreeTableRow(TableRow *row)
{
    if (row == NULL)
    {
        return;
    }
    LinkedList *now = row->items;
    while (now != NULL)
    {
        free(now->data);
        now = now->next;
    }
    FreeList(row->items);
    free(row);
}

void AppendTableRow(TableRow *row, const char *item)
{
    if (row == NULL || item == NULL)
    {
        return;
    }
    row->columnCount++;
    LinkedList *node = malloc(sizeof(LinkedList));
    node->data = CloneString(item);
    node->next = NULL;
    row->items = AppendNode(row->items, node);
}

Table *NewTable(TableRow *title, const char *remark)
{
    if (title == NULL || title->columnCount == 0)
    {
        return NULL;
    }
    Table *table = malloc(sizeof(Table));
    table->remark = CloneString(remark);
    table->rows = malloc(sizeof(LinkedList));
    table->rows->data = title;
    table->rows->next = NULL;
    return table;
}

void FreeTable(Table *table)
{
    if (table == NULL)
    {
        return;
    }
    free(table->remark);
    LinkedList *now = table->rows;
    while (now != NULL)
    {
        FreeTableRow(now->data);
        now = now->next;
    }
    FreeList(table->rows);
    free(table);
}

int AppendTable(Table *table, TableRow *row)
{
    if (table == NULL || row == NULL)
    {
        return 1;
    }

    TableRow *title = table->rows->data;
    if (title->columnCount != row->columnCount)
    {
        return 1;
    }

    LinkedList *node = malloc(sizeof(LinkedList));
    node->data = row;
    node->next = NULL;
    table->rows = AppendNode(table->rows, node);

    return 0;
}

TableRow *GetTableTitle(const Table *table)
{
    if (table == NULL)
    {
        return NULL;
    }
    return table->rows->data;
}

char *GetTableRemark(const Table *table)
{
    if (table == NULL)
    {
        return NULL;
    }
    return table->remark;
}

TableRow *GetRowByIndex(const Table *table, int index)
{
    if (table == NULL)
    {
        return NULL;
    }
    return GetNodeByIndex(table->rows, index)->data;
}

char *GetRowItemByColumnName(const Table *table, const TableRow *row, const char *columnName)
{
    if (table == NULL || row == NULL || columnName == NULL)
    {
        return NULL;
    }

    TableRow *title = table->rows->data;

    LinkedList *now = title->items;
    int nameIndex = 0;
    while (now != NULL)
    {
        if (strcmp(columnName, now->data) == 0)
        {
            return GetNodeByIndex(row->items, nameIndex)->data;
        }
        nameIndex++;
        now = now->next;
    }
    return NULL;
}
