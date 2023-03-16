#include "inventoryService.h"
#include "../data/amount.h"
#include "../data/inventory.h"
#include "../data/item.h"
#include "../data/linkedList.h"
#include "../data/lossEntry.h"
#include "../data/profit.h"
#include "../data/time.h"
#include "../utils.h"
#include <stdio.h>
#include <string.h>

// 转换函数 将字符1到9  转换为数字1到9
int change(char *q)
{
    int a = 0;
    for (int i = 0; q[i] != '\0'; i++)
    {
        a = a * 10 + q[i] - '0';
    }
    return a;
}

// 将数据Amount字符化
void AmountToString(const Amount *unitprice, char *t)
{
    int yuan = GetAmountYuan(unitprice);
    int jiao = GetAmountJiao(unitprice);
    int cent = GetAmountCent(unitprice);

    char *Yuan = LongLongToString(yuan);
    char *Jiao = LongLongToString(jiao);
    char *Cent = LongLongToString(cent);

    strcat(t, Yuan);
    strcat(t, ".");
    strcat(t, Jiao);
    strcat(t, Cent);
    strcat(t, "元");
    free(Yuan);
    free(Jiao);
    free(Cent);
}

// 从表格中获取货存系统所需要的信息
void GetInventoryInformationFromTable(Table *a, int *itemId, int *number, int *y1, int *m1, int *d1, int *h1, int *min1,
                                      int *s1, int *y2, int *m2, int *d2, int *h2, int *min2, int *s2, int *yuan,
                                      int *jiao, int *cent)
{

    TableRow *row = GetRowByIndex(a, 1);
    // 获取表格中相应的信息
    char *ItemId = GetRowItemByColumnName(a, row, "itemId"); // 获取商品的编号的字符串
    char *Number = GetRowItemByColumnName(a, row, "number"); // 获取这批货中商品的数量的字符串
    // 将数据转换为整数类型
    *itemId = change(ItemId);
    *number = change(Number);
    // 获取入库时间的各类信息
    char *y = GetRowItemByColumnName(a, row, "y1");
    char *m = GetRowItemByColumnName(a, row, "m1");
    char *d = GetRowItemByColumnName(a, row, "d1");
    char *h = GetRowItemByColumnName(a, row, "h1");
    char *min = GetRowItemByColumnName(a, row, "min1");
    char *s = GetRowItemByColumnName(a, row, "s1");
    // 将字符转化为数字
    *y1 = change(y);
    *m1 = change(m);
    *d1 = change(d);
    *h1 = change(h);
    *min1 = change(m);
    *s1 = change(s);
    // 获取生产日期的各类信息
    y = GetRowItemByColumnName(a, row, "y2");
    m = GetRowItemByColumnName(a, row, "m2");
    d = GetRowItemByColumnName(a, row, "d2");
    h = GetRowItemByColumnName(a, row, "h2");
    min = GetRowItemByColumnName(a, row, "min2");
    s = GetRowItemByColumnName(a, row, "s2");
    // 将字符转化为数字
    *y2 = change(y);
    *m2 = change(m);
    *d2 = change(d);
    *h2 = change(h);
    *min2 = change(m);
    *s2 = change(s);
    // 获取进价的各类信息
    y = GetRowItemByColumnName(a, row, "yuan");
    m = GetRowItemByColumnName(a, row, "jiao");
    d = GetRowItemByColumnName(a, row, "cent");
    // 将字符转化为数字
    *yuan = change(y);
    *jiao = change(m);
    *cent = change(d);
}

// 向货存链表中添加一批货物
Table *AddInventory(Table *a)
{
    int itemId = 0, number = 0, y1 = 0, m1 = 0, d1 = 0, h1 = 0, min1 = 0, s1 = 0, y2 = 0, m2 = 0, d2 = 0, h2 = 0,
        min2 = 0, s2 = 0, yuan = 0, jiao = 0, cent = 0;
    GetInventoryInformationFromTable(a, &itemId, &number, &y1, &m1, &d1, &h1, &min1, &s1, &y2, &m2, &d2, &h2, &min2,
                                     &s2, &yuan, &jiao, &cent);
    Time inboundTime = NewDateTime(y1, m1, d1, h1, m1, s1);    // 将入库时间信息转化
    Time productionTime = NewDateTime(y2, m2, d2, h2, m2, s2); // 将生产日期信息转化
    Amount unitPrice = NewAmount(yuan, jiao, cent);            // 将进价信息转化

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
        char matter[20] = "进货 商品名称:";               // 事项
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
Table *UpdateInventory()
{
    LinkedList *head = GetAllInventory(); // 获取货存管理中的全部链表

    int d = 0; // 用于判断是否发现货损

    Table *table1; // 用于记录发现货损时的表格

    Table *table2; // 用于记录未发现货损时的表格+
    table2 = NewTable(NULL, "本次更新中货物均完好 未发现货损");

    // 创建货损表头所需要的元素
    TableRow *tablerow = NewTableRow();
    AppendTableRow(tablerow, "货损货物的Id");
    AppendTableRow(tablerow, "货损货物的商品名称");
    AppendTableRow(tablerow, "货损的原因");
    AppendTableRow(tablerow, "货损的日期");

    table1 = NewTable(tablerow, "以下记录的货损均已自动进入货损系统中");

    while (head != NULL)
    {
        InventoryEntry *tmp = head->data;
        Time t1 = GetSystemTime();                      // 获取系统的时间
        Time t2 = GetInventoryEntryProductionTime(tmp); // 获取该批货物的生产日期

        // 获取该批货物所对应商品的保质期
        int itemid = GetInventoryEntryItemId(tmp); // 获取商品id
        Item *item1 = GetItemById(itemid);         // 通过商品Id得到item
        Time t3 = GetItemShelfLife(item1);         // 得到保质期

        Time t4 = AddTime(&t2, &t3); // 计算该批货物的到期时间

        int result = CompareTime(&t1, &t4); // 比较系统时间和到期时间

        if (result > 0)
        {
            d = 1;

            TableRow *TbaleRow1 = NewTableRow(); // 用于记录表格中相对应的信息

            int Id = GetInventoryEntryId(tmp);  // 获取该批货物的编号
            char *strin = LongLongToString(Id); // 将该批货物的id字符化
            AppendTableRow(TbaleRow1, strin);   // 将id信息插入

            char *name = GetItemName(item1); // 获取商品的名称
            AppendTableRow(TbaleRow1, name); // 将商品名称插入

            AppendTableRow(TbaleRow1, "过期"); // 将货损原因插入

            char *r = TimeToString(GetTimeInfo(&t1, 1)); // 将获取到的系统时间字符化
            AppendTableRow(TbaleRow1, r);                // 将时间插入

            AppendTable(table1, TbaleRow1); // 将创建好的tablerow插入到表格中

            AppendLossEntry(NewLossEntry(Id, "过期", &t1)); // 将货损的这批货放入货损管理系统中
            RemoveInventoryEntry(tmp);                      // 将该批货物删除
            FreeInventoryEntry(tmp);                        // 释放该批货物所占用的内存
            free(strin);                                    // 释放strin所占用的空间
            free(r);
        }
        head = head->next;
    }
    InventorySave();
    LossEntrySave();
    if (d == 1)
        return table1;
    else
        return table2;
}

Table *ShowLackInventory()
{

    Table *table1; // 创建表格 该表格用于有货存过少时使用
    Table *table2; // 创建表格 该表格用于没有货存过少时使用
    int k = 0;     // 用于存放是否有货存过少的情况发生 并记录有多少条信息
    // 创建表头
    TableRow *tablerow1 = NewTableRow();
    AppendTableRow(tablerow1, "商品编号");
    AppendTableRow(tablerow1, "商品名称");
    AppendTableRow(tablerow1, "剩余数量");

    table1 = NewTable(tablerow1, NULL);
    table2 = NewTable(NULL, "全部商品库存都很充足 未出现缺货现象");
    LinkedList *itemHead = GetAllItems(); // 获取全部的商品链表
    while (itemHead != NULL)
    {
        int itemId = GetItemId(itemHead->data); // 获取商品编号
        int count1 = 0;                         // 用于存储同一个商品剩余的货存数量
        LinkedList *tmp = GetInventoryByItemId(itemId);
        while (tmp != NULL)
        {
            int number = GetInventoryEntryNumber(tmp->data);
            count1 += number;
            tmp = tmp->next;
        }
        if (count1 < 20)
        {
            k++;
            // 加入商品编号
            TableRow *tablerow2 = NewTableRow();
            char *id = LongLongToString(itemId);
            AppendTableRow(tablerow2, id);
            // 加入商品名称
            char *name = GetItemName(itemHead->data);
            AppendTableRow(tablerow2, name);
            // 加入商品数量
            char *count2 = LongLongToString(count1);
            AppendTableRow(tablerow2, count2);
            free(count2);
            free(id);
            // 将该行插入表格
            AppendTable(table1, tablerow2);
        }
        itemHead = itemHead->next;
    }

    if (k != 0)
    {
        // 表格备注的具体实现
        char o[100] = "以下为货存系统中缺货的全部信息 一共记录了";
        char *p = LongLongToString(k);
        char *q = "条信息";
        strcat(o, p);
        strcat(o, q);
        free(p);

        SetTableRemark(table1, o); // 重新设置备注
        return table1;
    }
    else
        return table2;
}

// 删除货存系统中的某一批货物的全部信息
Table *DeleteInventoryById(Table *a)
{
    Table *table1; // 用于储存最后的结果

    // 获取要删除的货物的编号
    TableRow *row = GetRowByIndex(a, 1);
    char *y = GetRowItemByColumnName(a, row, "Id");
    int Id = change(y); // 将Id由字符转为整数类型

    InventoryEntry *tmp = GetInventoryById(Id);
    if (tmp != NULL)
    {
        RemoveInventoryEntry(tmp);
        FreeInventoryEntry(tmp);
        table1 = NewTable(NULL, "删除成功");
        LossEntrySave();
    }
    else
        table1 = NewTable(NULL, "编号输入有误 未查找到相关的货存信息");

    return table1;
}

Table *DeleteALossInventoryById(Table *a)
{
    Table *table1; // 用于储存最后的结果
    // 获取要修改的货物的编号
    TableRow *row = GetRowByIndex(a, 1);
    char *y = GetRowItemByColumnName(a, row, "Id");
    int Id = change(y); // 将Id由字符转为整数类型
    LinkedList *tmp = GetLossEntriesByInventoryId(Id);
    if (tmp != NULL)
    {
        RemoveLossEntry(tmp->data);
        table1 = NewTable(NULL, "删除成功");
        LossEntrySave();
        FreeLossEntry(tmp->data);
    }
    else
        table1 = NewTable(NULL, "编号输入有误 未查找到相关的货存信息");

    return table1;
}

// 修改一批货存的全部信息
Table *ReviseInventory(Table *a)
{
    Table *table1; // 用于储存最后的结果

    // 获取要修改的货物的编号
    TableRow *row = GetRowByIndex(a, 1);
    char *y = GetRowItemByColumnName(a, row, "Id");
    int Id = change(y); // 将Id由字符转为整数类型

    InventoryEntry *tmp = GetInventoryById(Id);
    if (tmp != NULL)
    {
        // 获取表格中相应的信息
        int itemId = 0, number = 0, y1 = 0, m1 = 0, d1 = 0, h1 = 0, min1 = 0, s1 = 0, y2 = 0, m2 = 0, d2 = 0, h2 = 0,
            min2 = 0, s2 = 0, yuan = 0, jiao = 0, cent = 0;
        GetInventoryInformationFromTable(a, &itemId, &number, &y1, &m1, &d1, &h1, &min1, &s1, &y2, &m2, &d2, &h2, &min2,
                                         &s2, &yuan, &jiao, &cent);

        Time inboundTime = NewDateTime(y1, m1, d1, h1, m1, s1);    // 将入库时间信息转化
        Time productionTime = NewDateTime(y2, m2, d2, h2, m2, s2); // 将生产日期信息转化
        Amount unitPrice = NewAmount(yuan, jiao, cent);            // 将进价信息转化

        SetInventoryEntryItemId(tmp, itemId);                  // 改变商品id
        SetInventoryEntryNumber(tmp, number);                  // 改变货存数量
        SetInventoryEntryInboundTime(tmp, &inboundTime);       // 改变该批货的入库时间
        SetInventoryEntryProductionTime(tmp, &productionTime); // 改变该批货的生产日期
        SetInventoryEntryInboundUnitPrice(tmp, &unitPrice);    // 改变该批货物的购入单价
        InventorySave();
        table1 = NewTable(NULL, "1 修改完成");
    }
    else
        table1 = NewTable(NULL, "0 编号输入有误 未查找到相关的货存信息");

    return table1;
}

// 添加一个商品
Table *AddItem(Table *a)
{
    Table *table1; // 用于储存最后的结果

    // 获取要加入的商品的编号
    TableRow *row = GetRowByIndex(a, 1);
    char *id = GetRowItemByColumnName(a, row, "Id");
    int Id = change(id); // 将Id由字符转为整数类型

    const char *itemname = GetRowItemByColumnName(a, row, "name"); // 获取该商品的名称

    // 获取售价的各类信息
    char *y = GetRowItemByColumnName(a, row, "yuan");
    char *m = GetRowItemByColumnName(a, row, "jiao");
    char *d = GetRowItemByColumnName(a, row, "cent");
    // 将字符转化为数字
    int yuan = change(y);
    int jiao = change(m);
    int cent = change(d);
    Amount saleprice = NewAmount(yuan, jiao, cent);

    // 获取保质期的各类信息
    y = GetRowItemByColumnName(a, row, "y1");
    m = GetRowItemByColumnName(a, row, "m1");
    d = GetRowItemByColumnName(a, row, "d1");
    char *h = GetRowItemByColumnName(a, row, "h1");
    char *min = GetRowItemByColumnName(a, row, "min1");
    char *s = GetRowItemByColumnName(a, row, "s2");

    // 将字符转化为数字
    int y1 = change(y);
    int m1 = change(m);
    int d1 = change(d);
    int h1 = change(h);
    int min1 = change(m);
    int s1 = change(s);
    Time shelfLife = NewDateTime(y1, m1, d1, h1, min1, s1);

    // 将准备好的信息用于创建商品
    int result = AppendItem(NewItem(s, &saleprice, &shelfLife));
    if (result == 1)
        table1 = NewTable(NULL, "1 添加商品成功");
    else
        table1 = NewTable(NULL, "0 添加商品失败");
    ItemsSave();
    return table1;
}

// 添加一批货物至货损系统中
Table *AddLossInventory(Table *a)
{
    Table *table1;
    // 先将该批货加入货存管理系统中获得Id 再将其放入货损系统中
    AddInventory(a);
    LinkedList *head = GetAllInventory();
    while (head->next != NULL)
        head = head->next;
    InventoryEntry *tmp = head->data;
    int Id = GetInventoryEntryId(tmp);
    // 将该批货物放入货损系统中
    Time time1 = GetSystemTime();
    int result = AppendLossEntry(NewLossEntry(Id, "破损", &time1));
    if (result == 1)
        table1 = NewTable(NULL, "货损添加成功"), LossEntrySave();
    else
        table1 = NewTable(NULL, "货损添加失败");
    return table1;
}

// 删除一个物品
Table *DeleteItemByName(Table *a)
{
    Table *table1; // 用于储存最后的结果

    // 获取要删除的货物的名字
    TableRow *row = GetRowByIndex(a, 1);
    const char *itemname = GetRowItemByColumnName(a, row, "name");

    // 得到要删除的货物的指针
    Item *item1 = GetItemByName(itemname);

    if (item1 != NULL)
    {
        RemoveItem(item1); // 用于存放删除结果

        FreeItem(item1);
        table1 = NewTable(NULL, "输入商品名称正确 成功删除商品");
        ItemsSave();
    }
    else
        table1 = NewTable(NULL, "输入商品名称有误 未查找到相关名字的商品");
    return table1;
}

Table *DeleteItemById(Table *a)
{
    Table *table1; // 用于储存最后的结果

    // 获取要删除的货物的名字
    TableRow *row = GetRowByIndex(a, 1);
    char *itemid = GetRowItemByColumnName(a, row, "itemId");
    int itemId = change(itemid);
    // 得到要删除的货物的指针
    Item *item1 = GetItemById(itemId);

    if (item1 != NULL)
    {
        RemoveItem(item1); // 用于存放删除结果

        FreeItem(item1);
        table1 = NewTable(NULL, "输入商品名称正确 成功删除商品");
        ItemsSave();
    }
    else
        table1 = NewTable(NULL, "输入商品名称有误 未查找到相关名字的商品");
    return table1;
}

// 展示货存系统中的全部信息
Table *ShowInventory()
{
    int count1 = 0; // 用于记录一共有多少批货物

    TableRow *tablerow1 = NewTableRow(); // 创建货存信息表格的表头
    AppendTableRow(tablerow1, "Id");
    AppendTableRow(tablerow1, "商品编号");
    AppendTableRow(tablerow1, "商品名称");
    AppendTableRow(tablerow1, "数量");
    AppendTableRow(tablerow1, "入库时间");
    AppendTableRow(tablerow1, "生产日期");
    AppendTableRow(tablerow1, "购入单价");

    Table *table1 = NewTable(tablerow1, NULL); // 创建好带表头的表格

    LinkedList *head = GetAllInventory();

    while (head != NULL)
    {
        count1++;
        const InventoryEntry *tmp = head->data;
        TableRow *tablerow2 = NewTableRow();

        // 获取该批货存的ID
        int id = GetInventoryEntryId(tmp);
        char *Id = LongLongToString(id); // 字符化id
        AppendTableRow(tablerow2, Id);   // 将Id信息加入

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
        AmountToString(&unitprice, t);
        AppendTableRow(tablerow2, t);
        AppendTable(table1, tablerow2);
        free(itemId);
        free(Id);
        free(Number);
        head = head->next;
        free(r);
        free(s);
    }

    // 备注的具体实现
    char o[100] = "以下为货存系统中的全部信息 一共记录了";
    char *p = LongLongToString(count1);
    char *q = "条信息";
    strcat(o, p);
    strcat(o, q);
    free(p);

    SetTableRemark(table1, o); // 重新设置备注
    return table1;
}

// 展示商品系统中的全部信息
Table *ShowItem()
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
