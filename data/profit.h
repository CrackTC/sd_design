/*
 * FileName: data/profit.h
 * Author: 陈若珂
 * Description: 包含收支信息的相关定义和声明
 * */

#ifndef PROFIT_H
#define PROFIT_H

#include "amount.h"
#include "linkedList.h"
#include "time.h"

/*
 * Name: Profit
 * Description: 表示一个收支条目
 * */
typedef struct Profit Profit;

/*
 * Name: NewProfit
 * Description: 创建一个新的收支条目
 * Arguments:
 *     amount: 金额
 *     matter: 事项
 *     time: 时间
 * Returns: 若成功创建，返回一个指针，指向创建的收支条目
 *          否则，返回NULL
 * */
Profit *NewProfit(Amount *amount, const char *matter, Time *time);

/*
 * Name: FreeProfit
 * Description: 释放指定的收支条目占用的空间
 * Arguments:
 *     profit: 一个指针，指向要释放空间的Profit对象
 * */
void FreeProfit(Profit *profit);

/*
 * Name: GetAllProfits
 * Description: 获取所有收支条目
 * Returns: 一个链表，包含所有的收支条目
 *          若不存在收支条目，返回NULL
 * */
LinkedList *GetAllProfits();

/*
 * Name: GetProfitXXX
 * Description: 获取收支条目的某个成员
 * Arguments:
 *     profit: 指向Profit的指针
 * Returns: 对应的值
 * */
Amount GetProfitAmount(const Profit *profit);
const char *GetProfitMatter(const Profit *profit);
Time GetProfitTime(const Profit *profit);

/*
 * Name: SetProfitXXX
 * Description: 设置收支条目的某个成员
 * Arguments:
 *     profit: 指向要设置成员的收支条目的指针
 *     value: 要设置成的值
 * */
void SetProfitAmount(Profit *profit, Amount *value);
void SetProfitMatter(Profit *profit, const char *value);
void SetProfitTime(Profit *profit, Time *value);

/*
 * Name: AppendProfit
 * Description: 向系统中追加一个Profit对象
 * Arguments:
 *     profit: 一个指针，指向要追加的Profit对象
 * Returns: 如果成功追加，返回0
 *          否则，返回1
 * */
int AppendProfit(Profit *profit);

/*
 * Name: RemoveProfit
 * Description: 向系统中删除一个Profit对象
 * Arguments:
 *     profit: 一个指针，指向要删除的Profit对象
 * Returns: 如果成功删除，返回0
 *          否则，返回1
 * */
int RemoveProfit(Profit *profit);

/*
 * Name: ProfitSave
 * Description: 保存对收支条目的所有更改
 * */
void ProfitSave();

#endif
