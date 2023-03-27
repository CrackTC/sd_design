#include "../../config.h"
#include "design/journalService.h"
#include "design/judgeService.h"
#include "design/layout.h"
#include "design/operation.h"
#include "design/staffService.h"
#include "design/utils.h"
#include <malloc.h>
#include <string.h>

struct Data
{
    struct EditData *editData;
    int enabledCheck;
    int permissionChecks[OPERATION_COUNT];
};

static void SendRequest(struct Data *data)
{
    int hasPermission;
    Operation operation = data->editData->modify ? OP_UPDATE_STAFF : OP_ADD_STAFF;
    Judge(data->editData->id, &hasPermission, data->editData->password, operation);
    if (!hasPermission)
    {
        data->editData->messageCallback = FinishCallback;
        data->editData->message = CloneString("没有权限");
        return;
    }

    TableRow *row = NewTableRow();
    if (data->editData->modify)
    {
        AppendTableRow(row, "员工编号");
    }
    AppendTableRow(row, "员工可用性");
    AppendTableRow(row, "员工密码");
    AppendTableRow(row, "员工姓名");
    AppendTableRow(row, "员工联系方式");
    AppendTableRow(row, "员工权限");
    Table *request = NewTable(row, NULL);

    row = NewTableRow();
    TableRow *sourceRow = GetRowByIndex(data->editData->data, 1);
    if (data->editData->modify)
    {
        AppendTableRow(row, GetRowItemByColumnName(data->editData->data, sourceRow, "员工编号"));
    }
    AppendTableRow(row, GetRowItemByColumnName(data->editData->data, sourceRow, "员工可用性"));
    AppendTableRow(row, GetRowItemByColumnName(data->editData->data, sourceRow, "员工密码"));
    AppendTableRow(row, GetRowItemByColumnName(data->editData->data, sourceRow, "员工姓名"));
    AppendTableRow(row, GetRowItemByColumnName(data->editData->data, sourceRow, "员工联系方式"));
    AppendTableRow(row, GetRowItemByColumnName(data->editData->data, sourceRow, "员工权限"));
    AppendTable(request, row);

    Table *response;
    if (data->editData->modify)
    {
        AddJournal(request, data->editData->id, OP_UPDATE_STAFF);
        response = UpdateStaff(request);
    }
    else
    {
        AddJournal(request, data->editData->id, OP_ADD_CUSTOMER);
        response = AddStaff(request);
    }
    FreeTable(request);

    if (response != NULL && response->remark != NULL && response->remark[0] != '\0')
    {
        data->editData->messageCallback = FinishCallback;
        data->editData->message = CloneString(response->remark);
    }
    else
    {
        data->editData->messageCallback = FinishCallback;
        data->editData->message = CloneString("操作成功完成");
    }

    if (response != NULL)
    {
        FreeTable(response);
    }
}

void StaffEditLayout(struct nk_context *context, Window *window)
{
    struct Data *data = window->data;
    DrawMessageBox(context, "", data->editData->message != NULL, data->editData->message, data->editData->messageCallback, data);
    TableRow *dataRow = GetRowByIndex(data->editData->data, 1);

    nk_style_push_font(context, &fontLarge->handle);
    {
        nk_layout_row_dynamic(context, 0, 1);
        nk_label(context, "员工编辑", NK_TEXT_CENTERED);

        nk_style_pop_font(context);
    }

    nk_style_push_font(context, &fontSmall->handle);
    {
        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 200);
                nk_label(context, "员工密码", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 300);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->editData->data, dataRow, "员工密码"), 512, nk_filter_default);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 200);
                nk_label(context, "员工姓名", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 100);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->editData->data, dataRow, "员工姓名"), 512, nk_filter_default);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 200);
                nk_label(context, "员工联系方式", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }
            nk_layout_row_push(context, 300);
            nk_edit_string_zero_terminated(
                context, (NK_EDIT_BOX | NK_EDIT_CLIPBOARD | NK_EDIT_AUTO_SELECT) & (~NK_EDIT_MULTILINE),
                GetRowItemByColumnName(data->editData->data, dataRow, "员工联系方式"), 512, nk_filter_default);

            nk_layout_row_end(context);
        }

        nk_layout_row_begin(context, NK_STATIC, 0, 2);
        {
            nk_style_push_font(context, &fontMedium->handle);
            {
                nk_layout_row_push(context, 200);
                nk_label(context, "员工可用性", NK_TEXT_CENTERED);
                nk_style_pop_font(context);
            }

            nk_layout_row_push(context, 100);
            if (nk_checkbox_label(context, "", &data->enabledCheck))
            {
                GetRowItemByColumnName(data->editData->data, dataRow, "员工可用性")[0] = data->enabledCheck ? '1' : '0';
            }

            nk_layout_row_end(context);
        }

        nk_layout_row_dynamic(context, 0, 1);
        nk_style_push_font(context, &fontMedium->handle);
        {
            nk_label(context, "员工权限", NK_TEXT_CENTERED);
            nk_style_pop_font(context);
        }

        nk_layout_row_dynamic(context, 0, 6);
        for (int i = 0; i < OPERATION_COUNT; i++)
        {
            PlaceNothing(context);
            if (nk_checkbox_label(context, OperationToString(i), &data->permissionChecks[i]))
            {
                GetRowItemByColumnName(data->editData->data, dataRow, "员工权限")[i] = data->permissionChecks[i] + '0';
            }
            PlaceNothing(context);
        }

        PlaceNothing(context);
        nk_layout_row_dynamic(context, 0, 5);
        {
            PlaceNothing(context);
            if (nk_button_label(context, "确定"))
            {
                SendRequest(data);
            }
            PlaceNothing(context);
            if (nk_button_label(context, "取消"))
            {
                window->isClosed = 1;
            }
        }

        nk_style_pop_font(context);
    }
}

void FreeStaffEdit(Window *window)
{
    struct Data *data = window->data;
    FreeTable(data->editData->data);
    free(window);
}

Window *NewStaffEdit(const char *title, int id, const char *password, Table *staff, int modify)
{
    Window *window = malloc(sizeof(Window));
    window->isClosed = 0;
    window->layoutFunc = StaffEditLayout;
    window->freeFunc = FreeStaffEdit;
    window->title = title;

    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 0, sizeof(struct Data));

    data->editData = malloc(sizeof(struct EditData));
    memset(data->editData, 0, sizeof(struct EditData));

    data->editData->data = CloneTableBuffered(staff, 512);
    data->editData->id = id;
    data->editData->password = password;
    data->editData->modify = modify;
    data->editData->window = window;
    data->enabledCheck =
        strcmp(GetRowItemByColumnName(data->editData->data, data->editData->data->rows->next->data, "员工可用性"), "1") == 0;
    const char *permissionString = GetRowItemByColumnName(data->editData->data, data->editData->data->rows->next->data, "员工权限");
    for (int i = 0; i < OPERATION_COUNT; i++)
    {
        data->permissionChecks[i] = permissionString[i] - '0';
    }

    window->data = data;
    window->next = NULL;

    return window;
}
