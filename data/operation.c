#include "design/operation.h"
#include <string.h>

static const char *operationStrings[] = {
        "登录", "添加员工", "删除员工", "更新员工", "读取员工", "读取日志",
        "读取统计", "添加库存", "删除库存", "更新库存", "读取库存", "添加商品",
        "删除商品", "更新商品", "读取商品", "添加客户", "删除客户", "更新客户",
        "读取客户", "添加订单", "删除订单", "更新订单", "读取订单", "添加折扣",
        "删除折扣", "更新折扣", "读取折扣", "添加退款", "删除退款", "更新退款",
        "读取退款", "添加货损", "删除货损", "更新货损", "读取货损" };

const char *OperationToString(Operation operation)
{
    return operationStrings[operation];
}

Operation StringToOperation(const char *string)
{
    for (int i = 0; i < OPERATION_COUNT; i++)
    {
        if (strcmp(string, operationStrings[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}
