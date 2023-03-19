#include <stdlib.h>
#include <string.h>

#include "../data/customer.h"
#include "../data/linkedList.h"
#include "../data/table.h"
#include "../utils.h"
#include "customerService.h"

// 添加客户
Table *AddCustomer(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int level = GetRowItemByColumnName(a, information, "level");
    const char *name = GetRowItemByColumnName(a, information, "name");
    const char *contact = GetRowItemByColumnName(a, information, "contact");
    // 判断客户是否存在
    LinkedList *customerhead = GetCustomersByName(name);
    if (customerhead != NULL)
    {
        LinkedList *customernode = customerhead;
        while (customernode != NULL)
        {
            Customer *thiscustomer = customernode->data;
            char *comparecontact = GetCustomerContact(thiscustomer);
            if (strcmp(comparecontact, contact) == 0)
            {
                TableRow *row = NewTableRow();
                Table *goback = NewTable(row, "客户已存在") return goback;
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
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "创建新客户失败");
        return goback;
    }
    int judge = AppendCustomer(append);
    if (judge)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "创建新客户失败");
        return goback;
    }

    CustomerSave();
    // 返回值
    TableRow *row = NewTableRow();
    Table *goback = NewTable(row, NULL);
    return goback;
}

// 删除客户
Table *DeleteCustomer(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int id = GetRowItemByColumnName(a, information, "id");
    // 查找客户判断并删除
    Customer *del = GetCustomerById(id);
    if (del == NULL)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "不存在符合条件的客户");
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
    int id = GetRowItemByColumnName(a, information, "id");
    int level = GetRowItemByColumnName(a, information, "level");
    const char *name = GetRowItemByColumnName(a, information, "name");
    const char *contact = GetRowItemByColumnName(a, information, "contact");
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

// 获取单个客户信息
Table *GetSingleCustomer(Table *a)
{
    // 读数据
    TableRow *information = GetRowByIndex(a, 1);
    int id = GetRowItemByColumnName(a, information, "id");

    // 获取信息
    Customer *info = GetCustomerById(id);
    // 判断
    if (info == NULL)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "不存在符合条件的客户");
        return goback;
    }

    // 数据准备和录入
    int level = GetCustomerLevel(info);
    const char *tempname = GetCustomerName(info);
    const char *tempcontact = GetCustomerContact(info);
    char *name = CloneString(tempname);
    char *contact = CloneString(tempcontact);
    char *idstring = LongLongToString(id);
    char *levelstring = LongLongToString(level);

    TableRow *row = NewTableRow();
    AppendTableRow(row, "id");
    AppendTableRow(row, "level");
    AppendTableRow(row, "name");
    AppendTableRow(row, "contact");
    Table *goback = NewTable(row, NULL);

    row = NewTableRow();
    AppendTableRow(row, idstring);
    AppendTableRow(row, levelstring);
    AppendTableRow(row, name);
    AppendTableRow(row, contact);

    free(idstring);
    free(levelstring);
    free(name);
    free(contact);

    AppendTable(goback, row);
    // 返回值
    return goback;
}

// 获取所有客户信息
Table *GetAllCustomer()
{
    LinkedList *head = GetAllCustomers();

    // 判断
    if (head == NULL)
    {
        TableRow *row = NewTableRow();
        Table *goback = NewTable(row, "无客户记录");
        return goback;
    }

    // 数据准备和录入
    TableRow *row = NewTableRow();
    AppendTableRow(row, "id");
    AppendTableRow(row, "level");
    AppendTableRow(row, "name");
    AppendTableRow(row, "contact");
    Table *goback = NewTable(row, NULL);

    LinkedList *customer = head;
    while (head != NULL)
    {
        Customer *customerinfo = customer->data;
        row = NewTableRow();

        int customerid = GetCustomerId(customerinfo);
        int customerlevel = GetCustomerLevel(customerinfo);

        const char *tempname = GetCustomerName(customerinfo);
        const char *tempcontact = GetCustomerContact(customerinfo);
        char *name = CloneString(tempname);
        char *contact = CloneString(tempcontact);

        char *idstring = LongLongToString(customerid);
        char *levelstring = LongLongToString(customerlevel);

        AppendTableRow(row, idstring);
        AppendTableRow(row, levelstring);
        AppendTableRow(row, name);
        AppendTableRow(row, contact);

        free(idstring);
        free(levelstring);
        free(name);
        free(contact);

        AppendTable(goback, row);

        customer = customer->next;
    }
    // 返回值
    return goback;
}
