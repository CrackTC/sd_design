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
Window *NewInventoryEdit(const char *title, int id, const char *password, Table *inventory);
Window *NewStaffDetail(const char *title, const Table *staff);
Window *NewCustomerDetail(const char *title, const Table *customer);
Window *NewJournalDetail(const char *title, const Table *journal);
Window *NewBasicDiscountDetail(const char *title, const Table *basicDiscount);
Window *NewLossEntryDetail(const char *title, const Table *lossEntry);
Window *NewOrderDetail(const char *title, const Table *order);
Window *NewProfitDetail(const char *title, const Table *profit);
Window *NewRefundEntryDetail(const char *title, const Table *refund);

void WelcomePageLayout(struct nk_context *context, struct Window *window);
void InventoryPageLayout(struct nk_context *context, struct Window *window);
void ItemPageLayout(struct nk_context *context, struct Window *window);

void PlaceNothing(struct nk_context *context);
void EnsureWindowSize(struct nk_context *context, Window *window, float width, float height);
void DrawMessageBox(struct nk_context *context, const char *title, int draw, const char *message,
                    void (*callbackOk)(void *), void (*callbackCancel)(void *), void *parameter);
void TableLayout(struct nk_context *context, const Table *table, LinkedList *checkList, const char *filter,
                 const char *value);
void PushWindow(struct nk_context *context, Window *window);

#endif
