#include "basicDiscount.h"
#include <malloc.h>

struct BasicDiscount
{
    int itemId;
    int ratio;
    int customerLevel;
    Time *deadline;
};

BasicDiscount *NewBasicDiscount(int itemId, int ratio, int customerLevel, Time *deadline)
{
    if (itemId < 0 || ratio < 0 || ratio > 100 || customerLevel < 0 || deadline == NULL)
        return NULL;

    BasicDiscount *discount = malloc(sizeof(BasicDiscount));
    discount->itemId = itemId;
    discount->ratio = ratio;
    discount->customerLevel = customerLevel;
    discount->deadline = deadline;
    return discount;
}

void FreeBasicDiscount(BasicDiscount *discount)
{
    FreeTime(discount->deadline);
    free(discount);
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

