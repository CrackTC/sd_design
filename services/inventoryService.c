#include "inventoryService.h"
#include "../data/amount.h"
#include "../data/basicDiscount.h"
#include "../data/inventory.h"
#include "../data/item.h"
#include "../data/linkedList.h"
#include "../data/lossEntry.h"
#include "../data/order.h"
#include "../data/profit.h"
#include "../data/time.h"
#include "../utils.h"
#include <stdio.h>
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

// 将数据Amount字符化
void AmountToString(const Amount *amount, char *dest)
{
    int yuan = GetAmountYuan(amount);
    int jiao = GetAmountJiao(amount);
    int cent = GetAmountCent(amount);

    char *yuanString = LongLongToString(yuan);
    char *jiaoString = LongLongToString(jiao);
    char *centString = LongLongToString(cent);

    strcat(dest, yuanString);
    strcat(dest, ".");
    strcat(dest, jiaoString);
    strcat(dest, centString);
    strcat(dest, "元");
    free(yuanString);
    free(jiaoString);
    free(centString);
}

// 从表格中获取货存系统所需要的信息
void GetInventoryInformationFromTable(Table *input, int *itemId, int *number, int *y1, int *m1, int *d1, int *h1,
                                      int *min1, int *s1, int *y2, int *m2, int *d2, int *h2, int *min2, int *s2,
                                      int *yuan, int *jiao, int *cent)
{

    TableRow *row = GetRowByIndex(input, 1);

    *itemId = change(GetRowItemByColumnName(input, row, "itemId"));
    *number = change(GetRowItemByColumnName(input, row, "number"));

    *y1 = change(GetRowItemByColumnName(input, row, "y1"));
    *m1 = change(GetRowItemByColumnName(input, row, "m1"));
    *d1 = change(GetRowItemByColumnName(input, row, "d1"));
    *h1 = change(GetRowItemByColumnName(input, row, "h1"));
    *min1 = change(GetRowItemByColumnName(input, row, "min1"));
    *s1 = change(GetRowItemByColumnName(input, row, "s1"));

    *y2 = change(GetRowItemByColumnName(input, row, "y2"));
    *m2 = change(GetRowItemByColumnName(input, row, "m2"));
    *d2 = change(GetRowItemByColumnName(input, row, "d2"));
    *h2 = change(GetRowItemByColumnName(input, row, "h2"));
    *min2 = change(GetRowItemByColumnName(input, row, "min2"));
    *s2 = change(GetRowItemByColumnName(input, row, "s2"));

    *yuan = change(GetRowItemByColumnName(input, row, "yuan"));
    *jiao = change(GetRowItemByColumnName(input, row, "jiao"));
    *cent = change(GetRowItemByColumnName(input, row, "cent"));
}

// 向货存链表中添加一批货物
Table *AddInventory(Table *input)
{
    int itemId = 0, number = 0, y1 = 0, m1 = 0, d1 = 0, h1 = 0, min1 = 0, s1 = 0, y2 = 0, m2 = 0, d2 = 0, h2 = 0,
        min2 = 0, s2 = 0, yuan = 0, jiao = 0, cent = 0;
    GetInventoryInformationFromTable(input, &itemId, &number, &y1, &m1, &d1, &h1, &min1, &s1, &y2, &m2, &d2, &h2, &min2,
                                     &s2, &yuan, &jiao, &cent);
    Time inboundTime = NewDateTime(y1, m1, d1, h1, min1, s1);    // 将入库时间信息转化
    Time productionTime = NewDateTime(y2, m2, d2, h2, min2, s2); // 将生产日期信息转化
    Amount unitPrice = NewAmount(yuan, jiao, cent);              // 将进价信息转化

    // 将获取的货存信息转化为结构体并添加到货存链表中
    int result1 = AppendInventoryEntry(
        NewInventoryEntry(itemId, number, &inboundTime, &productionTime, &unitPrice)); // result1 用于存储是否插入成功
    Table *table1 = NULL;
    if (result1 == 1)
    {
        InventorySave(); // 将添加结果进行保存

        // 将该批货物所花的钱计入profit结构体中
        Time t1 = GetSystemTime();                        // 获取系统时间
        Amount pay = AmountMultiply(&unitPrice, -number); // 计算出该批货物一共花费的钱
        Item *item1 = GetItemById(itemId);                // 通过商品Id得到item
        const char *name = GetItemName(item1);            // 获取商品的名称
        char matter[500] = "进货 商品名称:";              // 事项
        strcat(matter, name);
        Profit *profit1 = NewProfit(&pay, matter, &t1); // 创建一个新的profit结构体
        int result2 = AppendProfit(profit1); // 将新的profit结构体插入到链表中 并将插入结果返回到result2中
        // 储存账目是否添加成功
        if (result2 == 1)
            table1 = NewTable(NULL, "货存添加成功且账目添加完成"), ProfitSave();
        else
            table1 = NewTable(NULL, " 货存添加成功但账目添加失败");
    }
    else
        table1 = NewTable(NULL, " 货存添加失败");
    return table1;
}

// 更新货存链表 并将过期的货物放入货损链表中
Table *UpdateInventory(Table *input)
{
    LinkedList *head = GetAllInventory(); // 获取货存管理中的全部链表

    int hasLoss = 0; // 用于判断是否发现货损

    Table *tableLoss; // 用于记录发现货损时的表格

    Table *tableOk; // 用于记录未发现货损时的表格+
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
        InventoryEntry *entry = head->data;
        Time timeNow = GetSystemTime();                               // 获取系统的时间
        Time timeProduction = GetInventoryEntryProductionTime(entry); // 获取该批货物的生产日期

        // 获取该批货物所对应商品的保质期
        int itemid = GetInventoryEntryItemId(entry); // 获取商品id
        Item *item = GetItemById(itemid);            // 通过商品Id得到item
        Time timeShelf = GetItemShelfLife(item);     // 得到保质期

        Time timeOutOfDate = AddTime(&timeProduction, &timeShelf); // 计算该批货物的到期时间

        int result = CompareTime(&timeNow, &timeOutOfDate); // 比较系统时间和到期时间

        if (result < 0)
        {
            hasLoss = 1;

            row = NewTableRow(); // 用于记录表格中相对应的信息

            int id = GetInventoryEntryId(entry);

            free(AppendTableRow(row, LongLongToString(id)));                   // 将id信息插入
            AppendTableRow(row, GetItemName(item));                            // 将商品名称插入
            AppendTableRow(row, "过期");                                       // 将货损原因插入
            free(AppendTableRow(row, TimeToString(GetTimeInfo(&timeNow, 1)))); // 将时间插入

            AppendTable(tableLoss, row); // 将创建好的tablerow插入到表格中

            AppendLossEntry(
                NewLossEntry(id, GetInventoryEntryNumber(entry), "过期", &timeNow)); // 将货损的这批货放入货损管理系统中
            SetInventoryEntryNumber(entry, 0);                                       // 将该批货物删除
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

Table *ShowLackInventory(Table *input)
{
    Table *tableLack;  // 创建表格 该表格用于有货存过少时使用
    Table *tableOk;    // 创建表格 该表格用于没有货存过少时使用
    int lackCount = 0; // 用于存放是否有货存过少的情况发生 并记录有多少条信息
    // 创建表头
    TableRow *row = NewTableRow();
    AppendTableRow(row, "商品编号");
    AppendTableRow(row, "商品名称");
    AppendTableRow(row, "剩余数量");

    tableLack = NewTable(row, NULL);
    tableOk = NewTable(NULL, "全部商品库存都很充足 未出现缺货现象");
    LinkedList *itemNow = GetAllItems(); // 获取全部的商品链表
    while (itemNow != NULL)
    {
        int itemId = GetItemId(itemNow->data); // 获取商品编号
        int remainCount = 0;                   // 用于存储同一个商品剩余的货存数量
        LinkedList *inventoryNow = GetInventoryByItemId(itemId);
        while (inventoryNow != NULL)
        {
            remainCount += GetInventoryEntryNumber(inventoryNow->data);
            inventoryNow = inventoryNow->next;
        }
        if (remainCount < 20)
        {
            lackCount++;
            row = NewTableRow();

            free(AppendTableRow(row, LongLongToString(itemId)));
            AppendTableRow(row, GetItemName(itemNow->data));
            free(AppendTableRow(row, LongLongToString(remainCount)));

            AppendTable(tableLack, row);
        }
        itemNow = itemNow->next;
    }

    if (lackCount != 0)
    {
        // 表格备注的具体实现
        char o[100] = "以下为货存系统中缺货的全部信息 一共记录了";
        char *p = LongLongToString(lackCount);
        char *q = "条信息";
        strcat(o, p);
        strcat(o, q);
        SetTableRemark(tableLack, o); // 重新设置备注

        free(p);
        FreeTable(tableOk);
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
    Table *table; // 用于储存最后的结果

    // 获取要删除的货物的编号
    TableRow *row = GetRowByIndex(input, 1);
    int id = change(GetRowItemByColumnName(input, row, "Id")); // 将Id由字符转为整数类型

    if (GetOrdersByInventoryId(id) != NULL || GetLossEntriesByInventoryId(id) != NULL)
    {
        table = NewTable(NULL, "删除失败，该库存已存在关联的订单或货损条目");
        return table;
    }

    InventoryEntry *entry = GetInventoryById(id);
    if (entry != NULL)
    {
        RemoveInventoryEntry(entry);
        FreeInventoryEntry(entry);
        table = NewTable(NULL, "删除成功");
        InventorySave();
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
    int id = change(GetRowItemByColumnName(input, row, "Id")); // 将Id由字符转为整数类型
    LossEntry *entry = GetLossEntryById(id);
    if (entry != NULL)
    {
        InventoryEntry *inventoryEntry = GetInventoryById(GetLossEntryInventoryId(entry));
        SetInventoryEntryNumber(inventoryEntry, GetInventoryEntryNumber(inventoryEntry) + GetLossEntryNumber(entry));
        RemoveLossEntry(entry);
        table = NewTable(NULL, "删除成功");

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
    Table *table; // 用于储存最后的结果

    // 获取要修改的货物的编号
    TableRow *row = GetRowByIndex(input, 1);
    int id = change(GetRowItemByColumnName(input, row, "Id")); // 将Id由字符转为整数类型
                                                               //
    if (GetOrdersByInventoryId(id) != NULL || GetLossEntriesByInventoryId(id) != NULL)
    {
        table = NewTable(NULL, "1 修改失败，该库存已存在关联的订单或货损条目");
        return table;
    }

    InventoryEntry *entry = GetInventoryById(id);
    if (entry != NULL)
    {
        // 获取表格中相应的信息
        int itemId = 0, number = 0, y1 = 0, m1 = 0, d1 = 0, h1 = 0, min1 = 0, s1 = 0, y2 = 0, m2 = 0, d2 = 0, h2 = 0,
            min2 = 0, s2 = 0, yuan = 0, jiao = 0, cent = 0;
        GetInventoryInformationFromTable(input, &itemId, &number, &y1, &m1, &d1, &h1, &min1, &s1, &y2, &m2, &d2, &h2,
                                         &min2, &s2, &yuan, &jiao, &cent);

        Time inboundTime = NewDateTime(y1, m1, d1, h1, min1, s1);    // 将入库时间信息转化
        Time productionTime = NewDateTime(y2, m2, d2, h2, min2, s2); // 将生产日期信息转化
        Amount unitPrice = NewAmount(yuan, jiao, cent);              // 将进价信息转化

        SetInventoryEntryItemId(entry, itemId);                  // 改变商品id
        SetInventoryEntryNumber(entry, number);                  // 改变货存数量
        SetInventoryEntryInboundTime(entry, &inboundTime);       // 改变该批货的入库时间
        SetInventoryEntryProductionTime(entry, &productionTime); // 改变该批货的生产日期
        SetInventoryEntryInboundUnitPrice(entry, &unitPrice);    // 改变该批货物的购入单价
        InventorySave();
        table = NewTable(NULL, "0 修改完成");
    }
    else
        table = NewTable(NULL, "1 编号输入有误 未查找到相关的货存信息");

    return table;
}

// 添加一个商品
Table *AddItem(Table *input)
{
    Table *table; // 用于储存最后的结果

    // 获取要加入的商品的编号
    TableRow *row = GetRowByIndex(input, 1);
    int id = change(GetRowItemByColumnName(input, row, "Id")); // 将Id由字符转为整数类型

    const char *itemName = GetRowItemByColumnName(input, row, "name"); // 获取该商品的名称

    // 将字符转化为数字
    int yuan = change(GetRowItemByColumnName(input, row, "yuan"));
    int jiao = change(GetRowItemByColumnName(input, row, "jiao"));
    int cent = change(GetRowItemByColumnName(input, row, "cent"));
    Amount saleprice = NewAmount(yuan, jiao, cent);

    // 将字符转化为数字
    int y1 = change(GetRowItemByColumnName(input, row, "y1"));
    int m1 = change(GetRowItemByColumnName(input, row, "m1"));
    int d1 = change(GetRowItemByColumnName(input, row, "d1"));
    int h1 = change(GetRowItemByColumnName(input, row, "h1"));
    int min1 = change(GetRowItemByColumnName(input, row, "min1"));
    int s1 = change(GetRowItemByColumnName(input, row, "s1"));
    Time shelfLife = NewDateTime(y1, m1, d1, h1, min1, s1);

    // 将准备好的信息用于创建商品
    int result = AppendItem(NewItem(itemName, &saleprice, &shelfLife));
    if (result == 0)
        table = NewTable(NULL, "0 添加商品成功");
    else
        table = NewTable(NULL, "1 添加商品失败");
    ItemsSave();
    return table;
}

// 添加一批货物至货损系统中
Table *AddLossInventory(Table *input)
{
    Table *table;
    TableRow *row = GetRowByIndex(table, 1);

    int inventoryId = change(GetRowItemByColumnName(table, row, "id"));
    int number = change(GetRowItemByColumnName(table, row, "number"));
    const char *reason = GetRowItemByColumnName(table, row, "reason");
    Time time1 = GetSystemTime();

    InventoryEntry *inventoryEntry = GetInventoryById(inventoryId);
    if (GetInventoryEntryNumber(inventoryEntry) < number)
    {
        return NewTable(NULL, "货损添加失败：输入的货损数目大于库存数");
    }

    int result = AppendLossEntry(NewLossEntry(inventoryId, number, reason, &time1));
    if (result == 0)
        table = NewTable(NULL, "货损添加成功"), LossEntrySave();
    else
        table = NewTable(NULL, "货损添加失败");
    return table;
}

Table *DeleteItemById(Table *input)
{
    // 获取要删除的货物的名字
    TableRow *row = GetRowByIndex(input, 1);

    int id = change(GetRowItemByColumnName(input, row, "itemId"));

    // 得到要删除的货物的指针
    Item *item = GetItemById(id);

    if (item != NULL)
    {
        if (GetInventoryByItemId(id) != NULL || GetBasicDiscountsByItemId(id) != NULL)
        {
            return NewTable(NULL, "删除失败，存在与该商品关联的优惠或库存信息");
        }
        RemoveItem(item); // 用于存放删除结果

        FreeItem(item);
        ItemsSave();
        return NewTable(NULL, "删除成功");
    }
    else
        return NewTable(NULL, "商品编号有误 未查找到相关商品");
}

// 展示货存系统中的全部信息
Table *ShowInventory(Table *input)
{
    int inventoryCount = 0; // 用于记录一共有多少批货物

    TableRow *row = NewTableRow(); // 创建货存信息表格的表头
    AppendTableRow(row, "ID");
    AppendTableRow(row, "商品编号");
    AppendTableRow(row, "商品名称");
    AppendTableRow(row, "数量");
    AppendTableRow(row, "入库时间");
    AppendTableRow(row, "生产日期");
    AppendTableRow(row, "购入单价");

    Table *table = NewTable(row, NULL); // 创建好带表头的表格

    LinkedList *head = GetAllInventory();

    while (head != NULL)
    {
        inventoryCount++;
        const InventoryEntry *entry = head->data;
        row = NewTableRow();

        // 获取该批货存的ID
        free(AppendTableRow(row, LongLongToString(GetInventoryEntryId(entry)))); // 将Id信息加入

        int itemId = GetInventoryEntryItemId(entry);
        // 获取该批货存的商品编号
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
        Amount unitprice = GetInventoryEntryInboundUnitPrice(entry);
        char t[30];
        AmountToString(&unitprice, t);
        AppendTableRow(row, t);
        AppendTable(table, row);
        head = head->next;
    }

    // 备注的具体实现
    char o[100] = "以下为货存系统中的全部信息 一共记录了";
    char *p = LongLongToString(inventoryCount);
    char *q = "条信息";
    strcat(o, p);
    strcat(o, q);
    free(p);

    SetTableRemark(table, o); // 重新设置备注
    return table;
}

// 展示商品系统中的全部信息
Table *ShowItem(Table *input)
{
    int count1 = 0;                      // 用于记录一共有多少中商品
    TableRow *tablerow1 = NewTableRow(); // 创建表格的表头
    AppendTableRow(tablerow1, "商品编号");
    AppendTableRow(tablerow1, "商品名称");
    AppendTableRow(tablerow1, "售价");
    AppendTableRow(tablerow1, "保质期");

    Table *table1 = NewTable(tablerow1, NULL);

    LinkedList *head = GetAllItems();

    while (head != NULL)
    {
        count1++; // 用于记录一共有多少个商品
        const Item *tmp = head->data;
        TableRow *tablerow2 = NewTableRow();

        // 得到商品编号
        int itemid = GetItemId(tmp);
        char *itemId = LongLongToString(itemid);
        AppendTableRow(tablerow2, itemId);

        // 得到商品名称
        char *name = GetItemName(tmp);
        AppendTableRow(tablerow2, name);

        // 得到商品的售价
        Amount saleprice = GetItemPrice(tmp);
        char t[15];
        AmountToString(&saleprice, t);
        AppendTableRow(tablerow2, t);

        // 得到商品的保质期
        Time shelflife = GetItemShelfLife(tmp);
        char *o = TimeToString(GetTimeInfo(&shelflife, 0));
        AppendTableRow(tablerow2, o);

        AppendTable(table1, tablerow2);
        free(o);
        head = head->next;
    }

    // 备注的具体实现
    char o[100] = "以下为商品系统中的全部信息 一共记录了";
    char *p = LongLongToString(count1);
    char *q = "条信息";
    strcat(o, p);
    strcat(o, q);
    free(p);
    SetTableRemark(table1, o); // 重新设置备注
    return table1;
}

Table *ShowLossInventory()
{
    LinkedList *head = GetAllLoss();
    Table *table1;
    TableRow *tablerow1 = NewTableRow();
    AppendTableRow(tablerow1, "货损编号");
    AppendTableRow(tablerow1, "货损原因");
    AppendTableRow(tablerow1, "进入货损时的时间");

    table1 = NewTable(tablerow1, NULL);
    int count1 = 0; // 用于记录一共有多少条货损记录

    while (head != NULL)
    {
        count1++; // 用于记录有多少条信息
        LossEntry *tmp = head->data;
        TableRow *tablerow2 = NewTableRow();
        // 字符化Id
        int id = GetLossEntryInventoryId(tmp);
        char *Id = LongLongToString(id);
        AppendTableRow(tablerow2, Id);
        // 获取损坏原因
        const char *reason = GetLossEntryReason(tmp);
        free(AppendTableRow(tablerow2, CloneString(reason)));
        // 获取进入货损时的时间
        Time time1 = GetLossEntryTime(tmp);
        char *time2 = TimeToString(GetTimeInfo(&time1, 1));
        AppendTableRow(tablerow2, time2);

        AppendTable(table1, tablerow2);
        free(Id);
        free(time2);
        head = head->next;
    }
    // 备注的具体实现
    char o[100] = "以下为货损系统中的全部信息 一共记录了";
    char *p = LongLongToString(count1);
    char *q = "条信息";
    strcat(o, p);
    strcat(o, q);
    free(p);
    free(q);
    SetTableRemark(table1, o); // 重新设置备注
    return table1;
}

TableRow *ShowSingleInventoryByOperation(InventoryEntry *tmp)
{
    TableRow *tablerow2 = NewTableRow();
    // 获取Id信息
    int id = GetInventoryEntryId(tmp);
    AppendTableRow(tablerow2, LongLongToString(id)); // 将Id信息加入

    // 获取该批货存的商品编号
    int itemid = GetInventoryEntryItemId(tmp);
    char *itemId = LongLongToString(itemid); // 字符化itemid
    AppendTableRow(tablerow2, itemId);       // 将itemId信息加入

    // 获取商品名称
    Item *item1 = GetItemById(itemid);
    char *itemName = GetItemName(item1); // 得到商品名称
    AppendTableRow(tablerow2, itemName);

    // 获取该批货存的数量
    int number = GetInventoryEntryNumber(tmp);
    char *Number = LongLongToString(number); // 字符化id
    AppendTableRow(tablerow2, Number);

    // 获取该批货物的入库时间
    Time InboundTime = GetInventoryEntryInboundTime(tmp);
    char *r = TimeToString(GetTimeInfo(&InboundTime, 1)); // 用于存放字符化后的入库时间
    AppendTableRow(tablerow2, r);

    // 获取该批货物的生产日期
    Time productionTime = GetInventoryEntryProductionTime(tmp);
    char *s = TimeToString(GetTimeInfo(&productionTime, 1)); // 将生产日期字符化
    AppendTableRow(tablerow2, s);

    // 获取该批货物的进价
    Amount unitprice = GetInventoryEntryInboundUnitPrice(tmp);
    char t[15];
    AmountToString(&unitprice, t); // 将金钱字符化
    AppendTableRow(tablerow2, t);

    free(itemId);
    free(Number);
    free(r);
    free(s);
    return tablerow2;
}

// 查找某一批货物的全部信息 以Id为索引
Table *ShowSingleInventoryById(Table *a)
{
    Table *table1; // 用于存放该批货物的表格
    TableRow *row = GetRowByIndex(a, 1);
    // 获取表格中相应的信息
    char *id = GetRowItemByColumnName(a, row, "Id"); // 获取商品的编号的字符串
    int Id = change(id);
    InventoryEntry *tmp = GetInventoryById(Id);
    if (tmp != NULL)
    {
        TableRow *tablerow1 = NewTableRow(); // 创建货存信息表格的表头
        AppendTableRow(tablerow1, "Id");
        AppendTableRow(tablerow1, "商品编号");
        AppendTableRow(tablerow1, "商品名称");
        AppendTableRow(tablerow1, "数量");
        AppendTableRow(tablerow1, "入库时间");
        AppendTableRow(tablerow1, "生产日期");
        AppendTableRow(tablerow1, "购入单价");

        table1 = NewTable(tablerow1, "以下为要查找的信息");

        TableRow *tablerow2 = NewTableRow();
        tablerow2 = ShowSingleInventoryByOperation(tmp);
        AppendTable(table1, tablerow2);
    }
    else
        table1 = NewTable(NULL, "输入的Id有误 未查找到与该Id相关的货物信息");
    return table1;
}

Table *ShowSingleInventoryByItemId(Table *a)
{
    Table *table1; // 用于存放该批货物的表格
    TableRow *row = GetRowByIndex(a, 1);
    // 获取表格中相应的信息
    char *itemid = GetRowItemByColumnName(a, row, "itemid"); // 获取商品的编号的字符串
    int itemId = change(itemid);
    LinkedList *head = GetInventoryByItemId(itemId);
    if (head != NULL)
    {

        TableRow *tablerow1 = NewTableRow(); // 创建货存信息表格的表头
        AppendTableRow(tablerow1, "Id");
        AppendTableRow(tablerow1, "商品编号");
        AppendTableRow(tablerow1, "商品名称");
        AppendTableRow(tablerow1, "数量");
        AppendTableRow(tablerow1, "入库时间");
        AppendTableRow(tablerow1, "生产日期");
        AppendTableRow(tablerow1, "购入单价");

        int count1 = 0; // 用于记录有哪些货存
        table1 = NewTable(tablerow1, NULL);
        while (head != NULL)
        {
            count1++;
            InventoryEntry *tmp = head->data; // 获取货存信息
            TableRow *tablerow2 = NewTableRow();
            tablerow2 = ShowSingleInventoryByOperation(tmp);
            AppendTable(table1, tablerow2);
            head = head->next;
        }
        // 备注的具体实现
        char o[100] = "以下为货存系统中满足条件的全部信息 一共记录了";
        char *p = LongLongToString(count1);
        char *q = "条信息";
        strcat(o, p);
        strcat(o, q);
        free(p);
        SetTableRemark(table1, o); // 重新设置备注
    }
    else
        table1 = NewTable(NULL, "输入的商品id有误 未查找到与该商品id相关的货物信息");
    return table1;
}

// 展示一种商品在货存系统中的全部数据 用商品名称索引
Table *ShowAllInventoryByItemName(Table *a)
{
    Table *table1; // 用于存放该批货物的表格
    TableRow *row = GetRowByIndex(a, 1);
    // 获取表格中相应的信息
    const char *itemIdString = GetRowItemByColumnName(a, row, "itemId"); // 获取商品的编号的字符串
    int itemId;
    sscanf(itemIdString, "%d", &itemId);
    LinkedList *head = GetInventoryByItemId(itemId);
    if (head != NULL)
    {

        TableRow *tablerow1 = NewTableRow(); // 创建货存信息表格的表头
        AppendTableRow(tablerow1, "Id");
        AppendTableRow(tablerow1, "商品编号");
        AppendTableRow(tablerow1, "商品名称");
        AppendTableRow(tablerow1, "数量");
        AppendTableRow(tablerow1, "入库时间");
        AppendTableRow(tablerow1, "生产日期");
        AppendTableRow(tablerow1, "购入单价");

        int count1 = 0; // 用于记录有哪些货存
        table1 = NewTable(tablerow1, NULL);
        while (head != NULL)
        {
            count1++;
            InventoryEntry *tmp = head->data; // 获取货存信息
            TableRow *tablerow2 = NewTableRow();
            tablerow2 = ShowSingleInventoryByOperation(tmp);
            AppendTable(table1, tablerow2);
            head = head->next;
        }
        // 备注的具体实现
        char o[100] = "以下为货存系统中满足条件的全部信息 一共记录了";
        char *p = LongLongToString(count1);
        char *q = "条信息";
        strcat(o, p);
        strcat(o, q);
        free(p);
        free(q);
        SetTableRemark(table1, o); // 重新设置备注
    }
    else
        table1 = NewTable(NULL, "输入的商品id有误 未查找到与该商品id相关的货物信息");
    return table1;
}
TableRow *ShowSingleItemByOperation(Item *tmp)
{
    TableRow *tablerow2 = NewTableRow();
    // 写入商品编号
    int id = GetItemId(tmp);
    char *Id = LongLongToString(id); // 将id字符化
    AppendTableRow(tablerow2, Id);

    // 写入商品名称
    char *itemname = GetItemName(tmp);
    AppendTableRow(tablerow2, itemname);

    // 写入商品售价
    Amount saleprice = GetItemPrice(tmp);
    char t[15];
    AmountToString(&saleprice, t);
    AppendTableRow(tablerow2, t);

    // 得到商品的保质期
    Time shelflife = GetItemShelfLife(tmp);
    char *o = TimeToString(GetTimeInfo(&shelflife, 0));
    AppendTableRow(tablerow2, o);
    free(Id);
    free(o);
    return tablerow2;
}

// 展示一个商品的全部信息 以商品名称为索引
Table *ShowSingleItemByItemname(Table *a)
{

    Table *table1; // 用于存放该商品信息的表格
    TableRow *row = GetRowByIndex(a, 1);
    // 获取表格中相应的信息
    const char *itemname = GetRowItemByColumnName(a, row, "itemname"); // 获取商品的名称
    Item *tmp = GetItemByName(itemname);
    if (tmp == NULL)
    {

        // 创建表头
        TableRow *tablerow1 = NewTableRow();
        AppendTableRow(tablerow1, "商品编号");
        AppendTableRow(tablerow1, "商品名称");
        AppendTableRow(tablerow1, "售价");
        AppendTableRow(tablerow1, "保质期");
        table1 = NewTable(NULL, "以下为该商品的全部信息");

        TableRow *tablerow2 = NewTableRow();
        tablerow2 = ShowSingleItemByOperation(tmp);
        AppendTable(table1, tablerow2);
    }
    else
        table1 = NewTable(NULL, "输入商品名称有误 未查询到相关的商品名称");

    return table1;
}

Table *ShowSingleItemByItemId(Table *a)
{
    Table *table1; // 用于存放该商品信息的表格
    TableRow *row = GetRowByIndex(a, 1);
    // 获取表格中相应的信息
    char *id = GetRowItemByColumnName(a, row, "itemId"); // 获取商品的名称
    int Id = change(id);
    Item *tmp = GetItemById(Id);
    if (tmp == NULL)
    {

        // 创建表头
        TableRow *tablerow1 = NewTableRow();
        AppendTableRow(tablerow1, "商品编号");
        AppendTableRow(tablerow1, "商品名称");
        AppendTableRow(tablerow1, "售价");
        AppendTableRow(tablerow1, "保质期");
        table1 = NewTable(NULL, "以下为该商品的全部信息");

        TableRow *tablerow2 = NewTableRow();
        tablerow2 = ShowSingleItemByOperation(tmp);
        AppendTable(table1, tablerow2);
    }
    else
        table1 = NewTable(NULL, "输入商品编号有误 未查询到相关的商品编号");

    return table1;
}

Table *ShowSingleLossInventoryById(Table *a)
{
    TableRow *row = GetRowByIndex(a, 1);

    Table *table1; // 用于存放货损信息的表格
    TableRow *tablerow1;
    tablerow1 = NewTableRow();
    AppendTableRow(tablerow1, "库存编号");
    AppendTableRow(tablerow1, "货损原因");
    AppendTableRow(tablerow1, "损耗时间");

    table1 = NewTable(tablerow1, "以下为要查询的货损信息");

    // 获取表格中相应的信息
    char *id = GetRowItemByColumnName(a, row, "itemId"); // 获取商品的名称
    int Id = change(id);
    LinkedList *tmp = GetLossEntriesByInventoryId(Id);

    if (tmp != NULL)
    {
        tablerow1 = NewTableRow();
        int id = GetLossEntryInventoryId(tmp->data);
        char *Id = LongLongToString(id);
        AppendTableRow(tablerow1, Id);
        char *reason = GetLossEntryReason(tmp->data);
        AppendTableRow(tablerow1, reason);
        Time time1 = GetLossEntryTime(tmp->data);
        char *time2 = TimeToString(GetTimeInfo(&time1, 1));
        AppendTableRow(tablerow1, time2);
        free(Id);
        free(time2);
    }
    else
        table1 = NewTable(NULL, "输入Id有误 未查找到与相关Id有关的货损信息");

    return table1;
}

// 修改某一个商品的全部信息
Table *ReviseAnItemByItemName(Table *a)
{
    Table *table1; // 用于存放该商品信息的表格
    TableRow *row = GetRowByIndex(a, 1);
    // 获取表格中相应的信息
    const char *itemname = GetRowItemByColumnName(a, row, "olditemname"); // 获取商品的旧名称
    Item *tmp = GetItemByName(itemname);
    if (tmp != NULL)
    {
        // 获取商品的新名称
        itemname = GetRowItemByColumnName(a, row, "newitemname");
        // 获取商品的价格信息
        char *Yuan = GetRowItemByColumnName(a, row, "yuan");
        char *Jiao = GetRowItemByColumnName(a, row, "jiao");
        char *Cent = GetRowItemByColumnName(a, row, "cent");
        int yuan = change(Yuan);
        int jiao = change(Jiao);
        int cent = change(Cent);
        Amount saleprice = NewAmount(yuan, jiao, cent);

        // 获取商品的保质期信息
        char *y = GetRowItemByColumnName(a, row, "y");
        char *m = GetRowItemByColumnName(a, row, "m");
        char *d = GetRowItemByColumnName(a, row, "d");
        char *h = GetRowItemByColumnName(a, row, "h");
        char *min = GetRowItemByColumnName(a, row, "min");
        char *s = GetRowItemByColumnName(a, row, "s");
        Time shelflife = NewDateTime(change(y), change(m), change(d), change(h), change(min), change(s));

        // 将要修改的各类信息传入到item中
        SetItemName(tmp, itemname);
        SetItemPrice(tmp, &saleprice);
        SetItemShelfLife(tmp, &shelflife);
        table1 = NewTable(NULL, "修改成功");
        ItemsSave();
    }
    else
        table1 = NewTable(NULL, "输入商品名称有误 未查询到相关的商品名称");
    return table1;
}

Table *ReviseAnItemByItemId(Table *a)
{
    Table *table1; // 用于存放该商品信息的表格
    TableRow *row = GetRowByIndex(a, 1);
    // 获取表格中相应的信息
    char *itemid = GetRowItemByColumnName(a, row, "Id"); // 获取商品的旧名称
    int itemId = change(itemid);
    Item *tmp = GetItemById(itemId);
    if (tmp != NULL)
    {
        // 获取商品的新名称
        const char *itemname = GetRowItemByColumnName(a, row, "newitemname");
        // 获取商品的价格信息
        char *Yuan = GetRowItemByColumnName(a, row, "yuan");
        char *Jiao = GetRowItemByColumnName(a, row, "jiao");
        char *Cent = GetRowItemByColumnName(a, row, "cent");
        int yuan = change(Yuan);
        int jiao = change(Jiao);
        int cent = change(Cent);
        Amount saleprice = NewAmount(yuan, jiao, cent);

        // 获取商品的保质期信息
        char *y = GetRowItemByColumnName(a, row, "y");
        char *m = GetRowItemByColumnName(a, row, "m");
        char *d = GetRowItemByColumnName(a, row, "d");
        char *h = GetRowItemByColumnName(a, row, "h");
        char *min = GetRowItemByColumnName(a, row, "min");
        char *s = GetRowItemByColumnName(a, row, "s");
        Time shelflife = NewDateTime(change(y), change(m), change(d), change(h), change(min), change(s));

        // 将要修改的各类信息传入到item中
        SetItemName(tmp, itemname);
        SetItemPrice(tmp, &saleprice);
        SetItemShelfLife(tmp, &shelflife);
        table1 = NewTable(NULL, "修改成功");
        ItemsSave();
    }
    else
        table1 = NewTable(NULL, "输入商品名称有误 未查询到相关的商品名称");
    return table1;
}

Table *ReviseLossInventory(Table *a)
{
    Table *table1; // 用于存放该商品信息的表格
    TableRow *row = GetRowByIndex(a, 1);
    // 获取表格中相应的信息
    char *id = GetRowItemByColumnName(a, row, "Id");
    int Id = change(id);
    LinkedList *tmp = GetLossEntriesByInventoryId(Id);
    if (tmp != NULL)
    {
        char *reason = GetRowItemByColumnName(a, row, "reason");
        char *y = GetRowItemByColumnName(a, row, "y");
        char *m = GetRowItemByColumnName(a, row, "m");
        char *d = GetRowItemByColumnName(a, row, "d");
        char *h = GetRowItemByColumnName(a, row, "h");
        char *min = GetRowItemByColumnName(a, row, "min");
        char *s = GetRowItemByColumnName(a, row, "s");
        int y1 = change(y);
        int m1 = change(m);
        int d1 = change(d);
        int h1 = change(h);
        int min1 = change(min);
        int s1 = change(s);

        Time time1 = NewDateTime(change(y), change(m), change(d), change(h), change(min), change(s));
        SetLossEntryInventoryId(tmp->data, Id);
        SetLossEntryReason(tmp->data, reason);
        SetLossEntryTime(tmp->data, &time1);
        LossEntrySave();
        table1 = NewTable(NULL, "修改成功");
    }
    else
        table1 = NewTable(NULL, "输入货损编号有误 未查询到相关的货损");

    return table1;
}
