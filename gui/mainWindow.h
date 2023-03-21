#include "layout.h"

struct Data
{
    int id;
    char *name;
    char *password;
    char *message;
    int sectionSelected;

    struct nk_context *context;

    void (*messageCallback)(int, void *);

    int inventoryPropertySelected;
    char *inventoryValueBuffer;
    const char **inventoryProperties;
    struct Table *inventoryTable;
    struct LinkedList *inventoryCheckList;

    int itemPropertySelected;
    char *itemValueBuffer;
    const char **itemProperties;
    struct Table *itemTable;
    struct LinkedList *itemCheckList;

    int orderPropertySelected;
    char *orderValueBuffer;
    const char **orderProperties;
    struct Table *orderTable;
    struct LinkedList *orderCheckList;

    int discountPropertySelected;
    char *discountValueBuffer;
    const char **discountProperties;
    struct Table *discountTable;
    struct LinkedList *discountCheckList;

    int customerPropertySelected;
    char *customerValueBuffer;
    const char **customerProperties;
    struct Table *customerTable;
    struct LinkedList *customerCheckList;

    int journalPropertySelected;
    char *journalValueBuffer;
    const char **journalProperties;
    struct Table *journalTable;
    struct LinkedList *journalCheckList;

    int lossPropertySelected;
    char *lossValueBuffer;
    const char **lossProperties;
    struct Table *lossTable;
    struct LinkedList *lossCheckList;

    int profitPropertySelected;
    char *profitValueBuffer;
    const char **profitProperties;
    struct Table *profitTable;
    struct LinkedList *profitCheckList;

    int refundPropertySelected;
    char *refundValueBuffer;
    const char **refundProperties;
    struct Table *refundTable;
    struct LinkedList *refundCheckList;
};
