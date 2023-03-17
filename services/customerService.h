#ifndef CUSTOMERCONTROL_H
#define CUSTOMERCONTROL_H

#include "../data/table.h"

// 添加客户
Table *AddCustomer(Table *a); // 需要level,name,contact;成功，在备注返回“Success”；失败，在备注返回失败原因

// 删除客户
Table *RemoveCustomer_1(Table *a); // 需要客户id;成功，在备注返回“Success”；失败，在备注返回失败原因

// 修改客户
Table *UpdateCustomer(Table *a); // 需要客户id，level, name,
                                 // contact;无备注;返回修改后的客户信息（id,level,name,contact）

// 获取客户信息
Table *GetAllCustomer(Table *a);    // 在表格返回所有用户信息
Table *GetSingleCustomer(Table *a); // 需要客户id;在表格返回用户信息

#endif // CUSTOMERCONTROL_H
