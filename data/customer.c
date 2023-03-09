#include "customer.h"

static int idCount;

struct Customer
{
    int id;
    int level;
    const char *name;
    const char *contact;
};

Customer *NewCustomer(int level, const char *name, const char *contact)
{

}

void FreeCustomer(Customer *customer)
{

}

const LinkedList *GetAllCustomers()
{

}

const Customer *GetCustomerById(int id)
{

}

const LinkedList *GetCustomersByLevel(int level)
{

}

const LinkedList *GetCustomersByName(const char *name)
{

}

int GetCustomerId(const Customer *customer)
{

}

int GetCustomerLevel(const Customer *customer)
{

}

const char *GetCustomerName(const Customer *customer)
{

}

const char *GetCustomerContact(const Customer *customer)
{

}

void SetCustomerLevel(Customer *customer, int value)
{

}

void SetCustomerName(Customer *customer, const char *name)
{

}


void SetCustomerContact(Customer *customer, const char *contact)
{

}

int AppendCustomer(Customer *customer)
{

}

int RemoveCustomer(Customer *customer)
{

}

void CustomerSave()
{

}
