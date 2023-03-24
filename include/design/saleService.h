#ifndef SALE_H
#define SALE_H

#include "table.h"

//添加退货

/*  订单id    id
 *  原因      reason
 *  元        yuan
 *  角        jiao
 *  分        cent
 *  数量      number
 *  备注      remark
 *
 *  返回值：
 *      失败：备注返回“超过退货限制次数(1次)” 或  备注返回“退货失败”
 * * * * * */
Table *AddRefund(Table *a);

//删除退货

/*  订单id    id
 *
 *  返回值：
 *      失败：备注返回“不存在符合条件的退款条目” 或 “删除退货失败”
 * * * * * */
Table *RemoveRefund(Table *a);

//修改退货

/*  订单id    id
 *  原因      reason
 *  元        yuan
 *  角        jiao
 *  分        cent
 *  退款数量  number
 *  备注      remark
 *
 *  返回值：
 *      失败：备注返回“不存在符合条件的退款条目” 或 “修改退货失败”
 * * * * * */
Table *UpdateRefund(Table *a);

//查询退货

/*  无输入
 *
 *  返回值：
 *      有退款条目：   id   reason   time   yuan   jiao   cent   number   remark
 *      无退款条目：备注返回“无退款条目”
 * * * * * */
Table *GetAllRefund(__attribute__((unused)) Table *a);

/*  订单id    id
 *
 *  返回值:
 *      有退款条目：   id   reason   time   yuan   jiao   cent   number   remark
 *      无退款条目：备注返回“不存在符合条件的退款条目”
 * * * * * */
Table *GetSingleRefund(Table *a);

//读取订单

/*  无输入
 *
 *  返回值：
 *      有订单条目：   orderid   itemname   customerid   number   yuan   jiao   cent   time   inventoryid
 *      无订单条目：备注返回“无订单条目”
 * * * * * */
Table *GetAllOrder(__attribute__((unused)) Table *a);

/*  订单id    id
 *
 *  返回值：
 *      有订单条目：   orderid   itemname   customerid   number   yuan   jiao   cent   time   inventoryid
 *      无订单条目：备注返回“不存在符合条件的订单条目”
 * * * * * */
Table *GetSingleOrder(Table *a);

//添加折扣

/*  商品id    id
 *  折扣比率  ratio
 *  客户等级  level
 *  折扣截止
 *      年    year
 *      月    month
 *      日    day
 *      小时  hour
 *      分钟  minute
 *
 *  返回值：
 *      失败：备注返回“时间信息有误” 或 “创建折扣失败”
 * * * * * */
Table *AddDiscount(Table *a);

//删除折扣

/*  折扣id    id
 *
 *  返回值：
 *      失败：备注返回“不存在满足条件的折扣”
 * * * * * */
Table *RemoveDiscount(Table *a);

//修改折扣

/*  折扣id    discountid
 *  商品id    itemid
 *  折扣比率  ratio
 *  客户等级  level
 *  折扣截止
 *      年    year
 *      月    month
 *      日    day
 *      小时  hour
 *      分钟  minute
 *      秒    second
 *
 *  返回值：
 *      失败：备注返回“时间信息有误” 或 “不存在符合条件的折扣”
 * * * * * */
Table *UpdateDiscount(Table *a);

//清理过期折扣

/*  无输入
 *
 *  无输出
 * * * * * */
Table *ClearOutdateDiscount(__attribute__((unused)) Table *a);

//查询折扣

/*  无输入
 *
 *  返回值：
 *      有折扣：   id   itemname  ratio   customerlevel   deadline
 *      无折扣：备注返回“无折扣”
 */
Table *GetAllDiscount(__attribute__((unused)) Table *a);

/*  折扣id    id
 *
 *  返回值：
 *      有折扣：   id   itemname  ratio   customerlevel   deadline
 *      无折扣：备注返回“不存在满足条件的折扣”
 */
Table *GetSingleDiscount(Table *a);

//添加售货

/*  商品id    itemid
 *  客户id    customerid
 *  商品数量  number
 *
 *  返回值：
 *      成功：   customerid   itemname   number   yuan   jiao   cent   time
 *      失败：备注返回“不存在该客户” 或 “不存在该商品"
 */
Table *AddOrder(Table *a);

/*  商品id    itemid
 *  客户id    customerid
 *  商品数量  number
 *  库存编号  inventoryid
 *
 *  返回值：
 *      成功：   customerid   itemname   number   yuan   jiao   cent   time   inventoryid
 *      失败：备注返回“不存在该客户” 或 “不存在该商品” 或 “不存在该库存条目” 或 “库存不足”
 */
Table *AddSpecificOrder(Table *a);

//修改订单

/*  订单id    orderid
 *  库存id    inventoryid
 *  数量      number
 *  客户id    customerid
 *
 *  返回值：
 *      失败：备注返回“不存在符合条件的订单条目” 或 “不存在符合条件的库存条目” 或 “不存在符合条件的客户”
 * * * * * */
Table *UpdateOrder(Table *a);

//删除订单

/*  订单id    id
 *
 *  返回值：
 *      失败：备注返回“不存在符合条件的订单条目” 或 “删除订单失败” 或 “该订单不可删除”
 * * * * * */
Table *RemoveAnOrder(Table *a);

#endif // SALE_H
