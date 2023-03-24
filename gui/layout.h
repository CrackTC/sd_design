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

#include "../data/linkedList.h"
#include "../data/table.h"
#include "glad/glad.h"
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_glfw_gl3.h"

#define BUFFER_SIZE 256

typedef struct Window Window;

typedef void (*LayoutFunc)(struct nk_context *context, Window *window);

typedef void (*FreeFunc)(Window *window);

struct Window
{
    int isClosed;
    LayoutFunc layoutFunc;
    FreeFunc freeFunc;
    const char *title;
    void *data;
    struct Window *next;
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

void TableLayout(struct nk_context *context, const Table *table, LinkedList *checkList, const char *filter,
        const char *value);

void PushWindow(Window *window);

LinkedList *NewCheckList();

#endif
