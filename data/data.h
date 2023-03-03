/*
 * FileName: data/data.h
 * Author: 陈若珂
 * Description: 包含数据表示与访问的相关定义和声明
 * */

#ifndef DATA_H
#define DATA_H

#include <time.h>

typedef int Amount;
typedef time_t Time;

struct InventoryEntry;
struct Item;
struct BaseDiscount;
struct LossEntry;
struct RefundEntry;
struct Customer;
struct Order;
struct Statistics;
struct Profit;
struct Staff;
struct PermissionEntry;
struct JournalEntry;

/*
 * Name: Operation
 * Description: 定义用户操作的枚举值
 * */
enum Operation {
    OP_ADD_STAFF,
    OP_DELETE_STAFF,
    OP_UPDATE_STAFF,
    OP_READ_STAFF,

    OP_READ_JOURNAL,

    OP_READ_STATISTICS,

    OP_ADD_INVENTORY,
    OP_UPDATE_INVENTORY,
    OP_READ_INVENTORY,

    OP_ADD_CUSTOMER,
    OP_DELETE_CUSTOMER,
    OP_UPDATE_CUSTOMER,
    OP_READ_CUSTOMER,

    OP_READ_ORDER,

    OP_ADD_DISCOUNT,
    OP_DELETE_DISCOUNT,
    OP_UPDATE_DISCOUNT,
    OP_READ_DISCOUNT,

    OP_ADD_REFUND,
    OP_DELETE_REFUND,
    OP_UPDATE_REFUND,
    OP_READ_REFUND,
};

/*
 * Name: InventoryEntry
 * Description: 表示一个库存条目
 * */
struct InventoryEntry {
    // 库存编号
    int id;

    // 商品编号
    int itemId;

    // 数量
    int number;

    // 入库时间
    Time inboundTime;

    // 生产日期
    Time productionTime;

    // 购入单价
    Amount inboundUnitPrice;
};

/*
 * Name: Item
 * Description: 表示一个商品
 * */
struct Item {
    // 商品编号
    int id;

    // 商品名称
    char *name;

    // 售价
    Amount price;

    // 保质期
    Time shelfLife;
};

/*
 * Name: BaseDiscount
 * Description: 表示基于折扣比率的基本折扣
 * */
struct BasicDiscount {
    // 商品编号
    int itemId;

    // 折扣比率%
    int ratio;

    // 折扣截止时间
    Time deadline;
};

/*
 * Name: LossEntry
 * Description: 表示一个货物损耗条目
 * */
struct LossEntry {
    // 库存编号
    int inventoryId;

    // 损耗原因
    char *reason;

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
    char *reason;

    // 退款时间
    Time time;

    // 退款金额
    Amount refundAmount;

    // 回库数量
    int number;

    // 备注信息
    char *remark;
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
    char *name;

    // 客户联系方式
    char *contact;
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
    char *matter;

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
    char *name;

    // 员工密码
    char *password;

    // 联系方式
    char *contact;
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
    char **arguments;
};

#endif
