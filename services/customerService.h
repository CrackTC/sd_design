#ifndef CUSTOMERCONTROL_H
#define CUSTOMERCONTROL_H

#include "../data/table.h"

// 添加客户

/*  客户等级      level
 *  客户名称      name
 *  联系方式      contact
 *
 *  返回值：
 *      失败：备注返回“客户已存在”或“创建新客户失败”
 * * * * * */
Table *AddCustomer(Table *a);

// 删除客户

/*  客户id        id
 *
 *  返回值：
 *      失败：备注返回“不存在符合条件的客户” 或 “该客户不可删除”
 * * * * * */
Table *DeleteCustomer(Table *a);

// 修改客户信息

/*  客户id        id
 *  客户等级      level
 *  客户姓名      name
 *  联系方式      contact
 *
 *  返回值：
 *      失败：备注返回“不存在符合条件的客户”
 * * * * * */
Table *UpdateCustomer(Table *a);

// 获取客户信息

/*  无输入
 *
 *  返回值：
 *      有客户记录：   id   level   name   contact
 *      无客户记录：备注返回“无客户记录”
 * * * * * */
Table *GetAllCustomer(Table *a);
/*  客户id        id
 *
 *  返回值：
 *      有客户记录：   id   level   name   contact
 *      无客户记录：备注返回“不存在符合条件的客户”
 * * * * * */
Table *GetSingleCustomer(Table *a);

#endif // CUSTOMERCONTROL_H
