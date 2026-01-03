#include "hyprland.h"
#include "../raylib-5.5_linux_amd64/include/raylib.h"
#include "types.h"
#include "projection.h"
#include "transform.h"
#include "data/sphere_data.h"
#include <stddef.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_TITLE "3D stuff"
#define WIDTH 1920
#define HEIGHT 1080

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
    InitWindow(WIDTH, HEIGHT, WINDOW_TITLE);
    SetTargetFPS(165);

    int sock = ConnectHyprlandSocket();
    // printf("Sock: %d\n", sock);
    SendCommand(sock, "j/clients");
    char *buffer = GetReply(sock);
    // printf("JSON Buffer:\n%s\n", buffer);
    char *address = GetAddressByTitle(buffer, WINDOW_TITLE);
    printf("Window Address: %s\n", address);
    int *pos = GetPosByAddress(buffer, address);
    int initX = pos[0];
    int initY = pos[1];
    printf("Initial Position: %d, %d\n", pos[0], pos[1]);

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

    struct Point3 cubeCenter = GetCenter(cube, cubeArrayLength);
    struct Point3 pyramidCenter = GetCenter(pyramid, pyramidArrayLength);
    struct Point3 sphereCenter = GetCenter(sphere, sphereArrayLength);

    for (int i = 0; i < cubeArrayLength; i++) {
        cube[i] = Scale3(cube[i], 5);
        struct Direction direction = { -15, 0, 20 };
        cube[i] = Translate3(cube[i], direction);
    }

    for (int i = 0; i < pyramidArrayLength; i++) {
        pyramid[i] = Scale3(pyramid[i], 2);
        struct Direction direction = { 20, 0, 20 };
        pyramid[i] = Translate3(pyramid[i], direction);
    }

    for (int i = 0; i < sphereArrayLength; i++) {
        sphere[i] = Scale3(sphere[i], 6);
        struct Direction direction = { 0, 10, 30 };
        sphere[i] = Translate3(sphere[i], direction);
    }

    int prevX = initX;
    int prevY = initY;
    int dx = 0;
    int dy = 0;

    struct Direction windowDirection = { 0, 0, 0 };

    struct Point2 cube2d[cubeArrayLength];
    struct Point2 cubeScreen[cubeArrayLength];

    struct Point2 pyramid2d[pyramidArrayLength];
    struct Point2 pyramidScreen[pyramidArrayLength];

    struct Point2 sphere2d[sphereArrayLength];
    struct Point2 sphereScreen[sphereArrayLength];

    while (!WindowShouldClose()) {

        cubeCenter = GetCenter(cube, cubeArrayLength);
        pyramidCenter = GetCenter(pyramid, pyramidArrayLength);
        sphereCenter = GetCenter(sphere, sphereArrayLength);

        // printf("Sock Loop: %d\n", sock);
        sock = ConnectHyprlandSocket();
        SendCommand(sock, "j/clients");
        buffer = GetReply(sock);
        pos = GetPosByAddress(buffer, address);
        dx = prevX - pos[0];
        dy = prevY - pos[1];
        // printf("Changed Position: %d, %d\n", pos[0], pos[1]);
        // printf("Change in Position: %d, %d\n", dx, dy);
        prevX = pos[0];
        prevY = pos[1];
        close(sock);
        free(buffer);
        free(pos);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Fix this
        int CONST = 8;
        windowDirection.x = (float)dx/CONST;
        windowDirection.y = -(float)dy/CONST;

        for (int i = 0; i < cubeArrayLength; i++) {
            cube[i] = Translate3(cube[i], windowDirection);
            cube[i] = RotateAboutPoint(cube[i], cubeCenter, 0.01f, 0.015, 0.0f);
        }

        for (int i = 0; i < pyramidArrayLength; i++) {
            pyramid[i] = Translate3(pyramid[i], windowDirection);
            pyramid[i] = RotateAboutPoint(pyramid[i], pyramidCenter, 0.0f, -0.015f, 0.0f);
        }
        for (int i = 0; i < sphereArrayLength; i++) {
            sphere[i] = Translate3(sphere[i], windowDirection);
            sphere[i] = RotateAboutPoint(sphere[i], sphereCenter, 0.005f, 0.005f, 0.005f);
        }

        for (int i = 0; i < cubeArrayLength; i++) {
            cube2d[i] = Project(cube[i]);
            cubeScreen[i] = CartesianToScreen(cube2d[i], WIDTH, HEIGHT);
        }

        for (int i = 0; i < pyramidArrayLength; i++) {
            pyramid2d[i] = Project(pyramid[i]);
            pyramidScreen[i] = CartesianToScreen(pyramid2d[i], WIDTH, HEIGHT);
        }

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

    free(pos);
    free(buffer);
    CloseWindow();
    return 0;
}
