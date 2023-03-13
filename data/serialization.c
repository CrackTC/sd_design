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

    fprintf(filePointer, "remark:\n");
    fprintf(filePointer, "    len: %lu\n", table->remark == NULL ? 0 : strlen(table->remark));
    fprintf(filePointer, "    val: %s\n", table->remark == NULL ? "" : table->remark);
    fprintf(filePointer, "columnCount: %d\n", ((TableRow *)table->rows->data)->columnCount);
    fprintf(filePointer, "rows:\n");

    LinkedList *now = table->rows;
    while (now != NULL)
    {
        fprintf(filePointer, "    row:\n");

        TableRow *row = now->data;
        LinkedList *itemNow = row->items;
        while (itemNow != NULL)
        {
            fprintf(filePointer, "        len: %lu\n", strlen(itemNow->data));
            fprintf(filePointer, "        val: %s\n", (char *)itemNow->data);
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
        char ch = fgetc(filePointer);
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
    return 0;
}
