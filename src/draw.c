#include "../raylib-5.5_linux_amd64/include/raylib.h"
#include "types.h"
#include "object.h"

void DrawWireframe(struct Object *object, struct Point2 *ScreenPos, Color color) {
    for (size_t e = 0; e < object->edgeCount; e++) {
        size_t a = object->edges[e][0];
        size_t b = object->edges[e][1];
        DrawLine(
                (int)ScreenPos[a].x,
                (int)ScreenPos[a].y,
                (int)ScreenPos[b].x,
                (int)ScreenPos[b].y,
                color
                );
    }
}
