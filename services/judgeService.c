#include "judgeService.h"
#include "../data/amount.h"
#include "../data/permission.h"
#include "../data/staff.h"
#include "../utils.h"
#include "journalService.h"

/*
 * 用于判断是否有相应的权限
 * 参数:staffId 员工Id
 *      operation 该操作在枚举数组中对应的值
 * 有权限返回1 无则返回0
 * */
int HasPermission(int staffId, Operation operation)
{
    const int *PermissionArray = GetPermissionEntryPermissions(GetPermissionEntryByStaffId(staffId));
    if (PermissionArray[operation] == 1)
        return 1;
    else
        return 0;
}

Table *Judge(int staffId, int *hasPermission, const char *password, Operation operation)
{
    Staff *staff = GetStaffById(staffId);
    if (staff == NULL) {
        *hasPermission = 0;
        return NULL;
    }
    // 判断该职工是否还在工作
    if (GetStaffAvailability(staff))
    {
        // 判断是否为登录操作
        if (operation == OP_LOGIN)
        {
            // 判断是否 登录成功 如成功 返回员工姓名的表格
            if (VerifyStaffPassword(staff, password))
            {
                // 用于存放结果的表格
                Table *table;
                // 创建好表格的表头
                TableRow *row = NewTableRow();
                AppendTableRow(row, "员工姓名");
                table = NewTable(row, NULL);
                // 添加表格的具体内容
                row = NewTableRow();
                free(AppendTableRow(row, CloneString(GetStaffName(staff))));
                AppendTable(table, row);
                // 返回创建好的表格
                return table;
            }
            else
                *hasPermission = 0; // 登录失败 员工id和密码不匹配
        }
        else
            *hasPermission = HasPermission(staffId, operation);
    }
    else // 职工已不工作 无权限
        *hasPermission = 0;
    return NULL;
}