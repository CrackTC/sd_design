#include "layout.h"

struct Data
{
    int id;
    char *name;
    char *password;
    char *message;
    int sectionSelected;

    int inventorySelectedRadio;
    int inventoryPropertySelected;
    char *inventoryValueBuffer;
    const char **inventoryProperties;
    struct Table *inventoryTable;
    struct LinkedList *inventoryCheckList;
};
