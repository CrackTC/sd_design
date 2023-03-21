#ifndef STATISTIC_H
#define STATISTIC_H

#include "../data/table.h"

// 读取所有的收支条目并计算总金额、输出总金额

/*  无输入
 *
 *  返回值：
 *      有收支条目：   yuan   jiao   cent
 *      无收支条目：备注返回“不存在收支条目”
 * * * * * */
Table *GetStatistics(__attribute__((unused)) Table *a);

#endif // STATISTIC_H
