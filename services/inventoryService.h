#ifndef INVENTORY_SERVICE_H
#define INVENTORY_SERVICE_H
#include "../data/table.h"

/*
 *role:查验货存系统中的货物是否过期并将过期的货物放入货损系统中
 *input: NULL
 * return:如果有过期的货物 则返回过期货物的表格 否则返回无过期表格
 **/
Table *UpdateInventory(__attribute__((unused)) Table *input);

/*
 *role:展示库存紧缺的货存信息
 *input:NULL
 * return;返回库存紧缺的商品的剩余库存 若无缺货状态的商品 则返回无缺货商品
 **/
Table *ShowLackInventory(__attribute__((unused)) Table *input);

// 向某一个系统中添加某一个结构体的函数

/*
 *role:向货存系统中添加一批货物
 *input: 要添加货物的信息 包括 itemId,number,y1,m1,d1,h1,min1,s1,y2,m2,d2,h2,min2,s2,yuan,jiao,cent
 * return:货品是否添加成功和账目是否修改成功的表格
 **/
Table *AddInventory(Table *input);

/*
 *role:向商品系统中添加一个商品种类
 *input:要添加的商品信息 包括 Id name yuan jiao centy1,m1,d1,h1,min1,s1
 *return:商品是否添加成功的表格
 **/
Table *AddItem(Table *input);

/*
 *role: 增加一个货损信息
 *input:要添加的货损信息 包括 Id number reason
 *return:货损是否添加成功的表格
 **/
Table *AddLossInventory(Table *input); // 增加一个货损信息

// 查找并展示某一个信息的函数

/*
 *role:展示某一个货存的全部信息 以货存编号为索引
 *input:要展示的货存编号 id
 *return:相关编号的货存信息的表格
 **/
Table *ShowSingleInventoryById(Table *input);

/*
 * role:展示同一个商品编号的全部货存信息 以商品编号为索引
 * input:商品编号 ItemId
 * return:相关商品编号的全部货存信息
 */
Table *ShowSingleInventoryByItemId(Table *input);

/*
 * role:展示某一个商品的全部信息 以商品编号为索引
 * input:商品编号 ItemId
 * return:相关商品的全部信息的表格
 */
Table *ShowSingleItemByItemId(Table *input);

/*
 * 展示一个货损的信息 以货损编号为索引
 * input:相关的货损编号 Id
 *return:相关的货损信息的全部表格
 **/
Table *ShowSingleLossById(Table *input);

// 删除某一个系统中的一个结构体信息的函数

/*
 * role: 删除货存系统中的某一批货物的全部信息 以货存编号为索引
 * input:要删除的货存信息的标号 Id
 * return:删除表格
 **/
Table *DeleteInventoryById(Table *input);

/*
 * role: 删除货损系统中的某一批货物的全部信息 以货损编号为索引
 * input:要删除的货损信息的标号 Id
 * return:删除表格
 **/
Table *DeleteSingleLossById(Table *input);

/*
 * role: 删除商品系统中的某一批货物的全部信息 以商品编号为索引
 * input:要删除的商品信息的标号 Id
 * return:删除表格
 **/
Table *DeleteItemById(Table *input);

// 修改某一个系统中一个结构体的信息的函数

/*
 * role:修改货存系统中的某一批货物的全部信息 以货存编号为索引
 * input:要修改的货存编号和 修改的信息 Id,itemId,number,y,m,d,h,min,s,yuan,jiao,cent
 * return:是否修改成功的表格
 **/
Table *ReviseInventory(Table *input);

/*
 * role:修改某一个商品的全部信息 以商品编号为索引
 * input:要修改的商品编号和 修改的信息 Id,y,m,d,h,min,s,yuan,jiao,cent，newItemName
 * return:是否修改成功的表格
 **/
Table *ReviseAnItemByItemId(Table *input);

/*
 * role:修改某一个货损信息
 * input:要修改的货损编号和修改的信息 Id reason,y,m,d,h,min,s,number
 * return:是否修改成功的表格
 **/
Table *ReviseLossInventory(Table *input);

// 展示某一个系统中所储存的全部信息

/*
 * role:  展示所有的货存
 * input：NULL
 * return:全部货存信息的表格
 **/
Table *ShowInventory(__attribute__((unused)) Table *input);

/*
 * role: 展示所有的商品
 * input：NULL
 * return:全部商品信息的表格
 **/
Table *ShowItem(__attribute__((unused)) Table *input);

/*
 * role: 展示所有的货损
 * input：NULL
 * return:全部货损信息的表格
 **/
Table *ShowLossInventory(Table *input); // 展示所有的货损信息

#endif
