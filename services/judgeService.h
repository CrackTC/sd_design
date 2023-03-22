#ifndef JUDGE_H
#define JUDGE_H
#include "../data/operation.h"
#include "../data/table.h"
/*
 *鉴权函数
 *函数参数:
 *         staffId 员工编号
 *		  hasPermission 用于存放是否能调用相关函数 能调用则为1 否则为0
 *		  password 员工密码
 *		  operation 要进行的相关操作
 *返回值:  如果能调用函数 则返回调用函数后的表格 否则返回null
 **/
Table *Judge(int staffId, int *hasPermission, const char *password, Operation operation);

/*
 *写日志函数
 * 函数参数:
 *			staffid 员工编号
 *			hasPermission 用于鉴别是否能调用相关函数 能调用则为1 否则为0
 *			input:要写入日志的表格
 *		    operation 要进行的相关操作
 *返回值:写日志函数返回的表格
 **/
Table *AddJournalResult(int staffId, int *hasPermission, Operation operation, Table *inpuit);
#endif
