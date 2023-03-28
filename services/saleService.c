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
#include <string.h>
#include <stdio.h>

// 自动售货
Table *AddOrder(Table *a)
{
    //整合的根据日期更新折扣
    LinkedList *discountNow = GetAllBasicDiscounts();
    Time nowtime = GetSystemTime();
    while (discountNow != NULL)
    {
        BasicDiscount *discount = discountNow->data;
        Time deadline = GetBasicDiscountDeadline(discount);
        int judge = CompareTime(&nowtime, &deadline);
        if (judge > 0)
        {
            RemoveBasicDiscount(discount);
            FreeBasicDiscount(discount);
            BasicDiscountSave();
        }

        discountNow = discountNow->next;
    }

    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int itemid = atoi(GetRowItemByColumnName(a, information, "商品编号"));
    int customerid = atoi(GetRowItemByColumnName(a, information, "客户编号"));
    int number = atoi(GetRowItemByColumnName(a, information, "购买数量"));
    // 确认客户等级和所需金额
    Customer *cus = GetCustomerById(customerid);
    if (cus == NULL)
    {
        return NewTable(NULL, "不存在该客户");
    }
    int customerlevel = GetCustomerLevel(cus);

    Item *item = GetItemById(itemid);
    if (item == NULL)
    {
        return NewTable(NULL, "不存在该商品");
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
    int totalnum = 0;

    //判断库存是否充足
    while (latest != NULL)
    {
        totalnum += GetInventoryEntryNumber(latest->data);
        latest = latest->next;
    }
    if (totalnum < number)
    {
        return NewTable(NULL, "没有足够的库存");
    }
    latest = inventoryhead;

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

    return NULL;
}

// 删除订单
Table *RemoveAnOrder(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int orderId = atoi(GetRowItemByColumnName(a, information, "订单编号"));

    Order *order = GetOrderById(orderId);
    if (order == NULL)
    {
        return NewTable(NULL, "不存在符合条件的订单条目");
    }

    RefundEntry *thisrefund = GetRefundByOrderId(orderId);
    if (thisrefund != NULL)
    {
        return NewTable(NULL, "该订单不可删除");
    }

    Time systemTime = GetSystemTime();

    // 操作金额和收支
    Amount amount = GetOrderAmount(order);
    amount = AmountMultiply(&amount, -1);
    Profit *newProfit = NewProfit(&amount, "删除订单信息", &systemTime);
    int judge = AppendProfit(newProfit);
    if (judge)
    {
        return NewTable(NULL, "删除订单失败");
    }
    ProfitSave();

    // 操作数量和库存
    int number = GetOrderNumber(order);
    int inventoryId = GetOrderInventoryId(order);
    InventoryEntry *inventory = GetInventoryById(inventoryId);
    int inventoryEntryNumber = GetInventoryEntryNumber(inventory);
    SetInventoryEntryNumber(inventory, inventoryEntryNumber + number);
    InventorySave();

    // 删除订单条目
    RemoveOrder(order);
    FreeOrder(order);
    OrderSave();

    return NULL;
}

// 修改订单
Table *UpdateOrder(Table *a)
{
    //整合的根据日期更新折扣
    LinkedList *discountNow = GetAllBasicDiscounts();
    Time present = GetSystemTime();
    while (discountNow != NULL)
    {
        BasicDiscount *discount = discountNow->data;
        Time deadline = GetBasicDiscountDeadline(discount);
        int judge = CompareTime(&present, &deadline);
        if (judge > 0)
        {
            RemoveBasicDiscount(discount);
            FreeBasicDiscount(discount);
            BasicDiscountSave();
        }

        discountNow = discountNow->next;
    }

    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int orderId = atoi(GetRowItemByColumnName(a, information, "订单编号"));
    int inventoryId = atoi(GetRowItemByColumnName(a, information, "库存编号"));
    int number = atoi(GetRowItemByColumnName(a, information, "数量"));
    int customerId = atoi(GetRowItemByColumnName(a, information, "客户编号"));

    //判断该订单是否可以修改
    RefundEntry *thisrefund = GetRefundByOrderId(orderId);
    if (thisrefund != NULL)
    {
        return NewTable(NULL, "该订单不可删除");
    }

    Order *order = GetOrderById(orderId);
    if (order == NULL)
    {
        return NewTable(NULL, "不存在符合条件的订单条目");
    }

    Time systemTime = GetSystemTime();

    Amount oldPrice = GetOrderAmount(order);
    oldPrice = AmountMultiply(&oldPrice, -1);

    InventoryEntry *inventory = GetInventoryById(inventoryId);
    if (inventory == NULL)
    {
        return NewTable(NULL, "不存在符合条件的库存条目");
    }

    int newNumber = GetInventoryEntryNumber(inventory);
    int newItemId = GetInventoryEntryItemId(inventory);

    //判断库存是否充足
    LinkedList *inventoryhead = GetInventoryByItemId(newItemId);
    LinkedList *latest = inventoryhead;
    int totalnum = 0;
    while (latest != NULL)
    {
        totalnum += GetInventoryEntryNumber(latest->data);
        latest = latest->next;
    }
    if (totalnum < number)
    {
        return NewTable(NULL, "没有足够的库存");
    }

    Item *newItem = GetItemById(newItemId);
    Amount price = GetItemPrice(newItem);
    price = AmountMultiply(&price, number);

    int oldNumber = GetOrderNumber(order);
    int oldInventoryId = GetOrderInventoryId(order);
    InventoryEntry *oldInventory = GetInventoryById(oldInventoryId);
    int oldInventoryNumber = GetInventoryEntryNumber(oldInventory);

    Customer *newCustomer = GetCustomerById(customerId);
    if (inventory == NULL)
    {
        return NewTable(NULL, "不存在符合条件的客户");
    }
    int customerLevel = GetCustomerLevel(newCustomer);

    // 操作金额和收支

    // 给收支系统信息
    Profit *oldProfit = NewProfit(&oldPrice, "修改订单1", &systemTime);
    AppendProfit(oldProfit);
    ProfitSave();
    // 计算金额
    LinkedList *newItemDiscount = GetBasicDiscountsByItemId(newItemId); // 算折扣
    while (newItemDiscount != NULL)
    {
        BasicDiscount *discount = newItemDiscount->data;
        int level = GetBasicDiscountCustomerLevel(discount);
        if (level == customerLevel)
        {
            int rate = GetBasicDiscountRatio(discount);
            price = AmountMultiplyRatio(&price, rate);
        }
        newItemDiscount = newItemDiscount->next;
    }
    // 给收支系统信息
    Profit *put = NewProfit(&price, "修改订单2", &systemTime);
    AppendProfit(put);
    ProfitSave();
    // 修改金额
    SetOrderAmount(order, &price);

    // 操作数量和库存

    // 给库存系统信息
    SetInventoryEntryNumber(oldInventory, oldNumber + oldInventoryNumber);
    InventorySave();
    // 给库存系统信息
    SetInventoryEntryNumber(inventory, newNumber - number);
    InventorySave();
    // 修改数量
    SetOrderNumber(order, number);
    // 修改库存编号
    SetOrderInventoryId(order, inventoryId);

    // 操作时间
    SetOrderTime(order, &systemTime);

    // 操作客户编号
    SetOrderCustomerId(order, customerId);

    // 保存
    OrderSave();

    return NULL;
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

    int count = 0;

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

    size_t len = strlen("查询到") + IntegerStringLength(count) + strlen("条信息") + 1;
    char *remark = malloc(len);
    sprintf(remark, "查询到%d条信息", count);
    SetTableRemark(table, remark);
    free(remark);

    // 返回值
    return table;
}

// 添加折扣
/**/ /*错误判断不够详细*/ /**/
Table *AddDiscount(Table *a)
{
    //整合的根据日期更新折扣
    LinkedList *discountNow = GetAllBasicDiscounts();
    Time present = GetSystemTime();
    while (discountNow != NULL)
    {
        BasicDiscount *discount = discountNow->data;
        Time deadline = GetBasicDiscountDeadline(discount);
        int judge = CompareTime(&present, &deadline);
        if (judge > 0)
        {
            RemoveBasicDiscount(discount);
            FreeBasicDiscount(discount);
            BasicDiscountSave();
        }

        discountNow = discountNow->next;
    }

    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int itemId = atoi(GetRowItemByColumnName(a, information, "商品编号"));
    int rate = atoi(GetRowItemByColumnName(a, information, "折扣比率"));
    int customerLevel = atoi(GetRowItemByColumnName(a, information, "客户等级"));
    int year = atoi(GetRowItemByColumnName(a, information, "年"));
    int month = atoi(GetRowItemByColumnName(a, information, "月"));
    int day = atoi(GetRowItemByColumnName(a, information, "日"));
    int hour = atoi(GetRowItemByColumnName(a, information, "时"));
    int minute = atoi(GetRowItemByColumnName(a, information, "分"));
    int second = atoi(GetRowItemByColumnName(a, information, "秒"));

    Time create = NewDateTime(year, month, day, hour, minute, second);
    Time nowtime = GetSystemTime();
    Item *thisitem = GetItemById(itemId);
    if (thisitem == NULL)
    {
        return NewTable(NULL, "该商品不存在");
    }
    // 时间信息有误
    if (CompareTime(&nowtime, &create) > 0)
    {
        return NewTable(NULL, "时间信息有误");
    }

    if (GetItemIsEnabled(thisitem) == 0)
    {
        return NewTable(NULL, "该商品不可售");
    }

    if (rate <= 0 || rate >= 100)
    {
        return NewTable(NULL, "折扣比率应大于0且小于100");
    }
    // 创建折扣并判断是否失败
    BasicDiscount *newBasicDiscount = NewBasicDiscount(itemId, rate, customerLevel, &create);
    if (newBasicDiscount == NULL)
    {
        FreeBasicDiscount(newBasicDiscount);
        return NewTable(NULL, "创建折扣失败");
    }

    AppendBasicDiscount(newBasicDiscount);
    BasicDiscountSave();

    return NULL;
}

// 删除折扣
Table *RemoveDiscount(Table *a)
{
    //整合的根据日期更新折扣
    LinkedList *discountNow = GetAllBasicDiscounts();
    Time present = GetSystemTime();
    while (discountNow != NULL)
    {
        BasicDiscount *discount = discountNow->data;
        Time deadline = GetBasicDiscountDeadline(discount);
        int judge = CompareTime(&present, &deadline);
        if (judge > 0)
        {
            RemoveBasicDiscount(discount);
            FreeBasicDiscount(discount);
            BasicDiscountSave();
        }

        discountNow = discountNow->next;
    }

    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int discountId = atoi(GetRowItemByColumnName(a, information, "折扣编号"));

    BasicDiscount *discount = GetBasicDiscountById(discountId);
    if (discount == NULL)
    {
        return NewTable(NULL, "不存在满足条件的折扣");
    }

    // 删除折扣
    RemoveBasicDiscount(discount);
    FreeBasicDiscount(discount);
    BasicDiscountSave();

    return NULL;
}

// 修改折扣
/**/ /*错误判断不够详细*/ /**/
Table *UpdateDiscount(Table *a)
{
    //整合的根据日期更新折扣
    LinkedList *discountNow = GetAllBasicDiscounts();
    Time present = GetSystemTime();
    while (discountNow != NULL)
    {
        BasicDiscount *discount = discountNow->data;
        Time deadline = GetBasicDiscountDeadline(discount);
        int judge = CompareTime(&present, &deadline);
        if (judge > 0)
        {
            RemoveBasicDiscount(discount);
            FreeBasicDiscount(discount);
            BasicDiscountSave();
        }

        discountNow = discountNow->next;
    }

    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int discountId = atoi(GetRowItemByColumnName(a, information, "折扣编号"));
    int itemId = atoi(GetRowItemByColumnName(a, information, "商品编号"));
    int rate = atoi(GetRowItemByColumnName(a, information, "折扣比率"));
    int customerLevel = atoi(GetRowItemByColumnName(a, information, "客户等级"));
    int year = atoi(GetRowItemByColumnName(a, information, "年"));
    int month = atoi(GetRowItemByColumnName(a, information, "月"));
    int day = atoi(GetRowItemByColumnName(a, information, "日"));
    int hour = atoi(GetRowItemByColumnName(a, information, "时"));
    int minute = atoi(GetRowItemByColumnName(a, information, "分"));
    int second = atoi(GetRowItemByColumnName(a, information, "秒"));

    BasicDiscount *discount = GetBasicDiscountById(discountId);
    if (discount == NULL)
    {
        return NewTable(NULL, "不存在符合条件的折扣");
    }

    // 数据准备
    Time newTime = NewDateTime(year, month, day, hour, minute, second);
    if (newTime.value < 0)
    {
        return NewTable(NULL, "时间信息有误");
    }
    // 数据修改
    SetBasicDiscountItemId(discount, itemId);
    SetBasicDiscountRatio(discount, rate);
    SetBasicDiscountCustomerLevel(discount, customerLevel);
    SetBasicDiscountDeadline(discount, &newTime);
    BasicDiscountSave();

    // 返回值
    return NULL;
}

// 清理过期折扣
Table *ClearOutdateDiscount(Table *a)
{
    LinkedList *discountNow = GetAllBasicDiscounts();
    Time present = GetSystemTime();
    while (discountNow != NULL)
    {
        BasicDiscount *discount = discountNow->data;
        Time deadline = GetBasicDiscountDeadline(discount);
        int judge = CompareTime(&present, &deadline);
        if (judge > 0)
        {
            RemoveBasicDiscount(discount);
            FreeBasicDiscount(discount);
            BasicDiscountSave();
        }

        discountNow = discountNow->next;
    }

    return NULL;
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
    LinkedList *refundNow = GetAllRefunds();
    while (refundNow != NULL)
    {
        RefundEntry *refund = refundNow->data;
        int tempOrderId = GetRefundEntryOrderId(refund);
        if (tempOrderId == orderId)
        {
            return NewTable(NULL, "超过退货限制次数(1次)");
        }
        refundNow = refundNow->next;
    }

    // 数据准备
    Order *order = GetOrderById(orderId);
    int inventoryId = GetOrderInventoryId(order);
    InventoryEntry *inventory = GetInventoryById(inventoryId);

    Amount giveback = NewAmount(yuan, jiao, cent);

    Time present = GetSystemTime();

    // 添加退货
    RefundEntry *refund = NewRefundEntry(orderId, reason, &present, &giveback, number, remark);
    if (refund == NULL)
    {
        return NewTable(NULL, "退货失败");
    }
    int judge = AppendRefundEntry(refund);
    if (judge > 0)
    {
        return NewTable(NULL, "退货失败");
    }
    RefundEntrySave();

    // 修改库存
    int inventoryNumber = GetInventoryEntryNumber(inventory);
    SetInventoryEntryNumber(inventory, inventoryNumber + number);
    InventorySave();

    // 添加收支
    giveback = AmountMultiply(&giveback, -1);
    Profit *newProfit = NewProfit(&giveback, "退货", &present);
    AppendProfit(newProfit);
    ProfitSave();

    return NULL;
}

// 删除退货
Table *RemoveRefund(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int orderId = atoi(GetRowItemByColumnName(a, information, "订单编号"));

    RefundEntry *refund = GetRefundByOrderId(orderId);
    if (refund == NULL)
    {
        return NewTable(NULL, "不存在符合条件的退款条目");
    }

    Time systemTime = GetSystemTime();
    Order *order = GetOrderById(orderId);

    // 操作金额和收支
    Amount oldAmount = GetRefundEntryAmount(refund);
    Profit *newProfit = NewProfit(&oldAmount, "删除退货信息", &systemTime);
    int judge = AppendProfit(newProfit);
    if (judge)
    {
        return NewTable(NULL, "删除退货失败");
    }
    ProfitSave();

    // 操作数量和库存
    int oldNumber = GetRefundEntryNumber(refund);
    int inventoryId = GetOrderInventoryId(order);
    InventoryEntry *inventory = GetInventoryById(inventoryId);
    int number = GetInventoryEntryNumber(inventory);
    SetInventoryEntryNumber(inventory, number - oldNumber);
    InventorySave();

    // 删除退款条目
    RemoveRefundEntry(refund);
    FreeRefundEntry(refund);
    RefundEntrySave();

    return NULL;
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

    int count = 0;
    while (refundNow != NULL)
    {
        RefundEntry *entry = refundNow->data;
        row = NewTableRow();
        count++;

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

    size_t len = strlen("查询到") + IntegerStringLength(count) + strlen("条信息") + 1;
    char *remark = malloc(len);
    sprintf(remark, "查询到%d条信息", count);
    SetTableRemark(table, remark);
    free(remark);
    // 返回值
    return table;
}