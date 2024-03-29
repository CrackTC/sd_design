/*
 * FileName: data/permission.h
 * Author: 陈若珂
 * Description: 包含员工权限的相关定义和声明
 * */

#ifndef PERMISSION_H
#define PERMISSION_H

#include "linkedList.h"
#include "operation.h"

/*
 * Name: PermissionEntry
 * Description: 表示员工的权限信息
 * */
typedef struct PermissionEntry PermissionEntry;

/*
 * Name: NewPermissionEntry
 * Description: 创建一个新的权限信息对象
 * Arguments:
 *     staffId: 员工编号
 *     hasPermission: 一个指针，指向一个包含所有权限情况的数组
 * Returns: 若创建成功，返回一个指针，指向创建的PermissionEntry对象
 *          否则，返回NULL
 * */
PermissionEntry *NewPermissionEntry(int staffId, const int *hasPermission);

/*
 * Name: FreePermissionEntry
 * Description: 释放指定的PermissionEntry对象占用的空间
 * Arguments:
 *     entry: 一个指针，指向要释放空间的PermissionEntry对象
 * */
void FreePermissionEntry(PermissionEntry *entry);

/*
 * Name: GetAllPermissionEntry
 * Description: 获取所有权限条目
 * Returns: 一个链表，包含所有权限条目
 *          若不存在权限条目，返回NULL
 * */
LinkedList *GetAllPermissionEntry();

/*
 * Name: GetPermissionEntryByStaffId
 * Description: 获取具有指定员工编号的权限条目
 * Arguments:
 *     staffId: 指定的员工编号
 * Returns: 一个指针，指向具有指定员工编号的权限条目
 *          若不存在这样的权限条目，返回NULL
 * */
PermissionEntry *GetPermissionEntryByStaffId(int staffId);

/*
 * Name: PermissionToString
 * Description: 将权限条目转换为字符串形式
 * Arguments:
 *     entry: 权限条目
 * Returns: 字符串表示
 * */
char *PermissionToString(PermissionEntry *entry);

/*
 * Name: ParsePermission
 * Description: 从权限的字符串表示获取权限
 * Arguments:
 *     permissionString: 权限的字符串表示
 * Returns: 权限
 * */
int *ParsePermission(const char *permissionString);

/*
 * Name: GetPermissionEntryXXX
 * Description: 获取权限条目的某个成员
 * Arguments:
 *     entry: 指向PermissionEntry的指针
 * Returns: 对应的值
 * */
int GetPermissionEntryStaffId(const PermissionEntry *entry);

const int *GetPermissionEntryPermissions(const PermissionEntry *entry);

/*
 * Name: SetPermissionEntryXXX
 * Description: 设置权限条目的权限
 * Arguments:
 *     entry: 指向要设置的权限条目的指针
 *     operation: 要设置权限的操作
 *     allow: 允许则为1，不允许则为0
 * */
void SetPermissionEntryPermissions(PermissionEntry *entry, Operation operation, int allow);

/*
 * Name: AppendPermissionEntry
 * Description: 向系统中追加一个PermissionEntry对象
 * Arguments:
 *     entry: 一个指针，指向要追加的对象
 * Returns: 若成功追加，返回0
 *          否则，返回1
 * */
int AppendPermissionEntry(PermissionEntry *entry);

/*
 * Name: RemovePermissionEntry
 * Description: 向系统中删除一个PermissionEntry对象
 * Arguments:
 *     entry: 一个指针，指向要删除的对象
 * */
void RemovePermissionEntry(PermissionEntry *entry);

/*
 * Name: PermissionSave
 * Description: 保存对权限条目的所有更改
 * */
void PermissionSave();

#endif
