#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "types.h"

struct Point3 Translate3(struct Point3 point3, struct Direction direction);
struct Point3 Scale3(struct Point3 point3, float scale);
struct Point3 RotateX(struct Point3 point3, float angle);
struct Point3 RotateY(struct Point3 point3, float angle);
struct Point3 RotateZ(struct Point3 point3, float angle);
struct Point3 RotateAboutPoint(struct Point3 point, struct Point3 center, float angleX, float angleY, float angleZ);

struct Point2 Scale2(struct Point2 point2, float scale);

#endif
