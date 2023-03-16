/* FileName: data/lossEntry.h
 * Author: 陈若珂
 * Description: 包含货物损耗相关数据表示和操作的定义和声明
 * */

#ifndef LOSS_ENTRY_H
#define LOSS_ENTRY_H

#include "linkedList.h"
#include "time.h"

/*
 * Name: LossEntry
 * Description: 表示一个货物损耗条目
 * */
typedef struct LossEntry LossEntry;

/*
 * Name: NewLossEntry
 * Description: 使用给定的信息创建一个新的货损条目
 * Arguments:
 *     inventoryId: 库存条目编号
 *     reason: 货损原因
 *     time: 货损时间
 * Returns: 若成功创建，则返回指向创建的LossEntry对象的指针
 *          否则，返回NULL
 * */
LossEntry *NewLossEntry(int inventoryId, const char *reason, Time *time);

/*
 * Name: FreeLossEntry
 * Description: 释放指定的LossEntry对象占用的空间
 * Arguments:
 *     entry: 一个指针，指向要释放空间的LossEntry对象
 * */
void FreeLossEntry(LossEntry *entry);

/*
 * Name: GetAllLoss
 * Description: 获取所有货损条目
 * Returns: 一个链表，包含所有货损条目
 *          若不存在货损条目，返回NULL
 * */
LinkedList *GetAllLoss();

/*
 * Name: GetLossEntriesByInventoryId
 * Description: 获取包含指定库存编号的货损条目
 * Arguments:
 *     inventoryId: 指定的库存编号
 * Returns: 一个链表，包含所有符合条件的货损条目
 *          若不存在这样的条目，返回NULL
 * */
LinkedList *GetLossEntriesByInventoryId(int inventoryId);

/*
 * Name: GetLossEntryXXX
 * Description: 获取货损条目的某个成员
 * Arguments:
 *     entry: 指向LossEntry的指针
 * Returns: 对应的值
 * */
int GetLossEntryInventoryId(const LossEntry *entry);
char *GetLossEntryReason(const LossEntry *entry);
Time GetLossEntryTime(const LossEntry *entry);

/*
 * Name: SetLossEntryXXX
 * Description: 设置货损条目的某个成员
 * Arguments:
 *     entry: 指向要设置成员的货损条目的指针
 *     value: 要设置成的值
 * */
void SetLossEntryInventoryId(LossEntry *entry, int value);
void SetLossEntryReason(LossEntry *entry, char *value);
void SetLossEntryTime(LossEntry *entry, Time *value);

/*
 * Name: AppendLossEntry
 * Description: 向系统中追加一个LossEntry对象
 * Arguments:
 *     entry: 一个指针，指向要追加的LossEntry对象
 * Returns: 如果成功追加，返回0
 *          否则，返回1
 * */
int AppendLossEntry(LossEntry *entry);

/*
 * Name: RemoveLossEntry
 * Description: 向系统中删除一个LossEntry对象
 * Arguments:
 *     entry: 一个指针，指向要删除的LossEntry对象
 * */
void RemoveLossEntry(LossEntry *entry);

/*
 * Name: LossEntrySave
 * Description: 保存对货损条目的所有更改
 * */
void LossEntrySave();

#endif
