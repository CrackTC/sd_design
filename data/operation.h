/*
 * FileName: data/operation.h
 * Author: 陈若珂
 * Description: 包含用户操作相关的数据表示和操作的声明
 * */

#ifndef OPERATION_H
#define OPERATION_H

/*
 * Name: Operation
 * Description: 定义用户操作的枚举值
 * */
enum Operation {
    // 添加员工
    OP_ADD_STAFF,

    // 删除员工
    OP_DELETE_STAFF,

    // 修改员工
    OP_UPDATE_STAFF,

    // 查询员工
    OP_READ_STAFF,

    // 查询日志
    OP_READ_JOURNAL,

    // 查询统计
    OP_READ_STATISTICS,

    // 添加库存
    OP_ADD_INVENTORY,

    // 修改库存
    OP_UPDATE_INVENTORY,

    // 查询库存
    OP_READ_INVENTORY,

    // 添加客户
    OP_ADD_CUSTOMER,

    // 删除客户
    OP_DELETE_CUSTOMER,

    // 修改客户
    OP_UPDATE_CUSTOMER,

    // 读取客户
    OP_READ_CUSTOMER,

    // 读取订单
    OP_READ_ORDER,

    // 添加折扣
    OP_ADD_DISCOUNT,

    // 删除折扣
    OP_DELETE_DISCOUNT,

    // 更新折扣
    OP_UPDATE_DISCOUNT,

    // 查询折扣
    OP_READ_DISCOUNT,

    // 添加退货条目
    OP_ADD_REFUND,

    // 删除退货条目
    OP_DELETE_REFUND,

    // 修改退货条目
    OP_UPDATE_REFUND,

    // 查询退货条目
    OP_READ_REFUND,
};

typedef enum Operation Operation;

/*
 * Name: OperationToString
 * Desciption: 获取操作的枚举项的字符串表示
 * Arguments:
 *     operation: 要获取字符串表示的Operation的枚举项
 * Returns: operation的字符串表示
 * */
const char *OperationToString(Operation operation);

#endif
