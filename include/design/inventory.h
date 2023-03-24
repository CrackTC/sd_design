/*
 * FileName: data/inventory.h
 * Author: 陈若珂
 * Description: 包含库存相关的数据表示和操作的声明
 * */

#ifndef INVENTORY_H
#define INVENTORY_H

#include "linkedList.h"
#include "amount.h"
#include "time.h"

/*
 * Name: InventoryEntry
 * Description: 表示一个库存条目
 * */
typedef struct InventoryEntry InventoryEntry;

/*
 * Name: GetAllInventory
 * Description: 获取所有库存条目
 * Returns: 一个链表，包含所有库存条目
 *          若不存在库存条目，返回NULL
 * */
LinkedList *GetAllInventory();

/*
 * Name: GetInventoryById
 * Description: 获取具有指定库存编号的库存条目
 * Arguments:
 *     id: 指定的库存编号
 * Returns: 符合条件的库存条目
 *          若不存在符合条件的条目，返回NULL
 * */
InventoryEntry *GetInventoryById(int id);

/*
 * Name: GetInventoryByItemId
 * Description: 获取具有指定商品编号的库存条目
 * Arguments:
 *     itemId: 指定的商品编号
 * Returns: 一个链表，包含所有符合条件的库存条目
 *          若不存在符合条件的条目，返回NULL
 * */
LinkedList *GetInventoryByItemId(int itemId);

/*
 * Name: NewInventoryEntry
 * Description: 创建一个新的库存条目
 * Arguments:
 *     itemId: 商品编号
 *     number: 数量
 *     inboundTime: 指向入库时间的指针
 *     productionTime: 指向生产日期的指针
 *     inboundUnitPrice: 指向入库单价的指针
 * Returns: 若成功创建，返回指向所创建的InventoryEntry对象的指针
 *          否则，返回NULL
 * */
InventoryEntry *NewInventoryEntry(int itemId, int number, const Time *inboundTime, const Time *productionTime,
        const Amount *inboundUnitPrice);

/*
 * Name: FreeInventoryEntry
 * Description: 释放指定的InventoryEntry对象占用的空间
 * Arguments:
 *     entry: 一个指针，指向需要释放空间的InventoryEntry对象
 * */
void FreeInventoryEntry(InventoryEntry *entry);

/*
 * Name: GetInventoryEntryXXX
 * Description: 获取库存条目的某个成员
 * Arguments:
 *     entry: 指向InventoryEntry的指针
 * Returns: 对应的值
 * */
int GetInventoryEntryId(const InventoryEntry *entry);

int GetInventoryEntryItemId(const InventoryEntry *entry);

int GetInventoryEntryNumber(const InventoryEntry *entry);

Time GetInventoryEntryInboundTime(const InventoryEntry *entry);

Time GetInventoryEntryProductionTime(const InventoryEntry *entry);

Amount GetInventoryEntryInboundUnitPrice(const InventoryEntry *entry);

/*
 * Name: SetInventoryEntryXXX
 * Description: 设置库存条目的某个成员
 * Arguments:
 *     entry: 指向要设置成员的库存条目的指针
 *     value: 要设置成的值
 * */
void SetInventoryEntryItemId(InventoryEntry *entry, int value);

void SetInventoryEntryNumber(InventoryEntry *entry, int value);

void SetInventoryEntryInboundTime(InventoryEntry *entry, const Time *value);

void SetInventoryEntryProductionTime(InventoryEntry *entry, const Time *value);

void SetInventoryEntryInboundUnitPrice(InventoryEntry *entry, const Amount *value);

/*
 * Name: AppendInventoryEntry
 * Description: 向系统中追加一个InventoryEntry对象
 * Arguments:
 *     entry: 一个指针，指向要追加的InventoryEntry对象
 * Returns: 如果成功追加，返回0
 *          否则，返回1
 * */
int AppendInventoryEntry(InventoryEntry *entry);

/*
 * Name: RemoveInventoryEntry
 * Description: 向系统中删除一个InventoryEntry对象
 * Arguments:
 *     entry: 一个指针，指向要删除的InventoryEntry对象
 * */
void RemoveInventoryEntry(InventoryEntry *entry);

/*
 * Name: InventorySave
 * Description: 保存对库存条目的所有更改
 * */
void InventorySave();

#endif
