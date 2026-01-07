#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "object.h"

struct Object3D spawnCuboid(int x, int y, int z) {
    struct Object3D cube;
    cube.vertexCount = 8;
    cube.edgeCount = 12;

    cube.vertices = malloc(cube.vertexCount * sizeof(*cube.vertices));
    cube.edges = malloc(cube.edgeCount * sizeof(*cube.edges));

    struct Point3 vertices[8] = {
        { 1,  1,  1},
        { 1,  1, -1},
        { 1, -1,  1},
        { 1, -1, -1},
        {-1,  1,  1},
        {-1,  1, -1},
        {-1, -1,  1},
        {-1, -1, -1},
    };

    int edges[12][2] = {
        {0,1}, {0,2},
        {0,4}, {1,3},
        {1,5}, {2,3},
        {2,6}, {3,7},
        {4,5}, {4,6},
        {5,7}, {6,7}
    };

    for (size_t i = 0; i < 5; i++) {
        vertices[i].x *= x;
        vertices[i].y *= y;
        vertices[i].z *= z;
    }

    memcpy(cube.vertices, vertices, sizeof(vertices));
    memcpy(cube.edges, edges, sizeof(edges));

    return cube;
}

struct Object3D spawnPyramid(float x, float y, float z) {
    struct Object3D pyramid;
    pyramid.vertexCount = 5;
    pyramid.edgeCount = 8;

    pyramid.vertices = malloc(pyramid.vertexCount * sizeof(*pyramid.vertices));
    pyramid.edges = malloc(pyramid.edgeCount * sizeof(*pyramid.edges));

    struct Point3 vertices[5] = {
        { 1, -1,  1},
        { 1, -1, -1},
        {-1, -1, -1},
        {-1, -1,  1},
        { 0,  1,  0},
    };
    int edges[8][2] = {
        { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 },
        { 0, 4 }, { 1, 4 }, { 2, 4 }, { 3, 4 },
    };

    for (size_t i = 0; i < 5; i++) {
        vertices[i].x *= x;
        vertices[i].y *= y;
        vertices[i].z *= z;
    }

    memcpy(pyramid.vertices, vertices, sizeof(vertices));
    memcpy(pyramid.edges, edges, sizeof(edges));

    return pyramid;
}

#include "data/sphere_data.h"
struct Object3D spawnSphere(float x, float y, float z) {
    struct Object3D sphere;
    sphere.vertexCount = SPHERE_VERTEX_COUNT;
    sphere.edgeCount = SPHERE_EDGE_COUNT;

    sphere.vertices = malloc(sphere.vertexCount * sizeof(*sphere.vertices));
    sphere.edges = malloc(sphere.edgeCount * sizeof(*sphere.edges));

    memcpy(sphere.vertices, sphereVertex_data, sizeof(sphereVertex_data));
    memcpy(sphere.edges, sphereEdges_data, sizeof(sphereEdges_data));

    for (size_t i = 0; i < SPHERE_VERTEX_COUNT; i++) {
        sphere.vertices[i].x *= x;
        sphere.vertices[i].y *= y;
        sphere.vertices[i].z *= z;
    }

    return sphere;
}

void freeObject3D(struct Object3D *object) {
    free(object->vertices);
    free(object->edges);

    object->vertices = NULL;
    object->edges = NULL;
    object->vertexCount = 0;
    object->edgeCount = 0;
}
