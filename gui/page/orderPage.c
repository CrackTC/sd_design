#include "design/crud.h"
#include "design/linkedList.h"
#include "design/operation.h"
#include "design/table.h"
#include "design/saleService.h"
#include "design/utils.h"
#include "../../config.h"
#include "design/layout.h"
#include "design/mainWindow.h"

void SendOrderRequest(struct MainWindowData *data)
{
    Read(data, &data->dataArray[ORDER_INDEX], GetAllOrder, "缺少权限：读取订单", OP_READ_ORDER);
}

void OrderLookup(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[ORDER_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[ORDER_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *titleRow = CloneRow(GetTableTitle(data->dataArray[ORDER_INDEX].table));
            Table *table = NewTable(titleRow, "");
            AppendTable(table, CloneRow(rowNow->data));
            PushWindow(NewOrderDetail("订单详情", table));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个订单");
}

void OrderAdd(struct MainWindowData *data)
{
    LinkedList *itemCheckNow = data->dataArray[ITEM_INDEX].checkList->next;
    LinkedList *itemRowNow = data->dataArray[ITEM_INDEX].table->rows->next;
    while (itemCheckNow != NULL)
    {
        if (*(int *)itemCheckNow->data == 1)
        {
            LinkedList *customerCheckNow = data->dataArray[CUSTOMER_INDEX].checkList->next;
            LinkedList *customerRowNow = data->dataArray[CUSTOMER_INDEX].table->rows->next;
            while (customerCheckNow != NULL)
            {
                if (*(int *)customerCheckNow->data == 1)
                {
                    TableRow *row = NewTableRow();
                    AppendTableRow(row, "商品编号");
                    AppendTableRow(row, "商品名称");
                    AppendTableRow(row, "客户编号");
                    AppendTableRow(row, "客户姓名");
                    AppendTableRow(row, "购买数量");
                    Table *table = NewTable(row, "");

                    row = NewTableRow();
                    AppendTableRow(row,
                            GetRowItemByColumnName(data->dataArray[ITEM_INDEX].table, itemRowNow->data, "商品编号"));
                    AppendTableRow(row,
                            GetRowItemByColumnName(data->dataArray[ITEM_INDEX].table, itemRowNow->data, "商品名称"));
                    AppendTableRow(row,
                            GetRowItemByColumnName(data->dataArray[CUSTOMER_INDEX].table, customerRowNow->data,
                                    "客户编号"));
                    AppendTableRow(row,
                            GetRowItemByColumnName(data->dataArray[CUSTOMER_INDEX].table, customerRowNow->data,
                                    "客户姓名"));
                    AppendTableRow(row, "0");
                    AppendTable(table, row);

                    PushWindow(NewOrderEdit("订单编辑", data->id, data->password, table, 0));
                    FreeTable(table);
                    return;
                }
                customerCheckNow = customerCheckNow->next;
                customerRowNow = customerRowNow->next;
            }
            data->messageCallback = MessageBoxCallback;
            data->message = CloneString("请确保在商品页面选择一个商品条目，且在客户页面选择一个客户条目");
            return;
        }
        itemCheckNow = itemCheckNow->next;
        itemRowNow = itemRowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请确保在商品页面选择一个商品条目，且在客户页面选择一个客户条目");
}

void OrderModify(struct MainWindowData *data)
{
    LinkedList *now = data->dataArray[ORDER_INDEX].checkList->next;
    LinkedList *rowNow = data->dataArray[ORDER_INDEX].table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            TableRow *row = NewTableRow();

            {
                AppendTableRow(row, "订单编号");
                AppendTableRow(row, "商品编号");
                AppendTableRow(row, "商品名称");
                AppendTableRow(row, "客户编号");
                AppendTableRow(row, "客户姓名");
                AppendTableRow(row, "购买数量");
            }

            Table *table = NewTable(row, "");

            {
                row = NewTableRow();
                AppendTableRow(row,
                        GetRowItemByColumnName(data->dataArray[ORDER_INDEX].table, rowNow->data, "订单编号"));
                AppendTableRow(row,
                        GetRowItemByColumnName(data->dataArray[ORDER_INDEX].table, rowNow->data, "商品编号"));
                AppendTableRow(row,
                        GetRowItemByColumnName(data->dataArray[ORDER_INDEX].table, rowNow->data, "商品名称"));
                AppendTableRow(row,
                        GetRowItemByColumnName(data->dataArray[ORDER_INDEX].table, rowNow->data, "客户编号"));
                AppendTableRow(row,
                        GetRowItemByColumnName(data->dataArray[ORDER_INDEX].table, rowNow->data, "客户姓名"));
                AppendTableRow(row,
                        GetRowItemByColumnName(data->dataArray[ORDER_INDEX].table, rowNow->data, "购买数量"));
                AppendTable(table, row);
            }

            PushWindow(NewOrderEdit("订单编辑", data->id, data->password, table, 1));
            FreeTable(table);
            return;
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("请选择一个订单条目");
}

void OrderDelete(int ok, void *parameter)
{
    struct MainWindowData *data = parameter;
    Delete(ok, parameter, &data->dataArray[ORDER_INDEX], RemoveAnOrder, "缺少权限：删除订单", "订单编号",
            OP_DELETE_ORDER);
}

void ConfirmOrderDelete(struct MainWindowData *data)
{
    data->messageCallback = OrderDelete;
    data->message = CloneString("请选择一个订单条目");
}

void OrderPageLayout(struct nk_context *context, struct Window *window)
{
    struct MainWindowData *data = window->data;
    DrawMessageBox(context, "", data->message != NULL, data->message, data->messageCallback, data);
    BasicFilterLayout(context, "订单", &data->dataArray[ORDER_INDEX]);
    nk_layout_row_static(context, 10, 0, 0);
    OperationLayout(context,
            OP_TYPE_GET | OP_TYPE_DETAIL | OP_TYPE_ADD | OP_TYPE_DELETE | OP_TYPE_UPDATE,
            (OperationHandler)SendOrderRequest,
            (OperationHandler)OrderLookup,
            (OperationHandler)OrderAdd,
            (OperationHandler)ConfirmOrderDelete,
            (OperationHandler)OrderModify,
            data);
    DrawSeparateLine(context);
    char *from, *to;
    DateRangeFilterLayout(context, "筛选购买时间", &from, &to);
    DrawSeparateLine(context);
    PageResultLayout(context, &data->dataArray[ORDER_INDEX], "购买时间", from, to);
}
