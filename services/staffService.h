#ifndef STAFFH
#define STAFFH

#include "../data/linkedList.h"
#include "../data/staff.h"
#include "../data/table.h"

/*员工登录*/
Table *LogIn(Table *); // 参数表格中包括：password，Id；返回的Table中是Success（1/0）

/* 加入一个员工 */
Table *AddStaff();
/* 删除一个员工 */
Table *RemoveStaff(Table *); // 参数Table* 中要含有 要删除员工的员工工号
/*查询一名员工的信息*/
Table *GetItemOfOneStaff(Table *); // 参数Table* 中要含有 要查询员工的员工工号
/*查询所有员工的信息*/
Table *GetItemOfAllStaff();
/*改变一名员工的已有信息*/
Table *UpdateStaff(Table *); // 参数Table* 中要含有 要改变员工的员工工号

/*构造员工信息表头Title*/
Table *CreateStaffTableTitle(void);

/*将数字字符串转化为int型整数*/
int atoi(const char *ch);
/*将int整数转化为字符串*/
const char *itoa(int a);

#endif
