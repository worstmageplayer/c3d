#include "transform.h"
#include <math.h>

struct Point3 Translate3(struct Point3 point3, struct Direction direction) {
    struct Point3 returnVal;
    returnVal.x = point3.x + direction.x;
    returnVal.y = point3.y + direction.y;
    returnVal.z = point3.z + direction.z;
    return returnVal;
}

struct Point3 Scale3(struct Point3 point3, float scale) {
    struct Point3 returnVal;
    returnVal.x = point3.x * scale;
    returnVal.y = point3.y * scale;
    returnVal.z = point3.z * scale;
    return returnVal;
}

struct Point2 Scale2(struct Point2 point2, float scale) {
    struct Point2 returnVal;
    returnVal.x = point2.x * scale;
    returnVal.y = point2.y * scale;
    return returnVal;
}

struct Point3 RotateX(struct Point3 point3, float angle) {
    struct Point3 returnVal;
    returnVal.x = point3.x;
    returnVal.y = point3.y * cosf(angle) - point3.z * sinf(angle);
    returnVal.z = point3.y * sinf(angle) + point3.z * cosf(angle);
    return returnVal;
}

struct Point3 RotateY(struct Point3 point3, float angle) {
    struct Point3 returnVal;
    returnVal.x = point3.x * cosf(angle) + point3.z * sinf(angle);
    returnVal.y = point3.y;
    returnVal.z = -point3.x * sinf(angle) + point3.z * cosf(angle);
    return returnVal;
}

struct Point3 RotateZ(struct Point3 point3, float angle) {
    struct Point3 returnVal;
    returnVal.x = point3.x * cosf(angle) - point3.y * sinf(angle);
    returnVal.y = point3.x * sinf(angle) + point3.y * cosf(angle);
    returnVal.z = point3.z;
    return returnVal;
}

struct Point3 RotateAboutPoint(struct Point3 point, struct Point3 center, float angleX, float angleY, float angleZ) {
    struct Point3 returnVal;
    struct Direction toOrigin = { -center.x, -center.y, -center.z };
    returnVal = Translate3(point, toOrigin);

    if (angleX != 0.0f) returnVal = RotateX(returnVal, angleX);
    if (angleY != 0.0f) returnVal = RotateY(returnVal, angleY);
    if (angleZ != 0.0f) returnVal = RotateZ(returnVal, angleZ);

    struct Direction back = { center.x, center.y, center.z };
    returnVal = Translate3(returnVal, back);

    return returnVal;
}
