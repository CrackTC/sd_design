#ifndef STAFFH
#define STAFFH

#include "../data/linkedList.h"
#include "../data/staff.h"
#include "../data/table.h"

/* 加入一个员工 */
Table *AddStaff(Table *);

/* 删除一个员工 */
Table *DeleteStaff(Table *); // 参数Table* 中要含有 要删除员工的员工工号

/*查询一名员工的信息*/
Table *GetItemOfOneStaff(Table *); // 参数Table* 中要含有 要查询员工的员工工号

/*查询所有员工的信息*/
Table *GetItemOfAllStaff(Table *);

/*改变一名员工的已有信息*/
Table *UpdateStaff(Table *); // 参数Table* 中要含有 要改变员工的员工工号

/*构造员工信息表头Title*/
Table *CreateStaffTableTitle(void);

#endif
