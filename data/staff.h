/*
 * FileName: data/staff.h
 * Author: 陈若珂
 * Description: 包含员工表示与访问的相关定义和声明
 * */

#ifndef STAFF_H
#define STAFF_H

/*
 * Name: Staff
 * Description: 表示一个员工
 * */
#include "linkedList.h"
typedef struct Staff Staff;

/*
 * Name: NewStaff
 * Description: 创建新的员工
 * Arguments:
 *     isEnabled: 员工可用性
 *     name: 员工名称
 *     password: 员工密码
 *     contact: 员工联系方式
 * Returns: 若创建成功，返回一个指针，指向创建的Staff对象
 *          否则，返回NULL
 * */
Staff *NewStaff(int isEnabled, const char *name, const char *password, const char *contact);

/*
 * Name: FreeStaff
 * Description: 释放指定的Staff对象占用的空间
 * Arguments:
 *     staff: 一个指针，指向要释放空间的Staff对象
 * */
void FreeStaff(Staff *staff);

/*
 * Name: GetAllStaff
 * Description: 获取所有员工
 * Returns: 一个链表，包含所有员工
 *          若不存在员工，返回NULL
 * */
const LinkedList *GetAllStaff();

/*
 * Name: GetStaffById
 * Description: 获取包含指定工号的员工
 * Arguments:
 *     id: 指定的工号
 * Returns: 一个指针，指向包含指定工号的员工
 *          若不存在这样的员工，返回NULL
 * */
Staff *GetStaffById(int id);

/*
 * Name: GetStaffXXX
 * Description: 获取员工的特定信息
 * Arguments:
 *     staff: 指向Staff的指针
 * Returns: 对应的信息
 * */
int GetStaffId(const Staff *staff);
int GetStaffAvailability(const Staff *staff);
const char *GetStaffName(const Staff *staff);
const char *GetStaffContact(const Staff *staff);

/*
 * Name: VerifyStaffPassword
 * Description: 验证员工密码
 * Arguments:
 *     staff: 一个指针，指向要验证密码的员工
 *     givenPassword: 提供的密码
 * Returns: 若员工密码与givenPassword一致，返回1
 *          否则，返回0
 * */
int VerifyStaffPassword(const Staff *staff, const char *givenPassword);

/*
 * Name: SetStaffXXX
 * Description: 设置员工的某个信息
 * Arguments:
 *     staff: 指向要设置信息的员工的指针
 *     value: 要设置成的值
 * */
void SetStaffAvailability(Staff *staff, int value);
void SetStaffName(Staff *staff, const char *value);
void SetStaffContact(Staff *staff, const char *value);

/*
 * Name: AppendStaff
 * Description: 向系统中追加一个Staff对象
 * Arguments:
 *     staff: 一个指针，指向要追加的Staff对象
 * Returns: 如果成功追加，返回0
 *          否则，返回1
 * */
int AppendStaff(Staff *staff);

/*
 * Name: RemoveStaff
 * Description: 向系统中删除一个Staff对象
 * Argument:
 *     staff: 一个指针，指向要删除的Staff对象
 * Returns: 如果成功删除，返回0
 *          否则，返回1
 * */
int RemoveStaff(Staff *staff);

/*
 * Name: StaffSave
 * Description: 保存对员工信息的所有更改
 * */
void StaffSave();

#endif
