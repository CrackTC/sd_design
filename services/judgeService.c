#include "judgeService.h"
#include "../data/amount.h"
#include "../data/basicDiscount.h"
#include "../data/customer.h"
#include "../data/inventory.h"
#include "../data/item.h"
#include "../data/journal.h"
#include "../data/linkedList.h"
#include "../data/lossEntry.h"
#include "../data/operation.h"
#include "../data/order.h"
#include "../data/permission.h"
#include "../data/profit.h"
#include "../data/refundEntry.h"
#include "../data/serialization.h"
#include "../data/staff.h"
#include "../data/time.h"
#include "../utils.h"
#include "inventoryService.h"
#include "journalService.h"
#include <stdio.h>
#include <string.h>

/*
 * 用于判断是否有相应的权限
 * 参数:staffId 员工Id
 *      operation 该操作在枚举数组中对应的值
 * 有权限返回1 无则返回0
 * */
int IfHasThisPermission(int staffId, Operation operation)
{
    const int *PermissionArray = GetPermissionEntryPermissions(GetPermissionEntryByStaffId(staffId));
    if (PermissionArray[operation] == 1)
        return 1;
    else
        return 0;
}

Table *judge(int staffId, int *hasPermission, const char *password, Operation operation)
{
    // 判断该职工是否还在工作
    if (GetStaffAvailability(GetStaffById(staffId)))

    {
        // 判断是否为登录操作
        if (operation == OP_LOGIN)
        {
            // 判断是否 登录成功 如成功 返回员工姓名的表格
            if (VerifyStaffPassword(GetStaffById(staffId), password))
            {
                // 用于存放结果的表格
                Table *table;
                // 创建好表格的表头
                TableRow *row = NewTableRow();
                AppendTableRow(row, "员工姓名");
                table = NewTable(row, NULL);
                // 添加表格的具体内容
                row = NewTableRow();
                free(AppendTableRow(row, CloneString(GetStaffName(GetStaffById(staffId)))));
                AppendTable(table, row);
                // 返回创建好的表格
                return table;
            }
            else
                *hasPermission = 0; // 登录失败 员工id和密码不匹配
        }
        else
        {
            // 判断员工是否具有相应操作的权限
            if (IfHasThisPermission(staffId, operation))
            {
                *hasPermission = 1;
            } // 该职工没有调用相应函数的权限
            else
                *hasPermission = 0;
        }
    }
    else // 职工已不工作 无权限
        *hasPermission = 0;
    return NULL;
}

// 写日志函数
Table *AddTableResult(int staffId, int *hasPermission, Operation operation, Table *input)
{
    if (*hasPermission == 1)
    {
        return AddJournal(input, staffId, operation);
    }
    return NULL;
}
