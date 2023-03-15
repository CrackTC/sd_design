#ifndef INVENTORYMANAGEMENT_H
#define INVENTORYMANAGEMENT_H
#include "table.h"
#include "amount.h"
#include "time.h"
#include "inventory.h"
#include "item.h"
#include "lossEntry.h"
#include "linkedlist.h"
#include "profit.h"
Table* AddInventory(Table* a);//向货存系统中添加一批货物
Table* UpdateInventory();//查验货存系统中的货物是否过期并将过期的货物放入货损系统中
Table* DeleteInventory(Table* a);//删除货存系统中的某一批货物的全部信息
Table* ReviseInventory(Table* a);//修改货存系统中的某一批货物的全部信息
Table* AddItem(Table* a);//添加一个商品种类
Table* DeleteItem(Table* a);//删除一个货品种类
Table* ShowInventory();//展示所有的货存
Table* ShowItem();//展示所有的商品
Table* ShowSingleInventory(Table* a);//展示某一个货存的全部信息
Table* ShowSingleItem(Table* a);//展示某一个商品的全部信息
Table* ReviseItem(Table* a);//修改某一个商品的全部信息
Table* AddLossInventory(Table* a);//增加一个货损信息
Table* DeleteInventory(Table* a);//删除一个货损信息
Table* ShowLossInventory();//展示所有的货损信息
Table* ShowSingleInventory(Table* a);//展示某一个货损信息
Table* ReviseLossInventory(Table* a);//修改某一个货损信息
#endif