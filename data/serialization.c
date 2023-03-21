#include "serialization.h"
#include "table.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

int Serialize(const Table *table, const char *path)
{
    if (table == NULL || path == NULL)
    {
        return -1;
    }

    FILE *filePointer = fopen(path, "w");

    if (filePointer == NULL)
    {
        return -1;
    }

    fprintf(filePointer, "remark:");
    fprintf(filePointer, "\n    len: %lu", table->remark == NULL ? 0 : strlen(table->remark));
    fprintf(filePointer, "\n    val: %s", table->remark == NULL ? "" : table->remark);
    fprintf(filePointer, "\ncolumnCount: %d", ((TableRow *)table->rows->data)->columnCount);
    fprintf(filePointer, "\nrows:");

    LinkedList *now = table->rows;
    while (now != NULL)
    {
        fprintf(filePointer, "\n    row:");

        TableRow *row = now->data;
        LinkedList *itemNow = row->items;
        while (itemNow != NULL)
        {
            fprintf(filePointer, "\n        len: %lu", strlen(itemNow->data));
            fprintf(filePointer, "\n        val: %s", (char *)itemNow->data);
            itemNow = itemNow->next;
        }

        now = now->next;
    }

    fclose(filePointer);
    return 0;
}

void Skip(FILE *filePointer, int x)
{
    if (x == 0)
    {
        return;
    }
    fgetc(filePointer);
    Skip(filePointer, x - 1);
}

int Unserialize(Table **destination, const char *path)
{
    if (destination == NULL || path == NULL)
    {
        return -1;
    }

    FILE *filePointer = fopen(path, "r");
    if (filePointer == NULL)
    {
        return 1;
    }

    Skip(filePointer, strlen("remark:"));
    Skip(filePointer, strlen("\n    len: "));

    unsigned long remarkLength;
    fscanf(filePointer, "%lu", &remarkLength);

    Skip(filePointer, strlen("\n    val: "));

    char *remark = malloc((remarkLength + 1) * sizeof(char));
    fgets(remark, remarkLength + 1, filePointer);

    Skip(filePointer, strlen("\ncolumnCount: "));

    int columnCount;
    fscanf(filePointer, "%d", &columnCount);

    Skip(filePointer, strlen("\nrows:"));

    TableRow *row = NewTableRow();
    char *item;
    unsigned long itemLength;

    Skip(filePointer, strlen("\n    row:"));

    for (int i = 0; i < columnCount; i++)
    {
        Skip(filePointer, strlen("\n        len: "));
        fscanf(filePointer, "%lu", &itemLength);

        item = malloc((itemLength + 1) * sizeof(char));

        Skip(filePointer, strlen("\n        val: "));
        fgets(item, itemLength + 1, filePointer);

        AppendTableRow(row, item);
        free(item);
    }

    Table *table = NewTable(row, remark);

    while (1)
    {
        int ch = fgetc(filePointer);
        if (ch == EOF)
            break;
        ungetc(ch, filePointer);

        row = NewTableRow();

        Skip(filePointer, strlen("\n    row:"));
        for (int i = 0; i < columnCount; i++)
        {
            Skip(filePointer, strlen("\n        len: "));
            fscanf(filePointer, "%lu", &itemLength);

            item = malloc((itemLength + 1) * sizeof(char));

            Skip(filePointer, strlen("\n        val: "));
            fgets(item, itemLength + 1, filePointer);

            AppendTableRow(row, item);
            free(item);
        }

        AppendTable(table, row);
    }

    *destination = table;
    fclose(filePointer);
    return 0;
}
