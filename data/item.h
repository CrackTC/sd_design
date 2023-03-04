/*
 * FileName: data/item.h
 * Author: 陈若珂
 * Description: 包含商品相关数据表示和操作的声明
 * */

#ifndef ITEM_H
#define ITEM_H

#include "amount.h"
#include "linkedList.h"
#include "time.h"

/*
 * Name: Item
 * Description: 表示一个商品
 * */
struct Item;

/*
 * Name: NewItem
 * Description: 创建一个新的商品
 * Arguments:
 *     destination: 指向用于存放创建结果的Item对象的指针
 *     name: 商品名称
 *     price: 指向商品价格的指针
 *     shelfLife: 指向商品保质期的指针
 * Returns: 若成功创建，返回0
 *          否则，返回1
 * */
int NewItem(Item *destination, const char *name, Amount *price, Time *shelfLife);

/*
 * Name: GetAllItems
 * Description: 获取所有商品
 * Returns: 一个链表，包含所有商品
 *          若不存在商品，返回NULL
 * */
const LinkedListPointer GetAllItems();

/*
 * Name: GetItemById
 * Description: 获取具有指定商品编号的商品
 * Arguments:
 *     id: 指定的商品编号
 * Returns: 符合条件的商品
 *          若不存在符合条件的商品，返回NULL
 * */
Item *GetItemById(int id);

/*
 * Name: GetItemByName
 * Description: 获取具有指定商品名称的商品
 * Arguments:
 *     name: 指定的商品名称
 * Returns: 符合条件的商品
 *          若不存在符合条件的商品，返回NULL
 * */
Item *GetItemByName(const char *name);

/*
 * Name: GetItemXXX
 * Description: 获取商品的某个成员
 * Arguments:
 *     item: 指向Item的指针
 * Returns: 对应的值
 * */
int GetItemId(const Item *item);
const char *GetItemName(const Item *item);
Amount *GetItemPrice(const Item *item);
Time *GetItemShelfLife(const Item *item);

/*
 * Name: SetItemXXX
 * Description: 设置商品的某个成员
 * Argument:
 *     item: 指向要设置成员的商品的指针
 *     value: 要设置成的值
 * */
void SetItemName(Item *item, const char *value);
void SetItemPrice(Item *item, Amount *value);
void SetItemShelfLife(Item *item, Time *value);

/*
 * Name: ItemsSave
 * Description: 保存对商品的所有更改
 * */
void ItemsSave();

#endif
