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

    int customerPropertySelected;
    char *customerValueBuffer;
    const char **customerProperties;
    struct Table *customerTable;
    struct LinkedList *customerCheckList;
};
