/*
 * FileName: data/refundEntry.h
 * Author: 陈若珂
 * Description: 包含退款条目相关数据表示与访问的定义和声明
 * */

#ifndef REFUND_ENTRY_H
#define REFUND_ENTRY_H

#include "time.h"
#include "amount.h"
#include "linkedList.h"

/*
 * Name: RefundEntry
 * Description: 表示一个退款条目
 * */
typedef struct RefundEntry RefundEntry;

/*
 * Name: NewRefundEntry
 * Description: 使用给定的信息创建一个新的退款条目
 * Arguments:
 *     orderId: 订单编号
 *     reason: 退款原因
 *     time: 退款时间
 *     refundAmount: 退款金额
 *     number: 回库数量
 *     remark: 备注信息
 * Returns: 若成功创建，则返回指向创建的RefundEntry对象的指针
 *          否则，返回NULL
 * */
RefundEntry *NewRefundEntry(int orderId, const char *reason, Time *time, Amount *refundAmount, int number,
        const char *remark);

/*
 * Name: FreeRefundEntry
 * Description: 释放特定的RefundEntry对象占用的空间
 * Arguments:
 *     entry: 一个指针，指向要释放空间的RefundEntry对象
 * */
void FreeRefundEntry(RefundEntry *entry);

/*
 * Name: GetAllRefund()
 * Description: 获取所有退款条目
 * Returns: 一个链表，包含所有退款条目
 *          若不存在退款条目，返回NULL
 * */
LinkedList *GetAllRefunds();

/*
 * Name: GetRefundByOrderId
 * Description: 获取包含指定订单号的退款条目
 * Arguments:
 *     orderId: 指定的订单号
 * Returns: 一个指向包含指定订单号的RefundEntry对象的指针
 *          若不存在这样的条目，返回NULL
 * */
RefundEntry *GetRefundByOrderId(int orderId);

/*
 * Name: GetRefundEntryXXX
 * Description: 获取退款条目的某个成员
 * Arguments:
 *     entry: 指向RefundEntry的指针
 * Returns: 对应的值
 * */
int GetRefundEntryOrderId(const RefundEntry *entry);

const char *GetRefundEntryReason(const RefundEntry *entry);

Time GetRefundEntryTime(const RefundEntry *entry);

Amount GetRefundEntryAmount(const RefundEntry *entry);

int GetRefundEntryNumber(const RefundEntry *entry);

const char *GetRefundEntryRemark(const RefundEntry *entry);

/*
 * Name: SetRefundEntryXXX
 * Description: 设置退款条目的某个成员
 * Arguments:
 *     entry: 指向要设置成员的退款条目的指针
 *     value: 要设置成的值
 * */
void SetRefundEntryOrderId(RefundEntry *entry, int value);

void SetRefundEntryReason(RefundEntry *entry, const char *value);

void SetRefundEntryTime(RefundEntry *entry, Time *value);

void SetRefundEntryAmount(RefundEntry *entry, Amount *value);

void SetRefundEntryNumber(RefundEntry *entry, int value);

void SetRefundEntryRemark(RefundEntry *entry, const char *remark);

/*
 * Name: AppendRefundEntry
 * Description: 向系统中追加一个RefundEntry对象
 * Arguments:
 *     entry: 一个指针，指向要追加的RefundEntry对象
 * Returns: 如果成功追加，返回0
 *          否则，返回1
 * */
int AppendRefundEntry(RefundEntry *entry);

/*
 * Name: RemoveRefundEntry
 * Description: 从系统中删除一个RefundEntry对象
 * Arguments:
 *     entry: 一个指针，指向要删除的RefundEntry对象
 * */
void RemoveRefundEntry(RefundEntry *entry);

/*
 * Name: RefundEntrySave
 * Description: 保存对退款条目的所有更改
 * */
void RefundEntrySave();

#endif
