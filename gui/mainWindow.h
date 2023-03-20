#include "layout.h"

struct Data
{
    int id;
    char *name;
    char *password;
    char *message;
    int sectionSelected;

    void (*messageOK)(void *);
    void (*messageCancel)(void *);

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
};
