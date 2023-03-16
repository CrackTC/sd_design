#ifndef INVENTORY_SERVICE_H
#define INVENTORY_SERVICE_H
#include "../data/table.h"

Table *UpdateInventory();   // 查验货存系统中的货物是否过期并将过期的货物放入货损系统中
Table *ShowLackInventory(); // 展示库存紧缺的货存信息

// 向某一个系统中添加某一个结构体的函数
Table *AddInventory(Table *a);     // 向货存系统中添加一批货物
Table *AddItem(Table *a);          // 添加一个商品种类
Table *AddLossInventory(Table *a); // 增加一个货损信息

// 查找并展示某一个信息的函数
Table *ShowSingleInventoryById(Table *a);     // 展示某一个货存的全部信息 以货存编号为索引
Table *ShowSingleInventoryByItemId(Table *a); // 展示同一个商品编号的全部货存信息 以商品编号为索引
Table *ShowAllInventoryByItemName(Table *a); // 展示同一个商品名称的全部货存信息 以商品名称为索引
Table *ShowSingleItemByItemname(Table *a);   // 展示某一个商品的全部信息 以商品名称为索引
Table *ShowSingleItemByItemId(Table *a);     // 展示某一个商品的全部信息 以商品编号为索引
Table *ShowSingleLossInventoryById(Table *a); // 展示一个货损的信息 以货损编号为索引

// 删除某一个系统中的一个结构体信息的函数
Table *DeleteInventoryById(Table *a); // 删除货存系统中的某一批货物的全部信息 以货存编号为索引
Table *DeleteALossInventoryById(Table *a); // 删除某一个货损信息
Table *DeleteItemByName(Table *a);         // 删除一个商品种类 以商品名称为索引
Table *DeleteItemById(Table *a);           // 删除一个商品种类 以商品编号为索引

// 修改某一个系统中一个结构体的信息的函数
Table *ReviseInventory(Table *a); // 修改货存系统中的某一批货物的全部信息 以货存编号为索引
Table *ReviseAnItemByItemName(Table *a); // 修改某一个商品的全部信息 以商品名称为索引
Table *ReviseAnItemByItemId(Table *a);   // 修改某一个商品的全部信息 以商品编号为索引
Table *ReviseLossInventory(Table *a);    // 修改某一个货损信息

// 展示某一个系统中所储存的全部信息
Table *ShowInventory();     // 展示所有的货存
Table *ShowItem();          // 展示所有的商品
Table *ShowLossInventory(); // 展示所有的货损信息

#endif
