#ifndef SPHERE_DATA_H
#define SPHERE_DATA_H

#include "../types.h"

#define SPHERE_VERTEX_COUNT 362
#define SPHERE_EDGE_COUNT 744

extern const struct Point3 sphereVertex_data[SPHERE_VERTEX_COUNT];
extern const int sphereEdges_data[SPHERE_EDGE_COUNT][2];

#endif
