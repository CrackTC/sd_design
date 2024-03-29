#ifndef LAYOUT_H
#define LAYOUT_H

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT

#include "glad/glad.h"
#include "linkedList.h"
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_glfw_gl3.h"
#include "table.h"

#define BUFFER_SIZE 256

typedef struct Window Window;

typedef void (*LayoutFunc)(struct nk_context *context, Window *window);

typedef void (*FreeFunc)(Window *window);

typedef void (*OperationHandler)(void *);

struct Window
{
    int isClosed;
    LayoutFunc layoutFunc;
    FreeFunc freeFunc;
    const char *title;
    void *data;
    struct Window *next;
};

struct EditData
{
    struct Table *data;
    int id;
    const char *password;
    char *message;
    int modify;
    Window *window;

    void (*messageCallback)(int, void *);
};

struct PageData
{
    int propertySelected;
    char *valueBuffer;
    const char **properties;
    struct Table *table;
    struct LinkedList *checkList;
};

struct MainWindowData
{
    int id;
    char *name;
    char *password;
    char *message;
    int sectionSelected;

    struct nk_context *context;

    void (*messageCallback)(int, void *);

    struct PageData *dataArray;
};

Window *NewMainWindow(const char *title, const char *id, const char *password, const char *name);

Window *NewLoginWindow(const char *title);

Window *NewInventoryDetail(const char *title, const Table *inventory);

Window *NewInventoryEdit(const char *title, int id, const char *password, Table *inventory, int modify);

Window *NewItemDetail(const char *title, const Table *item);

Window *NewItemEdit(const char *title, int id, const char *password, Table *item, int modify);

Window *NewOrderDetail(const char *title, const Table *order);

Window *NewOrderEdit(const char *title, int id, const char *password, Table *order, int modify);

Window *NewDiscountDetail(const char *title, const Table *discount);

Window *NewDiscountEdit(const char *title, int id, const char *password, Table *discount, int modify);

Window *NewCustomerDetail(const char *title, const Table *discount);

Window *NewCustomerEdit(const char *title, int id, const char *password, Table *customer, int modify);

Window *NewJournalDetail(const char *title, const Table *journal);

Window *NewLossDetail(const char *title, const Table *loss);

Window *NewLossEdit(const char *title, int id, const char *password, Table *loss, int modify);

Window *NewProfitDetail(const char *title, const Table *profit);

Window *NewRefundDetail(const char *title, const Table *refund);

Window *NewRefundEdit(const char *title, int id, const char *password, Table *refund, int modify);

Window *NewStaffDetail(const char *title, const Table *staff);

Window *NewStaffEdit(const char *title, int id, const char *password, Table *staff, int modify);

Window *NewResultDialog(const char *title, Table *data);

void WelcomePageLayout(struct nk_context *context, struct Window *window);

void InventoryPageLayout(struct nk_context *context, struct Window *window);

void ItemPageLayout(struct nk_context *context, struct Window *window);

void OrderPageLayout(struct nk_context *context, struct Window *window);

void DiscountPageLayout(struct nk_context *context, struct Window *window);

void CustomerPageLayout(struct nk_context *context, struct Window *window);

void JournalPageLayout(struct nk_context *context, struct Window *window);

void LossPageLayout(struct nk_context *context, struct Window *window);

void ProfitPageLayout(struct nk_context *context, struct Window *window);

void RefundPageLayout(struct nk_context *context, struct Window *window);

void StaffPageLayout(struct nk_context *context, struct Window *window);

void PlaceNothing(struct nk_context *context);

void EnsureWindowSize(struct nk_context *context, Window *window, float width, float height);

void DrawMessageBox(struct nk_context *context, const char *title, int draw, const char *message,
                    void (*callback)(int, void *), void *parameter);

void DrawSeparateLine(struct nk_context *context);

void MessageBoxCallback(int ok, void *parameter);

void FinishCallback(int ok, void *parameter);

void TableLayout(struct nk_context *context, const Table *table, LinkedList *checkList, const char *filter,
                 const char *value, const char *rangeFilter, const char *from, const char *to);

void PushWindow(Window *window);

enum OperationTypes
{
    OP_TYPE_GET = 1 << 0,
    OP_TYPE_DETAIL = 1 << 1,
    OP_TYPE_ADD = 1 << 2,
    OP_TYPE_DELETE = 1 << 3,
    OP_TYPE_UPDATE = 1 << 4
};

void OperationLayout(struct nk_context *context,
        enum OperationTypes types,
        OperationHandler getHandler,
        OperationHandler detailHandler,
        OperationHandler addHandler,
        OperationHandler deleteHandler,
        OperationHandler updateHandler,
        void *data);

void DateRangeFilterLayout(struct nk_context *context, const char *title, char **from, char **to);

void BasicFilterLayout(struct nk_context *context, const char *title, struct PageData *data);

void PageResultLayout(struct nk_context *context, const struct PageData *data, const char *rangeFilter, const char *from, const char *to);

LinkedList *NewCheckList();

#endif
