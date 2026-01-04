#include "projection.h"

float Z = 1000.0f;

struct Point2 Project(struct Point3 point3) {
    struct Point2 returnVal;
    float z = (point3.z == 0.0f) ? 0.0001f : point3.z;
    returnVal.x = Z / z * point3.x;
    returnVal.y = Z / z * point3.y;
    return returnVal;
}

struct Point2 CartesianToScreen(struct Point2 point2, int width, int height) {
    struct Point2 returnVal;
    returnVal.x = point2.x + (float)width / 2.0f;
    returnVal.y = (float)height / 2.0f - point2.y;
    return returnVal;
}
