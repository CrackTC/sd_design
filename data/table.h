/*
 * FileName: data/table.h
 * Author: 陈若珂
 * Description: 
 * */

#ifndef TABLE_H
#define TABLE_H

#include "linkedList.h"

typedef struct TableRow {
    int columnCount;
    LinkedList *items;
} TableRow;

typedef struct Table {
    LinkedList *rows;
} Table;

TableRow *NewTableRow();

void FreeTableRow(TableRow *row);

void AppendTableRow(TableRow *row, const char *item);

Table *NewTable(TableRow *title);

void FreeTable(Table *table);

int AppendTable(Table *table, TableRow *row);

TableRow *GetTableTitle(const Table *table);

TableRow *GetRowByIndex(const Table *table, int index);

const char *GetRowItemByColumnName(const Table *table, const TableRow *row, const char *columnName);

#endif
