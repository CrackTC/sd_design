#ifndef INVENTORY_SERVICE_H
#define INVENTORY_SERVICE_H
#include "../data/table.h"

Table *UpdateInventory(Table *input); // 查验货存系统中的货物是否过期并将过期的货物放入货损系统中
Table *ShowLackInventory(Table *input); // 展示库存紧缺的货存信息

// 向某一个系统中添加某一个结构体的函数
Table *AddInventory(Table *input);     // 向货存系统中添加一批货物
Table *AddItem(Table *input);          // 添加一个商品种类
Table *AddLossInventory(Table *input); // 增加一个货损信息

// 查找并展示某一个信息的函数
Table *ShowSingleInventoryById(Table *input); // 展示某一个货存的全部信息 以货存编号为索引
Table *ShowSingleInventoryByItemId(Table *input); // 展示同一个商品编号的全部货存信息 以商品编号为索引
Table *ShowAllInventoryByItemName(Table *input); // 展示同一个商品名称的全部货存信息 以商品名称为索引
Table *ShowSingleItemByItemname(Table *input); // 展示某一个商品的全部信息 以商品名称为索引
Table *ShowSingleItemByItemId(Table *input);   // 展示某一个商品的全部信息 以商品编号为索引
Table *ShowSingleLossById(Table *input);       // 展示一个货损的信息 以货损编号为索引

// 删除某一个系统中的一个结构体信息的函数
Table *DeleteInventoryById(Table *input); // 删除货存系统中的某一批货物的全部信息 以货存编号为索引
Table *DeleteSingleLossInventoryById(Table *input); // 删除某一个货损信息
Table *DeleteItemById(Table *input);                // 删除一个商品种类 以商品编号为索引

// 修改某一个系统中一个结构体的信息的函数
Table *ReviseInventory(Table *input); // 修改货存系统中的某一批货物的全部信息 以货存编号为索引
Table *ReviseAnItemByItemName(Table *input); // 修改某一个商品的全部信息 以商品名称为索引
Table *ReviseAnItemByItemId(Table *input);   // 修改某一个商品的全部信息 以商品编号为索引
Table *ReviseLossInventory(Table *input);    // 修改某一个货损信息

// 展示某一个系统中所储存的全部信息
Table *ShowInventory(Table *input);     // 展示所有的货存
Table *ShowItem(Table *input);          // 展示所有的商品
Table *ShowLossInventory(Table *input); // 展示所有的货损信息

#endif
