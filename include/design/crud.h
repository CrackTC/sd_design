#ifndef CRUD_H
#define CRUD_H

#include "design/layout.h"
#include "design/operation.h"

typedef struct Table *(*Service)(struct Table *);

void Delete(int ok, void *parameter, struct PageData *pageData, Service deleteService, const char *permissionError, const char *idTitle, enum Operation operation);

void Read(struct MainWindowData *data, struct PageData *pageData, Service readService, const char *permissionError, Operation operation);

#endif //CRUD_H
