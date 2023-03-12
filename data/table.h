/*
 * FileName: data/table.h
 * Author: 陈若珂
 * Description: 包含表格相关的数据表示和操作的声明
 * */

#ifndef TABLE_H
#define TABLE_H

#include "linkedList.h"

/*
 * Name: TableRow
 * Description: 表示一个表格行
 * */
typedef struct TableRow {
    // 表格列数
    int columnCount;

    // 储存一行中项目的链表
    LinkedList *items;
} TableRow;

/*
 * Name: Table
 * Description: 表示一个表格
 * */
typedef struct Table {
    // 所有列
    LinkedList *rows;

    // 备注
    const char *remark;
} Table;

/*
 * Name: NewTableRow
 * Description: 创建一个新的TableRow对象
 * Returns: 一个指针，指向创建的TableRow对象
 * */
TableRow *NewTableRow();

/*
 * Name: FreeTableRow
 * Description: 释放指定的TableRow对象占用的空间
 * Arguments:
 *     row: 一个指针，指向要释放空间的TableRow对象
 * */
void FreeTableRow(TableRow *row);

/*
 * Name: AppendTableRow
 * Description: 向表格的一个行追加元素
 * Arguments:
 *     row: 表格的一个行
 *     item: 要追加的元素
 * Remark: item以逐字符复制形式加入行
 * */
void AppendTableRow(TableRow *row, const char *item);

/*
 * Name: NewTable
 * Description: 创建一个新的表格
 * Arguments:
 *     title: 表格各列的标题（字段名）
 * Returns: 若创建成功，返回一个指针，指向创建的表格
 *          否则，返回NULL
 * */
Table *NewTable(TableRow *title, const char *remark);

/*
 * Name: FreeTable
 * Description: 释放指定的Table对象占用的空间
 * Arguments:
 *     table: 一个指针，指向要释放空间的Table对象
 * */
void FreeTable(Table *table);

/*
 * Name: AppendTable
 * Description: 向表格追加一行
 * Arguments:
 *     table: 一个指针，指向要追加行的表格
 *     row: 一个指针，指向要追加的行
 * Returns: 若追加成功，返回0
 *          否则，返回1
 * */
int AppendTable(Table *table, TableRow *row);

/*
 * Name: GetTableTitle
 * Description: 获取表格的标题行
 * Arguments:
 *     table: 一个指针，指向要获取标题行的表格
 * Returns: 一个指针，指向表格的标题行
 * */
TableRow *GetTableTitle(const Table *table);

/*
 * Name: GetTableRemark
 * Description: 获取表格的备注部分
 * Arguments:
 *     table: 一个指针，指向要获取备注的表格
 * Returns: 表格的备注
 * */
const char *GetTableRemark(const Table *table);

/*
 * Name: GetRowByIndex
 * Description: 获取表格的指定行
 * Arguments:
 *     table: 一个指针，指向要获取行的表格
 *     index: 要获取的行的行号
 * */
TableRow *GetRowByIndex(const Table *table, int index);

/*
 * Name: GetRowItemByColumnName
 * Description: 根据指定列名获取指定表格的特定行的特定元素
 * Arguments:
 *     table: 要获取元素的表格
 *     row: 要获取元素的行
 *     columnName: 列名
 * Returns: 若存在这样的元素，返回该元素
 *          否则，返回NULL
 * */
char *GetRowItemByColumnName(const Table *table, const TableRow *row, const char *columnName);

#endif
