#include "hyprland.h"
#include "../raylib-5.5_linux_amd64/include/raylib.h"
#include "types.h"
#include "draw.h"
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
    if (sock < 0) {
        fprintf(stderr, "Failed to connect to Hyprland socket\n");
        return 1;
    }

    SendCommand(sock, "j/clients\0");
    char *buffer = GetReply(sock);
    // printf("Buffer: %s\n", buffer);
    if (!buffer) {
        fprintf(stderr, "No reply\n");
        close(sock);
        return 1;
    }

    char *address = GetAddressByTitle(buffer, WINDOW_TITLE);
    if (!address) {
        fprintf(stderr, "Could not find window with title: %s\n", WINDOW_TITLE);
        return 1;
    }

    int *pos = GetPosByAddress(buffer, address);
    int initX = 0;
    int initY = 0;
    if (pos) {
        initX = pos[0];
        initY = pos[1];
        free(pos);
    } else {
        fprintf(stderr, "Unable to get pos\n");
        return 1;
    }
    free(buffer);
    close(sock);

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

    int cubeEdgesArrayLength = (int)(sizeof(cubeEdges) / sizeof(cubeEdges[0]));
    int pyramidEdgesArrayLength = (int)(sizeof(pyramidEdges) / sizeof(pyramidEdges[0]));
    int sphereEdgesArrayLength = (int)(sizeof(sphereEdges) / sizeof(sphereEdges[0]));

    struct Point3 cubeCenter = GetCenter(cube, cubeArrayLength);
    struct Point3 pyramidCenter = GetCenter(pyramid, pyramidArrayLength);
    struct Point3 sphereCenter = GetCenter(sphere, sphereArrayLength);

    for (int i = 0; i < cubeArrayLength; i++) {
        cube[i] = Scale3(cube[i], 5);
        struct Direction direction = { -15, 0, 30 };
        cube[i] = Translate3(cube[i], direction);
    }

    for (int i = 0; i < pyramidArrayLength; i++) {
        pyramid[i] = Scale3(pyramid[i], 2);
        struct Direction direction = { 15, 0, 30 };
        pyramid[i] = Translate3(pyramid[i], direction);
    }

    for (int i = 0; i < sphereArrayLength; i++) {
        sphere[i] = Scale3(sphere[i], 5);
        struct Direction direction = { 0, 10, 45 };
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

        sock = ConnectHyprlandSocket();
        if (sock < 0) {
            fprintf(stderr, "Failed to connect to Hyprland socket\n");
            return 1;
        };

        SendCommand(sock, "j/clients\0");
        buffer = GetReply(sock);
        if (!buffer) {
            fprintf(stderr, "No reply\n");
            close(sock);
            return 1;
        }

        pos = GetPosByAddress(buffer, address);
        if (pos) {
            dx = prevX - pos[0];
            dy = prevY - pos[1];
            prevX = pos[0];
            prevY = pos[1];
            free(pos);
        } else {
            fprintf(stderr, "Unable to get pos\n");
            free(buffer);
            close(sock);
            return 1;
        }
        free(buffer);
        close(sock);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Fix this
        windowDirection.x = (float)dx / (Z/100);
        windowDirection.y = -(float)dy / (Z/100);

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

        DrawWireframe(
                cubeEdges,
                cubeEdgesArrayLength,
                cubeScreen,
                GREEN
                );
        DrawWireframe(
                pyramidEdges,
                pyramidEdgesArrayLength,
                pyramidScreen,
                RED
                );
        DrawWireframe(
                sphereEdges,
                sphereEdgesArrayLength,
                sphereScreen,
                BLUE
                );

        EndDrawing();
    }

    free(address);
    close(sock);
    CloseWindow();
    return 0;
}
