#include "design/inventoryService.h"
#include "design/amount.h"
#include "design/basicDiscount.h"
#include "design/inventory.h"
#include "design/item.h"
#include "design/lossEntry.h"
#include "design/order.h"
#include "design/profit.h"
#include "design/utils.h"
#include <malloc.h>
#include <string.h>

// 转换函数 将字符1到9  转换为数字1到9
int change(const char *q)
{
    int a = 0;
    for (int i = 0; q[i] != '\0'; i++)
    {
        a = a * 10 + q[i] - '0';
    }
    return a;
}

// 从表格中获取货存系统所需要的信息
void GetInventoryInformationFromTable(Table *input, int *itemId, int *number, int *y1, int *m1, int *d1, int *h1,
        int *min1, int *s1, int *y2, int *m2, int *d2, int *h2, int *min2, int *s2,
        int *yuan, int *jiao, int *cent)
{

    TableRow *row = GetRowByIndex(input, 1);

    *itemId = change(GetRowItemByColumnName(input, row, "商品编号"));
    *number = change(GetRowItemByColumnName(input, row, "数量"));

    *y1 = change(GetRowItemByColumnName(input, row, "年1"));
    *m1 = change(GetRowItemByColumnName(input, row, "月1"));
    *d1 = change(GetRowItemByColumnName(input, row, "日1"));
    *h1 = change(GetRowItemByColumnName(input, row, "时1"));
    *min1 = change(GetRowItemByColumnName(input, row, "分1"));
    *s1 = change(GetRowItemByColumnName(input, row, "秒1"));

    *y2 = change(GetRowItemByColumnName(input, row, "年2"));
    *m2 = change(GetRowItemByColumnName(input, row, "月2"));
    *d2 = change(GetRowItemByColumnName(input, row, "日2"));
    *h2 = change(GetRowItemByColumnName(input, row, "时2"));
    *min2 = change(GetRowItemByColumnName(input, row, "分2"));
    *s2 = change(GetRowItemByColumnName(input, row, "秒2"));

    *yuan = change(GetRowItemByColumnName(input, row, "元"));
    *jiao = change(GetRowItemByColumnName(input, row, "角"));
    *cent = change(GetRowItemByColumnName(input, row, "分"));
}

// 向货存链表中添加一批货物
Table *AddInventory(Table *input)
{
    int itemId = 0, number = 0, y1 = 0, m1 = 0, d1 = 0, h1 = 0, min1 = 0, s1 = 0, y2 = 0, m2 = 0, d2 = 0, h2 = 0,
            min2 = 0, s2 = 0, yuan = 0, jiao = 0, cent = 0;
    GetInventoryInformationFromTable(input, &itemId, &number, &y1, &m1, &d1, &h1, &min1, &s1, &y2, &m2, &d2, &h2, &min2,
            &s2, &yuan, &jiao, &cent);
    // 将入库时间信息转化
    Time inboundTime = NewDateTime(y1, m1, d1, h1, min1, s1);
    // 将生产日期信息转化
    Time productionTime = NewDateTime(y2, m2, d2, h2, min2, s2);
    // 将进价信息转化
    Amount unitPrice = NewAmount(yuan, jiao, cent);

    Time NowTime = GetSystemTime();
    Table *table1 = NULL;
    // 用于判断是否有该商品信息
    if (GetItemById(itemId) != NULL)
    {
        // 判断输入的入库时间是否大于现在的时间
        if (CompareTime(&NowTime, &inboundTime) >= 0)
        {
            // 判断输入的生产日期时间是否大于现在时间
            if (CompareTime(&NowTime, &productionTime) >= 0)
            {
                // 将获取的货存信息转化为结构体并添加到货存链表中
                int result1 = AppendInventoryEntry(NewInventoryEntry(itemId, number, &inboundTime, &productionTime,
                        &unitPrice)); // result1 用于存储是否插入成功
                if (result1 == 0)
                {
                    InventorySave(); // 将添加结果进行保存

                    // 将该批货物所花的钱计入profit结构体中

                    // 获取系统时间
                    Time t1 = GetSystemTime();
                    // 计算出该批货物一共花费的钱
                    Amount pay = AmountMultiply(&unitPrice, -number);
                    // 通过商品Id得到item
                    Item *item1 = GetItemById(itemId);
                    // 获取商品的名称
                    const char *name = GetItemName(item1);
                    // 事项
                    char matter[500] = "进货 商品名称:";
                    strcat(matter, name);
                    // 创建一个新的profit结构体
                    Profit *profit1 = NewProfit(&pay, matter, &t1);
                    // 将新的profit结构体插入到链表中 并将插入结果返回到result2中
                    int result2 = AppendProfit(profit1);
                    // 储存账目是否添加成功
                    if (result2 == 0)
                    {
                        table1 = NewTable(NULL, "货存添加成功且账目添加完成");
                        ProfitSave();
                    }
                    else
                        table1 = NewTable(NULL, " 货存添加成功但账目添加失败");
                }
                else
                    table1 = NewTable(NULL, "货存添加失败");
            }
            else
                table1 = NewTable(NULL, "输入的生产日期时间有误");
        }
        else
            table1 = NewTable(NULL, "输入的入库时间有误");
    }
    else
        table1 = NewTable(NULL, "输入的商品编号有误 未查找到相关的商品信息");
    return table1;
}

// 更新货存链表 并将过期的货物放入货损链表中
Table *UpdateInventory(Table *input)
{
    // 获取货存管理中的全部链表
    LinkedList *head = GetAllInventory();
    // 用于判断是否发现货损
    int hasLoss = 0;
    // 用于记录发现货损时的表格
    Table *tableLoss;
    // 用于记录未发现货损时的表格
    Table *tableOk;

    tableOk = NewTable(NULL, "本次更新中货物均完好 未发现货损");

    // 创建货损表头所需要的元素
    TableRow *row = NewTableRow();
    AppendTableRow(row, "货损货物的Id");
    AppendTableRow(row, "货损货物的商品名称");
    AppendTableRow(row, "货损的原因");
    AppendTableRow(row, "货损的日期");

    tableLoss = NewTable(row, "以下记录的货损均已自动进入货损系统中");

    while (head != NULL)
    {
        // 获取链表中的相关元素
        InventoryEntry *entry = head->data;
        // 获取系统的时间
        Time timeNow = GetSystemTime();
        // 获取该批货物的生产日期
        Time timeProduction = GetInventoryEntryProductionTime(entry);

        // 获取该批货物所对应商品的保质期

        // 获取商品id
        int itemid = GetInventoryEntryItemId(entry);
        // 通过商品Id得到item
        Item *item = GetItemById(itemid);
        // 得到保质期
        Time timeShelf = GetItemShelfLife(item);
        // 计算该批货物的到期时间
        Time timeOutOfDate = AddTime(&timeProduction, &timeShelf);
        // 比较系统时间和到期时间
        int result = CompareTime(&timeNow, &timeOutOfDate);

        if (result < 0)
        {
            hasLoss = 1;
            // 用于记录表格中相对应的信息
            row = NewTableRow();

            int id = GetInventoryEntryId(entry);
            // 将id信息插入
            free(AppendTableRow(row, LongLongToString(id)));
            // 将商品名称插入
            AppendTableRow(row, GetItemName(item));
            // 将货损原因插入
            AppendTableRow(row, "过期");
            // 将时间插入
            free(AppendTableRow(row, TimeToString(GetTimeInfo(&timeNow, 1))));
            // 将创建好的tablerow插入到表格中
            AppendTable(tableLoss, row);
            // 将货损的这批货放入货损管理系统中
            AppendLossEntry(NewLossEntry(id, GetInventoryEntryNumber(entry), "过期", &timeNow));
            // 将该批货物删除
            SetInventoryEntryNumber(entry, 0);
        }
        head = head->next;
    }

    InventorySave();
    LossEntrySave();

    if (hasLoss == 1)
    {
        FreeTable(tableOk);
        return tableLoss;
    }
    else
    {
        FreeTable(tableLoss);
        return tableOk;
    }
}

// 展示所有的缺货状态的货物
Table *ShowLackInventory(Table *input)
{
    // 创建表格 该表格用于有货存过少时使用
    Table *tableLack;
    // 创建表格 该表格用于没有货存过少时使用
    Table *tableOk;
    // 用于存放是否有货存过少的情况发生 并记录有多少条信息
    int lackCount = 0;
    // 创建表头 表头信息如下所示
    TableRow *row = NewTableRow();
    AppendTableRow(row, "商品编号");
    AppendTableRow(row, "商品名称");
    AppendTableRow(row, "剩余数量");
    // 根据有没有缺货的货存返回不同的表格
    tableLack = NewTable(row, NULL);
    tableOk = NewTable(NULL, "全部商品库存都很充足 未出现缺货现象");
    // 获取全部的商品链表
    LinkedList *itemNow = GetAllItems();
    while (itemNow != NULL)
    {
        // 获取商品编号
        int itemId = GetItemId(itemNow->data);
        // 用于存储同一个商品剩余的货存数量
        int remainCount = 0;
        // 由商品名称获得 其所对应的全部链表
        LinkedList *inventoryNow = GetInventoryByItemId(itemId);
        // 计算该商品的剩余库存的数量
        while (inventoryNow != NULL)
        {
            remainCount += GetInventoryEntryNumber(inventoryNow->data);
            inventoryNow = inventoryNow->next;
        }
        // 获取所有的订单 便于计算销量
        LinkedList *orderNow = GetAllOrders();
        // 用于统计近七天该商品的销售数量  如开业不满七天 则有多少天的就计算多少天的
        int saleCount = 0;
        while (orderNow != NULL)
        {
            // 获取货存编号
            int id = GetOrderInventoryId(orderNow->data);
            // 获取订单的商品编号
            int orderItemId = GetInventoryEntryItemId(GetInventoryById(id));

            if (itemId == orderItemId)
            {
                // 获得所需要的各类时间
                Time OrderTime = GetOrderTime(orderNow->data);
                Time NowTime = GetSystemTime();
                Time IntervalTime = NewTimeSpan(7, 0);
                Time time1 = AddTime(&OrderTime, &IntervalTime); //
                // 判断时间是否在近七天内
                if (CompareTime(&time1, &NowTime))
                {
                    // 统计近七天该商品的总销量
                    saleCount += GetOrderNumber(orderNow->data);
                }
            }
            orderNow = orderNow->next;
        }
        // 如果剩余的数量少于近七天的销售额的%20 那么就记为缺货
        if (remainCount < saleCount * 0.2)
        {
            lackCount++; // 缺货条目+1
            row = NewTableRow();
            // 插入商品Id
            free(AppendTableRow(row, LongLongToString(itemId)));
            // 插入商品名称
            AppendTableRow(row, GetItemName(itemNow->data));
            // 插入剩余数量
            free(AppendTableRow(row, LongLongToString(remainCount)));
            // 将这些数据插入到表格中
            AppendTable(tableLack, row);
        }
        itemNow = itemNow->next;
    }

    if (lackCount != 0)
    {
        // 表格备注的具体实现
        char remark[100] = "以下为货存系统中缺货的全部信息 一共记录了";
        char *lackCountString = LongLongToString(lackCount);

        strcat(remark, lackCountString);
        strcat(remark, "条信息");
        // 重新设置备注
        SetTableRemark(tableLack, remark);
        // 释放占用的空间
        free(lackCountString);
        FreeTable(tableOk); //
        return tableLack;
    }
    else
    {
        FreeTable(tableLack);
        return tableOk;
    }
}

// 删除货存系统中的某一批货物的全部信息
Table *DeleteInventoryById(Table *input)
{
    // 用于储存最后的结果
    Table *table;

    // 获取要删除的货物的编号

    TableRow *row = GetRowByIndex(input, 1);
    // 将Id由字符转为整数类型
    int id = change(GetRowItemByColumnName(input, row, "库存编号"));
    // 如果货存Id已经在货损条目里或订单条目里 那就不允许删除
    if (GetOrdersByInventoryId(id) != NULL || GetLossEntriesByInventoryId(id) != NULL)
    {
        table = NewTable(NULL, "删除失败，该库存已存在关联的订单或货损条目");
        return table;
    }

    InventoryEntry *entry = GetInventoryById(id);
    if (entry != NULL)
    {
        // 删除该条目货存
        RemoveInventoryEntry(entry);

        Amount amount = GetInventoryEntryInboundUnitPrice(entry);
        int number = GetInventoryEntryNumber(entry);
        amount = AmountMultiply(&amount, number);
        Time time = GetSystemTime();
        Profit *profit = NewProfit(&amount, "删除库存条目", &time);
        AppendProfit(profit);
        ProfitSave();

        // 释放其所占用的空间
        FreeInventoryEntry(entry);
        table = NULL;
        InventorySave(); // 向文件中保存
    }
    else
        table = NewTable(NULL, "编号输入有误 未查找到相关的货存信息");

    return table;
}

Table *DeleteSingleLossById(Table *input)
{
    Table *table; // 用于储存最后的结果
    // 获取要修改的货物的编号
    TableRow *row = GetRowByIndex(input, 1);
    // 将Id由字符转为整数类型
    int id = change(GetRowItemByColumnName(input, row, "货损编号"));
    LossEntry *entry = GetLossEntryById(id);
    if (entry != NULL)
    {
        // 得到该Id下的货存条目
        InventoryEntry *inventoryEntry = GetInventoryById(GetLossEntryInventoryId(entry));
        // 将货损中的数目重新添加到货存中
        SetInventoryEntryNumber(inventoryEntry, GetInventoryEntryNumber(inventoryEntry) + GetLossEntryNumber(entry));
        // 删除该货损条目
        RemoveLossEntry(entry);
        table = NULL;

        // 向文件中保存
        InventorySave();
        LossEntrySave();
        FreeLossEntry(entry);
    }
    else
        table = NewTable(NULL, "编号输入有误 未查找到相关的货损信息");

    return table;
}

// 修改一批货存的全部信息
Table *ReviseInventory(Table *input)
{
    // 用于储存最后的结果
    Table *table;

    // 获取要修改的货物的编号
    TableRow *row = GetRowByIndex(input, 1);
    // 将Id由字符转为整数类型
    int id = change(GetRowItemByColumnName(input, row, "Id"));
    // 如果货存条目已经在货损内或在销售条目内 那就不允许删除
    if (GetOrdersByInventoryId(id) != NULL || GetLossEntriesByInventoryId(id) != NULL)
    {
        table = NewTable(NULL, "1 修改失败，该库存已存在关联的订单或货损条目");
        return table;
    }

    // 由编号获得该货存信息
    InventoryEntry *entry = GetInventoryById(id);
    if (entry != NULL)
    {
        // 获取表格中相应的信息
        int itemId = 0, number = 0, y1 = 0, m1 = 0, d1 = 0, h1 = 0, min1 = 0, s1 = 0, y2 = 0, m2 = 0, d2 = 0, h2 = 0,
                min2 = 0, s2 = 0, yuan = 0, jiao = 0, cent = 0;
        GetInventoryInformationFromTable(input, &itemId, &number, &y1, &m1, &d1, &h1, &min1, &s1, &y2, &m2, &d2, &h2,
                &min2, &s2, &yuan, &jiao, &cent);

        // 将入库时间信息转化
        Time inboundTime = NewDateTime(y1, m1, d1, h1, min1, s1);
        // 将生产日期信息转化
        Time productionTime = NewDateTime(y2, m2, d2, h2, min2, s2);
        // 将进价信息转化
        Amount unitPrice = NewAmount(yuan, jiao, cent);

        // 判断是否存在相关的商品信息
        if (GetItemById(itemId) != NULL)
        {
            Time NowTime = GetSystemTime();
            // 判断输入的入库时间是否有误
            if (CompareTime(&NowTime, &inboundTime))
            {
                // 判断输入的生产日期是否有误
                if (CompareTime(&NowTime, &productionTime))
                {
                    // 改变商品id
                    SetInventoryEntryItemId(entry, itemId);
                    // 改变货存数量
                    SetInventoryEntryNumber(entry, number);
                    // 改变该批货的入库时间
                    SetInventoryEntryInboundTime(entry, &inboundTime);
                    // 改变该批货的生产日期
                    SetInventoryEntryProductionTime(entry, &productionTime);
                    // 改变该批货物的购入单价
                    SetInventoryEntryInboundUnitPrice(entry, &unitPrice);
                    InventorySave();
                    table = NewTable(NULL, " 0 修改完成");
                }
                else
                    table = NewTable(NULL, "输入的生产日期有误");
            }
            else
                table = NewTable(NULL, "输入的入库时间有误");
        }
        else
            table = NewTable(NULL, "输入的商品编号有误 未查询到相关的商品");
    }
    else
        table = NewTable(NULL, "1 编号输入有误 未查找到相关的货存信息");

    return table;
}

// 添加一个商品
Table *AddItem(Table *input)
{
    // 获取要加入的商品的信息
    TableRow *row = GetRowByIndex(input, 1);

    // 获取该商品的名称
    const char *itemName = GetRowItemByColumnName(input, row, "商品名称");

    // 将字符转化为数字
    int yuan = change(GetRowItemByColumnName(input, row, "元"));
    int jiao = change(GetRowItemByColumnName(input, row, "角"));
    int cent = change(GetRowItemByColumnName(input, row, "分"));
    Amount amount = NewAmount(yuan, jiao, cent);

    // 将字符转化为数字
    int d1 = change(GetRowItemByColumnName(input, row, "天"));
    int h1 = change(GetRowItemByColumnName(input, row, "时"));
    Time shelfLife = NewTimeSpan(d1, h1); // 获取保质期时间

    // 将准备好的信息用于创建商品
    int result = AppendItem(NewItem(itemName, &amount, &shelfLife));
    if (result == 0)
    {
        ItemsSave();
        return NULL;
    }
    return NewTable(NULL, "添加商品失败");
}

// 添加一批货物至货损系统中
Table *AddLossInventory(Table *input)
{
    // 用于统计结果的表格
    Table *table;
    TableRow *row = GetRowByIndex(input, 1);

    // 获取货存Id
    int inventoryId = change(GetRowItemByColumnName(input, row, "库存编号"));
    // 获取货损数量
    int number = change(GetRowItemByColumnName(input, row, "货损数量"));
    // 获取货损原因
    const char *reason = GetRowItemByColumnName(input, row, "货损原因");
    // 获取系统时间
    Time time1 = GetSystemTime();
    // 通过编号获取货存信息 如无 则返回NULL
    InventoryEntry *inventoryEntry = GetInventoryById(inventoryId);
    // 判断是否有相关货存Id的货存
    if (inventoryEntry != NULL)
    {
        // 判断货存数目是否满足货损数目
        if (GetInventoryEntryNumber(inventoryEntry) < number)
        {
            return NewTable(NULL, "货损添加失败：输入的货损数目大于库存数");
        }

        // 存放添加结果
        int result = AppendLossEntry(NewLossEntry(inventoryId, number, reason, &time1));
        // 重新设置货存中的商品数量
        if (result == 0)
        {
            SetInventoryEntryNumber(inventoryEntry, GetInventoryEntryNumber(inventoryEntry) - number);
            InventorySave();
            return NULL;
        }
        else
            table = NewTable(NULL, "输入的货存编号无误但货损添加失败");
    }
    else
        table = NewTable(NULL, "输入的货存编号有误");
    return table;
}

Table *DeleteItemById(Table *input)
{
    // 获取要删除的货物的名字
    TableRow *row = GetRowByIndex(input, 1);

    int id = change(GetRowItemByColumnName(input, row, "商品编号"));

    // 得到要删除的货物的指针
    Item *item = GetItemById(id);

    if (item != NULL)
    {
        if (GetInventoryByItemId(id) != NULL || GetBasicDiscountsByItemId(id) != NULL)
        {
            return NewTable(NULL, "删除失败，存在与该商品关联的优惠或库存信息");
        }
        // 删除该商品信息
        RemoveItem(item);
        // 释放所占用的空间
        FreeItem(item);
        // 向文件中保存
        ItemsSave();
        return NULL;
    }
    else
        return NewTable(NULL, "商品编号有误 未查找到相关商品");
}

// 展示货存系统中的全部信息
Table *ShowInventory(Table *input)
{
    // 用于记录一共有多少批货物
    int inventoryCount = 0;

    // 创建货存信息表格的表头
    TableRow *row = NewTableRow();
    AppendTableRow(row, "库存编号");
    AppendTableRow(row, "商品编号");
    AppendTableRow(row, "商品名称");
    AppendTableRow(row, "数量");
    AppendTableRow(row, "入库时间");
    AppendTableRow(row, "生产日期");
    AppendTableRow(row, "购入单价");
    // 创建好带表头的表格
    Table *table = NewTable(row, NULL);

    // 获取所有货存的表格
    LinkedList *head = GetAllInventory();

    while (head != NULL)
    {

        inventoryCount++;
        // 获取货存信息
        const InventoryEntry *entry = head->data;
        row = NewTableRow();

        // 获取该批货存的ID
        free(AppendTableRow(row, LongLongToString(GetInventoryEntryId(entry))));
        // 获取商品Id
        int itemId = GetInventoryEntryItemId(entry);
        // 获取该批货存的商品编号
        free(AppendTableRow(row, LongLongToString(itemId)));
        // 获取商品名称
        AppendTableRow(row, GetItemName(GetItemById(itemId)));

        // 获取该批货存的数量
        free(AppendTableRow(row, LongLongToString(GetInventoryEntryNumber(entry))));

        // 获取该批货物的入库时间
        Time inboundTime = GetInventoryEntryInboundTime(entry);
        free(AppendTableRow(row, TimeToString(GetTimeInfo(&inboundTime, 1))));

        // 获取该批货物的生产日期
        Time productionTime = GetInventoryEntryProductionTime(entry);
        free(AppendTableRow(row, TimeToString(GetTimeInfo(&productionTime, 1))));

        // 将价格信息字符化
        Amount unitPrice = GetInventoryEntryInboundUnitPrice(entry);
        free(AppendTableRow(row, AmountToString(&unitPrice)));
        AppendTable(table, row);
        head = head->next;
    }

    // 备注的具体实现
    char remark[100] = "以下为货存系统中的全部信息 一共记录了";
    char *inventoryCountString = LongLongToString(inventoryCount);
    strcat(remark, inventoryCountString);
    strcat(remark, "条信息");
    free(inventoryCountString);
    // 重新设置备注
    SetTableRemark(table, remark);
    return table;
}

// 展示商品系统中的全部信息
Table *ShowItem(Table *input)
{
    // 用于记录一共有多少中商品
    int itemCount = 0;

    // 创建表格的表头
    TableRow *row = NewTableRow();
    AppendTableRow(row, "商品编号");
    AppendTableRow(row, "商品名称");
    AppendTableRow(row, "售价");
    AppendTableRow(row, "保质期");

    // 将用于最后结果的表格创建好
    Table *table = NewTable(row, NULL);

    // 获取所有的商品信息
    LinkedList *head = GetAllItems();

    while (head != NULL)
    {
        // 用于记录一共有多少个商品
        itemCount++;
        // 获取相应的商品信息
        const Item *item = head->data;
        row = NewTableRow();

        // 得到商品编号
        free(AppendTableRow(row, LongLongToString(GetItemId(item))));

        // 得到商品名称
        AppendTableRow(row, GetItemName(item));

        // 得到商品的售价
        Amount salePrice = GetItemPrice(item);
        free(AppendTableRow(row, AmountToString(&salePrice)));

        // 得到商品的保质期
        Time shelfLife = GetItemShelfLife(item);
        free(AppendTableRow(row, TimeToString(GetTimeInfo(&shelfLife, 0))));

        AppendTable(table, row);
        head = head->next;
    }

    // 备注的具体实现
    char remark[100] = "以下为商品系统中的全部信息 一共记录了";
    char *itemCountString = LongLongToString(itemCount);
    strcat(remark, itemCountString);
    strcat(remark, "条信息");
    free(itemCountString);
    // 重新设置备注
    SetTableRemark(table, remark);
    return table;
}

Table *ShowLossInventory(Table *input)
{
    // 获取所有的货损信息
    LinkedList *head = GetAllLoss();
    Table *table;

    // 创建好货损信息表头
    TableRow *row = NewTableRow();
    AppendTableRow(row, "货损编号");
    AppendTableRow(row, "库存编号");
    AppendTableRow(row, "货损数量");
    AppendTableRow(row, "货损原因");
    AppendTableRow(row, "损耗时间");

    // 创建好表格
    table = NewTable(row, NULL);
    // 用于记录一共有多少条货损记录
    int lossCount = 0;

    while (head != NULL)
    {
        // 记录有多少条货损信息
        lossCount++;
        LossEntry *entry = head->data;

        row = NewTableRow();
        // 字符化货损Id并插入
        free(AppendTableRow(row, LongLongToString(GetLossEntryId(entry))));
        // 字符化库存Id并插入
        free(AppendTableRow(row, LongLongToString(GetLossEntryInventoryId(entry))));
        // 将货损数量字符化并插入
        free(AppendTableRow(row, LongLongToString(GetLossEntryNumber(entry))));
        // 获取损坏原因
        free(AppendTableRow(row, CloneString(GetLossEntryReason(entry))));
        // 获取进入货损时的时间
        Time time = GetLossEntryTime(entry);
        free(AppendTableRow(row, TimeToString(GetTimeInfo(&time, 1))));

        // 将每次的货损信息记入表格
        AppendTable(table, row);
        head = head->next;
    }

    // 备注的具体实现
    char remark[100] = "以下为货损系统中的全部信息 一共记录了";
    char *lossCountString = LongLongToString(lossCount);
    strcat(remark, lossCountString);
    strcat(remark, "条信息");
    free(lossCountString);
    // 重新设置备注
    SetTableRemark(table, remark);
    return table;
}

TableRow *ShowSingleInventoryByOperation(InventoryEntry *entry)
{
    TableRow *row = NewTableRow();
    // 获取Id信息
    free(AppendTableRow(row, LongLongToString(GetInventoryEntryId(entry))));

    // 获取该批货存的商品编号
    int itemId = GetInventoryEntryItemId(entry);
    free(AppendTableRow(row, LongLongToString(itemId))); // 将itemId信息加入

    // 获取商品名称
    AppendTableRow(row, GetItemName(GetItemById(itemId)));

    // 获取该批货存的数量
    free(AppendTableRow(row, LongLongToString(GetInventoryEntryNumber(entry))));

    // 获取该批货物的入库时间
    Time inboundTime = GetInventoryEntryInboundTime(entry);
    free(AppendTableRow(row, TimeToString(GetTimeInfo(&inboundTime, 1))));

    // 获取该批货物的生产日期
    Time productionTime = GetInventoryEntryProductionTime(entry);
    free(AppendTableRow(row, TimeToString(GetTimeInfo(&productionTime, 1))));

    // 获取该批货物的进价
    Amount unitPrice = GetInventoryEntryInboundUnitPrice(entry);
    free(AppendTableRow(row, AmountToString(&unitPrice)));

    return row;
}

// 查找某一批货物的全部信息 以Id为索引
Table *ShowSingleInventoryById(Table *input)
{
    // 用于存放该批货物的表格
    Table *table;
    TableRow *row = GetRowByIndex(input, 1);
    // 获取表格中相应的信息
    int id = change(GetRowItemByColumnName(input, row, "Id"));
    InventoryEntry *entry = GetInventoryById(id);
    if (entry != NULL)
    {
        // 创建货存信息表格的表头
        row = NewTableRow();
        AppendTableRow(row, "Id");
        AppendTableRow(row, "商品编号");
        AppendTableRow(row, "商品名称");
        AppendTableRow(row, "数量");
        AppendTableRow(row, "入库时间");
        AppendTableRow(row, "生产日期");
        AppendTableRow(row, "购入单价");

        // 创建表格
        table = NewTable(row, "以下为要查找的信息");

        // row = NewTableRow();
        row = ShowSingleInventoryByOperation(entry);
        AppendTable(table, row);
    }
    else
        table = NewTable(NULL, "输入的Id有误 未查找到与该Id相关的货物信息");
    return table;
}

Table *ShowSingleInventoryByItemId(Table *input)
{
    // 用于存放该批货物的表格
    Table *table;
    TableRow *row = GetRowByIndex(input, 1);
    // 获取表格中相应的信息
    LinkedList *head = GetInventoryByItemId(change(GetRowItemByColumnName(input, row, "itemId")));
    if (head != NULL)
    {
        // 创建货存信息表格的表头
        row = NewTableRow();
        AppendTableRow(row, "Id");
        AppendTableRow(row, "商品编号");
        AppendTableRow(row, "商品名称");
        AppendTableRow(row, "数量");
        AppendTableRow(row, "入库时间");
        AppendTableRow(row, "生产日期");
        AppendTableRow(row, "购入单价");

        // 用于记录有多少货存
        int inventoryCount = 0;
        table = NewTable(row, NULL);
        while (head != NULL)
        {
            // 记录货存数量
            inventoryCount++;
            // 获取货存信息
            InventoryEntry *entry = head->data;
            row = ShowSingleInventoryByOperation(entry);
            AppendTable(table, row);
            head = head->next;
        }
        // 备注的具体实现
        char remark[100] = "以下为货存系统中满足条件的全部信息 一共记录了";
        char *inventoryCountString = LongLongToString(inventoryCount);
        strcat(remark, inventoryCountString);
        strcat(remark, "条信息");
        free(inventoryCountString);
        // 重新设置备注
        SetTableRemark(table, remark);
    }
    else
        table = NewTable(NULL, "输入的商品编号有误 未查找到与该商品编号相关的货存信息");
    return table;
}

//
TableRow *ShowSingleItemByOperation(Item *item)
{
    TableRow *row = NewTableRow();
    // 写入商品编号
    free(AppendTableRow(row, LongLongToString(GetItemId(item))));

    // 写入商品名称
    AppendTableRow(row, GetItemName(item));

    // 写入商品售价
    Amount price = GetItemPrice(item);
    free(AppendTableRow(row, AmountToString(&price)));

    // 得到商品的保质期
    Time shelfLife = GetItemShelfLife(item);
    free(AppendTableRow(row, TimeToString(GetTimeInfo(&shelfLife, 0))));

    return row;
}

//
Table *ShowSingleItemByItemId(Table *input)
{
    Table *table; // 用于存放该商品信息的表格
    TableRow *row = GetRowByIndex(input, 1);

    // 获取表格中相应的信息
    Item *item = GetItemById(change(GetRowItemByColumnName(input, row, "itemId")));
    if (item != NULL)
    {

        // 创建表头
        row = NewTableRow();
        AppendTableRow(row, "商品编号");
        AppendTableRow(row, "商品名称");
        AppendTableRow(row, "售价");
        AppendTableRow(row, "保质期");
        table = NewTable(NULL, "以下为该商品的全部信息");

        row = ShowSingleItemByOperation(item);
        AppendTable(table, row);
    }
    else
        table = NewTable(NULL, "输入商品编号有误 未查询到相关的商品编号");

    return table;
}

// 展示某一个货损信息通过货损编号
Table *ShowSingleLossById(Table *input)
{
    // 用于存放货损信息的表格
    Table *table;
    TableRow *row = NewTableRow();
    AppendTableRow(row, "货损编号");
    AppendTableRow(row, "库存编号");
    AppendTableRow(row, "货损数量");
    AppendTableRow(row, "货损原因");
    AppendTableRow(row, "损耗时间");

    table = NewTable(row, "以下为要查询的货损信息");

    // 获取表格中相应的信息
    LinkedList *head =
            GetLossEntriesByInventoryId(change(GetRowItemByColumnName(input, GetRowByIndex(input, 1), "Id")));

    if (head != NULL)
    {
        row = NewTableRow();
        // 加入货损Id
        free(AppendTableRow(row, LongLongToString(GetLossEntryId(head->data))));
        // 加入货存Id
        free(AppendTableRow(row, LongLongToString(GetLossEntryInventoryId(head->data))));
        // 加入货损数量
        free(AppendTableRow(row, LongLongToString(GetLossEntryNumber(head->data))));
        // 加入货损原因
        AppendTableRow(row, GetLossEntryReason(head->data));
        Time time = GetLossEntryTime(head->data);
        // 加入货损时间
        free(AppendTableRow(row, TimeToString(GetTimeInfo(&time, 1))));
    }
    else
        table = NewTable(NULL, "输入Id有误 未查找到与相关Id有关的货损信息");

    return table;
}

// 修改一个商品 以商品编号为索引
Table *ReviseAnItemByItemId(Table *input)
{
    Table *table; // 用于存放该商品信息的表格
    TableRow *row = GetRowByIndex(input, 1);
    // 获取表格中相应的信息
    Item *item = GetItemById(change(GetRowItemByColumnName(input, row, "商品编号")));
    if (item != NULL)
    {
        // 获取商品的价格信息
        int yuan = change(GetRowItemByColumnName(input, row, "元"));
        int jiao = change(GetRowItemByColumnName(input, row, "角"));
        int cent = change(GetRowItemByColumnName(input, row, "分"));
        Amount price = NewAmount(yuan, jiao, cent);

        // 获取商品的保质期信息
        char *d = GetRowItemByColumnName(input, row, "天");
        char *h = GetRowItemByColumnName(input, row, "时");
        Time shelfLife = NewTimeSpan(change(d), change(h));

        // 将要修改的各类信息传入到item中
        SetItemName(item, GetRowItemByColumnName(input, row, "商品名称"));
        SetItemPrice(item, &price);
        SetItemShelfLife(item, &shelfLife);
        table = NULL;
        ItemsSave();
    }
    else
        table = NewTable(NULL, "输入商品名称有误 未查询到相关的商品名称");
    return table;
}

// 修改一个货损系统 以货损编号为索引
Table *ReviseLossInventory(Table *input)
{
    // 用于存放该商品信息的表格
    Table *table;
    TableRow *row = GetRowByIndex(input, 1);
    // 获取表格中相应的信息
    LossEntry *entry = GetLossEntryById(change(GetRowItemByColumnName(input, row, "货损编号")));
    if (entry != NULL)
    {
        // 判断要修改的货存编号是否存在
        if (GetInventoryById(change(GetRowItemByColumnName(input, row, "库存编号"))) != NULL)
        {
            int y = change(GetRowItemByColumnName(input, row, "年"));
            int m = change(GetRowItemByColumnName(input, row, "月"));
            int d = change(GetRowItemByColumnName(input, row, "日"));
            int h = change(GetRowItemByColumnName(input, row, "时"));
            int min = change(GetRowItemByColumnName(input, row, "分"));
            int s = change(GetRowItemByColumnName(input, row, "秒"));
            Time time1 = NewDateTime(y, m, d, h, min, s);

            // 获取原有的货损数量
            int oldNumber = GetLossEntryNumber(
                    GetLossEntryById(change(GetRowItemByColumnName(input, row, "货损编号"))));
            // 获取原有的货损库存编号
            int oldInventoryId =
                    GetLossEntryInventoryId(GetLossEntryById(change(GetRowItemByColumnName(input, row, "库存编号"))));
            // 获取修改后的数量
            int NewNumber = change(GetRowItemByColumnName(input, row, "货损数量"));
            if (NewNumber <=
                GetInventoryEntryNumber(GetInventoryById(change(GetRowItemByColumnName(input, row, "库存编号")))))
            {

                // 修改货损信息

                // 修改对应的货存编号
                SetLossEntryInventoryId(entry, change(GetRowItemByColumnName(input, row, "库存编号")));
                // 修改货损理由
                SetLossEntryReason(entry, GetRowItemByColumnName(input, row, "货损原因"));
                // 修改货损时间
                SetLossEntryTime(entry, &time1);
                // 修改货损数量
                SetLossEntryNumber(entry, change(GetRowItemByColumnName(input, row, "货损数量")));

                // 将原有的货损数量加回到货存系统中
                SetInventoryEntryNumber(GetInventoryById(oldInventoryId),
                        GetInventoryEntryNumber(GetInventoryById(oldInventoryId)) + oldNumber);
                // 将货存系统中的货存数量对应减少
                SetInventoryEntryNumber(GetInventoryById(change(GetRowItemByColumnName(input, row, "库存编号"))),
                        GetInventoryEntryNumber(GetInventoryById(
                                change(GetRowItemByColumnName(input, row, "库存编号")))) -
                        NewNumber); //
                // 向文件中保存
                LossEntrySave();
                InventorySave();
                table = NewTable(NULL, "修改成功");
            }
            else
                table = NewTable(NULL, "输入货损数量有误 未查询到相关的货损");
        }
        else
            table = NewTable(NULL, "输入的修改后的货存编号有误 未查询到相关的货存编号");
    }
    else
        table = NewTable(NULL, "输入货损编号有误 未查询到相关的货损");

    return table;
}
