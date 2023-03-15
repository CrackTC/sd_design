#ifndef SALE_H
#define SALE_H

#include "table.h"

/*
 * Name: AddRefund
 * Desciption: 添加退货
 * Arguments:
 *      orderId: 订单编号
 *      reason: 退款原因
 *      money: yuan,jiao,fen
 *      number: 回库数量
 *      remark: 备注信息
 * Returns: 若退货成功，返回0
 *          否则，返回1
 * */
//int AddRefund(int orderId, const char *reason,int yuan,int jiao,int fen,int number,const char *remark);
//添加退货
Table *AddRefund(Table *a);

//删除退货
Table *RemoveRefund(Table *a);

//修改退货
Table *UpdateRefund(Table *a);
//Table *ResetReason(Table *a);
//Table *ResetMoney(Table *a);
//Table *ResetNumber(Table *a);
//Table *ResetRemark(Table *a);

//查询退货
Table *GetAllRefunds(Table *a);
Table *GetSingleRefund(Table *a);

//读取订单
Table *GetAllOrders(Table *a);
Table *GetSingleOrder(Table *a);

//添加折扣
Table *AddDiscount(Table *a);

//删除折扣
Table *RemoveDiscount(Table *a);

//更新折扣
Table *UpdateDiscount(Table *a);

//查询折扣
Table *GetAllDiscount(Table *a);
Table *GetSingleDiscount(Table *a);

//添加售货
Table *AddOrder(Table *a);//需要商品id,客户id,number//自动选择出货批次
Table *AddSpecificOrder(Table *a);//需要商品id,客户id,number,指定库存编号//手动选择出货批次

//赠送商品
Table *Happy(Table *a);//需要商品id,客户id,number

#endif // SALE_H
