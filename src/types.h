#ifndef TYPES_H
#define TYPES_H

struct Point2 {
    float x;
    float y;
};

struct Point3 {
    float x;
    float y;
    float z;
};

struct Direction {
    float x;
    float y;
    float z;
};

struct Shear {
    float xy;
    float xz;
    float yx;
    float yz;
    float zx;
    float zy;
};

#endif
