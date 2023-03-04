/*
 * FileName: data/data.h
 * Author: 陈若珂
 * Description: 包含数据表示与访问的相关定义和声明
 * */

#ifndef DATA_H
#define DATA_H

struct LossEntry;
struct RefundEntry;
struct Customer;
struct Order;
struct Statistics;
struct Profit;
struct Staff;
struct PermissionEntry;
struct JournalEntry;

// TODO: add customer discount


/*
 * Name: LossEntry
 * Description: 表示一个货物损耗条目
 * */
struct LossEntry {
    // 库存编号
    int inventoryId;

    // 损耗原因
    const char *reason;

    // 损耗时间
    Time time;
};

/*
 * Name: RefundEntry
 * Description: 表示一个退款条目
 * */
struct RefundEntry {
    // 订单编号
    int orderId;

    // 退款理由
    const char *reason;

    // 退款时间
    Time time;

    // 退款金额
    Amount refundAmount;

    // 回库数量
    int number;

    // 备注信息
    const char *remark;
};

/*
 * Name: Customer
 * Description: 表示一个客户
 * */
struct Customer {
    // 客户编号
    int id;

    // 客户等级
    int level;

    // 客户姓名
    const char *name;

    // 客户联系方式
    const char *contact;
};

/*
 * Name: Order
 * Description: 表示一个订单
 * */
struct Order {
    // 订单编号
    int id;

    // 库存编号
    int inventoryId;

    // 数量
    int number;

    // 客户编号
    int customerId;

    // 总价
    Amount amount;
};


/*
 * Name: Statistics
 * Description: 表示统计信息
 * */
struct Statistics {
    // 收支记录
    Profit *profitHistory;
};

/*
 * Name: Profit
 * Description: 表示一个收支条目
 * */
struct Profit {
    // 金额
    Amount amount;

    // 事项
    const char *matter;

    // 时间
    Time time;
};

/*
 * Name: Staff
 * Description: 表示一个员工
 * */
struct Staff {
    // 员工编号
    int id;

    // 员工是否可用
    int isEnabled;

    // 员工姓名
    const char *name;

    // 员工密码
    const char *password;

    // 联系方式
    const char *contact;
};


/*
 * Name: PermissionEntry
 * Description: 表示员工的权限信息
 * */
struct PermissionEntry {
    // 员工编号
    int staffId;

    // 表示员工是否拥有指定权限
    int *hasPermission;
};

/*
 * Name: JournalEntry
 * Description: 表示一个日志条目
 * */
struct JournalEntry {
    // 员工编号
    int staffId;

    // 时间
    Time time;

    // 执行的操作
    Operation operation;

    // 操作相关参数
    const char **arguments;
};

#endif
