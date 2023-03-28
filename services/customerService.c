#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "design/customer.h"
#include "design/order.h"
#include "design/table.h"
#include "design/utils.h"
#include "design/customerService.h"

// 添加客户
Table *AddCustomer(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int level = atoi(GetRowItemByColumnName(a, information, "客户等级"));
    const char *name = GetRowItemByColumnName(a, information, "客户姓名");
    const char *contact = GetRowItemByColumnName(a, information, "客户联系方式");
    if (level <= 0)
    {
        return NewTable(NULL, "客户等级要大于等于0");
    }
    // 判断客户是否存在
    LinkedList *customerhead = GetCustomersByName(name);
    if (customerhead != NULL)
    {
        LinkedList *customernode = customerhead;
        while (customernode != NULL)
        {
            Customer *thiscustomer = customernode->data;
            const char *comparecontact = GetCustomerContact(thiscustomer);
            if (strcmp(comparecontact, contact) == 0)
            {
                TableRow *row = NewTableRow();
                Table *goback = NewTable(row, "客户已存在");
                return goback;
            }

            customernode = customernode->next;
        }
    }
    // 创建新客户并检验
    char *newname = CloneString(name);
    char *newcontact = CloneString(contact);
    Customer *append = NewCustomer(level, newname, newcontact);
    if (append == NULL)
    {
        free(newname);
        free(newcontact);

        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "创建新客户失败");
        return goback;
    }
    int judge = AppendCustomer(append);
    if (judge)
    {
        free(newname);
        free(newcontact);

        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "创建新客户失败");
        return goback;
    }

    CustomerSave();
    // 返回值
    free(newname);
    free(newcontact);

    return NULL;
}

// 删除客户
Table *DeleteCustomer(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int id = atoi(GetRowItemByColumnName(a, information, "客户编号"));
    // 查找客户判断是否存在
    Customer *del = GetCustomerById(id);
    if (del == NULL)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "不存在符合条件的客户");
        return goback;
    }
    // 判断客户是否创建过订单
    LinkedList *ordernode = GetAllOrders();
    LinkedList *singleordernode = ordernode;
    int judge = 0;
    while (singleordernode != NULL)
    {
        Order *thisorder = singleordernode->data;
        int thiscustomerid = GetOrderCustomerId(thisorder);
        if (thiscustomerid == id)
        {
            judge = 1;
            break;
        }
        singleordernode = singleordernode->next;
    }
    if (judge)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "该客户不可删除");
        return goback;
    }

    RemoveCustomer(del);
    FreeCustomer(del);
    CustomerSave();

    // 返回值
    TableRow *row = NewTableRow();
    Table *goback = NewTable(row, NULL);
    return goback;
}

// 修改用户信息
Table *UpdateCustomer(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int id = atoi(GetRowItemByColumnName(a, information, "客户编号"));
    int level = atoi(GetRowItemByColumnName(a, information, "客户等级"));
    const char *name = GetRowItemByColumnName(a, information, "客户姓名");
    const char *contact = GetRowItemByColumnName(a, information, "客户联系方式");
    if (level <= 0)
    {
        return NewTable(NULL, "客户等级要大于等于0");
    }
    // 修改数据
    Customer *re = GetCustomerById(id);
    if (re == NULL)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "不存在符合条件的客户");
        return goback;
    }
    SetCustomerContact(re, contact);
    SetCustomerLevel(re, level);
    SetCustomerName(re, name);
    CustomerSave();
    // 数据准备和录入
    TableRow *row = NewTableRow();
    Table *goback = NewTable(row, NULL);
    return goback;
}

// 获取所有客户信息
Table *GetAllCustomer(Table *a)
{
    LinkedList *customerNow = GetAllCustomers();

    // 数据准备和录入
    TableRow *row = NewTableRow();
    AppendTableRow(row, "客户编号");
    AppendTableRow(row, "客户等级");
    AppendTableRow(row, "客户姓名");
    AppendTableRow(row, "客户联系方式");
    Table *table = NewTable(row, NULL);

    // 判断
    if (customerNow == NULL)
    {
        SetTableRemark(table, "无客户记录");
        return table;
    }

    int count = 0;

    while (customerNow != NULL)
    {
        Customer *customerInfo = customerNow->data;
        row = NewTableRow();
        count++;

        int customerId = GetCustomerId(customerInfo);
        int customerLevel = GetCustomerLevel(customerInfo);

        const char *customerName = GetCustomerName(customerInfo);
        const char *customerContact = GetCustomerContact(customerInfo);
        char *name = CloneString(customerName);
        char *contact = CloneString(customerContact);

        char *idString = LongLongToString(customerId);
        char *levelString = LongLongToString(customerLevel);

        AppendTableRow(row, idString);
        AppendTableRow(row, levelString);
        AppendTableRow(row, name);
        AppendTableRow(row, contact);

        free(idString);
        free(levelString);
        free(name);
        free(contact);

        AppendTable(table, row);

        customerNow = customerNow->next;
    }

    size_t len = strlen("查询到") + IntegerStringLength(count) + strlen("条信息") + 1;
    char *remark = malloc(len);
    sprintf(remark, "查询到%d条信息", count);
    SetTableRemark(table, remark);
    free(remark);

    // 返回值
    return table;
}
