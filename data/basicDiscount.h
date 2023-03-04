/*
 * FileName: data/basicDiscount.h
 * Author: 陈若珂
 * Description: 包含基本折扣相关数据表示和操作的声明
 * */

#ifndef BASIC_DISCOUNT_H
#define BASIC_DISCOUNT_H

#include "linkedList.h"
#include "time.h"

/*
 * Name: BaseDiscount
 * Description: 表示基于折扣比率的基本折扣
 * */
typedef struct BasicDiscount BasicDiscount;

/*
 * Name: NewBasicDiscount
 * Description: 创建新的基本折扣
 * Arguments:
 *     itemId: 商品编号
 *     ratio: 折扣比率
 *     customerLevel: 客户等级
 *     deadline: 折扣截止时间
 * Returns: 若成功创建，返回指向创建的BasicDiscount对象的指针
 *          否则，返回NULL
 * */
BasicDiscount *NewBasicDiscount(int itemId, int ratio, int customerLevel, Time *deadline);

/*
 * Name: FreeBasicDiscount
 * Description: 释放指定的BasicDiscount对象占用的空间
 * Arguments:
 *     discount: 指向要释放空间的BasicDiscount对象的指针
 * */
void FreeBasicDiscount(BasicDiscount *discount);

/*
 * Name: GetAllBasicDiscounts
 * Description: 获取所有基本折扣
 * Returns: 一个链表，包含所有基本折扣
 *          若不存在基本折扣，返回NULL
 * */
const LinkedList *GetAllBasicDiscounts();

/*
 * Name: GetBasicDiscountsByItemId
 * Description: 获取具有指定商品编号的基本折扣
 * Arguments:
 *     itemId: 指定的商品编号
 * Returns: 一个链表，包含所有符合条件的基本折扣
 *          若不存在符合条件的基本折扣，返回NULL
 * */
const LinkedList *GetBasicDiscountsByItemId(int itemId);

/*
 * Name: GetBasicDiscountXXX
 * Description: 获取基本折扣的某个成员
 * Arguments:
 *     discount: 指向BasicDiscount的指针
 * Returns: 对应的值
 * */
int GetBasicDiscountItemId(const BasicDiscount *discount);
int GetBasicDiscountRatio(const BasicDiscount *discount);
int GetBasicDiscountCustomerLevel(const BasicDiscount *discount);
Time *GetBasicDiscountDeadline(const BasicDiscount *discount);

/*
 * Name: SetBasicDiscountXXX
 * Description: 设置基本折扣的某个成员
 * Arguments:
 *     discount: 指向要设置成员的基本折扣的指针
 *     value: 要设置成的值
 * */
void SetBasicDiscountItemId(BasicDiscount *discount, int value);
void SetBasicDiscountRatio(BasicDiscount *discount, int value);
void SetBasicDiscountCustomerLevel(BasicDiscount *discount, int value);
void SetBasicDiscountDeadline(BasicDiscount *discount, Time *value);

/*
 * Name: AppendBasicDiscount
 * Description: 向系统追加一个BaseDiscount对象
 * Arguments:
 *     discount: 一个指针，指向要追加的BaseDiscount对象
 * Returns: 如果追加成功，返回0
 *          否则，返回1
 * */
int AppendBasicDiscount(BasicDiscount *discount);

/*
 * Name: BasicDiscountSave
 * Description: 保存对基本折扣的所有更改
 * */
void BasicDiscountSave();

#endif
