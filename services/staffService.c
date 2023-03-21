#include "staffService.h"
#include "../data/permission.h"
#include "../utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*将int整数转化为字符串*/
/* const char *itoa0(char *ch, int num) */
/* { */
/*     sprintf(ch, "%d", num); */
/*     return ch; */
/* } */
/*字符串清零*/
char *empty(char *ch)
{
    int k = strlen(ch);
    for (int i = 0; i < k; i++)
        ch[i] = '\0';
    return ch;
}

// 创建一个属于员工信息表头的表格，其首行Title依次为：工号，姓名，可用性，联系方式
Table *CreateStaffTableTitle()
{
    TableRow *staffTableRowTitle = NewTableRow();     // 创建一个“员工信息表头表格行”
    AppendTableRow(staffTableRowTitle, "id");         // 表头第一列赋名“Id”    工号
    AppendTableRow(staffTableRowTitle, "name");       // 表头第二列赋名“name”姓名
    AppendTableRow(staffTableRowTitle, "isEnabled");  // 表头第三列赋名“IsEnabled”可用性
    AppendTableRow(staffTableRowTitle, "contact");    // 表头第四列赋名“contact”联系方式
    AppendTableRow(staffTableRowTitle, "permission"); // 表头第五列赋名“permission”（员工）权限
    Table *staffTableTitle = NewTable(staffTableRowTitle, "Staff Items Table Titles"); // 创建一个员工信息表格
    return staffTableTitle;
}

/*添加一位员工*/
Table *AddStaff(Table *newStaff)
{
    if (newStaff != NULL)
    { // 若传入的不是非空表格
        TableRow *titleRow = GetTableTitle(newStaff);
        LinkedList *title = titleRow->items;
        TableRow *row = GetRowByIndex(newStaff, 1); // 获取员工信息行
        int isEnabled = -1;
        const char *password = NULL;
        const char *contact = NULL;
        const char *name = NULL;
        const char *permission = NULL;
        while (title != NULL)
        {
            char *oneTitle = (char *)title->data; // 获取一个标题
            if (strcmp(oneTitle, "password"))
                password = GetRowItemByColumnName(newStaff, row, oneTitle);
            if (strcmp(oneTitle, "name"))
                name = GetRowItemByColumnName(newStaff, row, oneTitle);
            ;
            if (strcmp(oneTitle, "contact"))
                contact = GetRowItemByColumnName(newStaff, row, oneTitle);
            if (strcmp(oneTitle, "isEnabled"))
                isEnabled = atoi(GetRowItemByColumnName(newStaff, row, oneTitle));
            if (strcmp(oneTitle, "permission"))
                permission = GetRowItemByColumnName(newStaff, row, oneTitle);

            title = title->next;
        }

        Staff *staff;
        PermissionEntry *staffPermission = NULL;              // 新擦黄建员工的权限
        staff = NewStaff(isEnabled, name, password, contact); // 新建一个具有上述信息的员工
        int judge1 = AppendStaff(staff); // 追加这个新员工，judge1：判断是否追加成功
        if (judge1 == 0)
            StaffSave(); // 若追加成功，则保存这个更改
        else
        { // 若追加失败，返回一个空表格，指出添加员工时出错
            TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
            Table *remark_table = NewTable(blank_row, "There is fault exsiting in adding a new staff!");
            /*创建一个空表格，只有备注说明添加新员工出错*/
            return remark_table; // 返回空表格
        }

        int staffid = GetStaffId(staff);
        int permissionInt = atoi(permission);
        staffPermission = NewPermissionEntry(staffid, &permissionInt); // 创建该员工的权限条目
        int judge2 = AppendPermissionEntry(staffPermission);
        if (judge2 == 0)
            PermissionSave(); // 若追加成功，则保存这个更改
        else
        { // 若追加失败，返回一个空表格，指出添加员工时出错
            TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
            Table *remark_table =
                NewTable(blank_row, "There is fault exsiting in adding a permission of one new staff!");
            /*创建一个空表格，只有备注说明添加新员工权限出错*/
            return remark_table; // 返回空表格
        }

        return newStaff;
    }

    else
    {                                        // 若传入的是空表格
        TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "There is no provided information ");
        /*创建一个空表格，只有备注说明 没有给予任何信息*/
        return remark_table; // 返回空表格
    }
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
        int Id = atoi(GetRowItemByColumnName(staff, row, "id")); // 得到要查询的员工的工号
        PermissionEntry *permission = GetPermissionEntryByStaffId(Id);

        Staff *targetStaff = GetStaffById(Id); // 依据工号找到要查询的员工
        /*构建这个要查询的员工信息表格*/
        Table *targetStaffTitle = CreateStaffTableTitle(); // 创建表头
        TableRow *targetStaffItem = NewTableRow();         // 创建一个具有该员工具体信息的表格行
        free(AppendTableRow(targetStaffItem, LongLongToString(GetStaffId(targetStaff))));           // 加入Id
        free(AppendTableRow(targetStaffItem, CloneString(GetStaffName(targetStaff))));              // 加入名字
        free(AppendTableRow(targetStaffItem, LongLongToString(GetStaffAvailability(targetStaff)))); // 加入可用性
        free(AppendTableRow(targetStaffItem, CloneString(GetStaffContact(targetStaff)))); // 加入联系方式
        free(AppendTableRow(targetStaffItem, PermissionToString(permission)));            // 加入员工权限
        int judge =
            AppendTable(targetStaffTitle, targetStaffItem); // 将员工具体信息追加到表格中，judge用来判断追加是否成功
        if (judge == 0)
            return targetStaffTitle; // 若成功，返回该表格
        else
        {                                        // 若失败，返回空表格，指出出错信息
            TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
            Table *remark_table = NewTable(blank_row, "There is fault exsiting in reading items of a staff!");
            /*创建一个空表格，只有备注说明查询员工信息时出错*/
            return remark_table; // 返回空表格
        }
    }

    else
    {
        TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "There is no provided information ");
        /*创建一个空表格，只有备注说明 没有给予任何信息*/
        return remark_table; // 返回空表格
    }
}

/*查询所有员工信息*/
Table *GetItemOfAllStaff(Table *table)
{
    const LinkedList *allStaff = GetAllStaff();    // 用allStaff获取所有的员工
    Table *allStaffItem = CreateStaffTableTitle(); // 创建一个员工信息标题表格行

    const LinkedList *p = allStaff; // p指向员工信息链表头
    while (p != NULL)
    {
        Staff *staff = (Staff *)p->data;        // 获取当前员工
        TableRow *oneStaffItem = NewTableRow(); // 创建一个员工信息标题表格行
        PermissionEntry *permission = GetPermissionEntryByStaffId(GetStaffId(staff));      // 获取该员工的权限
        free(AppendTableRow(oneStaffItem, LongLongToString(GetStaffId(staff))));           // 向表格中加入工号
        free(AppendTableRow(oneStaffItem, CloneString(GetStaffName(staff))));              // 向表格中加入名字
        free(AppendTableRow(oneStaffItem, LongLongToString(GetStaffAvailability(staff)))); // 向表格中加入可用性
        free(AppendTableRow(oneStaffItem, CloneString(GetStaffContact(staff)))); // 向表格中加入联系方式
        free(AppendTableRow(oneStaffItem, PermissionToString(permission)));
        int judge = AppendTable(allStaffItem, oneStaffItem); // 判断是否加入员工具体信息表格行成功
        if (judge != 0)
        {                                        // 若加入失败
            TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
            Table *remark_table = NewTable(blank_row, "There is fault exsiting in searching for all items of staffs !");
            /*创建一个空表格，只有备注说明查询所有员工信息时出错*/
            return remark_table; // 返回空表格
        }
        p = p->next; // p指向下一位员工
        /*FreeTableRow(oneStaffItem);   //释放当前员工的表格行*/
    }

    return allStaffItem; // 返回全部员工信息表格
}

/*改变一名员工的已有信息*/ /*改密码吗*/ /*没有更改权限的代码，待完成*/
/*   Table*staff传入要修改的信息   */
Table *UpdateStaff(Table *staff)
{
    if (staff != NULL)
    {
        TableRow *titleRow = GetTableTitle(staff);
        LinkedList *title = titleRow->items;
        const char *oneTitle = (char *)title->data; // 获取标题名

        TableRow *row = GetRowByIndex(staff, 1);                 // 得到这个员工的具体信息所在行
        int id = atoi(GetRowItemByColumnName(staff, row, "id")); // 得到这个员工的工号id
        Staff *objectStaff = GetStaffById(id);                   // 根据id得到这个员工
        PermissionEntry *staffPermission = GetPermissionEntryByStaffId(id); // 获得该员工的权限条目

        char name[50];
        int isEnabled = -1;
        char contact[20];
        char operationString[30];
        /*获取要修改的信息*/
        while (title != NULL)
        {
            oneTitle = (char *)title->data;                                       // 获取标题
            const char *givenItem = GetRowItemByColumnName(staff, row, oneTitle); // 获取标题对应的信息
            /*获取保存要修改的信息*/
            if (strcmp(oneTitle, "isEnabled"))
            {
                isEnabled = atoi(givenItem);
                SetStaffAvailability(objectStaff, isEnabled);
            }
            if (strcmp(oneTitle, "contact"))
            {
                strcpy(contact, givenItem);
                SetStaffContact(objectStaff, contact);
            }
            if (strcmp(oneTitle, "name"))
            {
                strcpy(name, givenItem);
                SetStaffName(objectStaff, name);
            }
            if (strcmp(oneTitle, "permission"))
            {
                strcpy(operationString, givenItem);
                Operation operation = StringToOperation(operationString);
                int allow = atoi(GetRowItemByColumnName(staff, row, "allow"));
                SetPermissionEntryPermissions(staffPermission, operation, allow);
            }

            title = title->next;
        }
        /*修改员工信息*/

        StaffSave();      // 保存员工基本信息修改
        PermissionSave(); // 保存权限修改

        return staff;
    }
    else
    {
        TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "There is no provided information ");
        /*创建一个空表格，只有备注说明 没有给予任何信息*/
        return remark_table; // 返回空表格
    }
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
        SetStaffAvailability(objectStaff, 0);  // 将这个员工设置为不可用
        /*创建一个表格，包含已经删除的员工的信息*/
        Table *oriStaff = CreateStaffTableTitle();
        TableRow *oriStaffItem = NewTableRow();
        char tempString1[20], tempString2[20];
        PermissionEntry *permission = GetPermissionEntryByStaffId(GetStaffId(objectStaff));
        free(AppendTableRow(oriStaffItem, LongLongToString(GetStaffId(objectStaff))));           // 加入Id
        free(AppendTableRow(oriStaffItem, CloneString(GetStaffName(objectStaff))));              // 加入名字
        free(AppendTableRow(oriStaffItem, LongLongToString(GetStaffAvailability(objectStaff)))); // 加入可用性
        free(AppendTableRow(oriStaffItem, CloneString(GetStaffContact(objectStaff))));           // 加入联系方式
        free(AppendTableRow(oriStaffItem, PermissionToString(permission)));
        int judge = AppendTable(oriStaff, oriStaffItem); // 判断是否添加入表格成功

        /*释放该员工*/
        RemoveStaff(objectStaff); // 释放这个员工
        if (judge0 == 0)
            StaffSave(); // 若释放成功，则保存这个更改
        else
        {                                        // 若释放失败，返回一个空表格
            TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
            Table *remark_table = NewTable(blank_row, "There is fault exsiting in deleting one staff !");
            /*创建一个空表格，只有备注说明删除员工出错*/
            return remark_table; // 返回空表格
        }
        /*释放该员工的权限*/
        RemovePermissionEntry(permission);
        PermissionSave();

        /*返回删除的员工信息表格*/
        if (judge == 0)
            return oriStaff; // 若添加表格成功，则返回该表格
        else
        {                                        // 若失败，返回一个空表格
            TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
            Table *remark_table = NewTable(blank_row, "There is fault exsiting in recording one deleted staff !");
            /*创建一个空表格，只有备注说明添加员工信息表格出错*/
            return remark_table; // 返回空表格
        }
    }
    else
    {
        TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "There is no provided information ");
        /*创建一个空表格，只有备注说明 没有给予任何信息*/
        return remark_table; // 返回空表格
    }
}
