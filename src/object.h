#ifndef OBJECT_H
#define OBJECT_H

#include "types.h"
#include <stddef.h>

struct Object {
    struct Point3 *vertices;
    size_t vertexCount;

    const int (*edges)[2];
    size_t edgeCount;
};


#endif
