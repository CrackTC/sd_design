/*
 * FileName: data/order.h
 * Author: 陈若珂
 * Description: 包含订单相关数据表示与访问的声明和定义
 * */

#ifndef ORDER_H
#define ORDER_H

#include "amount.h"
#include "linkedList.h"

/*
 * Name: Order
 * Description: 表示一个订单
 * */
typedef struct Order Order;

/*
 * Name: NewOrder
 * Description: 创建一个新的订单
 * Arguments:
 *     inventoryId: 库存编号
 *     number: 数量
 *     customerId: 客户编号
 *     amount: 总售出价
 * Returns: 若成功创建，返回一个指针，指向创建的Order对象
 *          否则，返回NULL
 * */
Order *NewOrder(int inventoryId, int number, int customerId, Amount *amount);

/*
 * Name: FreeOrder
 * Description: 释放指定Order对象所占用的空间
 * Arguments:
 *     order: 一个指针，指向需要释放空间的Order对象
 * */
void FreeOrder(Order *order);

/*
 * Name: GetAllOrders
 * Description: 获取所有订单
 * Returns: 一个链表，其中包含所有订单
 *          若不存在订单，则返回NULL
 * */
LinkedList *GetAllOrders();

/*
 * Name: GetOrderById
 * Description: 获取具有指定订单编号的订单
 * Arguments:
 *     id: 指定的订单编号
 * Returns: 符合条件的订单
 *          若不存在符合条件的订单，返回NULL
 * */
Order *GetOrderById(int id);

/*
 * Name: GetOrdersByCustomerId
 * Description: 获取具有指定客户编号的所有订单
 * Arguments:
 *     customerId: 指定的等级
 * Returns: 一个链表，包含所有具有指定客户编号的订单
 *          若不存在这样的订单，返回NULL
 * */
LinkedList *GetOrdersByCustomerId(int customerId);

/*
 * Name: GetOrderXXX
 * Description: 获取订单的特定信息
 * Arguments:
 *     order: 指向Order的指针
 * Returns: 对应的值
 * */
int GetOrderId(const Order *order);
int GetOrderCustomerId(const Order *order);
int GetOrderNumber(const Order *order);
Amount GetOrderAmount(const Order *order);

/*
 * Name: SetOrderXXX
 * Description: 设置订单的特定信息
 * Arguments:
 *     order: 指向要设置信息的Order对象的指针
 *     value: 要设置成的值
 * */
void SetOrderCustomerId(Order *order, int value);
void SetOrderNumber(Order *order, int value);
void SetOrderAmount(Order *order, Amount *value);

/*
 * Name: AppendOrder
 * Description: 向系统中追加一个Order对象
 * Arguments:
 *     order: 一个指针，指向要追加的Order对象
 * Returns: 如果成功追加，返回0
 *          否则，返回1
 * */
int AppendOrder(Order *order);

/*
 * Name: RemoveOrder
 * Description: 向系统中删除一个Order对象
 * Arguments:
 *     order: 一个指针，指向要删除的Order对象
 * */
void RemoveOrder(Order *order);

/*
 * Name: OrderSave
 * Description: 保存对订单信息的所有更改
 * */
void OrderSave();

#endif
