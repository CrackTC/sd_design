/*
 * FileName: data/inventory.h
 * Author: 陈若珂
 * Description: 包含库存相关的数据表示和操作的声明
 * */

#ifndef INVENTORY_H
#define INVENTORY_H

#include "time.h"
#include "amount.h"

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
    Time *inboundTimePointer;

    // 生产日期
    Time *productionTimePointer;

    // 购入单价
    Amount *inboundUnitPricePointer;
};

#endif
