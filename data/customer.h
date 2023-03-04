/*
 * FileName: data/customer.h
 * Author: 陈若珂
 * Description: 包含客户相关数据表示与访问的声明和定义
 * */

#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "linkedList.h"

/*
 * Name: Customer
 * Description: 表示一个客户
 * */
typedef struct Customer Customer;

/*
 * Name: NewCustomer
 * Description: 创建一个新的客户
 * Arguments:
 *     level: 客户等级
 *     name: 客户名称
 *     contact: 客户联系方式
 * Returns: 若成功创建，返回一个指针，指向创建的Customer对象
 *          否则，返回NULL
 * */
Customer *NewCustomer(int level, const char *name, const char *contact);

/*
 * Name: FreeCustomer
 * Description: 释放指定Customer对象所占用的空间
 * Arguments:
 *     customer: 一个指针，指向需要释放空间的Customer对象
 * */
void FreeCustomer(Customer *customer);

/*
 * Name: GetAllCustomers
 * Description: 获取所有客户
 * Returns: 一个链表，其中包含所有客户
 *          若不存在客户，则返回NULL
 * */
const LinkedList *GetAllCustomers();

/*
 * Name: GetCustomerById
 * Description: 获取具有指定客户编号的客户
 * Arguments:
 *     id: 指定的客户编号
 * Returns: 符合条件的库存条目
 *          若不存在符合条件的条目，返回NULL
 * */
const Customer *GetCustomerById(int id);

/*
 * Name: GetCustomersByLevel
 * Description: 获取具有指定等级的所有客户
 * Arguments:
 *     level: 指定的等级
 * Returns: 一个链表，包含所有具有指定等级的客户
 *          若不存在这样的客户，返回NULL
 * */
const LinkedList *GetCustomersByLevel(int level);

/*
 * Name: GetCustomersByName
 * Description: 获取具有指定名称的所有客户
 * Arguments:
 *     name: 指定的客户名称
 * Return: 一个链表，包含所有具有指定名称的客户
 *         若不存在这样的客户，返回NULL*/
const LinkedList *GetCustomersByName(const char *name);

/*
 * Name: GetCustomerXXX
 * Description: 获取客户的特定信息
 * Arguments:
 *     customer: 指向Customer的指针
 * Returns: 对应的值
 * */
int GetCustomerId(const Customer *customer);
int GetCustomerLevel(const Customer *customer);
const char *GetCustomerName(const Customer *customer);
const char *GetCustomerContact(const Customer *customer);

/*
 * Name: SetCustomerXXX
 * Description: 设置客户的特定信息
 * Arguments:
 *     customer: 指向要设置信息的Customer对象的指针
 *     value: 要设置成的值
 * */
void SetCustomerLevel(Customer *customer, int value);
void SetCustomerName(Customer *customer, const char *name);
void SetCustomerContact(Customer *customer, const char *contact);

/*
 * Name: AppendCustomer
 * Description: 向系统中追加一个Customer对象
 * Arguments:
 *     custmer: 一个指针，指向要追加的Customer对象
 * Returns: 如果成功追加，返回0
 *          否则，返回1
 * */
int AppendCustomer(Customer *customer);

/*
 * Name: CustomerSave
 * Description: 保存对客户信息的所有更改
 * */
void CustomerSave();

#endif
