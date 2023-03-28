#ifndef CRUD_H
#define CRUD_H

#include "design/layout.h"

typedef struct Table *(*Service)(struct Table *);

void Delete(int ok, void *parameter, struct PageData *pageData, Service deleteService, const char *permissionError, const char *idTitle);

#endif //CRUD_H
