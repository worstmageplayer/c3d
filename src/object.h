#ifndef OBJECT_H
#define OBJECT_H

#include "types.h"
#include <stddef.h>

struct Object3D {
    struct Point3 *vertices;
    size_t vertexCount;

    int (*edges)[2];
    size_t edgeCount;
};


struct Object3D spawnCuboid(int x, int y, int z);
struct Object3D spawnPyramid(float x, float y, float z);
struct Object3D spawnSphere(float x, float y, float z);

void freeObject3D(struct Object3D *object);

#endif
