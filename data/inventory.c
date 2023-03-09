#include "inventory.h"

struct InventoryEntry
{
    int id;
    int itemId;
    int number;
    const Time *inboundTime;
    const Time *productionTime;
    const Amount *inboundUnitPrice;
};
