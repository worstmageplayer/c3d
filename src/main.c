#include "../raylib-5.5_linux_amd64/include/raylib.h"
#include "types.h"
#include "projection.h"
#include "transform.h"
#include "data/sphere_data.h"
#include <stddef.h>
#include <sys/types.h>
#include <string.h>

struct Point3 GetCenter(const struct Point3 *points, size_t count) {
    struct Point3 center = { 0, 0, 0 };

    if (!points || count == 0)  return center;

    double x = 0, y = 0, z = 0;
    for (size_t i = 0; i < count; i++) {
        x += points[i].x;
        y += points[i].y;
        z += points[i].z;
    }

    center.x = x / count;
    center.y = y / count;
    center.z = z / count;

    return center;
}

int main(void) {
    int WIDTH = 1920;
    int HEIGHT = 1080;

    InitWindow(WIDTH, HEIGHT, "3D stuff");
    SetTargetFPS(165);

    struct Point3 sphere[SPHERE_VERTEX_COUNT];
    for (int i = 0; i < SPHERE_VERTEX_COUNT; i++) {
        sphere[i] = sphere_data[i];
    }

    int sphereEdges[SPHERE_EDGE_COUNT][2];
    for (int i = 0; i < SPHERE_EDGE_COUNT; i++) {
        for (int j = 0; j < 2; j++) {
            sphereEdges[i][j] = sphereEdges_data[i][j];
        }
    }

    struct Point3 pyramid[5] = {
        { 1, -1,  1},
        { 1, -1, -1},
        {-1, -1, -1},
        {-1, -1,  1},
        { 0,  1,  0},
    };

    const int pyramidEdges[8][2] = {
        { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 },
        { 0, 4 }, { 1, 4 }, { 2, 4 }, { 3, 4 },
    };

    struct Point3 cube[8] = {
        { 1,  1,  1},
        { 1,  1, -1},
        { 1, -1,  1},
        { 1, -1, -1},
        {-1,  1,  1},
        {-1,  1, -1},
        {-1, -1,  1},
        {-1, -1, -1},
    };

    const int cubeEdges[12][2] = {
        {0,1}, {0,2},
        {0,4}, {1,3},
        {1,5}, {2,3},
        {2,6}, {3,7},
        {4,5}, {4,6},
        {5,7}, {6,7}
    };

    int cubeArrayLength = (int)(sizeof(cube) / sizeof(cube[0]));
    int pyramidArrayLength = (int)(sizeof(pyramid) / sizeof(pyramid[0]));
    int sphereArrayLength = (int)(sizeof(sphere) / sizeof(sphere[0]));

    for (int i = 0; i < cubeArrayLength; i++) {
        struct Direction direction = { -1.5, 0, 2 };
        cube[i] = Translate3(cube[i], direction);
        cube[i] = Scale3(cube[i], 5);
    }

    for (int i = 0; i < pyramidArrayLength; i++) {
        struct Direction direction = { 2, 0, 2 };
        pyramid[i] = Translate3(pyramid[i], direction);
        pyramid[i] = Scale3(pyramid[i], 2);
    }

    for (int i = 0; i < sphereArrayLength; i++) {
        struct Direction direction = { 0, 1, 3 };
        sphere[i] = Translate3(sphere[i], direction);
        sphere[i] = Scale3(sphere[i], 6);
    }

    struct Point3 cubeCenter = GetCenter(cube, cubeArrayLength);
    struct Point3 pyramidCenter = GetCenter(pyramid, pyramidArrayLength);
    struct Point3 sphereCenter = GetCenter(sphere, sphereArrayLength);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < cubeArrayLength; i++) {
            cube[i] = RotateAboutPoint(cube[i], cubeCenter, 0.01f, 0.015, 0.0f);
        }

        for (int i = 0; i < pyramidArrayLength; i++) {
            pyramid[i] = RotateAboutPoint(pyramid[i], pyramidCenter, 0.0f, -0.015f, 0.0f);
        }
        for (int i = 0; i < sphereArrayLength; i++) {
            sphere[i] = RotateAboutPoint(sphere[i], sphereCenter, 0.005f, 0.005f, 0.005f);
        }

        struct Point2 cube2d[cubeArrayLength];
        struct Point2 cubeScreen[cubeArrayLength];

        for (int i = 0; i < cubeArrayLength; i++) {
            cube2d[i] = Project(cube[i]);
            cubeScreen[i] = CartesianToScreen(cube2d[i], WIDTH, HEIGHT);
        }

        struct Point2 pyramid2d[pyramidArrayLength];
        struct Point2 pyramidScreen[pyramidArrayLength];

        for (int i = 0; i < pyramidArrayLength; i++) {
            pyramid2d[i] = Project(pyramid[i]);
            pyramidScreen[i] = CartesianToScreen(pyramid2d[i], WIDTH, HEIGHT);
        }

        struct Point2 sphere2d[sphereArrayLength];
        struct Point2 sphereScreen[sphereArrayLength];

        for (int i = 0; i < sphereArrayLength; i++) {
            sphere2d[i] = Project(sphere[i]);
            sphereScreen[i] = CartesianToScreen(sphere2d[i], WIDTH, HEIGHT);
        }

        for (int e = 0; e < (int)(sizeof(cubeEdges) / sizeof(cubeEdges[0])); e++) {
            int a = cubeEdges[e][0];
            int b = cubeEdges[e][1];
            DrawLine(
                (int)cubeScreen[a].x,
                (int)cubeScreen[a].y,
                (int)cubeScreen[b].x,
                (int)cubeScreen[b].y,
                GREEN
            );
        }

        for (int e = 0; e < (int)(sizeof(pyramidEdges) / sizeof(pyramidEdges[0])); e++) {
            int a = pyramidEdges[e][0];
            int b = pyramidEdges[e][1];
            DrawLine(
                (int)pyramidScreen[a].x,
                (int)pyramidScreen[a].y,
                (int)pyramidScreen[b].x,
                (int)pyramidScreen[b].y,
                RED
            );
        }

        for (int e = 0; e < (int)(sizeof(sphereEdges) / sizeof(sphereEdges[0])); e++) {
            int a = sphereEdges[e][0];
            int b = sphereEdges[e][1];
            DrawLine(
                (int)sphereScreen[a].x,
                (int)sphereScreen[a].y,
                (int)sphereScreen[b].x,
                (int)sphereScreen[b].y,
                BLUE
            );
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
