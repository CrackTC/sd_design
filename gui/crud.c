#include "design/crud.h"
#include "design/layout.h"
#include "design/judgeService.h"
#include "design/utils.h"
#include "design/journalService.h"

void Delete(int ok, void *parameter, struct PageData *pageData, Service deleteService, const char *permissionError, const char *idTitle)
{
    MessageBoxCallback(ok, parameter);
    if (ok == 0)
    {
        return;
    }

    struct MainWindowData *data = parameter;

    int hasPermission;
    Judge(data->id, &hasPermission, data->password, OP_DELETE_CUSTOMER);
    if (!hasPermission)
    {
        data->messageCallback = MessageBoxCallback;
        data->message = CloneString(permissionError);
        return;
    }

    LinkedList *now = pageData->checkList->next;
    LinkedList *rowNow = pageData->table->rows->next;
    while (now != NULL)
    {
        if (*(int *)now->data == 1)
        {
            char *id = GetRowItemByColumnName(pageData->table, rowNow->data, idTitle);

            TableRow *row = NewTableRow();
            AppendTableRow(row, CloneString(idTitle));
            Table *table = NewTable(row, NULL);

            row = NewTableRow();
            AppendTableRow(row, id);
            AppendTable(table, row);

            AddJournal(table, data->id, OP_DELETE_CUSTOMER);
            Table *response = deleteService(table);
            FreeTable(table);

            if (response != NULL)
            {
                int error = 0;
                if (response->remark != NULL && response->remark[0] != '\0')
                {
                    data->messageCallback = MessageBoxCallback;
                    data->message = CloneString(response->remark);
                    error = 1;
                }
                FreeTable(response);
                if (error)
                {
                    return;
                }
            }
        }
        now = now->next;
        rowNow = rowNow->next;
    }
    data->messageCallback = MessageBoxCallback;
    data->message = CloneString("删除成功");
}