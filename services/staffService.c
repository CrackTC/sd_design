#include "design/staffService.h"
#include "design/permission.h"
#include "design/utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 创建一个属于员工信息表头的表格，其首行Title依次为：工号，姓名，可用性，联系方式
Table *CreateStaffTableTitle()
{
    TableRow *staffTableRowTitle = NewTableRow();     // 创建一个“员工信息表头表格行”
    AppendTableRow(staffTableRowTitle, "员工编号");
    AppendTableRow(staffTableRowTitle, "员工可用性");
    AppendTableRow(staffTableRowTitle, "员工姓名");
    AppendTableRow(staffTableRowTitle, "员工联系方式");
    AppendTableRow(staffTableRowTitle, "员工权限");
    Table *staffTableTitle = NewTable(staffTableRowTitle, NULL); // 创建一个员工信息表格
    return staffTableTitle;
}

/*添加一位员工*/
Table *AddStaff(Table *newStaff)
{
    if (newStaff == NULL)
        return NewTable(NULL, "您没有给予任何信息 ");

    TableRow *row = GetRowByIndex(newStaff, 1); // 获取员工信息行
    const char *password = GetRowItemByColumnName(newStaff, row, "员工密码");
    const char *name = GetRowItemByColumnName(newStaff, row, "员工姓名");
    const char *contact = GetRowItemByColumnName(newStaff, row, "员工联系方式");
    int isEnabled = atoi(GetRowItemByColumnName(newStaff, row, "员工可用性"));
    const char *permissionString = GetRowItemByColumnName(newStaff, row, "员工权限");

    Staff *staff;
    staff = NewStaff(isEnabled, name, password, contact); // 新建一个具有上述信息的员工

    int judge1 = AppendStaff(staff); // 追加这个新员工，judge1：判断是否追加成功
    if (judge1 != 0)
        return NewTable(NULL, "添加新员工出错!");

    StaffSave(); // 若追加成功，则保存这个更改

    int staffId = GetStaffId(staff);
    int *permission = ParsePermission(permissionString);
    PermissionEntry *staffPermission = NewPermissionEntry(staffId, permission); // 创建该员工的权限条目
    free(permission);

    int judge2 = AppendPermissionEntry(staffPermission);
    if (judge2 != 0)
        return NewTable(NULL, "添加新员工权限出错!");

    PermissionSave(); // 若追加成功，则保存这个更改
    return NULL;
}

/*查询一名员工的信息*/
Table *GetItemOfOneStaff(Table *staff)
{
    // TableRow* titleRow = GetTableTitle(staff);
    // LinkedList* title = titleRow->items;
    // const char* oneTitle = (char*)title->data;//获取标题名
    if (staff != NULL)
    {
        TableRow *row = GetRowByIndex(staff, 1);
        int id = atoi(GetRowItemByColumnName(staff, row, "员工编号")); // 得到要查询的员工的工号
        Staff *targetStaff = GetStaffById(id);                   // 依据工号找到要查询的员工
        if (targetStaff != NULL)
        { // 若可以找到相应员工
            PermissionEntry *permission = GetPermissionEntryByStaffId(id);

            /*构建这个要查询的员工信息表格*/
            Table *targetStaffTitle = CreateStaffTableTitle(); // 创建表头
            TableRow *targetStaffItem = NewTableRow();         // 创建一个具有该员工具体信息的表格行
            free(AppendTableRow(targetStaffItem, LongLongToString(GetStaffId(targetStaff)))); // 加入Id
            free(AppendTableRow(targetStaffItem, CloneString(GetStaffName(targetStaff))));    // 加入名字
            free(AppendTableRow(targetStaffItem,
                    LongLongToString(GetStaffAvailability(targetStaff))));        // 加入可用性
            free(AppendTableRow(targetStaffItem, CloneString(GetStaffContact(targetStaff)))); // 加入联系方式
            free(AppendTableRow(targetStaffItem, PermissionToString(permission)));            // 加入员工权限
            int judge =
                    AppendTable(targetStaffTitle, targetStaffItem); // 将员工具体信息追加到表格中，judge用来判断追加是否成功
            if (judge == 0)
                return targetStaffTitle; // 若成功，返回该表格
            else
            {                                        // 若失败，返回空表格，指出出错信息
                TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
                Table *remark_table = NewTable(blank_row, "查询员工信息时出错");
                /*创建一个空表格，只有备注说明查询员工信息时出错*/
                return remark_table; // 返回空表格
            }
        }
        else
        {                                        // 若找不到相应员工
            TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
            Table *remark_table = NewTable(blank_row, "没有该员工 !");
            /*创建一个空表格，只有备注说明 没有该员工*/
            return remark_table; // 返回空表格
        }
    }
    else
    {                                        // 若传入的staff是空表格
        TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "没有给予任何信息 ");
        /*创建一个空表格，只有备注说明 没有给予任何信息*/
        return remark_table; // 返回空表格
    }
}

/*查询所有员工信息*/
Table *GetItemOfAllStaff(Table *table)
{
    const LinkedList *allStaff = GetAllStaff();    // 用allStaff获取所有的员工
    Table *allStaffItem = CreateStaffTableTitle(); // 创建一个员工信息标题表格行

    const LinkedList *p = allStaff;
    if (p == NULL)
    {                                        // 如果没有员工
        TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "没有任何员工 ");
        /*创建一个空表格，只有备注说明 没有员工*/
        return remark_table; // 返回空表格
    }

    int count = 0;
    while (p != NULL)
    {                                           // 至少有一个员工
        Staff *staff = (Staff *)p->data;        // 获取当前员工
        count++;
        TableRow *oneStaffItem = NewTableRow(); // 创建一个员工信息标题表格行
        PermissionEntry *permission = GetPermissionEntryByStaffId(GetStaffId(staff));      // 获取该员工的权限
        free(AppendTableRow(oneStaffItem, LongLongToString(GetStaffId(staff))));           // 向表格中加入工号
        free(AppendTableRow(oneStaffItem, LongLongToString(GetStaffAvailability(staff)))); // 向表格中加入可用性
        free(AppendTableRow(oneStaffItem, CloneString(GetStaffName(staff))));              // 向表格中加入名字
        free(AppendTableRow(oneStaffItem, CloneString(GetStaffContact(staff)))); // 向表格中加入联系方式
        free(AppendTableRow(oneStaffItem, PermissionToString(permission)));
        int judge = AppendTable(allStaffItem, oneStaffItem); // 判断是否加入员工具体信息表格行成功
        if (judge != 0)
        {                                        // 若加入失败
            TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
            Table *remark_table = NewTable(blank_row, "查询所有员工信息时出错 !");
            /*创建一个空表格，只有备注说明查询所有员工信息时出错*/
            return remark_table; // 返回空表格
        }
        p = p->next; // p指向下一位员工
    }

    size_t len = strlen("查询到") + IntegerStringLength(count) + strlen("条信息") + 1;
    char *remark = malloc(len);
    sprintf(remark, "查询到%d条信息", count);
    SetTableRemark(allStaffItem, remark);
    free(remark);

    return allStaffItem; // 返回全部员工信息表格
}

/*改变一名员工的已有信息*/ /*改密码吗*/
/*   Table*staff传入要修改的信息   */
Table *UpdateStaff(Table *staff)
{
    if (staff == NULL)
        return NewTable(NULL, "没有提供信息 ");

    TableRow *row = GetRowByIndex(staff, 1);                 // 得到这个员工的具体信息所在行
    int id = atoi(GetRowItemByColumnName(staff, row, "员工编号")); // 得到这个员工的工号id
    Staff *objectStaff = GetStaffById(id);                   // 根据id得到这个员工

    if (objectStaff == NULL)
        return NewTable(NULL, "没有该工号的员工!");

    SetStaffName(objectStaff, GetRowItemByColumnName(staff, row, "员工姓名"));
    SetStaffAvailability(objectStaff, atoi(GetRowItemByColumnName(staff, row, "员工可用性")));
    SetStaffContact(objectStaff, GetRowItemByColumnName(staff, row, "员工联系方式"));
    SetStaffPassword(objectStaff, GetRowItemByColumnName(staff, row, "员工密码"));

    int *permission = ParsePermission(GetRowItemByColumnName(staff, row, "员工权限"));
    PermissionEntry *entry = GetPermissionEntryByStaffId(id);
    for (int i = 0; i < OPERATION_COUNT; i++)
        SetPermissionEntryPermissions(entry, i, permission[i]);
    free(permission);

    StaffSave();      // 保存员工基本信息修改
    PermissionSave(); // 保存权限修改

    return NULL;
}

/* 删除一个员工 */
Table *DeleteStaff(Table *staff)
{
    if (staff != NULL)
    {
        TableRow *titleRow = GetTableTitle(staff);
        LinkedList *title = titleRow->items;
        const char *oneTitle = (char *)title->data; // 获取标题名

        TableRow *row = GetRowByIndex(staff, 1);                     // 得到这个员工的具体信息所在行
        int id = atoi(GetRowItemByColumnName(staff, row, oneTitle)); // 得到这个员工的工号id
        /*FreeTable(staff);                */                        // 释放原有的员工具体信息表格

        Staff *objectStaff = GetStaffById(id); // 根据id得到这个员工
//		if (objectStaff != NULL)
//		{                                         // 若可以找到这个员工
//			SetStaffAvailability(objectStaff, 0); // 将这个员工设置为不可用
//			/*创建一个表格，包含已经删除的员工的信息*/
//			Table* oriStaff = CreateStaffTableTitle();
//			TableRow* oriStaffItem = NewTableRow();
//			PermissionEntry* permission = GetPermissionEntryByStaffId(GetStaffId(objectStaff));
//			free(AppendTableRow(oriStaffItem, LongLongToString(GetStaffId(objectStaff)))); // 加入Id
//			free(AppendTableRow(oriStaffItem, CloneString(GetStaffName(objectStaff))));    // 加入名字
//			free(AppendTableRow(oriStaffItem,
//					LongLongToString(GetStaffAvailability(objectStaff))));     // 加入可用性
//			free(AppendTableRow(oriStaffItem, CloneString(GetStaffContact(objectStaff)))); // 加入联系方式
//			free(AppendTableRow(oriStaffItem, PermissionToString(permission)));
//			int judge = AppendTable(oriStaff, oriStaffItem); // 判断是否添加入表格成功

        /*释放该员工*/
        RemoveStaff(objectStaff); // 释放这个员工
        StaffSave(); // 若释放成功，则保存这个更改
//
//			/*释放该员工的权限*/
//			RemovePermissionEntry(permission);
//			PermissionSave();
//
//			/*返回删除的员工信息表格*/
//			if (judge == 0)
//				return oriStaff; // 若添加表格成功，则返回该表格
//			else
//			{                                        // 若失败，返回一个空表格
//				TableRow* blank_row = NewTableRow(); // 创建一个空的表格行
//				Table* remark_table = NewTable(blank_row, "将该员工的信息填入表格时出错 !");
//				/*创建一个空表格，只有备注说明添加员工信息表格出错*/
//				return remark_table; // 返回空表格
//			}
//		}
//		else
//		{                                        // 若没有这个员工
//			TableRow* blank_row = NewTableRow(); // 创建一个空的表格行
//			Table* remark_table = NewTable(blank_row, "没有该员工 !");
//			/*创建一个空表格，只有备注说明 没有该员工*/
//			return remark_table; // 返回空表格
//		}
//	}
//	else
//	{                                        // 若传入的表格是空表
//		TableRow* blank_row = NewTableRow(); // 创建一个空的表格行
//		Table* remark_table = NewTable(blank_row, "没有提供任何信息 ");
//		/*创建一个空表格，只有备注说明 没有给予任何信息*/
//		return remark_table; // 返回空表格
    }
    return NewTable(NULL, "删除失败: 员工为NULL");
}

/*暂时无法获得/修改staff的密码*/
///*修改密码*/ /*table中包括：要修改密码的员工，要修改的密码 */
// Table* UpdatePassword(Table* table) {
//	if (table == NULL) {//传进来的是空表格
//		TableRow* blank_row = NewTableRow();                   //创建一个空的表格行
//		Table* remark_table = NewTable(blank_row, "There is no provided information ");
//		/*创建一个空表格，只有备注说明 没有给予任何信息*/
//		return remark_table;//返回空表格
//	}
//	else {//传入的表格非空
//		TableRow* row = GetRowByIndex(table, 1);
//		const char* newPassword = GetRowItemByColumnName(table, row, "password");
//		int staffid = atoi(GetRowItemByColumnName(table, row, "id"));
//		Staff* staff=GetStaffById(staffid);
//		if (staff == NULL) {//若没有找到该员工
//			TableRow* blank_row = NewTableRow();                   //创建一个空的表格行
//			Table* remark_table = NewTable(blank_row, "There is no target-staff ! ");
//			/*创建一个空表格，只有备注说明 没有给予任何信息*/
//			return remark_table;//返回空表格
//		}
//		else {//由目标员工
//
//		}
//
//	}
// }
