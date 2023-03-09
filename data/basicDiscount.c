#include "basicDiscount.h"

struct BasicDiscount
{
    int itemId;
    int ratio;
    int customerLevel;
    const Time *deadline;
};

BasicDiscount *NewBasicDiscount(int itemId, int ratio, int customerLevel, const Time *deadline)
{

}

void FreeBasicDiscount(BasicDiscount *discount)
{

}

const LinkedList *GetAllBasicDiscounts()
{

}

const LinkedList *GetBasicDiscountsByItemId(int itemId)
{

}

const LinkedList *GetBasicDiscountsByCustomerLevel(int level)
{

}

int GetBasicDiscountItemId(const BasicDiscount *discount)
{

}

int GetBasicDiscountRatio(const BasicDiscount *discount)
{

}

int GetBasicDiscountCustomerLevel(const BasicDiscount *discount)
{

}

const Time *GetBasicDiscountDeadline(const BasicDiscount *discount)
{

}

void SetBasicDiscountItemId(BasicDiscount *discount, int value)
{

}

void SetBasicDiscountRatio(BasicDiscount *discount, int value)
{

}

void SetBasicDiscountCustomerLevel(BasicDiscount *discount, int value)
{

}

void SetBasicDiscountDeadline(BasicDiscount *discount, const Time *value)
{

}

int AppendBasicDiscount(BasicDiscount *discount)
{

}

int RemoveBasicDiscount(BasicDiscount *discount)
{

}

void BasicDiscountSave()
{

}

