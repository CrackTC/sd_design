#include "table.h"
#include "../utils.h"
#include "linkedList.h"
#include <malloc.h>
#include <stdio.h>
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

char *AppendTableRow(TableRow *row, char *item)
{
    if (row == NULL || item == NULL)
    {
        printf("appending: arg null\n");
        return NULL;
    }
    printf("appending: %s\n", item);
    row->columnCount++;
    row->items = AppendData(row->items, CloneString(item));
    return item;
}

TableRow *CloneRow(const TableRow *source)
{
    if (source == NULL)
    {
        return NULL;
    }

    TableRow *result = NewTableRow();
    LinkedList *itemNow = source->items;
    while (itemNow != NULL)
    {
        AppendTableRow(result, itemNow->data);
        itemNow = itemNow->next;
    }
    return result;
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
    if (table->remark != NULL)
    {
        free(table->remark);
    }
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
        printf("table: append failed, arg null\n");
        return 1;
    }

    TableRow *title = table->rows->data;
    if (title->columnCount != row->columnCount)
    {
        printf("table: append failed, count mismatch\n");
        return 1;
    }

    table->rows = AppendData(table->rows, row);
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

void SetTableRemark(Table *table, const char *value)
{
    if (table == NULL)
    {
        return;
    }
    if (table->remark != NULL)
    {
        free(table->remark);
    }
    table->remark = CloneString(value);
}

char *GetRowItemByColumnName(const Table *table, const TableRow *row, const char *columnName)
{
    if (table == NULL || row == NULL || columnName == NULL)
    {
        printf("getrowitem failed: arg null\n");
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
    printf("getrowitem failed: columnName: %s\n", columnName);
    return NULL;
}

Table *CloneTable(const Table *source)
{
    if (source == NULL)
    {
        return NULL;
    }

    TableRow *newTitleRow = CloneRow(GetTableTitle(source));
    Table *result = NewTable(newTitleRow, GetTableRemark(source));
    LinkedList *dataRowNow = source->rows->next;
    while (dataRowNow != NULL)
    {
        TableRow *newRow = CloneRow(dataRowNow->data);
        AppendTable(result, newRow);
        dataRowNow = dataRowNow->next;
    }
    return result;
}

Table *CloneTableBuffered(const Table *source, size_t bufferSize)
{
    if (source == NULL)
    {
        return NULL;
    }

    TableRow *newTitleRow = NewTableRow();
    TableRow *titleRow = GetTableTitle(source);
    LinkedList *titleNow = titleRow->items;
    while (titleNow != NULL)
    {
        char *buffer = malloc(bufferSize);
        strcpy(buffer, titleNow->data);
        newTitleRow->items = AppendData(newTitleRow->items, buffer);
        newTitleRow->columnCount++;
        titleNow = titleNow->next;
    }
    Table *result = NewTable(newTitleRow, GetTableRemark(source));
    LinkedList *dataRowNow = source->rows->next;
    while (dataRowNow != NULL)
    {
        TableRow *newRow = NewTableRow();
        LinkedList *dataNow = ((TableRow *)(dataRowNow->data))->items;
        while (dataNow != NULL)
        {
            char *buffer = malloc(bufferSize);
            strcpy(buffer, dataNow->data);
            newRow->items = AppendData(newRow->items, buffer);
            newRow->columnCount++;
            dataNow = dataNow->next;
        }
        AppendTable(result, newRow);

        dataRowNow = dataRowNow->next;
    }
    return result;
}
