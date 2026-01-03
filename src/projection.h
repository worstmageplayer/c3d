#ifndef PROJECTION_H
#define PROJECTION_H

#include "types.h"

extern float Z;

struct Point2 Project(struct Point3 point3);

struct Point2 CartesianToScreen(struct Point2 point2, int width, int height);

#endif
