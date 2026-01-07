#include "hyprland.h"
#include "../raylib-5.5_linux_amd64/include/raylib.h"
#include "types.h"
#include "draw.h"
#include "projection.h"
#include "transform.h"
#include "object.h"
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

struct Point3 GetCenter(struct Object3D object) {
    struct Point3 center = { 0, 0, 0 };

    if (!object.vertices || object.vertexCount == 0)  return center;

    double x = 0, y = 0, z = 0;
    for (size_t i = 0; i < object.vertexCount; i++) {
        x += object.vertices[i].x;
        y += object.vertices[i].y;
        z += object.vertices[i].z;
    }

    center.x = x / object.vertexCount;
    center.y = y / object.vertexCount;
    center.z = z / object.vertexCount;

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

    struct Object3D cube = spawnCuboid(1, 1, 1);
    struct Object3D pyramid = spawnPyramid(1, 0.5, 1);
    struct Object3D sphere = spawnSphere(1, 0.9, 1);

    struct Point3 cubeCenter = GetCenter(cube);
    struct Point3 pyramidCenter = GetCenter(pyramid);
    struct Point3 sphereCenter = GetCenter(sphere);

    for (size_t i = 0; i < cube.vertexCount; i++) {
        cube.vertices[i] = Scale3(cube.vertices[i], 5);
        struct Direction direction = { -15, 0, 30 };
        cube.vertices[i] = Translate3(cube.vertices[i], direction);
    }

    for (size_t i = 0; i < pyramid.vertexCount; i++) {
        pyramid.vertices[i] = Scale3(pyramid.vertices[i], 2);
        struct Direction direction = { 15, 0, 30 };
        pyramid.vertices[i] = Translate3(pyramid.vertices[i], direction);
    }

    for (size_t i = 0; i < sphere.vertexCount; i++) {
        sphere.vertices[i] = Scale3(sphere.vertices[i], 5);
        struct Direction direction = { 0, 10, 45 };
        sphere.vertices[i] = Translate3(sphere.vertices[i], direction);
    }

    int prevX = initX;
    int prevY = initY;
    int dx = 0;
    int dy = 0;

    struct Direction windowDirection = { 0, 0, 0 };

    struct Point2 cube2d[cube.vertexCount];
    struct Point2 cubeScreen[cube.vertexCount];

    struct Point2 pyramid2d[pyramid.vertexCount];
    struct Point2 pyramidScreen[pyramid.vertexCount];

    struct Point2 sphere2d[sphere.vertexCount];
    struct Point2 sphereScreen[sphere.vertexCount];

    while (!WindowShouldClose()) {

        cubeCenter = GetCenter(cube);
        pyramidCenter = GetCenter(pyramid);
        sphereCenter = GetCenter(sphere);

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

        for (size_t i = 0; i < cube.vertexCount; i++) {
            cube.vertices[i] = Translate3(cube.vertices[i], windowDirection);
            cube.vertices[i] = RotateAboutPoint(cube.vertices[i], cubeCenter, 0.01f, 0.015, 0.0f);
            cube2d[i] = Project(cube.vertices[i]);
            cubeScreen[i] = CartesianToScreen(cube2d[i], WIDTH, HEIGHT);
        }

        for (size_t i = 0; i < pyramid.vertexCount; i++) {
            pyramid.vertices[i] = Translate3(pyramid.vertices[i], windowDirection);
            pyramid.vertices[i] = RotateAboutPoint(pyramid.vertices[i], pyramidCenter, 0.0f, -0.015f, 0.0f);
            pyramid2d[i] = Project(pyramid.vertices[i]);
            pyramidScreen[i] = CartesianToScreen(pyramid2d[i], WIDTH, HEIGHT);
        }

        for (size_t i = 0; i < sphere.vertexCount; i++) {
            sphere.vertices[i] = Translate3(sphere.vertices[i], windowDirection);
            sphere.vertices[i] = RotateAboutPoint(sphere.vertices[i], sphereCenter, 0.001f, 0.02f, 0.005f);
            sphere2d[i] = Project(sphere.vertices[i]);
            sphereScreen[i] = CartesianToScreen(sphere2d[i], WIDTH, HEIGHT);
        }

        DrawWireframe(
                &cube,
                cubeScreen,
                GREEN
                );
        DrawWireframe(
                &pyramid,
                pyramidScreen,
                RED
                );
        DrawWireframe(
                &sphere,
                sphereScreen,
                BLUE
                );

        EndDrawing();
    }

    freeObject3D(&cube);
    freeObject3D(&pyramid);
    freeObject3D(&sphere);
    free(address);
    close(sock);
    CloseWindow();
    return 0;
}
