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

const LinkedList *GetAllInventory()
{

}

const InventoryEntry *GetInventoryById(int id)
{

}

LinkedList *GetInventoryByItemId(int itemId)
{

}

InventoryEntry *NewInventoryEntry(int itemId, int number, const Time *inboundTime, const Time *productionTime,
                                  const Amount *inboundUnitPrice)
{

}

void FreeInventoryEntry(InventoryEntry *entry)
{

}


int GetInventoryEntryId(const InventoryEntry *entry)
{

}

int GetInventoryEntryItemId(const InventoryEntry *entry)
{

}

int GetInventoryEntryNumber(const InventoryEntry *entry)
{

}

const Time *GetInventoryEntryInboundTimePointer(const InventoryEntry *entry)
{

}

const Time *GetInventoryEntryProductionTimePointer(const InventoryEntry *entry)
{

}

const Amount *GetInventoryEntryInboundUnitPricePointer(const InventoryEntry *entry)
{

}

void SetInventoryEntryItemId(InventoryEntry *entry, int value)
{

}

void SetInventoryEntryNumber(InventoryEntry *entry, int value)
{

}

void SetInventoryEntryInboundTimePointer(InventoryEntry *entry, const Time *value)
{

}

void SetInventoryEntryProductionTimePointer(InventoryEntry *entry, const Time *value)
{

}

void SetInventoryEntryInboundUnitPricePointer(InventoryEntry *entry, const Amount *value)
{

}

int AppendInventoryEntry(InventoryEntry *entry)
{

}

int RemoveInventoryEntry(InventoryEntry *entry)
{

}

void InventorySave()
{

}
