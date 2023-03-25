#include "design/saleService.h"
#include "design/amount.h"
#include "design/basicDiscount.h"
#include "design/customer.h"
#include "design/inventory.h"
#include "design/item.h"
#include "design/order.h"
#include "design/profit.h"
#include "design/refundEntry.h"
#include "design/utils.h"

#include <stdlib.h>

// 自动售货
Table *AddOrder(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int itemid = atoi(GetRowItemByColumnName(a, information, "商品编号"));
    int customerid = atoi(GetRowItemByColumnName(a, information, "客户编号"));
    int number = atoi(GetRowItemByColumnName(a, information, "购买数量"));
    // 确认客户等级和所需金额
    Customer *cus = GetCustomerById(customerid);
    if (cus == NULL)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "不存在该客户");
        return errorreturn;
    }
    int customerlevel = GetCustomerLevel(cus);

    Item *item = GetItemById(itemid);
    if (item == NULL)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "不存在该商品");
        return errorreturn;
    }
    Amount price = GetItemPrice(item);
    Amount store = NewAmount(0, 0, 0);

    // 算折扣
    LinkedList *itemdiscount = GetBasicDiscountsByItemId(itemid);
    LinkedList *singlediscount = itemdiscount;
    while (singlediscount != NULL)
    {
        BasicDiscount *discount = singlediscount->data;
        int level = GetBasicDiscountCustomerLevel(discount);
        if (level == customerlevel)
        {
            int rate = GetBasicDiscountRatio(discount);
            price = AmountMultiplyRatio(&price, rate);
        }
        singlediscount = singlediscount->next;
    }

    // 给收支系统信息
    Time present = GetSystemTime();
    store = AmountMultiply(&price, number);
    Profit *put = NewProfit(&store, "售货", &present);
    AppendProfit(put);
    ProfitSave();

    // 向库存系统确认并修改信息
    LinkedList *inventoryhead = GetInventoryByItemId(itemid);
    LinkedList *latest = inventoryhead;
    LinkedList *nextlatest = latest->next;
    int judge = 1; // 判断是否达到出货数量要求
    while (judge)  // 出货并记录订单直至达到需要的数量
    {
        while (nextlatest != NULL) // 找出生产日期最早的产品
        {
            Time now = GetInventoryEntryProductionTime(latest->data);
            Time com = GetInventoryEntryProductionTime(nextlatest->data);
            int templeft = GetInventoryEntryNumber(nextlatest->data);
            if (CompareTime(&now, &com) > 0 && templeft > 0)
                latest = nextlatest;
            nextlatest = nextlatest->next;
        }
        int left = GetInventoryEntryNumber(latest->data);
        if (left >= number) // 一批货充足
        {
            judge = 0;

            store = AmountMultiply(&price, number);

            int inventoryid = GetInventoryEntryId(latest->data);
            Order *neworder = NewOrder(inventoryid, number, customerid, &present, &store); // 添加订单
            AppendOrder(neworder);
            OrderSave();

            SetInventoryEntryNumber(latest->data, left - number); // 修改库存数量
            InventorySave();
        }
        else // 一批货不足但库存有存货
        {
            store = AmountMultiply(&price, left);

            int inventoryid = GetInventoryEntryId(latest->data);
            Order *neworder = NewOrder(inventoryid, left, customerid, &present, &store); // 添加订单
            AppendOrder(neworder);
            OrderSave();

            SetInventoryEntryNumber(latest->data, 0); // 修改库存数量
            InventorySave();

            number -= left;         // 确认新的所需数量
            latest = inventoryhead; // 重新设定指针位置以便下一次循环
            nextlatest = latest->next;
        }
    }

    // 返回值
    TableRow *row = NewTableRow(); // 空行

    AppendTableRow(row, "customerid");
    AppendTableRow(row, "itemname");
    AppendTableRow(row, "number");
    AppendTableRow(row, "yuan");
    AppendTableRow(row, "jiao");
    AppendTableRow(row, "cent");
    AppendTableRow(row, "time");
    Table *goback = NewTable(row, NULL);

    char *customeridstring = LongLongToString(customerid);
    char *itemname = GetItemName(item);
    char *numberstring = LongLongToString(number);
    int yuan = GetAmountYuan(&price);
    int jiao = GetAmountJiao(&price);
    int cent = GetAmountCent(&price);
    char *yuanstring = LongLongToString(yuan);
    char *jiaostring = LongLongToString(jiao);
    char *centstring = LongLongToString(cent);
    TimeInfo thistime = GetTimeInfo(&present, 1);
    char *timestring = TimeToString(thistime);

    row = NewTableRow();
    AppendTableRow(row, customeridstring);
    AppendTableRow(row, itemname);
    AppendTableRow(row, numberstring);
    AppendTableRow(row, yuanstring);
    AppendTableRow(row, jiaostring);
    AppendTableRow(row, centstring);
    AppendTableRow(row, timestring);
    AppendTable(goback, row);

    free(customeridstring);
    free(numberstring);
    free(yuanstring);
    free(jiaostring);
    free(centstring);
    free(timestring);

    return goback;
}

// 手动售货
Table *AddSpecificOrder(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int itemid = atoi(GetRowItemByColumnName(a, information, "itemid"));
    int customerid = atoi(GetRowItemByColumnName(a, information, "customerid"));
    int number = atoi(GetRowItemByColumnName(a, information, "number"));
    int inventoryid = atoi(GetRowItemByColumnName(a, information, "inventoryid"));

    // 准备数据
    Customer *cus = GetCustomerById(customerid);
    if (cus == NULL)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "不存在该客户");
        return errorreturn;
    }
    int customerlevel = GetCustomerLevel(cus);

    InventoryEntry *thisinventory = GetInventoryById(inventoryid);
    if (thisinventory == NULL)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "不存在该库存条目");
        return errorreturn;
    }

    Time present = GetSystemTime();

    Item *item = GetItemById(itemid);
    if (item == NULL)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "不存在该商品");
        return errorreturn;
    }
    Amount price = GetItemPrice(item);
    price = AmountMultiply(&price, number);

    // 判断
    int remain = GetInventoryEntryNumber(thisinventory);
    if (remain < number)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "库存不足");
        return goback;
    }

    // 计算折扣后金额
    LinkedList *itemdiscount = GetBasicDiscountsByItemId(itemid); // 算折扣
    LinkedList *singlediscount = itemdiscount;
    while (singlediscount != NULL)
    {
        BasicDiscount *discount = singlediscount->data;
        int level = GetBasicDiscountCustomerLevel(discount);
        if (level == customerlevel)
        {
            int rate = GetBasicDiscountRatio(discount);
            price = AmountMultiplyRatio(&price, rate);
        }
        singlediscount = singlediscount->next;
    }

    // 库存充足
    // 修改库存
    SetInventoryEntryNumber(thisinventory, remain - number);
    InventorySave();

    // 添加订单
    Order *neworder = NewOrder(inventoryid, number, customerid, &present, &price);
    AppendOrder(neworder);
    OrderSave();

    // 给收支系统信息
    /**/ Profit *put = NewProfit(&price, "售货", &present);
    AppendProfit(put);
    ProfitSave();

    // 返回值
    TableRow *row = NewTableRow(); // 空行

    AppendTableRow(row, "customerid");
    AppendTableRow(row, "itemname");
    AppendTableRow(row, "number");
    AppendTableRow(row, "yuan");
    AppendTableRow(row, "jiao");
    AppendTableRow(row, "cent");
    AppendTableRow(row, "time");
    AppendTableRow(row, "inventoryid");
    Table *goback = NewTable(row, NULL);

    char *customeridstring = LongLongToString(customerid);
    char *itemname = GetItemName(item);
    char *numberstring = LongLongToString(number);
    int yuan = GetAmountYuan(&price);
    int jiao = GetAmountJiao(&price);
    int cent = GetAmountCent(&price);
    char *yuanstring = LongLongToString(yuan);
    char *jiaostring = LongLongToString(jiao);
    char *centstring = LongLongToString(cent);
    /**/ TimeInfo thistime = GetTimeInfo(&present, 1);
    char *timestring = TimeToString(thistime);
    char *inventoryidstring = LongLongToString(inventoryid);

    row = NewTableRow();
    AppendTableRow(row, customeridstring);
    AppendTableRow(row, itemname);
    AppendTableRow(row, numberstring);
    AppendTableRow(row, yuanstring);
    AppendTableRow(row, jiaostring);
    AppendTableRow(row, centstring);
    AppendTableRow(row, timestring);
    AppendTableRow(row, inventoryidstring);
    AppendTable(goback, row);

    free(customeridstring);
    free(numberstring);
    free(yuanstring);
    free(jiaostring);
    free(centstring);
    free(timestring);
    free(inventoryidstring);

    return goback;
}

// 删除订单
Table *RemoveAnOrder(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int orderid = atoi(GetRowItemByColumnName(a, information, "orderid"));

    Order *thisorder = GetOrderById(orderid);
    if (thisorder == NULL)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "不存在符合条件的订单条目");
        return errorreturn;
    }

    RefundEntry *thisrefund = GetRefundByOrderId(orderid);
    if (thisrefund != NULL)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "该订单不可删除");
        return errorreturn;
    }

    Time presenttime = GetSystemTime();

    // 操作金额和收支
    Amount beforeamount = GetOrderAmount(thisorder);
    beforeamount = AmountMultiply(&beforeamount, -1);
    Profit *newprofit = NewProfit(&beforeamount, "删除订单信息", &presenttime);
    int judge = AppendProfit(newprofit);
    if (judge)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "删除订单失败");
        return errorreturn;
    }
    ProfitSave();

    // 操作数量和库存
    int beforenumber = GetOrderNumber(thisorder);
    int inventoryid = GetOrderInventoryId(thisorder);
    InventoryEntry *thisinventory = GetInventoryById(inventoryid);
    int inventorynumber = GetInventoryEntryNumber(thisinventory);
    SetInventoryEntryNumber(thisinventory, inventorynumber + beforenumber);
    InventorySave();

    // 删除订单条目
    RemoveOrder(thisorder);
    FreeOrder(thisorder);
    OrderSave();

    // 返回值
    TableRow *row = NewTableRow();
    Table *goback = NewTable(row, NULL);
    return goback;
}

// 修改订单
Table *UpdateOrder(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int orderid = atoi(GetRowItemByColumnName(a, information, "orderid"));
    int inventoryid = atoi(GetRowItemByColumnName(a, information, "inventoryid"));
    int number = atoi(GetRowItemByColumnName(a, information, "number"));
    int customerid = atoi(GetRowItemByColumnName(a, information, "customerid"));

    Order *thisorder = GetOrderById(orderid);
    if (thisorder == NULL)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "不存在符合条件的订单条目");
        return errorreturn;
    }

    Time presenttime = GetSystemTime();

    Amount beforeprice = GetOrderAmount(thisorder);
    beforeprice = AmountMultiply(&beforeprice, -1);

    InventoryEntry *newinventory = GetInventoryById(inventoryid);
    if (newinventory == NULL)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "不存在符合条件的库存条目");
        return errorreturn;
    }
    int newinventorynumber = GetInventoryEntryNumber(newinventory);
    int newitemid = GetInventoryEntryItemId(newinventory);
    Item *newitem = GetItemById(newitemid);
    Amount price = GetItemPrice(newitem);
    price = AmountMultiply(&price, number);

    int beforenumber = GetOrderNumber(thisorder);
    int beforeinventoryid = GetOrderInventoryId(thisorder);
    InventoryEntry *beforeinventory = GetInventoryById(beforeinventoryid);
    int beforeinventorynumber = GetInventoryEntryNumber(beforeinventory);

    Customer *newcustomer = GetCustomerById(customerid);
    if (newinventory == NULL)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "不存在符合条件的客户");
        return errorreturn;
    }
    int customerlevel = GetCustomerLevel(newcustomer);

    // 操作金额和收支

    // 给收支系统信息
    Profit *beforeput = NewProfit(&beforeprice, "修改订单1", &presenttime);
    AppendProfit(beforeput);
    ProfitSave();
    // 计算金额
    LinkedList *newitemdiscount = GetBasicDiscountsByItemId(newitemid); // 算折扣
    LinkedList *newsinglediscount = newitemdiscount;
    while (newsinglediscount != NULL)
    {
        BasicDiscount *discount = newsinglediscount->data;
        int level = GetBasicDiscountCustomerLevel(discount);
        if (level == customerlevel)
        {
            int rate = GetBasicDiscountRatio(discount);
            price = AmountMultiplyRatio(&price, rate);
        }
        newsinglediscount = newsinglediscount->next;
    }
    // 给收支系统信息
    Profit *put = NewProfit(&price, "修改订单2", &presenttime);
    AppendProfit(put);
    ProfitSave();
    // 修改金额
    SetOrderAmount(thisorder, &price);

    // 操作数量和库存

    // 给库存系统信息
    SetInventoryEntryNumber(beforeinventory, beforenumber + beforeinventorynumber);
    InventorySave();
    // 给库存系统信息
    SetInventoryEntryNumber(newinventory, newinventorynumber - number);
    InventorySave();
    // 修改数量
    SetOrderNumber(thisorder, number);
    // 修改库存编号
    SetOrderInventoryId(thisorder, inventoryid);

    // 操作时间
    SetOrderTime(thisorder, &presenttime);

    // 操作客户编号
    SetOrderCustomerId(thisorder, customerid);

    // 保存
    OrderSave();

    // 返回值
    TableRow *row = NewTableRow();
    Table *goback = NewTable(row, NULL);
    return goback;
}

// 读取订单
Table *GetAllOrder(Table *a)
{
    // 标题行
    TableRow *row = NewTableRow();

    AppendTableRow(row, "订单编号");
    AppendTableRow(row, "库存编号");
    AppendTableRow(row, "商品编号");
    AppendTableRow(row, "商品名称");
    AppendTableRow(row, "客户编号");
    AppendTableRow(row, "客户姓名");
    AppendTableRow(row, "购买数量");
    AppendTableRow(row, "购买时间");
    AppendTableRow(row, "总价");
    Table *table = NewTable(row, NULL);

    // 循环获取数据并写入行
    LinkedList *orderNow = GetAllOrders();
    if (orderNow == NULL)
    {
        SetTableRemark(table, "无订单条目");
        return table;
    }
    while (orderNow != NULL)
    {
        Order *order = orderNow->data;
        row = NewTableRow();

        // 数据准备
        int orderId = GetOrderId(order);
        int inventoryId = GetOrderInventoryId(order);

        InventoryEntry *entry = GetInventoryById(inventoryId);
        int itemId = GetInventoryEntryItemId(entry);
        Item *item = GetItemById(itemId);
        const char *itemName = GetItemName(item);

        int customerId = GetOrderCustomerId(order);
        Customer *customer = GetCustomerById(customerId);
        const char *customerName = GetCustomerName(customer);

        int number = GetOrderNumber(order);
        Time orderTime = GetOrderTime(order);
        TimeInfo orderTimeInfo = GetTimeInfo(&orderTime, 1);
        Amount amount = GetOrderAmount(order);

        char *orderIdString = LongLongToString(orderId);
        char *inventoryIdString = LongLongToString(inventoryId);
        char *itemIdString = LongLongToString(itemId);
        char *itemNameString = CloneString(itemName);
        char *customerIdString = LongLongToString(customerId);
        char *customerNameString = CloneString(customerName);
        char *numberString = LongLongToString(number);
        char *timeString = TimeToString(orderTimeInfo);
        char *amountString = AmountToString(&amount);

        // 数据添加
        AppendTableRow(row, orderIdString);
        AppendTableRow(row, inventoryIdString);
        AppendTableRow(row, itemIdString);
        AppendTableRow(row, itemNameString);
        AppendTableRow(row, customerIdString);
        AppendTableRow(row, customerNameString);
        AppendTableRow(row, numberString);
        AppendTableRow(row, timeString);
        AppendTableRow(row, amountString);
        AppendTable(table, row);

        free(orderIdString);
        free(inventoryIdString);
        free(itemIdString);
        free(itemNameString);
        free(customerIdString);
        free(customerNameString);
        free(numberString);
        free(timeString);
        free(amountString);

        orderNow = orderNow->next;
    }
    // 返回值
    return table;
}

// 读取单个订单信息
Table *GetSingleOrder(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int orderid = atoi(GetRowItemByColumnName(a, information, "id"));

    Order *singleorder = GetOrderById(orderid);
    if (singleorder == NULL)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "不存在符合条件的订单条目");
        return errorreturn;
    }
    // 标题行
    TableRow *row = NewTableRow();

    AppendTableRow(row, "orderid");
    AppendTableRow(row, "itemname");
    AppendTableRow(row, "customerid");
    AppendTableRow(row, "number");
    AppendTableRow(row, "yuan");
    AppendTableRow(row, "jiao");
    AppendTableRow(row, "cent");
    AppendTableRow(row, "time");
    AppendTableRow(row, "inventoryid");
    Table *goback = NewTable(row, NULL);

    // 数据准备
    row = NewTableRow();

    int customerid = GetOrderCustomerId(singleorder);
    int number = GetOrderNumber(singleorder);
    int inventoryid = GetOrderInventoryId(singleorder);
    InventoryEntry *thisinventory = GetInventoryById(inventoryid);
    int itemid = GetInventoryEntryItemId(thisinventory);
    Item *thisitem = GetItemById(itemid);
    char *itemname = GetItemName(thisitem);
    Time ordertime = GetOrderTime(singleorder);
    /**/ TimeInfo ordertimeinfo = GetTimeInfo(&ordertime, 1);
    Amount price = GetOrderAmount(singleorder);

    char *orderidstring = LongLongToString(orderid);
    char *customeridstring = LongLongToString(customerid);
    char *numberstring = LongLongToString(number);
    int yuan = GetAmountYuan(&price);
    int jiao = GetAmountJiao(&price);
    int cent = GetAmountCent(&price);
    char *yuanstring = LongLongToString(yuan);
    char *jiaostring = LongLongToString(jiao);
    char *centstring = LongLongToString(cent);
    char *timestring = TimeToString(ordertimeinfo);
    char *inventoryidstring = LongLongToString(inventoryid);

    // 数据添加
    AppendTableRow(row, orderidstring);
    AppendTableRow(row, itemname);
    AppendTableRow(row, customeridstring);
    AppendTableRow(row, numberstring);
    AppendTableRow(row, yuanstring);
    AppendTableRow(row, jiaostring);
    AppendTableRow(row, centstring);
    AppendTableRow(row, timestring);
    AppendTableRow(row, inventoryidstring);
    AppendTable(goback, row);

    free(orderidstring);
    free(customeridstring);
    free(numberstring);
    free(yuanstring);
    free(jiaostring);
    free(centstring);
    free(timestring);
    free(inventoryidstring);

    return goback;
}

// 添加折扣
/**/ /*错误判断不够详细*/ /**/
Table *AddDiscount(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int itemid = atoi(GetRowItemByColumnName(a, information, "商品编号"));
    int rate = atoi(GetRowItemByColumnName(a, information, "折扣比率"));
    int customerLevel = atoi(GetRowItemByColumnName(a, information, "客户等级"));
    int year = atoi(GetRowItemByColumnName(a, information, "年"));
    int month = atoi(GetRowItemByColumnName(a, information, "月"));
    int day = atoi(GetRowItemByColumnName(a, information, "日"));
    int hour = atoi(GetRowItemByColumnName(a, information, "时"));
    int minute = atoi(GetRowItemByColumnName(a, information, "分"));
    int second = atoi(GetRowItemByColumnName(a, information, "秒"));

    Time create = NewDateTime(year, month, day, hour, minute, second);
    // 时间信息有误
    if (create.value < 0)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "时间信息有误");
        return goback;
    }
    // 创建折扣并判断是否失败
    BasicDiscount *newdiscount = NewBasicDiscount(itemid, rate, customerLevel, &create);
    if (newdiscount == NULL)
    {
        FreeBasicDiscount(newdiscount);
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "创建折扣失败");
        return goback;
    }

    AppendBasicDiscount(newdiscount);
    BasicDiscountSave();

    return NULL;
}

// 删除折扣
Table *RemoveDiscount(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int discountid = atoi(GetRowItemByColumnName(a, information, "id"));

    BasicDiscount *discount = GetBasicDiscountById(discountid);
    if (discount == NULL)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "不存在满足条件的折扣");
        return goback;
    }

    // 删除折扣
    RemoveBasicDiscount(discount);
    FreeBasicDiscount(discount);
    BasicDiscountSave();

    // 返回值
    TableRow *row = NewTableRow();
    Table *goback = NewTable(row, NULL);
    return goback;
}

// 修改折扣
/**/ /*错误判断不够详细*/ /**/
Table *UpdateDiscount(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int discountid = atoi(GetRowItemByColumnName(a, information, "折扣编号"));
    int itemid = atoi(GetRowItemByColumnName(a, information, "商品编号"));
    int rate = atoi(GetRowItemByColumnName(a, information, "折扣比率"));
    int customerlevel = atoi(GetRowItemByColumnName(a, information, "客户等级"));
    int year = atoi(GetRowItemByColumnName(a, information, "年"));
    int month = atoi(GetRowItemByColumnName(a, information, "月"));
    int day = atoi(GetRowItemByColumnName(a, information, "日"));
    int hour = atoi(GetRowItemByColumnName(a, information, "时"));
    int minute = atoi(GetRowItemByColumnName(a, information, "分"));
    int second = atoi(GetRowItemByColumnName(a, information, "秒"));

    BasicDiscount *discount = GetBasicDiscountById(discountid);
    if (discount == NULL)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "不存在符合条件的折扣");
        return errorreturn;
    }

    // 数据准备
    Time newtime = NewDateTime(year, month, day, hour, minute, second);
    if (newtime.value < 0)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "时间信息有误");
        return goback;
    }
    // 数据修改
    SetBasicDiscountItemId(discount, itemid);
    SetBasicDiscountRatio(discount, rate);
    SetBasicDiscountCustomerLevel(discount, customerlevel);
    SetBasicDiscountDeadline(discount, &newtime);
    BasicDiscountSave();

    // 返回值
    TableRow *row = NewTableRow();
    Table *goback = NewTable(row, NULL);
    return goback;
}

// 清理过期折扣
Table *ClearOutdateDiscount(Table *a)
{
    LinkedList *alldiscountshead = GetAllBasicDiscounts();
    LinkedList *discountnode = alldiscountshead;
    Time present = GetSystemTime();
    while (discountnode != NULL)
    {
        BasicDiscount *discount = discountnode->data;
        Time deadline = GetBasicDiscountDeadline(discount);
        int judge = CompareTime(&present, &deadline);
        if (judge > 0)
        {
            RemoveBasicDiscount(discount);
            FreeBasicDiscount(discount);
            BasicDiscountSave();
        }

        discountnode = discountnode->next;
    }

    // 返回值
    TableRow *row = NewTableRow();
    Table *goback = NewTable(row, NULL);
    return goback;
}

// 查询折扣
Table *GetAllDiscount(Table *a)
{
    TableRow *row = NewTableRow();

    AppendTableRow(row, "折扣编号");
    AppendTableRow(row, "商品编号");
    AppendTableRow(row, "商品名称");
    AppendTableRow(row, "折扣比率");
    AppendTableRow(row, "客户等级");
    AppendTableRow(row, "截止时间");
    Table *table = NewTable(row, NULL);

    LinkedList *discountNow = GetAllBasicDiscounts();
    // 判断
    if (discountNow == NULL)
    {
        SetTableRemark(table, "无折扣");
        return table;
    }

    int count = 0;

    while (discountNow != NULL)
    {
        BasicDiscount *discount = discountNow->data;
        row = NewTableRow();
        count++;

        // 数据准备
        int discountId = GetBasicDiscountId(discount);
        char *discountIdString = LongLongToString(discountId);

        int itemId = GetBasicDiscountItemId(discount);
        char *itemIdString = LongLongToString(itemId);

        Item *item = GetItemById(itemId);
        char *itemName = GetItemName(item);

        int ratio = GetBasicDiscountRatio(discount);
        char *ratioString = LongLongToString(ratio);

        int customerLevel = GetBasicDiscountCustomerLevel(discount);
        char *customerLevelString = LongLongToString(customerLevel);

        Time deadline = GetBasicDiscountDeadline(discount);
        TimeInfo deadLineInfo = GetTimeInfo(&deadline, 1);
        char *deadLineString = TimeToString(deadLineInfo);

        // 添加数据
        AppendTableRow(row, discountIdString);
        AppendTableRow(row, itemIdString);
        AppendTableRow(row, itemName);
        AppendTableRow(row, ratioString);
        AppendTableRow(row, customerLevelString);
        AppendTableRow(row, deadLineString);
        AppendTable(table, row);

        free(discountIdString);
        free(itemIdString);
        free(ratioString);
        free(customerLevelString);
        free(deadLineString);

        discountNow = discountNow->next;
    }

    size_t len = strlen("查询到") + IntegerStringLength(count) + strlen("条信息") + 1;
    char *remark = malloc(len);
    sprintf(remark, "查询到%d条信息", count);
    SetTableRemark(table, remark);
    free(remark);

    // 返回值
    return table;
}

// 查询单个折扣信息
Table *GetSingleDiscount(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int discountid = atoi(GetRowItemByColumnName(a, information, "id"));

    BasicDiscount *discount = GetBasicDiscountById(discountid);
    // 判断
    if (discount == NULL)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "不存在满足条件的折扣");
        return errorreturn;
    }
    // 数据准备
    TableRow *row = NewTableRow();

    AppendTableRow(row, "id");
    AppendTableRow(row, "itemname");
    AppendTableRow(row, "ratio");
    AppendTableRow(row, "customerlevel");
    AppendTableRow(row, "deadline");
    Table *goback = NewTable(row, NULL);

    row = NewTableRow();

    char *discountidstring = LongLongToString(discountid);

    int itemid = GetBasicDiscountItemId(discount);
    Item *thisitem = GetItemById(itemid);
    char *itemname = GetItemName(thisitem);

    int rate = GetBasicDiscountRatio(discount);
    char *ratestring = LongLongToString(rate);

    int customerlevel = GetBasicDiscountCustomerLevel(discount);
    char *customerlevelstring = LongLongToString(customerlevel);

    Time deadline = GetBasicDiscountDeadline(discount);
    TimeInfo deadlineinfo = GetTimeInfo(&deadline, 1);
    char *deadlinestring = TimeToString(deadlineinfo);

    // 添加数据
    AppendTableRow(row, discountidstring);
    AppendTableRow(row, itemname);
    AppendTableRow(row, ratestring);
    AppendTableRow(row, customerlevelstring);
    AppendTableRow(row, deadlinestring);
    AppendTable(goback, row);

    free(discountidstring);
    free(ratestring);
    free(customerlevelstring);
    free(deadlinestring);

    // 返回值
    return goback;
}

// 添加退货
/**/ /*出错信息不够详细*/ /**/
Table *AddRefund(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int orderId = atoi(GetRowItemByColumnName(a, information, "订单编号"));
    char *reason = GetRowItemByColumnName(a, information, "退款原因");
    int yuan = atoi(GetRowItemByColumnName(a, information, "元"));
    int jiao = atoi(GetRowItemByColumnName(a, information, "角"));
    int cent = atoi(GetRowItemByColumnName(a, information, "分"));
    int number = atoi(GetRowItemByColumnName(a, information, "退回数目"));
    char *remark = GetRowItemByColumnName(a, information, "备注");

    // 判断是否可以退货（只能退货一次）
    LinkedList *totalrefundhead = GetAllRefunds();
    LinkedList *refundnode = totalrefundhead;
    while (refundnode != NULL)
    {
        RefundEntry *temprefund = refundnode->data;
        int temporderid = GetRefundEntryOrderId(temprefund);
        if (temporderid == orderId)
        {
            TableRow *row = NewTableRow();
            Table *goback = NewTable(row, "超过退货限制次数(1次)");
            return goback;
        }
    }

    // 数据准备
    Order *thisorder = GetOrderById(orderId);
    int inventoryid = GetOrderInventoryId(thisorder);
    InventoryEntry *thisinventory = GetInventoryById(inventoryid);

    Amount giveback = NewAmount(yuan, jiao, cent);

    Time present = GetSystemTime();

    // 添加退货
    RefundEntry *newrefund = NewRefundEntry(orderId, reason, &present, &giveback, number, remark);
    if (newrefund == NULL)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "退货失败");
        return goback;
    }
    int judge = AppendRefundEntry(newrefund);
    if (judge > 0)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "退货失败");
        return goback;
    }
    RefundEntrySave();

    // 修改库存
    int presentnumber = GetInventoryEntryNumber(thisinventory);
    SetInventoryEntryNumber(thisinventory, presentnumber + number);
    InventorySave();

    // 添加收支
    giveback = AmountMultiply(&giveback, -1);
    Profit *newprofit = NewProfit(&giveback, "退货", &present);
    AppendProfit(newprofit);
    ProfitSave();

    // 返回值
    TableRow *row = NewTableRow();
    Table *goback = NewTable(row, NULL);
    return goback;
}

// 删除退货
Table *RemoveRefund(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int orderid = atoi(GetRowItemByColumnName(a, information, "订单编号"));

    RefundEntry *thisrefund = GetRefundByOrderId(orderid);
    if (thisrefund == NULL)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "不存在符合条件的退款条目");
        return goback;
    }

    Time presenttime = GetSystemTime();
    Order *thisorder = GetOrderById(orderid);

    // 操作金额和收支
    Amount beforeamount = GetRefundEntryAmount(thisrefund);
    Profit *newprofit = NewProfit(&beforeamount, "删除退货信息", &presenttime);
    int judge = AppendProfit(newprofit);
    if (judge)
    {
        TableRow *error = NewTableRow();
        Table *errorreturn = NewTable(error, "删除退货失败");
        return errorreturn;
    }
    ProfitSave();

    // 操作数量和库存
    int beforenumber = GetRefundEntryNumber(thisrefund);
    int inventoryid = GetOrderInventoryId(thisorder);
    InventoryEntry *thisinventory = GetInventoryById(inventoryid);
    int inventorynumber = GetInventoryEntryNumber(thisinventory);
    SetInventoryEntryNumber(thisinventory, inventorynumber - beforenumber);
    InventorySave();

    // 删除退款条目
    RemoveRefundEntry(thisrefund);
    FreeRefundEntry(thisrefund);
    RefundEntrySave();

    // 返回值
    TableRow *row = NewTableRow();
    Table *goback = NewTable(row, NULL);
    return goback;
}

// 修改退货
Table *UpdateRefund(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int orderId = atoi(GetRowItemByColumnName(a, information, "订单编号"));
    const char *reason = GetRowItemByColumnName(a, information, "退款原因");
    int yuan = atoi(GetRowItemByColumnName(a, information, "元"));
    int jiao = atoi(GetRowItemByColumnName(a, information, "角"));
    int cent = atoi(GetRowItemByColumnName(a, information, "分"));
    int number = atoi(GetRowItemByColumnName(a, information, "退回数目"));
    const char *remark = GetRowItemByColumnName(a, information, "备注");

    RefundEntry *refund = GetRefundByOrderId(orderId);
    if (refund == NULL)
    {
        return NewTable(NULL, "不存在符合条件的退款条目");
    }

    Time timeNow = GetSystemTime();
    Order *order = GetOrderById(orderId);

    // 操作金额和收支
    Amount newAmount = NewAmount(yuan, jiao, cent);
    Amount oldAmount = GetRefundEntryAmount(refund);
    oldAmount = AmountMultiply(&oldAmount, -1);
    Amount deltaAmount = AmountAdd(&oldAmount, &newAmount);

    Profit *profit = NewProfit(&deltaAmount, "修改退货信息", &timeNow);
    int judge = AppendProfit(profit);
    if (judge)
    {
        return NewTable(NULL, "修改退货失败: 未能写入收支");
    }
    ProfitSave();

    SetRefundEntryAmount(refund, &newAmount);

    // 操作数量和库存
    SetRefundEntryNumber(refund, number);

    int oldNumber = GetRefundEntryNumber(refund);
    int deltaNumber = number - oldNumber;

    int inventoryId = GetOrderInventoryId(order);
    InventoryEntry *inventory = GetInventoryById(inventoryId);
    int inventoryNumber = GetInventoryEntryNumber(inventory);
    SetInventoryEntryNumber(inventory, inventoryNumber + deltaNumber);
    InventorySave();

    // 操作时间
    SetRefundEntryTime(refund, &timeNow);
    // 操作原因
    SetRefundEntryReason(refund, reason);
    // 操作备注
    SetRefundEntryRemark(refund, remark);

    // 保存
    RefundEntrySave();

    // 返回值
    return NULL;
}

// 查询所有退货
Table *GetAllRefund(Table *a)
{
    TableRow *row = NewTableRow();

    AppendTableRow(row, "订单编号");
    AppendTableRow(row, "退款原因");
    AppendTableRow(row, "退款时间");
    AppendTableRow(row, "退款金额");
    AppendTableRow(row, "退回数目");
    AppendTableRow(row, "备注");
    Table *table = NewTable(row, NULL);

    LinkedList *refundNow = GetAllRefunds();
    if (refundNow == NULL)
    {
        SetTableRemark(table, "无退款条目");
        return table;
    }

    while (refundNow != NULL)
    {
        RefundEntry *entry = refundNow->data;
        row = NewTableRow();

        // 数据准备
        int orderId = GetRefundEntryOrderId(entry);
        char *orderIdString = LongLongToString(orderId);

        const char *reason = GetRefundEntryReason(entry);
        char *reasonString = CloneString(reason);

        Time refundTime = GetRefundEntryTime(entry);
        TimeInfo refundTimeInfo = GetTimeInfo(&refundTime, 1);
        char *refundTimeString = TimeToString(refundTimeInfo);

        Amount price = GetRefundEntryAmount(entry);
        char *priceString = AmountToString(&price);

        int number = GetRefundEntryNumber(entry);
        char *numberString = LongLongToString(number);

        const char *remark = GetRefundEntryRemark(entry);
        char *remarkString = CloneString(remark);

        // 添加数据
        AppendTableRow(row, orderIdString);
        AppendTableRow(row, reasonString);
        AppendTableRow(row, refundTimeString);
        AppendTableRow(row, priceString);
        AppendTableRow(row, numberString);
        AppendTableRow(row, remarkString);
        AppendTable(table, row);

        free(orderIdString);
        free(reasonString);
        free(refundTimeString);
        free(priceString);
        free(numberString);
        free(remarkString);

        refundNow = refundNow->next;
    }
    // 返回值
    return table;
}

// 查询单个退货
/**/ /*出错判断不够详细*/ /**/
Table *GetSingleRefund(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int orderId = atoi(GetRowItemByColumnName(a, information, "订单编号"));

    // 标题行
    TableRow *row = NewTableRow();

    AppendTableRow(row, "订单编号");
    AppendTableRow(row, "退款原因");
    AppendTableRow(row, "退款时间");
    AppendTableRow(row, "退款金额");
    AppendTableRow(row, "退回数目");
    AppendTableRow(row, "备注");
    Table *table = NewTable(row, NULL);

    // 数据准备
    RefundEntry *entry = GetRefundByOrderId(orderId);
    if (entry == NULL)
    {
        SetTableRemark(table, "不存在符合条件的退款条目");
        return table;
    }

    char *orderIdString = LongLongToString(orderId);

    const char *reason = GetRefundEntryReason(entry);
    char *reasonString = CloneString(reason);

    Time refundTime = GetRefundEntryTime(entry);
    char *refundTimeString = TimeToString(GetTimeInfo(&refundTime, 1));

    Amount amount = GetRefundEntryAmount(entry);
    char *amountString = AmountToString(&amount);

    int number = GetRefundEntryNumber(entry);
    char *numberString = LongLongToString(number);

    const char *remark = GetRefundEntryRemark(entry);
    char *remarkString = CloneString(remark);

    // 添加数据
    row = NewTableRow();
    AppendTableRow(row, orderIdString);
    AppendTableRow(row, reasonString);
    AppendTableRow(row, refundTimeString);
    AppendTableRow(row, amountString);
    AppendTableRow(row, numberString);
    AppendTableRow(row, remarkString);
    AppendTable(table, row);

    free(orderIdString);
    free(reasonString);
    free(refundTimeString);
    free(amountString);
    free(numberString);
    free(remarkString);

    // 返回值
    return table;
}
